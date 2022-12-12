/**
 * @file EmulatedDevice.h
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef SSPMODULES_SRC_ANLBOARD_EMULATEDDEVICE_HPP_
#define SSPMODULES_SRC_ANLBOARD_EMULATEDDEVICE_HPP_

#include "detdataformats/ssp/SSPTypes.hpp"

#include "Device.hpp"
#include "SafeQueue.hpp"

#include <atomic>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdint.h>
#include <string>
#include <unistd.h>
#include <vector>

namespace dunedaq {
namespace sspmodules {

class EmulatedDevice : public Device
{

  friend class DeviceManager;

public:
  explicit EmulatedDevice(unsigned int deviceNumber = 0);

  virtual ~EmulatedDevice() {}

  // Implementation of base class interface

  inline virtual bool IsOpen() { return isOpen; }

  virtual void Close();

  virtual void DevicePurgeComm();

  virtual void DevicePurgeData();

  virtual void DeviceQueueStatus(unsigned int* numWords);

  virtual void DeviceReceive(std::vector<unsigned int>& data, unsigned int size);

  virtual void DeviceRead(unsigned int address, unsigned int* value);

  virtual void DeviceReadMask(unsigned int address, unsigned int mask, unsigned int* value);

  virtual void DeviceWrite(unsigned int address, unsigned int value);

  virtual void DeviceWriteMask(unsigned int address, unsigned int mask, unsigned int value);

  virtual void DeviceSet(unsigned int address, unsigned int mask);

  virtual void DeviceClear(unsigned int address, unsigned int mask);

  virtual void DeviceArrayRead(unsigned int address, unsigned int size, unsigned int* data);

  virtual void DeviceArrayWrite(unsigned int address, unsigned int size, unsigned int* data);

private:
  virtual void Open(bool slowControlOnly = false);

  // Start generation of events by emulator thread
  // Called when appropriate register is set via DeviceWrite
  void Start();

  // Stop generation of events by emulator thread
  // Called when appropriate register is set via DeviceWrite
  void Stop();

  // Add fake events to fEmulatedBuffer periodically
  void EmulatorLoop();

  // Device number to put into event headers
  unsigned int fDeviceNumber;

  bool isOpen;

  // Separate thread to generate fake data asynchronously
  std::unique_ptr<std::thread> fEmulatorThread;

  // Buffer for fake data, popped from by DeviceReceive
  SafeQueue<unsigned int> fEmulatedBuffer;

  // Set by Stop method; tells emulator thread to stop generating data
  std::atomic<bool> fEmulatorShouldStop;
};

} // namespace sspmodules
} // namespace dunedaq

#endif // SSPMODULES_SRC_ANLBOARD_EMULATEDDEVICE_HPP_
