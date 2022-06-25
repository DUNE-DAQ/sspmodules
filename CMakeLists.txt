cmake_minimum_required(VERSION 3.12)
project(sspmodules VERSION 1.1.1)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

# ers: for exceptions and normal output
# Boost unit_test_framework: for the unit tests
# Boost program_options: to allow command line options with descriptions/default values

find_package(appfwk REQUIRED)
find_package(iomanager REQUIRED)
find_package(logging REQUIRED)
find_package(ers REQUIRED)
find_package(detdataformats REQUIRED)
find_package(readoutlibs REQUIRED)
find_package(fdreadoutlibs REQUIRED)
find_package(opmonlib REQUIRED)

daq_codegen(sspledcalibmodule.jsonnet TEMPLATES Structs.hpp.j2 Nljs.hpp.j2 )
#daq_codegen( *info.jsonnet DEP_PKGS opmonlib TEMPLATES opmonlib/InfoStructs.hpp.j2 opmonlib/InfoNljs.hpp.j2 )

set(DUNEDAQ_DEPENDENCIES appfwk::appfwk readoutlibs::readoutlibs fdreadoutlibs::fdreadoutlibs detdataformats::detdataformats)

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
#daq_add_library(IntPrinter.cpp LINK_LIBRARIES ers::ers)

#if(WITH_FTD2XX_AS_PACKAGE)
  # All good
#else()
  # Hack target
#  target_include_directories(sspmodules PUBLIC $ENV{FTD2XX_INC})
#endif()

##############################################################################
#daq_add_python_bindings(ssp_wrapper.cpp LINK_LIBRARIES ${PROJECT_NAME})

##############################################################################
#daq_add_plugin(SSPCardReader duneDAQModule LINK_LIBRARIES sspmodules)
daq_add_plugin(SSPLEDCalibModule duneDAQModule LINK_LIBRARIES sspmodules)

##############################################################################
#daq_add_application( toylibrary_test_program toylibrary_test_program.cxx TEST LINK_LIBRARIES ${Boost_PROGRAM_OPTIONS_LIBRARY} toylibrary )

##############################################################################
#daq_add_unit_test(ValueWrapper_test)

##############################################################################

# Could add "toylibrary_test_program" to the argument list here, but
# presumably end users wouldn't care about an integration test program
# and just want the shared object library installed

daq_install()



