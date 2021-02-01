#include <iostream>
#include <algorithm>

#include "sps_account.h"

/**
 * @brief   Constructor
*/
SpsAccount::SpsAccount()
{}

/**
 * @brief   Destructor
*/
SpsAccount::~SpsAccount()
{
   std::cout << "*** pj::Account is being deleted: No of calls="
            << m_calls.size() << std::endl;
}


void SpsAccount::create_and_register ()
{

}

/**
 * @brief   Initiate an audio call
 *
 * @param[in]  uri   destination URI to call
*/
SpsCall::shared_ptr SpsAccount::audio_call (const std::string& uri, SpsAudioMediaPlayer::unique_ptr &audioplayer_ptr)
{
   SpsCall::shared_ptr call_ptr = std::make_shared<SpsCall>(*this, audioplayer_ptr);
   m_calls.push_back(call_ptr);

   pj::CallOpParam call_param(true);
   call_param.opt.audioCount = 1;
   call_param.opt.videoCount = 0;
   call_ptr->makeCall(uri, call_param);
   std::cout << "CALL ID = " << call_ptr->getId() << std::endl;

   return call_ptr;
}


void SpsAccount::release(SpsCall *call_ptr)
{
   std::vector<SpsCall::shared_ptr>::iterator it = std::find_if (m_calls.begin(), m_calls.end(), [&](SpsCall::shared_ptr &p) { return p.get() == call_ptr;});
   if (it != m_calls.end()) {
      std::cout << "*** RELEASING CALL" << std::endl;
      m_calls.erase(it);
   } else {
      std::cout << "***  CALL NOT FOUND" << std::endl;
   }
}

/**
 * @brief   Wait for the registration to complete
 *
 * @param[in] timeout   maximum time to wait in seconds (0=infinite)
*/
bool SpsAccount::waitRegistration (const unsigned int timeout)
{
   std::unique_lock<std::mutex> lock(m_mutex);
   if (timeout) {
      m_cv.wait_for(lock, std::chrono::seconds(timeout));
   } else {
      m_cv.wait(lock);
   }

   return getInfo().regIsActive;
}

void SpsAccount::onRegState(pj::OnRegStateParam &reg_state)
{
   pj::AccountInfo ai = getInfo();
   std::cout << (ai.regIsActive? "*** Register: code=" : "*** Unregister: code=") << reg_state.code << std::endl;

   // Awakes any thread waiting
   std::unique_lock<std::mutex> lock(m_mutex);
   m_cv.notify_all();
}
