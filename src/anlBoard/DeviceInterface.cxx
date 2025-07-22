/**
 * @file DeviceInterface.cxx
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

// FIXME: This code implementation is a mess, with most parameters hardcoded instead of taking them from the configuration
#ifndef SSPMODULES_SRC_ANLBOARD_DEVICEINTERFACE_CXX_
#define SSPMODULES_SRC_ANLBOARD_DEVICEINTERFACE_CXX_


#include "DeviceInterface.hpp"
#include "RegMap.hpp"
#include "SSPIssues.hpp"
#include "anlExceptions.hpp"

#include "boost/asio.hpp"

#include <algorithm>
#include <ctime>
#include <memory>
#include <string>
#include <utility>
#include <vector>

enum
{
  TLVL_ENTER_EXIT_METHODS = 5,
  TLVL_WORK_STEPS = 10,
  TLVL_BOOKKEEPING = 15,
  TLVL_FULL_DEBUG = 63
};

dunedaq::sspmodules::DeviceInterface::DeviceInterface()
  : fDeviceId(0)
  , fState(dunedaq::sspmodules::DeviceInterface::kUninitialized)
  , fUseExternalTimestamp(true)
  , fHardwareClockRateInMHz(128)
  , fDummyPeriod(-1)
  , fSlowControlOnly(false)
  , fPartitionNumber(0)
  , fTimingAddress(0)
  , exception_(false)
{
}

void
dunedaq::sspmodules::DeviceInterface::OpenSlowControl()
{

  TLOG_DEBUG(TLVL_FULL_DEBUG) << "SSP Device Interface OpenSlowControl called.";
  // Ask device manager for a pointer to the specified device
  dunedaq::sspmodules::DeviceManager& devman = dunedaq::sspmodules::DeviceManager::Get();
  dunedaq::sspmodules::Device* device = 0;

  device = devman.OpenDevice(fDeviceId, true); // slow control only

  if (!device) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Unable to get handle to device; giving up!" << std::endl;
    throw(ENoSuchDevice());
  }

  fDevice = device;
  fSlowControlOnly = true;
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface OpenSlowControl completed.";
}

/*
void
dunedaq::sspmodules::DeviceInterface::Initialize(const nlohmann::json& args)
{

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface Initialize called.";
  fState = kInitialized;
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface Initailize complete.";
}

void
dunedaq::sspmodules::DeviceInterface::Stop()
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface Stop called.";

  dunedaq::sspmodules::RegMap& duneReg = dunedaq::sspmodules::RegMap::Get();

  fDevice->DeviceWrite(duneReg.eventDataControl, 0x0013001F);
  fDevice->DeviceClear(duneReg.master_logic_control, 0x00000101);
  // Clear the FIFOs
  fDevice->DeviceWrite(duneReg.fifo_control, 0x08000000);
  fDevice->DeviceWrite(duneReg.PurgeDDR, 0x00000001);
  // Reset the links and flags
  fDevice->DeviceWrite(duneReg.event_data_control, 0x00020001);
  // Flush RX buffer
  fDevice->DevicePurgeData();
  TLOG_DEBUG(TLVL_WORK_STEPS) << "Hardware set to stopped state" << std::endl;
  fState = kStopped;
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface Stop complete.";
}


void
dunedaq::sspmodules::DeviceInterface::Start()
{

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface Start called.";


  TLOG_DEBUG(TLVL_WORK_STEPS) << "Device interface starting hardware run..." << std::endl;
  dunedaq::sspmodules::RegMap& duneReg = dunedaq::sspmodules::RegMap::Get();
  // This script enables all logic and FIFOs and starts data acquisition in the device
  // Operations MUST be performed in this order

  // Load window settings, charge injection settings and bias voltage into channels
  fDevice->DeviceWrite(duneReg.channel_pulsed_control, 0x1);
  fDevice->DeviceWrite(duneReg.bias_control, 0x1);
  fDevice->DeviceWriteMask(duneReg.vmon_control, 0x1, 0x1);
  fDevice->DeviceWriteMask(duneReg.imon_control, 0x1, 0x1);
  fDevice->DeviceWriteMask(duneReg.qi_dac_control, 0x1, 0x1);
  fDevice->DeviceWriteMask(duneReg.qi_pulsed, 0x00030000, 0x00030000);

  fDevice->DeviceWrite(duneReg.event_data_control, 0x00000000);
  // Release the FIFO reset
  fDevice->DeviceWrite(duneReg.fifo_control, 0x00000000);
  // Registers in the Zynq FPGA (Comm)
  // Reset the links and flags (note eventDataControl!=event_data_control)
  fDevice->DeviceWrite(duneReg.eventDataControl, 0x00000000);
  // Registers in the Artix FPGA (DSP)
  // Release master logic reset & enable active channels

  fDevice->DeviceWrite(duneReg.master_logic_control, 0x00000041);
  fState = kRunning;
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface Start complete.";
}

void
dunedaq::sspmodules::DeviceInterface::Shutdown()
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface Shutdown called.";

  fDevice->Close();
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface Shutdown complete.";
}
*/

