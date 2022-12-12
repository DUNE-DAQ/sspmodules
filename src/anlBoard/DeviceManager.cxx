/**
 * @file DeviceManager.cxx
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef SSPMODULES_SRC_ANLBOARD_DEVICEMANAGER_CXX_
#define SSPMODULES_SRC_ANLBOARD_DEVICEMANAGER_CXX_

#include "detdataformats/ssp/SSPTypes.hpp"

#include "DeviceManager.hpp"
//#include "ftd2xx.h"
//#include "dune-artdaq/DAQLogger/DAQLogger.hh"
#include "anlExceptions.hpp"

#include "boost/asio.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>
#include <utility>

dunedaq::sspmodules::DeviceManager&
dunedaq::sspmodules::DeviceManager::Get()
{
  static dunedaq::sspmodules::DeviceManager instance;
  return instance;
}

dunedaq::sspmodules::DeviceManager::DeviceManager() {}

//
// unsigned int SSPDAQ::DeviceManager::GetNUSBDevices(){
//  if(!fHaveLookedForDevices){
//    this->RefreshDevices();
//  }
//  return fUSBDevices.size();
//}
//

void
dunedaq::sspmodules::DeviceManager::RefreshDevices()
{
  //
  //  for(auto device=fUSBDevices.begin();device!=fUSBDevices.end();++device){
  //    if(device->IsOpen()){
  //      //dune::DAQLogger::LogWarning("SSP_DeviceManager")<<"Device manager refused request to refresh device list"
  //      //<<"due to USB devices still open"<<std::endl;
  //    }
  //  }
  //
  for (auto device = fEthernetDevices.begin(); device != fEthernetDevices.end(); ++device) {
    if ((device->second)->IsOpen()) {
      // dune::DAQLogger::LogWarning("SSP_DeviceManager")<<"Device manager refused request to refresh device list"
      //<<"due to ethernet devices still open"<<std::endl;
    }
  }
  for (auto device = fEmulatedDevices.begin(); device != fEmulatedDevices.end(); ++device) {
    if ((*device)->IsOpen()) {
      // dune::DAQLogger::LogWarning("SSP_DeviceManager")<<"Device manager refused request to refresh device list"
      //<<"due to emulated devices still open"<<std::endl;
    }
  }

  // Clear Device List
  // fUSBDevices.clear();
  fEthernetDevices.clear();
  fEmulatedDevices.clear();

  //
  //  //===========================//
  //  //==Find USB devices=========//
  //  //===========================//
  //
  //  unsigned int ftNumDevs;
  //
  //  std::map<std::string,FT_DEVICE_LIST_INFO_NODE*> dataChannels;
  //  std::map<std::string,FT_DEVICE_LIST_INFO_NODE*> commChannels;
  //
  //  // This code uses FTDI's D2XX driver to determine the available devices
  //  if(FT_CreateDeviceInfoList(&ftNumDevs)!=FT_OK){
  //    try {
  //      //dune::DAQLogger::LogError("SSP_DeviceManager")<<"Failed to create FTDI device info list"<<std::endl;
  //    } catch (...) {}
  //    throw(EFTDIError("Error in FT_CreateDeviceInfoList"));
  //  }
  //
  //  FT_DEVICE_LIST_INFO_NODE* deviceInfoNodes=new FT_DEVICE_LIST_INFO_NODE[ftNumDevs];
  //
  //  if(FT_GetDeviceInfoList(deviceInfoNodes,&ftNumDevs)!=FT_OK){
  //    delete deviceInfoNodes;
  //    try {
  //      //dune::DAQLogger::LogError("SSP_DeviceManager")<<"Failed to get FTDI device info list"<<std::endl;
  //    } catch (...) {}
  //    throw(EFTDIError("Error in FT_GetDeviceInfoList"));
  //  }
  //
  //  //Search through all devices for compatible interfaces
  //  for (unsigned int i = 0; i < ftNumDevs; i++) {
  //      // NOTE 1: Each device is actually 2 FTDI devices. Device with "A" at the end of serial number is the
  //      // data channel. "B" is the comms channel. Need to associate FTDI devices with the same base
  //      // number together to get a "whole" board.
  //      //
  //      // NOTE 2: There is a rare error in which the FTDI driver returns FT_OK but the device info is incorrect
  //      // In this case, the check on ftType and/or length of ftSerial will fail
  //      // The discover code will therefore fail to find any useable devices but will not return an error
  //      // The calling code should check numDevices and rerun FindDevices if it equals zero
  //
  //      // Search only for devices we can use (skip any others)
  //      if (deviceInfoNodes[i].Type != FT_DEVICE_2232H) {
  //	continue;	// Skip to next device
  //      }
  //
  //      // Add FTDI device to Device List using Serial number
  //      //If length is zero, then device is probably open in another process (though maybe we don't get type then
  //      either...)
  //      //===TODO: Should check flags for open devices and report the number open in other processes to cout
  //      unsigned int length = strlen(deviceInfoNodes[i].SerialNumber);	// Find length of serial number
  //      (including 'A' or 'B') if (length == 0) {
  //	continue;	// Skip to next device
  //      }
  //
  //      char serial[16];
  //
  //      strncpy(serial, deviceInfoNodes[i].SerialNumber, length - 1);	// Copy base serial number
  //      serial[length-1] = 0;					// Append NULL because strncpy() didn't!
  //
  //      // Update device list with FTDI device number
  //      switch (deviceInfoNodes[i].SerialNumber[length -1]) {
  //      case 'A':
  //	dataChannels[serial]=&(deviceInfoNodes[i]);
  //	break;
  //      case 'B':
  //	commChannels[serial]=&(deviceInfoNodes[i]);
  //	break;
  //      default:
  //	break;
  //      }
  //  }
  //
  //  //Check that device list is as expected, then construct USB device objects for each board
  //  if(dataChannels.size()!=commChannels.size()){
  //    try {
  //      //dune::DAQLogger::LogError("SSP_DeviceManager")<<"Different number of data and comm channels on
  //      FTDI!"<<std::endl;
  //    } catch (...) {}
  //    delete deviceInfoNodes;
  //    throw(EBadDeviceList());
  //  }
  //  std::map<std::string,FT_DEVICE_LIST_INFO_NODE*>::iterator dIter=dataChannels.begin();
  //  std::map<std::string,FT_DEVICE_LIST_INFO_NODE*>::iterator cIter=commChannels.begin();
  //
  //  for(;dIter!=dataChannels.end();++dIter,++cIter){
  //    if(dIter->first!=cIter->first){
  //      try {
  //	//dune::DAQLogger::LogError("SSP_DeviceManager")<<"Non-matching serial numbers for data and comm channels on
  // FTDI!"<<std::endl;
  //      } catch (...) {}
  //      delete deviceInfoNodes;
  //      throw(EBadDeviceList());
  //    }
  //    fUSBDevices.push_back(USBDevice(dIter->second,cIter->second));
  //    //dune::DAQLogger::LogInfo("SSP_DeviceManager")<<"Found a device with serial "<<dIter->first<<std::endl;
  //  }
  //
  //  delete[] deviceInfoNodes;
  //
}

dunedaq::sspmodules::Device*
dunedaq::sspmodules::DeviceManager::OpenDevice(dunedaq::detdataformats::ssp::Comm_t commType,
                                               unsigned int deviceNum,
                                               bool slowControlOnly)
{
  // Check for devices if this hasn't yet been done
  if (!fHaveLookedForDevices && commType != dunedaq::detdataformats::ssp::kEmulated) {
    this->RefreshDevices();
  }

  Device* device = 0;
  switch (commType) {
      //
      //  case SSPDAQ::kUSB:
      //    device=&fUSBDevices[deviceNum];
      //    if(device->IsOpen()){
      //      try {
      //      //dune::DAQLogger::LogError("SSP_DeviceManager")<<"Attempt to open already open device!"<<std::endl;
      //      } catch (...) {}
      //      throw(EDeviceAlreadyOpen());
      //    }
      //    else{
      //      device->Open(slowControlOnly);
      //    }
      //    break;
      //
    case dunedaq::detdataformats::ssp::kEthernet:
      if (fEthernetDevices.find(deviceNum) == fEthernetDevices.end()) {
        fEthernetDevices[deviceNum] = (std::move(
          std::unique_ptr<dunedaq::sspmodules::EthernetDevice>(new dunedaq::sspmodules::EthernetDevice(deviceNum))));
      }
      if (fEthernetDevices[deviceNum]->IsOpen()) {
        // dune::DAQLogger::LogError("SSP_DeviceManager")<<"Attempt to open already open device!"<<std::endl;
        throw(EDeviceAlreadyOpen());
      } else {
        device = fEthernetDevices[deviceNum].get();
        device->Open(slowControlOnly);
      }
      break;

    case dunedaq::detdataformats::ssp::kEmulated:
      while (fEmulatedDevices.size() <= deviceNum) {
        fEmulatedDevices.push_back(std::move(std::unique_ptr<dunedaq::sspmodules::EmulatedDevice>(
          new dunedaq::sspmodules::EmulatedDevice(fEmulatedDevices.size()))));
      }
      device = fEmulatedDevices[deviceNum].get();
      if (device->IsOpen()) {
        // dune::DAQLogger::LogError("SSP_DeviceManager")<<"Attempt to open already open device!"<<std::endl;
        throw(EDeviceAlreadyOpen());
      } else {
        device->Open(slowControlOnly);
      }
      break;
    default:
      // dune::DAQLogger::LogError("SSP_DeviceManager")<<"Unrecognised interface type!"<<std::endl;
      throw(std::invalid_argument(""));
  }
  return device;
}

#endif // SSPMODULES_SRC_ANLBOARD_DEVICEMANAGER_CXX_
