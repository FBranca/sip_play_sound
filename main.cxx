#include <pjsua2.hpp>
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>


#include "sps_account.h"
#include "sps_call.h"
#include "sps_config.h"

// return codes
enum RETURN_CODES {
    RET_BAD_OPTION = 1,
    RET_ERR_LOADING_WAV,    // Error loading wav file
    RET_ERR_ACCOUNT,        // Error creating SIP account (bad configuration)
    RET_ERR_ACCOUNT_REG,    // Error registering SIP account
};


extern "C"
int main(int argc, char **argv)
{
    // read option file and parse command line
    SpsConfig &config = SpsConfig::instance();
    if (!config.init (argc, argv))
    {
        return RET_BAD_OPTION;
    }

    int ret = 0;
    pj::Endpoint ep;

    try {
        ep.libCreate();

        // Init library
        pj::EpConfig ep_cfg;
        ep_cfg.logConfig.msgLogging = 0;
        ep_cfg.logConfig.level = 5;
        ep_cfg.logConfig.consoleLevel = config.getLogLevelConsole();
        ep_cfg.logConfig.filename = "log.log";
        ep_cfg.logConfig.fileFlags = 0;
        ep_cfg.logConfig.writer = nullptr;
        ep.libInit( ep_cfg );

        // Transport
        pj::TransportConfig tcfg;
        tcfg.port = config.getLocalPort();
        ep.transportCreate(PJSIP_TRANSPORT_UDP, tcfg);

        // start library
        ep.libStart();

        // not using any audio device
        ep.audDevManager().setNullDev();

        // First : Create & initialize the Wav player
        // If an error occurs, we won't register nor make the call
        std::unique_ptr<SpsAudioMediaPlayer>    wavplay_ptr(new SpsAudioMediaPlayer());
        try {
            wavplay_ptr->createPlayer(config.getAudioFile(), 0);
        } catch (...) {
            std::cout << "Failed opening wav file " <<  config.getAudioFile() << std::endl;
            wavplay_ptr.release();
            return RET_ERR_LOADING_WAV;
        }

        // create & configure  account
        std::unique_ptr<SpsAccount> account_ptr(new SpsAccount);
        pj::AccountConfig acc_cfg;
        acc_cfg.idUri = config.getAccountURI();
        acc_cfg.regConfig.registrarUri = config.getRegistrarURI();
        acc_cfg.sipConfig.authCreds.push_back( pj::AuthCredInfo(
            "digest",
            config.getAccountAuthRealm(),
            config.getAccountAuthUser(),
            0,
            config.getAccountAuthPassword()
        ));

        // create the account
        try {
            account_ptr->create(acc_cfg);
        } catch (...) {
            std::cerr << "Creating account failed" << std::endl;
            return RET_ERR_ACCOUNT;
        }

        if (config.isDumpConfig()) {
            pj::JsonDocument jdoc;
            jdoc.writeObject(acc_cfg);
            std::cout << "Configuration DUMP:" << std::endl;
            std::cout << jdoc.saveString() << std::endl << std::endl;
        }

        // account registration
        account_ptr->waitRegistration(3);
        if (! account_ptr->getInfo().regIsActive) {
            std::cerr << "Account registration failed" << std::endl;
            return RET_ERR_ACCOUNT_REG;
        }

        // dial and play the sound
        SpsCall::shared_ptr call_ptr = account_ptr->audio_call (config.getDestUri(), wavplay_ptr);
        bool established = call_ptr->waitCallEstablished (5);
        if (established) {
            call_ptr->waitCallEnd ();
        } else {
            const pj::CallInfo call_info = call_ptr->getInfo();
            std::cerr << "Call not established, state:" << call_info.stateText << ", reason:" << call_info.lastReason << std::endl;
    
            pj::CallOpParam call_param(false);
            call_ptr->hangup(call_param);                
            return 4;
        }

        account_ptr->release (call_ptr.get());
        call_ptr.reset();

        ret = PJ_SUCCESS;
    } catch (pj::Error & err) {
        std::cout << "Exception: " << err.info() << std::endl;
        ret = 1;
    }

    try {
    	ep.libDestroy();
    } catch(pj::Error &err) {
        std::cout << "Exception: " << err.info() << std::endl;
        ret = 1;
    }

    return ret;
}


