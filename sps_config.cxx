#include "lib/CLI11/CLI.hpp"

#include "sps_config.h"

SpsConfig SpsConfig::m_instance;


SpsConfig::SpsConfig () :
   m_local_port         (5060),
   m_account_uri        (),
   m_registrar_uri      (),
   m_auth_realm         (),
   m_auth_username      (),
   m_auth_password      (),
   m_log_level_console  (1),
   m_dump_config        (false)
{
}

bool SpsConfig::init (int argc, char **argv)
{
   CLI::App app("Sip Play Sound - Dial a number, play a sound and hangup");

   app.add_flag ("--dump-config", m_dump_config, "Dump the full PJSIP configuration");
   app.add_option ("--log-console", m_log_level_console, "PJSIP log level for console outputs");

   app.add_option ("--local-port",     m_local_port,     "Local TCP port for SIP protocol");

   app.add_option ("--account-uri",    m_account_uri,    "SIP account URI");
   app.add_option ("--registrar-uri",  m_registrar_uri,  "SIP registrar URI");
   app.add_option ("--auth-realm",     m_auth_realm,     "SIP account auth realm");
   app.add_option ("--auth-user",      m_auth_username,  "SIP account auth username");
   app.add_option ("--auth-passwd",    m_auth_password,  "SIP account auth password");

   app.add_option ("--wav-file",    m_audio_file,  "WAV file to play on call establishment");
   app.add_option ("dest-uri",      m_dest_uri,    "SIP destination to call");

   bool ok = true;
   try {
      app.parse (argc, argv);
   }
   catch (const CLI::ParseError &e) {
      app.exit(e);
      ok = false;
   }

   return ok;
}
