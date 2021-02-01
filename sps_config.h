#pragma once

#include <string>


class SpsConfig
{
public:
   static SpsConfig& instance () { return m_instance; }

   bool init (int argc, char **argv);

   // network configuration
   inline unsigned int getLocalPort () const { return m_local_port; }
   
   // Audio file to play
   inline const std::string& getAudioFile () const { return m_audio_file; }

   // SIP configuration
   inline const std::string& getAccountURI          () const { return m_account_uri; }
   inline const std::string& getRegistrarURI        () const { return m_registrar_uri; }
   inline const std::string& getAccountAuthRealm    () const { return m_auth_realm; }
   inline const std::string& getAccountAuthUser     () const { return m_auth_username; }
   inline const std::string& getAccountAuthPassword () const { return m_auth_password; }
   inline const std::string& getDestUri             () const { return m_dest_uri; }

   // vebose / debugging options
   inline int  getLogLevelConsole () const { return m_log_level_console; }
   inline bool isDumpConfig () const { return m_dump_config; }

private:
   SpsConfig ();
   SpsConfig (SpsConfig &instance);

private:
   /** Singleton instance
    * Static allocation is simplier.
    * No need for a allocation on first use since all access are done after entering main()
   */
   static SpsConfig m_instance;

   unsigned int m_local_port; //!< Local SIP TCP port (default: 5060)
   
   std::string m_account_uri;    //!< Account URI
   std::string m_registrar_uri;  //!< Registrar URI
   std::string m_auth_realm;     //!< Authentication realm ('*' by default)
   std::string m_auth_username;  //!< Authentication user name
   std::string m_auth_password;  //!< Authentication password

   std::string m_audio_file;  //!< Audio file to play on call establishment
   std::string m_dest_uri;    //!< SIP destination to call

   int   m_log_level_console; //!< Console log level
   bool  m_dump_config; //!< true to dump the PJSIP account configuration
};
