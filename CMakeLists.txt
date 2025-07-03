cmake_minimum_required(VERSION 3.12)
project(sspmodules VERSION 1.5.0)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

# ers: for exceptions and normal output
# Boost unit_test_framework: for the unit tests
# Boost program_options: to allow command line options with descriptions/default values

find_package(appfwk REQUIRED)
find_package(confmodel REQUIRED)
find_package(appmodel REQUIRED)
find_package(logging REQUIRED)
find_package(ers REQUIRED)

# daq_oks_codegen(sspmodules.schema.xml NAMESPACE dunedaq::sspmodules::dal DALDIR dal DEP_PKGS confmodel)
set(DUNEDAQ_DEPENDENCIES appfwk::appfwk confmodel::confmodel appmodel::appmodel)

# Provide override functionality for SSP dependencies
#option(WITH_FTD2XX_AS_PACKAGE "SSP external (ftd2xx) as a dunedaq package" OFF)

#if(WITH_FTD2XX_AS_PACKAGE)
#  set(SSP_DEPENDENCIES
#    ftd2xx::ftd2xx
#  )
#else()
#  set(SSP_DEPENDENCIES
#    $ENV{FTD2XX_LIB}/libftd2xx.so
#  )
#endif()

##############################################################################
# Note that when the ers library is linked in, it uses a "::" because
# it's in an external package, not a local CMake target. The semantics
# are <namespace>::<shared library / executable>
daq_add_library(anlBoard/*.cxx SSPLEDCalibWrapper.cpp LINK_LIBRARIES ${SSP_DEPENDENCIES} ${DUNEDAQ_DEPENDENCIES})

#if(WITH_FTD2XX_AS_PACKAGE)
  # All good
#else()
  # Hack target
#  target_include_directories(sspmodules PUBLIC $ENV{FTD2XX_INC})
#endif()

##############################################################################
daq_add_plugin(SSPLEDCalibModule duneDAQModule LINK_LIBRARIES sspmodules)

daq_add_application( ssp_tcp_test ssp_tcp_test.cxx TEST LINK_LIBRARIES sspmodules )
daq_add_application( ssp_udp_test ssp_udp_test.cxx TEST LINK_LIBRARIES sspmodules )

daq_install()