void
dunedaq::sspmodules::DeviceInterface::SetRegister(unsigned int address, unsigned int value, unsigned int mask)
{

  if (mask == 0xFFFFFFFF) {
    fDevice->DeviceWrite(address, value);
  } else {
    fDevice->DeviceWriteMask(address, mask, value);
  }
}

void
dunedaq::sspmodules::DeviceInterface::SetRegisterArray(unsigned int address, std::vector<unsigned int> value)
{

  this->SetRegisterArray(address, &(value[0]), value.size());
}

void
dunedaq::sspmodules::DeviceInterface::SetRegisterArray(unsigned int address, unsigned int* value, unsigned int size)
{

  fDevice->DeviceArrayWrite(address, size, value);
}

void
dunedaq::sspmodules::DeviceInterface::ReadRegister(unsigned int address, unsigned int& value, unsigned int mask)
{

  if (mask == 0xFFFFFFFF) {
    fDevice->DeviceRead(address, &value);
  } else {
    fDevice->DeviceReadMask(address, mask, &value);
  }
}

void
dunedaq::sspmodules::DeviceInterface::ReadRegisterArray(unsigned int address,
                                                        std::vector<unsigned int>& value,
                                                        unsigned int size)
{

  value.resize(size);
  this->ReadRegisterArray(address, &(value[0]), size);
}

void
dunedaq::sspmodules::DeviceInterface::ReadRegisterArray(unsigned int address, unsigned int* value, unsigned int size)
{

  fDevice->DeviceArrayRead(address, size, value);
}
void
dunedaq::sspmodules::DeviceInterface::SetRegisterByName(std::string name, unsigned int value)
{
  dunedaq::sspmodules::RegMap::Register reg = (dunedaq::sspmodules::RegMap::Get())[name];

  this->SetRegister(reg, value, reg.WriteMask());
}

void
dunedaq::sspmodules::DeviceInterface::SetRegisterElementByName(std::string name, unsigned int index, unsigned int value)
{
  dunedaq::sspmodules::RegMap::Register reg = (dunedaq::sspmodules::RegMap::Get())[name][index];

  this->SetRegister(reg, value, reg.WriteMask());
}

void
dunedaq::sspmodules::DeviceInterface::SetRegisterArrayByName(std::string name, unsigned int value)
{
  unsigned int regSize = (dunedaq::sspmodules::RegMap::Get())[name].Size();
  std::vector<unsigned int> arrayContents(regSize, value);

  this->SetRegisterArrayByName(name, arrayContents);
}

void
dunedaq::sspmodules::DeviceInterface::SetRegisterArrayByName(std::string name, std::vector<unsigned int> values)
{
  dunedaq::sspmodules::RegMap::Register reg = (dunedaq::sspmodules::RegMap::Get())[name];
  if (reg.Size() != values.size()) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Request to set named register array " << name << ", length " << reg.Size()
                                << "with vector of " << values.size() << " values!" << std::endl;
    throw(std::invalid_argument(""));
  }
  this->SetRegisterArray(reg[0], values);
}

void
dunedaq::sspmodules::DeviceInterface::ReadRegisterByName(std::string name, unsigned int& value)
{
  dunedaq::sspmodules::RegMap::Register reg = (dunedaq::sspmodules::RegMap::Get())[name];

  this->ReadRegister(reg, value, reg.ReadMask());
}

void
dunedaq::sspmodules::DeviceInterface::ReadRegisterElementByName(std::string name,
                                                                unsigned int index,
                                                                unsigned int& value)
{
  dunedaq::sspmodules::RegMap::Register reg = (dunedaq::sspmodules::RegMap::Get())[name][index];

  this->ReadRegister(reg, value, reg.ReadMask());
}

void
dunedaq::sspmodules::DeviceInterface::ReadRegisterArrayByName(std::string name, std::vector<unsigned int>& values)
{
  dunedaq::sspmodules::RegMap::Register reg = (dunedaq::sspmodules::RegMap::Get())[name];
  this->ReadRegisterArray(reg[0], values, reg.Size());
}

