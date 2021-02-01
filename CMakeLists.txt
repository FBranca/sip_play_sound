cmake_minimum_required(VERSION 3.10)

# set the project name
project(sps)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Use pkg-config to locate pjproject installation and compile options
find_package(PkgConfig REQUIRED)

pkg_check_modules(PJSIP libpjproject>=2.0 REQUIRED)
message(STATUS "Using PJSIP version " ${PJSIP_VERSION} " from " ${PJSIP_PREFIX})

# add the executable
add_executable(sps main.cxx sps_account.cxx sps_call.cxx sps_config.cxx)

# target_link_libraries(sip_sound -lpjsua2 -lpj -lpjsua -lpjmedia)

target_link_libraries(sps ${PJSIP_STATIC_LIBRARIES} -lasound -lpthread -lssl -lcrypto)
target_include_directories(sps PUBLIC ${PJSIP_STATIC_INCLUDE_DIRS})
target_compile_options(sps PUBLIC ${PJSIP_STATIC_CFLAGS_OTHER})