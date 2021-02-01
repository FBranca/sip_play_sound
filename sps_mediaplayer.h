#pragma once

#include <mutex>
#include <condition_variable>
#include <pjsua2.hpp>


/**
 * @brief Custom AudioMediaPlayer 
 *
 * Audio player that can blocking wait until the end of file
*/
class SpsAudioMediaPlayer : public pj::AudioMediaPlayer
{
// Types & consts
public:
    typedef std::unique_ptr<SpsAudioMediaPlayer> unique_ptr;

    // Callback to notify EOF
    class ICallback
    {
    public:
        virtual ~ICallback () {};
        virtual void mediaPlayerEof (SpsAudioMediaPlayer* player) = 0;
    };

// Public API
public:
    SpsAudioMediaPlayer () :
        m_callback (nullptr)
    {
    }

    inline void setCallback (ICallback *callback)
    {
        m_callback = callback;
    }

// Methods from pj::AudioMediaPlayer
private:
    virtual void onEof2 () override final
    {
        if (m_callback) {
            m_callback->mediaPlayerEof (this);
        }
    };

// Members
private:
    ICallback *m_callback;
};