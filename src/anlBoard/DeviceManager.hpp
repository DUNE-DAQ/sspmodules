/**
 * @file DeviceManager.h
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef SSPMODULES_SRC_ANLBOARD_DEVICEMANAGER_HPP_
#define SSPMODULES_SRC_ANLBOARD_DEVICEMANAGER_HPP_

#include "detdataformats/ssp/SSPTypes.hpp"

//#include "ftd2xx.h"
//#include "USBDevice.h"
#include "EmulatedDevice.hpp"
#include "EthernetDevice.hpp"

#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <stdint.h>
#include <string>
#include <unistd.h>
#include <vector>

namespace dunedaq {
namespace sspmodules {

class DeviceManager
{

public:
  // Get reference to instance of DeviceManager singleton
  static DeviceManager& Get();

  // unsigned int GetNUSBDevices();

  // Open a device and return a pointer containing a handle to it
  Device* OpenDevice(dunedaq::detdataformats::ssp::Comm_t commType,
                     unsigned int deviceId,
                     bool slowControlOnly = false);

  // Interrogate FTDI for list of devices. GetNUSBDevices and OpenDevice will call this
  // if it has not yet been run, so it should not normally be necessary to call this directly.
  void RefreshDevices();

private:
  DeviceManager();

  DeviceManager(DeviceManager const&); // Don't implement

  void operator=(DeviceManager const&); // Don't implement

  // List of USB devices on FTDI link
  // std::vector<USBDevice> fUSBDevices;

  // Ethernet devices keyed by IP address
  std::map<unsigned long, std::unique_ptr<EthernetDevice>> fEthernetDevices; // NOLINT(runtime/int)

  // List of emulated devices
  std::vector<std::unique_ptr<EmulatedDevice>> fEmulatedDevices;

  bool fHaveLookedForDevices;
};

} // namespace sspmodules
} // namespace dunedaq

#endif // SSPMODULES_SRC_ANLBOARD_DEVICEMANAGER_HPP_
