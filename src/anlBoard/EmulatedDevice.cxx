/**
 * @file EmulatedDevice.cxx
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef SSPMODULES_SRC_ANLBOARD_EMULATEDDEVICE_CXX_
#define SSPMODULES_SRC_ANLBOARD_EMULATEDDEVICE_CXX_

#include "detdataformats/ssp/SSPTypes.hpp"

#include "anlExceptions.hpp"
#include "EmulatedDevice.hpp"
//#include "dune-artdaq/DAQLogger/DAQLogger.hh"
#include "RegMap.hpp"

#include <cstdlib>
#include <random>
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

dunedaq::sspmodules::EmulatedDevice::EmulatedDevice(unsigned int deviceNumber){
  fDeviceNumber=deviceNumber;
  isOpen=false;
  fEmulatorThread=0;
}

void dunedaq::sspmodules::EmulatedDevice::Open(bool slowControlOnly){

  fSlowControlOnly=slowControlOnly;
  //dune::DAQLogger::LogInfo("SSP_EmulatedDevice")<<"Emulated device open"<<std::endl;
  isOpen=true;
}

void dunedaq::sspmodules::EmulatedDevice::Close(){
  this->DevicePurgeData();
  isOpen=false;
  //dune::DAQLogger::LogInfo("SSP_EmulatedDevice")<<"Emulated Device closed"<<std::endl;
}

void dunedaq::sspmodules::EmulatedDevice::DevicePurgeComm()
{
}

void dunedaq::sspmodules::EmulatedDevice::DevicePurgeData()
{
  while(fEmulatedBuffer.size()){
    fEmulatedBuffer.pop();
  }
}

void dunedaq::sspmodules::EmulatedDevice::DeviceQueueStatus (unsigned int* numWords)
{
  (*numWords)=fEmulatedBuffer.size();
}

void dunedaq::sspmodules::EmulatedDevice::DeviceReceive(std::vector<unsigned int>& data, unsigned int size){

  data.clear();
  unsigned int element;
  for(unsigned int iElement=0;iElement<size;++iElement){
    bool gotData=fEmulatedBuffer.try_pop(element,std::chrono::microseconds(1000));
    if(gotData){
      data.push_back(element);
	} else {
      break;
    }
  }
}

//==============================================================================
// Command Functions
//==============================================================================

//Only respond to specific commands needed to simulate normal operations
void dunedaq::sspmodules::EmulatedDevice::DeviceRead (unsigned int address, unsigned int* value)
{
  (void)address;
  (void)value;
}

void dunedaq::sspmodules::EmulatedDevice::DeviceReadMask (unsigned int address, unsigned int mask, unsigned int* value)
{
  (void)address;
  (void)mask;
  (void)value;
}

void dunedaq::sspmodules::EmulatedDevice::DeviceWrite (unsigned int address, unsigned int value)
{
  dunedaq::sspmodules::RegMap& duneReg=dunedaq::sspmodules::RegMap::Get();
  if(address==duneReg.master_logic_control&&value==0x00000001){
    this->Start();
  } else if (address==duneReg.event_data_control&&value==0x00020001){
    this->Stop();
  }
}

void dunedaq::sspmodules::EmulatedDevice::DeviceWriteMask (unsigned int address, unsigned int mask, unsigned int value)
{
  (void)address;
  (void)mask;
  (void)value;
}

void dunedaq::sspmodules::EmulatedDevice::DeviceSet (unsigned int address, unsigned int mask)
{
  DeviceWriteMask(address, mask, 0xFFFFFFFF);
}

void dunedaq::sspmodules::EmulatedDevice::DeviceClear (unsigned int address, unsigned int mask)
{
  DeviceWriteMask(address, mask, 0x00000000);
}

void dunedaq::sspmodules::EmulatedDevice::DeviceArrayRead (unsigned int address, unsigned int size, unsigned int* data)
{
  (void)address;
  (void)size;
  (void)data;
}

void dunedaq::sspmodules::EmulatedDevice::DeviceArrayWrite (unsigned int address, unsigned int size, unsigned int* data)
{
  (void)address;
  (void)size;
  (void)data;
}

//==============================================================
//Emulator-specific functions
//==============================================================

void dunedaq::sspmodules::EmulatedDevice::Start(){
  //dune::DAQLogger::LogDebug("SSP_EmulatedDevice")<<"Creating emulator thread..."<<std::endl;
  fEmulatorShouldStop=false;
  fEmulatorThread=std::unique_ptr<std::thread>(new std::thread(&dunedaq::sspmodules::EmulatedDevice::EmulatorLoop,this));
}

void dunedaq::sspmodules::EmulatedDevice::Stop(){
  fEmulatorShouldStop=true;
  if(fEmulatorThread){
    fEmulatorThread->join();
    fEmulatorThread.reset();
  }  
}

void dunedaq::sspmodules::EmulatedDevice::EmulatorLoop(){

  //dune::DAQLogger::LogDebug("SSP_EmulatedDevice")<<"Starting emulator loop..."<<std::endl;
  static unsigned int headerSizeInWords=sizeof(dunedaq::detdataformats::EventHeader)/sizeof(unsigned int);

  //We want to generate events on random channels at random times
  std::default_random_engine generator;
  std::exponential_distribution<double> timeDistribution(1./100000.);//10Hz
  std::uniform_int_distribution<int> channelDistribution(0,11);//12 channels

  std::chrono::steady_clock::time_point runStartTime = std::chrono::steady_clock::now();

  //Thread should terminate once "hardware" stop request has been issued
  while(!fEmulatorShouldStop){

    //Wait for random period, then generate event with system timestamp and random channel
    double waitTime = timeDistribution(generator);
    usleep(static_cast<int>(waitTime));
    std::chrono::steady_clock::time_point eventTime = std::chrono::steady_clock::now();
    unsigned long eventTimestamp = (std::chrono::duration_cast<std::chrono::duration<unsigned long,std::ratio<1, 150000000>>>(eventTime - runStartTime)).count(); //150MHz clock // NOLINT(runtime/int)
    int channel = channelDistribution(generator);

    //Build an event header. 
    dunedaq::detdataformats::EventHeader header;

    //Standard header word
    header.header=0xAAAAAAAA;
    //Assign a junk payload of 100 words
    header.length=headerSizeInWords+100;
    //Assign randomly generated channel
    header.group2=channel;
    
    //Set timestamps correctly? Need to figure out better how these are defined
    for(int iWord=1;iWord<=3;++iWord){
      header.timestamp[iWord]=(eventTimestamp>>(iWord)*16)%65536;
    }
    for(int iWord=0;iWord<=2;++iWord){
      header.intTimestamp[iWord+1]=(eventTimestamp>>(iWord)*16)%65536;//First word of intTimestamp is reserved
    }

    //Don't bother with any other fields for now
    header.group1=0x01;
    header.triggerID=0x0;
    header.peakSumLow=0x0;
    header.group3=0x0;
    header.preriseLow=0x0;
    header.group4=0x0;
    header.intSumHigh=0x0;
    header.baseline=0x0;

    for(int iWord=0;iWord<=3;++iWord){
      header.cfdPoint[iWord]=0;
    }

    
    //Push header onto emulated buffer
    unsigned int* headerPtr=(unsigned int*)(&header);
    for(unsigned int element=0;element<headerSizeInWords;++element){
      fEmulatedBuffer.push(headerPtr[element]);
    }

    //Payload contains 100 words; each word is just iWord+channel number
    for(unsigned int iWord=0;iWord<100;++iWord){
      fEmulatedBuffer.push(iWord+channel);
    }
  }
}

#endif // SSPMODULES_SRC_ANLBOARD_EMULATEDDEVICE_CXX_
