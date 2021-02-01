#pragma once

#include <memory>
#include <mutex>
#include <condition_variable>
#include <pjsua2.hpp>
#include "sps_mediaplayer.h"


class SpsAccount;


class SpsCall : public pj::Call, private SpsAudioMediaPlayer::ICallback
{
// Types & consts
public:
   // alias for smart pointers on this class
   typedef std::shared_ptr<SpsCall> shared_ptr;
   typedef std::unique_ptr<SpsCall> unique_ptr;

// Public API
public:
   SpsCall(SpsAccount& account, SpsAudioMediaPlayer::unique_ptr &audioplayer_ptr);
   virtual ~SpsCall();

   bool waitCallEstablished (unsigned int timeout);
   void waitCallEnd ();

// Methods from pj::Call
private:
   virtual void onCallState (pj::OnCallStateParam &state) override final;
   virtual void onCallMediaState (pj::OnCallMediaStateParam &media_state) override final;

// Methods from SpsAudioMediaPlayer::ICallback
private:
   virtual void mediaPlayerEof (SpsAudioMediaPlayer* player) override final;

// Members
private:
   SpsAccount& m_account;
   std::unique_ptr<SpsAudioMediaPlayer> m_wavplay_ptr;
   
   /// Wave player reached EOF
   bool  m_player_eof; 

   /// Condition variable for blocking wait on call status change or media player EOF
   std::mutex m_mutex;
   std::condition_variable m_cv;
};