void
dunedaq::sspmodules::DeviceInterface::ConfigureLEDCalib(const appmodel::SSPLEDCalibModule* conf)
{

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP LED Calib Device Interface Configure called.";

  std::stringstream ss;
  fDeviceId = inet_network(conf->get_board_ip().c_str()); // inet_network("10.73.137.56");
  TLOG() << "DeviceInterface: trying to connect to " << boost::asio::ip::address_v4(fDeviceId).to_string();
  // Ask device manager for a pointer to the specified device
  dunedaq::sspmodules::DeviceManager& devman = dunedaq::sspmodules::DeviceManager::Get();
  dunedaq::sspmodules::Device* device = 0;


  device = devman.OpenDevice(fDeviceId);

  if (!device) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Unable to get handle to device; giving up!" << std::endl;
    throw(ENoSuchDevice());
  }

  fDevice = device;

  // Reset timing endpoint
  dunedaq::sspmodules::RegMap& duneReg = dunedaq::sspmodules::RegMap::Get();

  unsigned int pdts_status = 0;
  unsigned int pdts_control = 0;
  unsigned int dsp_clock_control = 0;

  fDevice->DeviceRead(duneReg.pdts_status, &pdts_status);
  TLOG() << "The pdts_status read back as 0x" << std::hex << pdts_status << std::dec << std::endl;
  fDevice->DeviceRead(duneReg.pdts_control, &pdts_control);
  TLOG() << "The pdts_control read back as 0x" << std::hex << pdts_control << std::dec << std::endl;
  fDevice->DeviceRead(duneReg.dsp_clock_control, &dsp_clock_control);
  TLOG() << "The dsp_clock_control read back as 0x" << std::hex << dsp_clock_control << std::dec << std::endl;

  unsigned int presentTimingAddress = (pdts_control >> 16) & 0xFF;
  unsigned int presentTimingPartition = pdts_control & 0x3;

  TLOG() << "SSP HW presently on partition " << presentTimingPartition << ", address 0x" << std::hex
	 << presentTimingAddress << " with endpoint status 0x" << (pdts_status & 0xF)
	 << " and dsp_clock_control at 0x" << dsp_clock_control << std::dec << std::endl;

  //if ((pdts_status & 0xF) >= 0x6 && (pdts_status & 0xF) <= 0x8 && presentTimingAddress == fTimingAddress &&
  //    presentTimingPartition == fPartitionNumber && dsp_clock_control == 0x31) {
  if ((pdts_status & 0xF) >= 0x6 && (pdts_status & 0xF) <= 0x8 && presentTimingAddress == fTimingAddress &&
      presentTimingPartition == fPartitionNumber && (dsp_clock_control & 0xF) == 0x1) { //NOTE THAT THIS WAS CHANGED SO THAT IF THE DSP_CLOCK_STATUS LOWEST BIT IS STILL HIGH 0x1
    //THEN THE CLOCK ALREADY IS ASSUMED TO BE GOOD, AND WE DON'T TRY TO RESYNCH WITH THE PDTS

    TLOG() << "Clock already looks ok... skipping endpoint reset." << std::endl;
  } else {

    TLOG() << "Syncing SSP LED Calib to PDTS (partition " << fPartitionNumber << ", endpoint address 0x"
	   << std::hex << fTimingAddress << std::dec << ")" << std::endl;

    unsigned int nTries = 0;
    
    while (nTries < 5) {
      fDevice->DeviceWrite(duneReg.dsp_clock_control, 0x30);
      TLOG() << "The dsp_clock_control was set to 0x" << std::hex << 0x30 << std::dec
	     << std::endl; // setting the lowest bit to 0 sets the DSP clock to internal.
      fDevice->DeviceWrite(duneReg.pdts_control, 0x80000000 + fPartitionNumber + fTimingAddress * 0x10000);
      TLOG() << "The pdts_control value was set to 0x" << std::hex << 0x80000000 + fPartitionNumber + fTimingAddress * 0x10000
	     << std::dec << std::endl; // setting the highest bit (0x80000000) to 1 puts the SSP in Reset mode for the PDTS.

      fDevice->DeviceRead(duneReg.pdts_status, &pdts_status);
      TLOG() << "The pdts_status read back as 0x" << std::hex << pdts_status << std::dec
                                  << std::endl;
      fDevice->DeviceRead(duneReg.pdts_control, &pdts_control);
      TLOG() << "The pdts_control read back as 0x" << std::hex << pdts_control << std::dec
	     << std::endl;
      fDevice->DeviceRead(duneReg.dsp_clock_control, &dsp_clock_control);
      TLOG() << "The dsp_clock_control read back as 0x" << std::hex << dsp_clock_control << std::dec
	     << std::endl;

      fDevice->DeviceWrite(duneReg.pdts_control, 0x00000000 + fPartitionNumber + fTimingAddress * 0x10000);
      TLOG() << "The pdts_status value was set to 0x" << std::hex
	     << 0x00000000 + fPartitionNumber + fTimingAddress * 0x10000 << std::dec << std::endl;
      usleep(2000000); // setting the highest bit (0x80000000) to zero puts the SSP in run mode for the PDTS.
      fDevice->DeviceWrite(duneReg.dsp_clock_control,
                           0x31); // setting the lowest bit to 1 sets the DSP clock to external.
      TLOG() << "The dsp_clock_control was set to 0x" << std::hex << 0x31 << std::dec << std::endl;
      usleep(2000000);
      fDevice->DeviceRead(duneReg.pdts_status, &pdts_status);
      TLOG() << "The pdts_status read back as 0x" << std::hex << pdts_status << std::dec
	     << std::endl;
      if ((pdts_status & 0xF) >= 0x6 && (pdts_status & 0xF) <= 0x8)
        break;
      TLOG() << "Timing endpoint sync failed (try " << nTries << ")" << std::endl;
      ++nTries;
    }
    
    if ((pdts_status & 0xF) >= 0x6 && (pdts_status & 0xF) <= 0x8) {
      TLOG() << "The pdts_status value is 0x" << std::hex << pdts_status
	     << " and the 0xF bit masked value is 0x" << (pdts_status & 0xF) << std::dec
	     << std::endl;
      TLOG() << "Timing endpoint synced!" << std::endl;
    } else {
      TLOG() << "The pdts_status value is 0x" << std::hex << pdts_status
	     << " and the 0xF bit masked value is 0x" << (pdts_status & 0xF) << std::dec
	     << std::endl;
      TLOG() << "Giving up on endpoint sync after 5 tries. Value of pdts_status register was 0x"
	     << std::hex << pdts_status << std::dec << std::endl;
    }
  }
  
  TLOG() << "Woke up from 2 seconds of sleep and Waiting for endpoint to reach status 0x8..."
	 << std::endl;
  // Wait until pdts_status reaches exactly 0x8 before resolving.
  if ((pdts_status & 0xF) != 0x8) {
    TLOG() << "Waiting for endpoint to reach status 0x8..." << std::endl;
    TLOG() << "The pdts_status value is 0x" << std::hex << pdts_status
	   << " and the 0xF bit masked value is 0x" << (pdts_status & 0xF) << std::dec << std::endl;
  }
  int nTries = 0;
  while ((pdts_status & 0xF) != 0x8) {
    if (nTries == 2) {
      TLOG() << "Wrong PDTS status!" << std::endl;
      throw DeviceInterfacePDTSStatus(ERS_HERE);
    }
    usleep(2000000);
    TLOG() << "Woke up from 2 seconds of sleep and Waiting for endpoint to reach status 0x8..."
	   << std::endl;
    fDevice->DeviceRead(duneReg.pdts_status, &pdts_status);
    TLOG() << "The pdts_status value is 0x" << std::hex << pdts_status
                                << " and the 0xF bit masked value is 0x" << (pdts_status & 0xF) << std::dec << std::endl;
    nTries++;
  }

  TLOG() << "Endpoint is in running state, continuing with configuration!" << std::endl;
  TLOG() << "SSP LED Calib Device Interface Configured complete.";
} // NOLINT(readability/fn_size)

