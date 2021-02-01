#pragma once

#include <pjsua2.hpp>
#include <mutex>
#include <condition_variable>
#include <vector>

#include "sps_call.h"


/**
 * @brief   Customized SIP account
 *
 * SIP account augmented with some helpers:
 * - wait for the registration to complete
 * - handling an incoming call
*/
class SpsAccount : public pj::Account
{
// Public API
public:
    SpsAccount ();
    virtual ~SpsAccount ();

    void create_and_register ();
    SpsCall::shared_ptr audio_call (const std::string& uri, SpsAudioMediaPlayer::unique_ptr &audioplayer_ptr);

    // Wait until the registration completes.
    bool waitRegistration (const unsigned int timeout);

    void release (SpsCall *call_ptr);

// Callback methods from pj::Account
private:
    virtual void onRegState (pj::OnRegStateParam &reg_state) override final;

// Members
private:
    // Mutex and condition variable used to wait until the registration completes.
    std::mutex  m_mutex;
    std::condition_variable m_cv;

    // Call list
    std::vector<SpsCall::shared_ptr>  m_calls;
};
