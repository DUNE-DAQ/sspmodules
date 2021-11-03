/**
 * @file EthernetDevice.h
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef SSPMODULES_SRC_ANLBOARD_ETHERNETDEVICE_HPP_
#define SSPMODULES_SRC_ANLBOARD_ETHERNETDEVICE_HPP_

#include "detdataformats/ssp/SSPTypes.hpp"

#include "Device.hpp"
#include "boost/asio.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <vector>

namespace dunedaq {
namespace sspmodules {

class EthernetDevice : public Device{

public:

  //Create a device object using FTDI handles given for data and communication channels
  explicit EthernetDevice(unsigned long ipAddress);  // NOLINT

  //Implementation of base class interface

  inline virtual bool IsOpen(){
    return isOpen;
  }

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

  //Internal functions - make public so debugging code can access them

  void SendReceive(dunedaq::detdataformats::CtrlPacket& tx, dunedaq::detdataformats::CtrlPacket& rx, unsigned int txSize, unsigned int rxSizeExpected, unsigned int retryCount=0);

  void SendEthernet(dunedaq::detdataformats::CtrlPacket& tx, unsigned int txSize);

  void ReceiveEthernet(dunedaq::detdataformats::CtrlPacket& rx, unsigned int rxSizeExpected);

  void DevicePurge(boost::asio::ip::tcp::socket& socket);

private:

  friend class DeviceManager;

  bool isOpen;

  static boost::asio::io_service fIo_service;

  boost::asio::ip::tcp::socket fCommSocket;
  boost::asio::ip::tcp::socket fDataSocket;

  boost::asio::ip::address fIP;

  //Can only be opened by DeviceManager, not by user
  virtual void Open(bool slowControlOnly);

};

} // namespace sspmodules
} // namespace dunedaq

#endif // SSPMODULES_SRC_ANLBOARD_ETHERNETDEVICE_HPP_
