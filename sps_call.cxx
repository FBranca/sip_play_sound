#include <iostream>

#include "sps_account.h"
#include "sps_call.h"
#include "sps_config.h"


SpsCall::SpsCall (SpsAccount &account, SpsAudioMediaPlayer::unique_ptr &audioplayer_ptr) :
    pj::Call(account),
    m_account (account),
    m_wavplay_ptr (std::move(audioplayer_ptr)),
    m_player_eof (false)
{
    m_wavplay_ptr->setCallback(this);
}

SpsCall::~SpsCall ()
{
}

/**
 * @brief   Wait for the call to be established or terminated
 *
 * @param[in] timeout   maximum time to wait in seconds (0=infinite)
*/
bool SpsCall::waitCallEstablished (const unsigned int timeout)
{
    bool established = false;
    bool wait = true;
    while (wait) {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (timeout) {
            std::cv_status cvs = m_cv.wait_for(lock, std::chrono::seconds(timeout));
            if (std::cv_status::timeout == cvs) {
                break;
            }
        } else {
            m_cv.wait(lock);
        }

        if (this->isActive()) {
            pj::CallInfo ci = getInfo();
            wait = (ci.state != PJSIP_INV_STATE_CONFIRMED) && (ci.state != PJSIP_INV_STATE_DISCONNECTED);
            established = (ci.state == PJSIP_INV_STATE_CONFIRMED);
        } else {
            wait = false;
        }
    }

    return established;
}

/**
 * @brief  Wait until call is terminated or EOF of audio media
*/
void SpsCall::waitCallEnd ()
{
    bool wait = true;
    while (wait) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock);

        wait = (this->isActive() && ! m_player_eof);
    }
}

/**
 * @brief   Call state change callback
 *
 * Invoked on a call state change.
*/
void SpsCall::onCallState(pj::OnCallStateParam &state)
{
    PJ_UNUSED_ARG(state);

    pj::CallInfo ci = getInfo();
    std::cout << "*** Call: " <<  ci.remoteUri << " [" << ci.stateText << "]" << std::endl;

    std::unique_lock<std::mutex> lock(m_mutex);
    m_cv.notify_all();
}

/**
 * @brief   Media state callback
 *
 * Create wave player and connects it to this call.
*/
void SpsCall::onCallMediaState(pj::OnCallMediaStateParam &media_state)
{
    PJ_UNUSED_ARG(media_state);

    pj::Endpoint::instance().utilLogWrite (1, "sps_call", "onCallMediaState");

// Following section can help having a more precise understanding of the call/media state during a call
#if defined(SPS_CALL_DEBUG)
    pjsua_call_info pj_ci;
    unsigned mi;
    
    if (pjsua_call_get_info(getInfo().id, &pj_ci) == PJ_SUCCESS) {
        std::cout << "Media count: " << pj_ci.media_cnt << std::endl;
        for (mi = 0; mi < pj_ci.media_cnt; mi++) {
            std::cout << "  media " << mi
                << " type " << pj_ci.media[mi].type
                << " isaudio:" << (pj_ci.media[mi].type == PJMEDIA_TYPE_AUDIO)
                << " slot:" << pj_ci.media[mi].stream.aud.conf_slot
                << std::endl;
        }
	}
#endif

    pj::AudioMedia  call_audio_media;
    try {
    	// Get the first audio media for this call
    	call_audio_media = getAudioMedia(-1);
    } catch(...) {
	    std::cerr << "Failed to get audio media" << std::endl;
	    return;
    }

    // onCallMediaState can be called multiple times during a call
    // Especially, it can be invoked when a codec re-negociation occures during the call.
    // The wave player needs to be reconnected to the new call media
    m_wavplay_ptr->startTransmit(call_audio_media);
}

/**
 * @brief   Wav player reached EOF
 *
 * @param[in] player  Pointer to the player
*/
void SpsCall::mediaPlayerEof (SpsAudioMediaPlayer* player)
{
    PJ_UNUSED_ARG (player);

    std::unique_lock<std::mutex> lock(m_mutex);
    m_player_eof = true;
    m_cv.notify_all();
}