std::string
dunedaq::sspmodules::DeviceInterface::GetIdentifier()
{
  std::string ident;
  boost::asio::ip::address ip = boost::asio::ip::address_v4(fDeviceId);
  std::string ipString = ip.to_string();
  ident += "(";
  ident += ipString;
  ident += "):";
  return ident;
}
void
dunedaq::sspmodules::DeviceInterface::PrintHardwareState()
{

  TLOG_DEBUG(TLVL_WORK_STEPS) << "===SSP DIAGNOSTIC REGISTERS===" << std::endl;

  dunedaq::sspmodules::RegMap& duneReg = dunedaq::sspmodules::RegMap::Get();
  unsigned int val;

  fDevice->DeviceRead(duneReg.dp_clock_status, &val);
  TLOG_DEBUG(TLVL_WORK_STEPS) << "dsp_clock_status: 0x" << std::hex << val << std::endl;
  fDevice->DeviceRead(duneReg.live_timestamp_msb, &val);
  TLOG_DEBUG(TLVL_WORK_STEPS) << "live_timestamp_msb: " << val << std::endl;
  fDevice->DeviceRead(duneReg.live_timestamp_lsb, &val);
  TLOG_DEBUG(TLVL_WORK_STEPS) << "live_timestamp_lsb: " << val << std::endl;
  fDevice->DeviceRead(duneReg.sync_delay, &val);
  TLOG_DEBUG(TLVL_WORK_STEPS) << "sync_delay: " << val << std::endl;
  fDevice->DeviceRead(duneReg.sync_count, &val);
  TLOG_DEBUG(TLVL_WORK_STEPS) << "sync_count: " << val << std::dec << std::endl;
}

#endif // SSPMODULES_SRC_ANLBOARD_DEVICEINTERFACE_CXX_
