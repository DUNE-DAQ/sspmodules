/**
 * @file DeviceInterface.hpp
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef SSPMODULES_SRC_ANLBOARD_DEVICEINTERFACE_HPP_
#define SSPMODULES_SRC_ANLBOARD_DEVICEINTERFACE_HPP_

#include "logging/Logging.hpp"
#include "sspmodules/dal/SSPCalibModule.hpp"
#include "sspmodules/dal/SSPRegister.hpp"

#include "DeviceManager.hpp"
#include "Device.hpp"
#include "EventPacket.hpp"

#include <string>
#include <memory>
#include <map>
#include <deque>
#include <queue>
#include <vector>

namespace dunedaq {
namespace sspmodules {

struct TriggerInfo{
  unsigned long startTime;    // NOLINT(runtime/int)
  unsigned long endTime;      // NOLINT(runtime/int)
  unsigned long triggerTime;  // NOLINT(runtime/int)
  unsigned short triggerType; // NOLINT(runtime/int)
};

class DeviceInterface{

public:

  enum State_t{kUninitialized,kInitialized,kRunning,kStopping,kStopped,kBad};

  //Just sets the fields needed to request the device.
  //Real work is done in Initialize which is called manually.
  explicit DeviceInterface();

  ~DeviceInterface(){ }

  void OpenSlowControl();

  void ConfigureLEDCalib(const dal::SSPCalibModule* conf);


  //Called by ReadEvents
  //Get an event off the hardware buffer.
  //Timeout after some wait period
  void ReadEventFromDevice(EventPacket& event);

  //Obtain current state of device
  //inline State_t State(){return fState;}

  //Setter for single register
  //If mask is given then only bits which are high in the mask will be set.
  void SetRegister(unsigned int address, unsigned int value, unsigned int mask=0xFFFFFFFF);

  //Setter for series of contiguous registers, with vector input
  void SetRegisterArray(unsigned int address, std::vector<unsigned int> value);

  //Setter for series of contiguous registers, with C array input
  void SetRegisterArray(unsigned int address, unsigned int* value, unsigned int size);

  //Getter for single register
  //If mask is set then bits which are low in the mask will be returned as zeros.
  void ReadRegister(unsigned int address, unsigned int& value, unsigned int mask=0xFFFFFFFF);

  //Getter for series of contiguous registers, with vector output
  void ReadRegisterArray(unsigned int address, std::vector<unsigned int>& value, unsigned int size);

  //Getter for series of contiguous registers, with C array output
  void ReadRegisterArray(unsigned int address, unsigned int* value, unsigned int size);

  //Methods to set registers with names (as defined in SSPDAQ::RegMap)

  //Set single named register
  void SetRegisterByName(std::string name, unsigned int value);

  //Set single element of an array of registers
  void SetRegisterElementByName(std::string name, unsigned int index, unsigned int value);

  //Set all elements of an array to a single value
  void SetRegisterArrayByName(std::string name, unsigned int value);

  //Set all elements of an array using values vector
  void SetRegisterArrayByName(std::string name, std::vector<unsigned int> values);

  //Read single named register
  void ReadRegisterByName(std::string name, unsigned int& value);

  //Read single element of an array of registers
  void ReadRegisterElementByName(std::string name, unsigned int index, unsigned int& value);

  //Read all elements of an array into values vector
  void ReadRegisterArrayByName(std::string name, std::vector<unsigned int>& values);

  void SetHardwareClockRateInMHz(unsigned int rate){fHardwareClockRateInMHz = rate;}

  void SetDummyPeriod(int period){fDummyPeriod=period;}

  void SetUseExternalTimestamp(bool val){fUseExternalTimestamp = val;}

  void SetPartitionNumber(unsigned int val){fPartitionNumber=val;}

  void SetTimingAddress(unsigned int val){fTimingAddress=val;}

  void PrintHardwareState();

  std::string GetIdentifier();

  bool exception() const { return exception_.load(); }

private:

  //Internal device object used for hardware operations.
  //Owned by the device manager, not this object.
  Device* fDevice;

  //Index of the device in the hardware-returned list
  unsigned long fDeviceId;    // NOLINT(runtime/int)

  //Holds current device state. Hopefully this matches the state of the
  //hardware itself.
  State_t fState;

  void set_exception( bool exception ) { exception_.store( exception ); }

  bool fUseExternalTimestamp;

  unsigned int fHardwareClockRateInMHz;

  int fDummyPeriod;

  bool fSlowControlOnly;

  unsigned int fPartitionNumber;

  unsigned int fTimingAddress;


  std::atomic<bool> exception_;

};

} // namespace sspmodules
} // namespace dunedaq

#endif // SSPMODULES_SRC_ANLBOARD_DEVICEINTERFACE_HPP_
