# sip_play_sound
Dial a number, play a sound, hangup using PJSUA2

A simple program to call a SIP number, play a WAV file, and hangup.
All parameters are passed using the command line.

A found very few similar programs, and none were written using the PJSUA2 C++ API (which is still in alpha version at this time, mostly because of high level language bindings ?).

It features :
- Check if the sound is found before trying to make a call, exit if the file is not found.
- Account registration timeout, exit on timeout
- Call ringing timeout, exit on timeout

The binary is staticaly linked, so it can easily used anywhere.

# Prerequisites

The following environment is needed:
* CMake
* PJSIP library 2.10

I've not tested with anterior versions of PJSIP.
Since the PJSUA2 API is still instable it may not compile with other versions.

# Usage
```
Usage: ./sps [OPTIONS] [dest-uri]

Positionals:
  dest-uri TEXT               SIP destination to call

Options:
  -h,--help                   Print this help message and exit
  --dump-config               Dump the full PJSIP configuration
  --log-console INT           PJSIP log level for console outputs
  --local-port UINT           Local TCP port for SIP protocol
  --account-uri TEXT          SIP account URI
  --registrar-uri TEXT        SIP registrar URI
  --auth-realm TEXT           SIP account auth realm
  --auth-user TEXT            SIP account auth username
  --auth-passwd TEXT          SIP account auth password
  --wav-file TEXT             WAV file to play on call establishment
```
