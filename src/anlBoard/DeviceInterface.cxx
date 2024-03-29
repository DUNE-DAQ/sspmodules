/**
 * @file DeviceInterface.cxx
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef SSPMODULES_SRC_ANLBOARD_DEVICEINTERFACE_CXX_
#define SSPMODULES_SRC_ANLBOARD_DEVICEINTERFACE_CXX_

#include "appfwk/app/Nljs.hpp"
#include "fdreadoutlibs/SSPFrameTypeAdapter.hpp"
#include "sspmodules/sspledcalibmodule/Nljs.hpp"

#include "DeviceInterface.hpp"
#include "RegMap.hpp"
#include "SSPIssues.hpp"
#include "anlExceptions.hpp"
#include "iomanager/IOManager.hpp"

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

// SSPDAQ::DeviceInterface::DeviceInterface(SSPDAQ::Comm_t commType, unsigned long deviceId)
dunedaq::sspmodules::DeviceInterface::DeviceInterface(dunedaq::fddetdataformats::ssp::Comm_t commType)
  : fCommType(commType)
  , fDeviceId(0)
  , fState(dunedaq::sspmodules::DeviceInterface::kUninitialized)
  , fUseExternalTimestamp(true)
  , fHardwareClockRateInMHz(128)
  , fPreTrigLength(1E8)
  , fPostTrigLength(1E7)
  , fTriggerWriteDelay(1000)
  , fTriggerLatency(0)
  , fTriggerMask(0)
  , fDummyPeriod(-1)
  , fSlowControlOnly(false)
  , fPartitionNumber(0)
  , fTimingAddress(0)
  , exception_(false)
  , fDataThread(0)
{
  //, fRequestReceiver(0){
}

void
dunedaq::sspmodules::DeviceInterface::OpenSlowControl()
{

  TLOG_DEBUG(TLVL_FULL_DEBUG) << "SSP Device Interface OpenSlowControl called.";
  // Ask device manager for a pointer to the specified device
  dunedaq::sspmodules::DeviceManager& devman = dunedaq::sspmodules::DeviceManager::Get();
  dunedaq::sspmodules::Device* device = 0;

  TLOG_DEBUG(TLVL_WORK_STEPS) << "Opening "
                              << ((fCommType == dunedaq::fddetdataformats::ssp::kUSB)
                                    ? "USB"
                                    : ((fCommType == dunedaq::fddetdataformats::ssp::kEthernet) ? "Ethernet" : "Emulated"))
                              << " device #" << fDeviceId << " for slow control only..." << std::endl;

  device = devman.OpenDevice(fCommType, fDeviceId, true);

  if (!device) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Unable to get handle to device; giving up!" << std::endl;
    throw(ENoSuchDevice());
  }

  fDevice = device;
  fSlowControlOnly = true;
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface OpenSlowControl completed.";
}

inline void
tokenize(std::string const& str, const char delim, std::vector<std::string>& out)
{
  std::size_t start;
  std::size_t end = 0;
  while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
    end = str.find(delim, start);
    out.push_back(str.substr(start, end - start));
  }
}

void
dunedaq::sspmodules::DeviceInterface::Initialize(const nlohmann::json& args)
{

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface Initialize called.";

  // init queues here.... I know...
  auto ini = args.get<dunedaq::appfwk::app::ModInit>();
  for (const auto& qi : ini.conn_refs) {
    
      TLOG_DEBUG(TLVL_WORK_STEPS) << ": SSPLEDCalib output is " << qi.name;
      const char delim = '_';
      std::string target = qi.uid;
      std::vector<std::string> words;
      tokenize(target, delim, words);
      int linkid = -1;

      try {
        linkid = std::stoi(words.back());

        m_sink_queues[linkid] = get_iom_sender<dunedaq::fdreadoutlibs::types::SSPFrameTypeAdapter>(qi.uid);
      } catch (const std::exception& ex) {
        TLOG() << "SSP Channel ID could not be parsed on queue instance name!";
        // ers::fatal(InitializationError(ERS_HERE, "SSP Channel ID could not be parsed on queue instance name! "));
      }
    
  }
  fState = kInitialized;
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface Initailize complete.";
}

void
dunedaq::sspmodules::DeviceInterface::Stop()
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface Stop called.";

  if (fState != dunedaq::sspmodules::DeviceInterface::kRunning &&
      fState != dunedaq::sspmodules::DeviceInterface::kUninitialized) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Running stop command for non-running device!" << std::endl;
  }

  if (fState == dunedaq::sspmodules::DeviceInterface::kRunning) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Device interface stopping run" << std::endl;
    fShouldStop = true;
    // if(fRequestReceiver){
    // fRequestReceiver->stop();
    //}
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Signalling read thread to end..." << std::endl;
    fDataThread->join();
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Read thread terminated!" << std::endl;
  }

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

  if (fState == dunedaq::sspmodules::DeviceInterface::kRunning) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "DeviceInterface stop transition complete!" << std::endl;
  }

  fState = dunedaq::sspmodules::DeviceInterface::kStopped;
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface Stop complete.";
}

// void dunedaq::sspmodules::DeviceInterface::StartRequestReceiver(std::string address){
//
//  //dune::DAQLogger::LogInfo("SSP_DeviceInterface")<<
//  TLOG_DEBUG(TLVL_WORK_STEPS) << "Subscribing to software triggers at "<<address<<" and ignoring hardware
//  triggers."<<std::endl; fRequestReceiver=new RequestReceiver(address);
//  }

void
dunedaq::sspmodules::DeviceInterface::Start()
{

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface Start called.";

  if (fState != kStopped) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Attempt to start acquisition on non-stopped device refused!" << std::endl;
    return;
  }

  if (fSlowControlOnly) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Naughty Zoot! Attempt to start run on slow control interface refused!" << std::endl;
    return;
  }

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

  fState = dunedaq::sspmodules::DeviceInterface::kRunning;
  fShouldStop = false;

  TLOG_DEBUG(TLVL_WORK_STEPS) << "Starting read thread..." << std::endl;
  fDataThread = new std::thread(&dunedaq::sspmodules::DeviceInterface::HardwareReadLoop, this);
  TLOG_DEBUG(TLVL_WORK_STEPS) << "Read thread is up!" << std::endl;

  // if(fRequestReceiver){
  // fRequestReceiver->start();
  //}

  TLOG_DEBUG(TLVL_WORK_STEPS) << "Run started!" << std::endl;
  // TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface Start complete.";
}

void
dunedaq::sspmodules::DeviceInterface::HardwareReadLoop()
{

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface HardwareReadLoop called.";

  while (!fShouldStop) {

    //    PrintHardwareState();

    /////////////////////////////////////////////////////////
    // Read an event from SSP. If there is no data, break. //
    /////////////////////////////////////////////////////////

    dunedaq::sspmodules::EventPacket newPacket;
    this->ReadEventFromDevice(newPacket);
    if (newPacket.header.header != 0xAAAAAAAA) {
      usleep(1000);
      continue;
    }

    newPacket.DumpHeader();
    TLOG_DEBUG(TLVL_BOOKKEEPING) << std::endl << " GetTimestamp return value before modification: " << GetTimestamp(newPacket.header)
				<< std::endl;
    unsigned long m_external_packetTime = GetTimestamp(newPacket.header);
    unsigned long m_corrected_external_packetTime = 0;
    m_corrected_external_packetTime = (m_external_packetTime + fFragmentTimestampOffset)/3;
    TLOG_DEBUG(TLVL_BOOKKEEPING) << std::endl << " External timestamp straight from the header: " << m_external_packetTime
				<< std::endl;
    TLOG_DEBUG(TLVL_BOOKKEEPING) << std::endl << " modified external timestamp after removing offset and dividing by 3: " << m_corrected_external_packetTime
				<< std::endl;
    this->SetExternalTimestamp(newPacket.header, m_corrected_external_packetTime);
    newPacket.DumpHeader();
    TLOG_DEBUG(TLVL_BOOKKEEPING) << std::endl << " GetTimestamp return value after modification: " << GetTimestamp(newPacket.header)
				<< std::endl;

    //    unsigned long m_external_packetTime = 0;
    //    for(unsigned int iWord=0;iWord<=3;++iWord){
    //      m_external_packetTime += ((unsigned long)(newPacket.header.timestamp[iWord]))<<16*iWord;
    //    }
    //    unsigned long m_internal_packetTime = 0;
    //    for (unsigned int iWord = 1; iWord <= 3; ++iWord){
    //      m_internal_packetTime += ((unsigned long)(newPacket.header.intTimestamp[iWord]))<<16*(iWord-1);
    //    }
    //
    //    unsigned long m_internal_pretrig_time = m_internal_packetTime - fPreTrigLength;
    //    unsigned long m_internal_posttrig_time = m_internal_packetTime + fPostTrigLength;
    //
    //    TLOG_DEBUG(TLVL_WORK_STEPS) << std::endl << " GetTimestamp return value: " << GetTimestamp(newPacket.header)
    //    << std::endl
    //                                << " external packetTime: " << m_external_packetTime << std::endl
    //				<< " raw internal packetTime: " << m_internal_packetTime
    //                                << " raw internal pretrig Time: " << m_internal_pretrig_time
    //                                << " raw internal posttrig Time: " << m_internal_posttrig_time << std::endl
    //				<< " scaled internal packetTime: " << m_internal_packetTime/3
    //                                << " scaled internal pretrig Time: " << m_internal_pretrig_time/3
    //                                << " scaled internal posttrig Time: " << m_internal_posttrig_time/3;

    TLOG_DEBUG(TLVL_WORK_STEPS) << "HWRead getting mutex..." << std::endl;
    std::unique_lock<std::mutex> mlock(fBufferMutex);
    TLOG_DEBUG(TLVL_WORK_STEPS) << "HWRead got mutex!" << std::endl;

    /////////////////////////////////////////////////////////
    // Push event onto deque.                              //
    /////////////////////////////////////////////////////////

    dunedaq::fdreadoutlibs::types::SSPFrameTypeAdapter sspfs;
    sspfs.header = newPacket.header;
    // memcpy();
    memcpy(sspfs.data, newPacket.data.data(), newPacket.data.size());

    auto chid = ((newPacket.header.group2 & 0x000F) >> 0);
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Getting ready to write newPacket to chid: " << chid << std::endl;
    m_sink_queues[chid]->send(std::move(sspfs), std::chrono::milliseconds(10));
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Guess it wasn't writing to the sink_queues... " << chid << std::endl;

    // fPacketBuffer.emplace_back(std::move(newPacket));

    ///////////////////////////////////////////////////////////////
    // Pass event to trigger finder method. If it contains       //
    // a new trigger then add this to queue of pending triggers. //
    // If trigger start is before end of previous trigger then   //
    // fall over.                                                //
    ///////////////////////////////////////////////////////////////

    //        if(fRequestReceiver){
    //      while(true){
    //	auto t=fRequestReceiver->getNextRequest(0);
    //	if(t.timestamp==0){
    //	  break;
    //	}
    //
    //        auto localTimestamp = t.timestamp*3 - fFragmentTimestampOffset;
    //
    //	SSPDAQ::TriggerInfo newTrigger;
    //
    //	newTrigger.triggerTime=localTimestamp;
    //	newTrigger.startTime=localTimestamp-fPreTrigLength;
    //	newTrigger.endTime=localTimestamp+fPostTrigLength;
    //	newTrigger.triggerType=0xFFFF;
    //	fTriggers.push(newTrigger);
    //      }
    //    }
    //    else{
    //
    //      SSPDAQ::TriggerInfo newTrigger;
    //
    //      if(this->GetTriggerInfo(fPacketBuffer.back(),newTrigger)){
    //	if(fTriggers.size()&&(newTrigger.startTime<fTriggers.back().endTime)){
    //	  //dune::DAQLogger::LogError("SSP_DeviceInterface")<<"Seen trigger with start time overlapping with previous,
    //falling over!"<<std::endl; 	  throw(EEventReadError());
    //	  //	set_exception(true);
    //	  return;
    //	}
    //	fTriggers.push(newTrigger);
    //      }
    //    }
    //
    //    ////////////////////////////////////////////////////////////
    //    //Cull old packets which are not associated with a trigger//
    //    ////////////////////////////////////////////////////////////
    //
    //    unsigned long packetTime = GetTimestamp(fPacketBuffer.back().header);
    //    unsigned long firstInterestingTime;
    //
    //    if (fTriggers.size()){
    //      firstInterestingTime = fTriggers.front().startTime - fTriggerWriteDelay;
    //    } else {
    //      firstInterestingTime = packetTime - fPreTrigLength - fTriggerLatency - fTriggerWriteDelay;
    //    }
    //
    //    auto globalTimestamp = (packetTime + fFragmentTimestampOffset) / 3;
    //
    //
    //    auto frontTS = GetTimestamp(fPacketBuffer.front().header);
    //    auto dropCount = 0;
    //    while ( GetTimestamp(fPacketBuffer.front().header) < firstInterestingTime ) {
    //      fPacketBuffer.pop_front();
    //      dropCount++;
    //      }
    //
    //    TLOG_DEBUG(TLVL_WORK_STEPS) << "packetTime: " << packetTime
    //				<< " firstInterestingTime: " << firstInterestingTime
    //				<< " frontTS: " << frontTS
    //				<< " globalTS: " << globalTimestamp
    //				<< " dropped: " << dropCount;

    TLOG_DEBUG(TLVL_WORK_STEPS) << "HWRead releasing mutex..." << std::endl;
    mlock.unlock();
  }
  TLOG_DEBUG(TLVL_WORK_STEPS) << "HWRead thread ending" << std::endl;
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface HardwareReadLoop complete.";
}

void
dunedaq::sspmodules::DeviceInterface::ReadEvent(std::vector<unsigned int>& fragment)
{

  // TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface ReadEvent called.";

  if (fState != kRunning) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Attempt to get data from non-running device refused!" << std::endl;
    return;
  }

  /////////////////////////////////////////////////////////////////////////
  // Check whether current event timestamp is after end of next trigger. //
  // If so, pass trigger to fragment builder method.                     //
  /////////////////////////////////////////////////////////////////////////

  TLOG_DEBUG(TLVL_WORK_STEPS) << "ReadEvent thread getting mutex..." << std::endl;
  std::unique_lock<std::mutex> mlock(fBufferMutex);

  if (!fTriggers.size()) {
    return;
  }
  TLOG_DEBUG(TLVL_WORK_STEPS) << "ReadEvent thread got mutex!" << std::endl;
  unsigned long packetTime = GetTimestamp(fPacketBuffer.back().header); // NOLINT(runtime/int)

  TLOG_DEBUG(TLVL_WORK_STEPS) << "packetTime: " << packetTime;

  if (packetTime > fTriggers.front().endTime + fTriggerWriteDelay) {
    this->BuildFragment(fTriggers.front(), fragment);
    fTriggers.pop();
  }
  TLOG_DEBUG(TLVL_WORK_STEPS) << "ReadEvent thread releasing mutex..." << std::endl;
  mlock.unlock();

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface ReadEvent complete.";
}

bool
dunedaq::sspmodules::DeviceInterface::GetTriggerInfo(const dunedaq::sspmodules::EventPacket& event,
                                                     dunedaq::sspmodules::TriggerInfo& newTrigger)
{

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface GetTriggerInfo called.";
  static unsigned long currentTriggerTime = 0; // NOLINT(runtime/int)
  static bool channelsSeen[12] = { false };

  static unsigned long lastDummyTrigger = 0; // NOLINT(runtime/int)

  int channel = event.header.group2 & 0x000F;
  unsigned long packetTime = GetTimestamp(event.header); // NOLINT(runtime/int)

  if (fDummyPeriod > 0 && (lastDummyTrigger != packetTime / fDummyPeriod)) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Generating dummy trigger for packet around " << packetTime << "!" << std::endl;
    lastDummyTrigger = packetTime / fDummyPeriod;
    currentTriggerTime = packetTime;
    newTrigger.startTime = currentTriggerTime - fPreTrigLength;
    newTrigger.endTime = currentTriggerTime + fPostTrigLength;
    newTrigger.triggerType = event.header.group1 & 0xFFFF;

    for (unsigned int i = 0; i < 12; ++i) {
      channelsSeen[i] = false;
    }
    channelsSeen[channel] = true;
    return true;
  }

  if (((event.header.group1 & 0xFFFF) & fTriggerMask) != 0) {

    if (!channelsSeen[channel] && packetTime < currentTriggerTime + 1000) {
      channelsSeen[channel] = true;
      TLOG_DEBUG(TLVL_WORK_STEPS)
        << "Packet contains trigger word but this trigger was already generated from another channel" << std::endl;
    } else {
      currentTriggerTime = packetTime;
      newTrigger.triggerTime = currentTriggerTime;
      newTrigger.startTime = currentTriggerTime - fPreTrigLength;
      newTrigger.endTime = currentTriggerTime + fPostTrigLength;
      newTrigger.triggerType = event.header.group1 & 0xFFFF;
      auto globalTimestamp = (packetTime + fFragmentTimestampOffset) / 3;
      TLOG_DEBUG(TLVL_WORK_STEPS) << "Seen packet containing global trigger, timestamp " << packetTime << " / "
                                  << globalTimestamp << std::endl;
      for (unsigned int i = 0; i < 12; ++i) {
        channelsSeen[i] = false;
      }
      channelsSeen[channel] = true;
      return true;
    }
  }
  TLOG_DEBUG(TLVL_WORK_STEPS) << "Packet contains no trigger... trigger logic will ignore it." << std::endl;
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface GetTriggerInfo complete.";

  return false;
}

void
dunedaq::sspmodules::DeviceInterface::BuildFragment(const dunedaq::sspmodules::TriggerInfo& theTrigger,
                                                    std::vector<unsigned int>& fragmentData)
{

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface BuildFragment called.";

  std::vector<dunedaq::sspmodules::EventPacket> eventsToPutBack;
  std::vector<dunedaq::sspmodules::EventPacket*> eventsToWrite;

  auto lastPacket = fPacketBuffer.begin();

  for (auto packetIter = fPacketBuffer.begin(); packetIter != fPacketBuffer.end(); ++packetIter) {
    unsigned long packetTime = GetTimestamp(packetIter->header); // NOLINT(runtime/int)

    TLOG_DEBUG(TLVL_WORK_STEPS) << "packetTime=" << packetTime << " triggerTime=[" << theTrigger.startTime << " to "
                                << theTrigger.endTime << "]";

    if (packetTime >= theTrigger.startTime && packetTime < theTrigger.endTime) {
      lastPacket = packetIter;
    }
  }

  for (auto packetIter = fPacketBuffer.begin();; ++packetIter) {
    unsigned long packetTime = GetTimestamp(packetIter->header); // NOLINT(runtime/int)

    if (packetTime >= theTrigger.startTime) {
      if (packetTime >= theTrigger.endTime) {
        eventsToPutBack.push_back(std::move(*packetIter));
      } else {
        eventsToWrite.push_back(&*packetIter);
      }
    }
    if (packetIter == lastPacket)
      break;
  }

  //=====================================//
  // Calculate required size of millislice//
  //=====================================//

  unsigned int dataSizeInWords = 0;

  dataSizeInWords += dunedaq::fddetdataformats::ssp::MillisliceHeader::sizeInUInts;
  for (auto ev = eventsToWrite.begin(); ev != eventsToWrite.end(); ++ev) {
    dataSizeInWords += (*ev)->header.length;
  }

  //==================//
  // Build slice header//
  //==================//

  dunedaq::fddetdataformats::ssp::MillisliceHeader sliceHeader;
  sliceHeader.length = dataSizeInWords;
  sliceHeader.nTriggers = eventsToWrite.size();
  sliceHeader.startTime = theTrigger.startTime;
  sliceHeader.endTime = theTrigger.endTime;
  sliceHeader.triggerTime = theTrigger.triggerTime;
  sliceHeader.triggerType = theTrigger.triggerType;

  //=================================================//
  // Allocate space for whole slice and fill with data//
  //=================================================//

  fragmentData.resize(dataSizeInWords);

  static unsigned int headerSizeInWords =
    sizeof(dunedaq::fddetdataformats::ssp::EventHeader) / sizeof(unsigned int); // Size of DAQ event header

  // Put millislice header at front of vector
  auto sliceDataPtr = fragmentData.begin();
  unsigned int* millisliceHeaderPtr = static_cast<unsigned int*>(static_cast<void*>(&sliceHeader));
  std::copy(
    millisliceHeaderPtr, millisliceHeaderPtr + dunedaq::fddetdataformats::ssp::MillisliceHeader::sizeInUInts, sliceDataPtr);

  // Fill rest of vector with event data
  sliceDataPtr += dunedaq::fddetdataformats::ssp::MillisliceHeader::sizeInUInts;

  for (auto ev = eventsToWrite.begin(); ev != eventsToWrite.end(); ++ev) {
    // DAQ event header
    unsigned int* headerPtr = static_cast<unsigned int*>(static_cast<void*>(&((*ev)->header)));
    std::copy(headerPtr, headerPtr + headerSizeInWords, sliceDataPtr);

    // DAQ event payload
    sliceDataPtr += headerSizeInWords;
    std::copy((*ev)->data.begin(), (*ev)->data.end(), sliceDataPtr);
    sliceDataPtr += (*ev)->header.length - headerSizeInWords;
  }

  TLOG_DEBUG(TLVL_WORK_STEPS) << "Building fragment with " << eventsToWrite.size() << " packets" << std::endl;

  //=======================//
  // Add millislice to queue//
  //=======================//

  // This log message is too verbose...
  // TLOG_DEBUG(TLVL_WORK_STEPS) << this->GetIdentifier()<<"Pushing slice with "<<events.size()<<" triggers, starting at
  // "<<startTime<<" onto queue!"<<std::endl;
  ++fMillislicesBuilt;

  unsigned int nDropped = 0;

  if (lastPacket == fPacketBuffer.begin()) {
    fPacketBuffer.pop_front();
    ++nDropped;
  } else {
    for (auto packetIter = (fPacketBuffer.begin())++; packetIter != lastPacket; ++packetIter) {
      fPacketBuffer.pop_front();
      ++nDropped;
    }
    fPacketBuffer.pop_front();
    ++nDropped;
  }

  for (auto packetIter = eventsToPutBack.rbegin(); packetIter != eventsToPutBack.rend(); ++packetIter) {
    fPacketBuffer.push_front(std::move(*packetIter));
  }

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface BuildFragment complete.";
}

// void SSPDAQ::DeviceInterface::BuildEmptyMillislice(unsigned long startTime, unsigned long endTime){
//  std::vector<SSPDAQ::EventPacket> emptySlice;
//  this->BuildMillislice(emptySlice,startTime,endTime);
//}

// void SSPDAQ::DeviceInterface::GetMillislice(std::vector<unsigned int>& sliceData){
//  if(fQueue.try_pop(sliceData,std::chrono::microseconds(100000))){
//    ++fMillislicesSent;//Try to pop from queue for 100ms
//    if(!(fMillislicesSent%1000)){
//    dune::DAQLogger::LogDebug("SSP_DeviceInterface")<<this->GetIdentifier()
//		       <<"Interface sending slice "<<fMillislicesSent
//		       <<", total built slices "<<fMillislicesBuilt
//		       <<", current queue length "<<fQueue.size()<<std::endl;
//    }
//  }
//}

void
dunedaq::sspmodules::DeviceInterface::ReadEventFromDevice(EventPacket& event)
{

  // TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface ReadEventFromDevice called.";

  if (fState != kRunning) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Attempt to get data from non-running device refused!" << std::endl;
    event.SetEmpty();
    return;
  }

  std::vector<unsigned int> data;

  unsigned int skippedWords = 0;
  unsigned int firstSkippedWord = 0;

  unsigned int queueLengthInUInts = 0;

  // Find first word in event header (0xAAAAAAAA)
  while (true) {

    fDevice->DeviceQueueStatus(&queueLengthInUInts);
    data.clear();
    if (queueLengthInUInts) {
      fDevice->DeviceReceive(data, 1);
    }

    // If no data is available in pipe then return
    // without filling packet
    if (data.size() == 0) {
      if (skippedWords) {
        TLOG_DEBUG(TLVL_WORK_STEPS) << this->GetIdentifier() << "Warning: GetEvent skipped " << skippedWords
                                    << "words and has not seen header for next event!" << std::endl;
      }
      event.SetEmpty();
      return;
    }

    // Header found - continue reading rest of event
    if (data[0] == 0xAAAAAAAA) {
      break;
    }
    // Unexpected non-header word found - continue to
    // look for header word but need to issue warning
    if (data.size() > 0) {
      if (!skippedWords)
        firstSkippedWord = data[0];
      ++skippedWords;
      TLOG_DEBUG(TLVL_WORK_STEPS) << this->GetIdentifier() << "Warning: GetEvent skipping over word " << data[0] << " (0x"
                                  << std::hex << data[0] << std::dec << ")" << std::endl;
    }
  }

  if (skippedWords) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << this->GetIdentifier() << "Warning: GetEvent skipped " << skippedWords
                                << "words before finding next event header!" << std::endl
                                << "First skipped word was 0x" << std::hex << firstSkippedWord << std::dec << std::endl;
  }

  unsigned int* headerBlock = (unsigned int*)&event.header;
  headerBlock[0] = 0xAAAAAAAA;

  static const unsigned int headerReadSize = (sizeof(dunedaq::fddetdataformats::ssp::EventHeader) / sizeof(unsigned int) - 1);

  // Wait for hardware queue to fill with full header data
  unsigned int timeWaited = 0; // in us

  do {
    fDevice->DeviceQueueStatus(&queueLengthInUInts);
    if (queueLengthInUInts < headerReadSize) {
      usleep(100); // 100us
      TLOG_DEBUG(TLVL_WORK_STEPS) << this->GetIdentifier()
                                  << "Warning: we slept after finding pattern word while waiting for header."
                                  << std::endl;

      timeWaited += 100;
      if (timeWaited > 10000000) { // 10s
        TLOG_DEBUG(TLVL_WORK_STEPS) << this->GetIdentifier()
                                    << "SSP delayed 10s between issuing header word and full header; giving up"
                                    << std::endl;
        event.SetEmpty();
        throw(EEventReadError());
      }
    }
  } while (queueLengthInUInts < headerReadSize);

  // Get header from device and check it is the right length
  fDevice->DeviceReceive(data, headerReadSize);
  if (data.size() != headerReadSize) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << this->GetIdentifier()
                                << "SSP returned truncated header even though FIFO queue is of sufficient length!"
                                << std::endl;
    event.SetEmpty();
    throw(EEventReadError());
  }

  // Copy header into event packet
  std::copy(data.begin(), data.end(), &(headerBlock[1]));

  // Wait for hardware queue to fill with full event data
  unsigned int bodyReadSize = event.header.length - (sizeof(dunedaq::fddetdataformats::ssp::EventHeader) / sizeof(unsigned int));
  queueLengthInUInts = 0;
  timeWaited = 0; // in us

  do {
    fDevice->DeviceQueueStatus(&queueLengthInUInts);
    if (queueLengthInUInts < bodyReadSize) {
      usleep(100); // 100us
      TLOG_DEBUG(TLVL_WORK_STEPS) << this->GetIdentifier()
                                  << "Warning: we slept after finding header before reading full event." << std::endl;
      timeWaited += 100;
      if (timeWaited > 10000000) { // 10s
        TLOG_DEBUG(TLVL_WORK_STEPS) << this->GetIdentifier()
                                    << "SSP delayed 10s between issuing header and full event; giving up" << std::endl;
        event.DumpHeader();
        event.SetEmpty();
        throw(EEventReadError());
      }
    }
  } while (queueLengthInUInts < bodyReadSize);

  // Get event from SSP and check that it is the right length
  fDevice->DeviceReceive(data, bodyReadSize);

  if (data.size() != bodyReadSize) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << this->GetIdentifier()
                                << "SSP returned truncated event even though FIFO queue is of sufficient length!"
                                << std::endl;
    event.SetEmpty();
    throw(EEventReadError());
  }

  // Copy event data into event packet
  event.data = std::move(data);

  auto ehsize = sizeof(struct dunedaq::fddetdataformats::ssp::EventHeader);
  auto ehlength = event.header.length;
  TLOG_DEBUG(TLVL_WORK_STEPS) << "Event data size: " << event.data.size() << " ehsize: " << ehsize
                              << " ehl: " << ehlength;

  // event.DumpHeader();
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface ReadEventFromDevice complete.";

  return;
} // NOLINT(readability/fn_size)

void
dunedaq::sspmodules::DeviceInterface::Shutdown()
{
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface Shutdown called.";

  fDevice->Close();
  fState = kUninitialized;
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP Device Interface Shutdown complete.";
}

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
dunedaq::sspmodules::DeviceInterface::ConfigureLEDCalib(const nlohmann::json& args)
{

  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP LED Calib Device Interface Configure called.";

  if ((fState == kRunning) || (fState == kUninitialized)) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Attempt to reconfigure uninitialized or running device refused!" << std::endl;
    return;
  }

  auto m_cfg = args.get<dunedaq::sspmodules::sspledcalibmodule::Conf>();
  int interfaceTypeCode = m_cfg.interface_type; // dunedaq::detdataformats::kEthernet;
  std::stringstream ss;
  switch (interfaceTypeCode) {
    case 0:
      fCommType = dunedaq::fddetdataformats::ssp::kUSB;
      break;
    case 1:
      fCommType = dunedaq::fddetdataformats::ssp::kEthernet;
      break;
    case 2:
      fCommType = dunedaq::fddetdataformats::ssp::kEmulated;
      break;
    case 999:
      ss << "Error: Invalid interface type set (" << interfaceTypeCode << ")!" << std::endl;
      TLOG() << ss.str();
      throw ConfigurationError(ERS_HERE, ss.str());
    default:
      ss << "Error: Unknown interface type set (" << interfaceTypeCode << ")!" << std::endl;
      TLOG() << ss.str();
      throw ConfigurationError(ERS_HERE, ss.str());
  }
  //
  if (fCommType != dunedaq::fddetdataformats::ssp::kEthernet) {
    fDeviceId = 0;
    std::stringstream ss;
    ss << "Error: Non-functioning interface type set: " << fCommType
       << "so forcing an exit and having none of this USB based SSP crap." << std::endl;
    TLOG() << ss.str();
    throw ConfigurationError(ERS_HERE, ss.str());
  } else {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Board IP is listed as: " << m_cfg.board_ip << std::endl;
    fDeviceId = inet_network(m_cfg.board_ip.c_str()); // inet_network("10.73.137.56");
  }

  // Ask device manager for a pointer to the specified device
  dunedaq::sspmodules::DeviceManager& devman = dunedaq::sspmodules::DeviceManager::Get();
  dunedaq::sspmodules::Device* device = 0;

  TLOG_DEBUG(TLVL_WORK_STEPS) << "Configuring "
                              << ((fCommType == dunedaq::fddetdataformats::ssp::kUSB)
                                    ? "USB"
                                    : ((fCommType == dunedaq::fddetdataformats::ssp::kEthernet) ? "Ethernet" : "Emulated"))
                              << " device #" << fDeviceId << "..." << std::endl;

  device = devman.OpenDevice(fCommType, fDeviceId);

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
  TLOG_DEBUG(TLVL_FULL_DEBUG) << "The pdts_status read back as 0x" << std::hex << pdts_status << std::dec << std::endl;
  fDevice->DeviceRead(duneReg.pdts_control, &pdts_control);
  TLOG_DEBUG(TLVL_FULL_DEBUG) << "The pdts_control read back as 0x" << std::hex << pdts_control << std::dec << std::endl;
  fDevice->DeviceRead(duneReg.dsp_clock_control, &dsp_clock_control);
  TLOG_DEBUG(TLVL_FULL_DEBUG) << "The dsp_clock_control read back as 0x" << std::hex << dsp_clock_control << std::dec
                              << std::endl;

  unsigned int presentTimingAddress = (pdts_control >> 16) & 0xFF;
  unsigned int presentTimingPartition = pdts_control & 0x3;

  TLOG_DEBUG(TLVL_WORK_STEPS) << "SSP HW presently on partition " << presentTimingPartition << ", address 0x" << std::hex
                              << presentTimingAddress << " with endpoint status 0x" << (pdts_status & 0xF)
                              << " and dsp_clock_control at 0x" << dsp_clock_control << std::dec << std::endl;

  //if ((pdts_status & 0xF) >= 0x6 && (pdts_status & 0xF) <= 0x8 && presentTimingAddress == fTimingAddress &&
  //    presentTimingPartition == fPartitionNumber && dsp_clock_control == 0x31) {
  if ((pdts_status & 0xF) >= 0x6 && (pdts_status & 0xF) <= 0x8 && presentTimingAddress == fTimingAddress &&
      presentTimingPartition == fPartitionNumber && (dsp_clock_control & 0xF) == 0x1) { //NOTE THAT THIS WAS CHANGED SO THAT IF THE DSP_CLOCK_STATUS LOWEST BIT IS STILL HIGH 0x1
    //THEN THE CLOCK ALREADY IS ASSUMED TO BE GOOD, AND WE DON'T TRY TO RESYNCH WITH THE PDTS

    TLOG_DEBUG(TLVL_WORK_STEPS) << "Clock already looks ok... skipping endpoint reset." << std::endl;
  } else {

    TLOG_DEBUG(TLVL_WORK_STEPS) << "Syncing SSP LED Calib to PDTS (partition " << fPartitionNumber << ", endpoint address 0x"
                                << std::hex << fTimingAddress << std::dec << ")" << std::endl;

    unsigned int nTries = 0;

    while (nTries < 5) {
      fDevice->DeviceWrite(duneReg.dsp_clock_control, 0x30);
      TLOG_DEBUG(TLVL_FULL_DEBUG) << "The dsp_clock_control was set to 0x" << std::hex << 0x30 << std::dec
                                  << std::endl; // setting the lowest bit to 0 sets the DSP clock to internal.
      fDevice->DeviceWrite(duneReg.pdts_control, 0x80000000 + fPartitionNumber + fTimingAddress * 0x10000);
      TLOG_DEBUG(TLVL_FULL_DEBUG)
        << "The pdts_control value was set to 0x" << std::hex << 0x80000000 + fPartitionNumber + fTimingAddress * 0x10000
        << std::dec << std::endl; // setting the highest bit (0x80000000) to 1 puts the SSP in Reset mode for the PDTS.

      fDevice->DeviceRead(duneReg.pdts_status, &pdts_status);
      TLOG_DEBUG(TLVL_FULL_DEBUG) << "The pdts_status read back as 0x" << std::hex << pdts_status << std::dec
                                  << std::endl;
      fDevice->DeviceRead(duneReg.pdts_control, &pdts_control);
      TLOG_DEBUG(TLVL_FULL_DEBUG) << "The pdts_control read back as 0x" << std::hex << pdts_control << std::dec
                                  << std::endl;
      fDevice->DeviceRead(duneReg.dsp_clock_control, &dsp_clock_control);
      TLOG_DEBUG(TLVL_FULL_DEBUG) << "The dsp_clock_control read back as 0x" << std::hex << dsp_clock_control << std::dec
                                  << std::endl;

      fDevice->DeviceWrite(duneReg.pdts_control, 0x00000000 + fPartitionNumber + fTimingAddress * 0x10000);
      TLOG_DEBUG(TLVL_FULL_DEBUG) << "The pdts_status value was set to 0x" << std::hex
                                  << 0x00000000 + fPartitionNumber + fTimingAddress * 0x10000 << std::dec << std::endl;
      usleep(2000000); // setting the highest bit (0x80000000) to zero puts the SSP in run mode for the PDTS.
      fDevice->DeviceWrite(duneReg.dsp_clock_control,
                           0x31); // setting the lowest bit to 1 sets the DSP clock to external.
      TLOG_DEBUG(TLVL_FULL_DEBUG) << "The dsp_clock_control was set to 0x" << std::hex << 0x31 << std::dec << std::endl;
      usleep(2000000);
      fDevice->DeviceRead(duneReg.pdts_status, &pdts_status);
      TLOG_DEBUG(TLVL_FULL_DEBUG) << "The pdts_status read back as 0x" << std::hex << pdts_status << std::dec
                                  << std::endl;
      if ((pdts_status & 0xF) >= 0x6 && (pdts_status & 0xF) <= 0x8)
        break;
      TLOG_DEBUG(TLVL_WORK_STEPS) << "Timing endpoint sync failed (try " << nTries << ")" << std::endl;
      ++nTries;
    }

    if ((pdts_status & 0xF) >= 0x6 && (pdts_status & 0xF) <= 0x8) {
      TLOG_DEBUG(TLVL_FULL_DEBUG) << "The pdts_status value is 0x" << std::hex << pdts_status
                                  << " and the 0xF bit masked value is 0x" << (pdts_status & 0xF) << std::dec
                                  << std::endl;
      TLOG_DEBUG(TLVL_WORK_STEPS) << "Timing endpoint synced!" << std::endl;
    } else {
      TLOG_DEBUG(TLVL_FULL_DEBUG) << "The pdts_status value is 0x" << std::hex << pdts_status
                                  << " and the 0xF bit masked value is 0x" << (pdts_status & 0xF) << std::dec
                                  << std::endl;
      TLOG_DEBUG(TLVL_WORK_STEPS) << "Giving up on endpoint sync after 5 tries. Value of pdts_status register was 0x"
                                  << std::hex << pdts_status << std::dec << std::endl;
    }
  }

  TLOG_DEBUG(TLVL_WORK_STEPS) << "Woke up from 2 seconds of sleep and Waiting for endpoint to reach status 0x8..."
                              << std::endl;
  // Wait until pdts_status reaches exactly 0x8 before resolving.
  if ((pdts_status & 0xF) != 0x8) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Waiting for endpoint to reach status 0x8..." << std::endl;
    TLOG_DEBUG(TLVL_FULL_DEBUG) << "The pdts_status value is 0x" << std::hex << pdts_status
                                << " and the 0xF bit masked value is 0x" << (pdts_status & 0xF) << std::dec << std::endl;
  }
  while ((pdts_status & 0xF) != 0x8) {
    usleep(2000000);
    TLOG_DEBUG(TLVL_WORK_STEPS) << "Woke up from 2 seconds of sleep and Waiting for endpoint to reach status 0x8..."
                                << std::endl;
    fDevice->DeviceRead(duneReg.pdts_status, &pdts_status);
    TLOG_DEBUG(TLVL_FULL_DEBUG) << "The pdts_status value is 0x" << std::hex << pdts_status
                                << " and the 0xF bit masked value is 0x" << (pdts_status & 0xF) << std::dec << std::endl;
  }

  TLOG_DEBUG(TLVL_WORK_STEPS) << "Endpoint is in running state, continuing with configuration!" << std::endl;
  TLOG_DEBUG(TLVL_ENTER_EXIT_METHODS) << "SSP LED Calib Device Interface Configured complete.";
} // NOLINT(readability/fn_size)

std::string
dunedaq::sspmodules::DeviceInterface::GetIdentifier()
{

  std::string ident;
  ident += "SSP@";
  if (fCommType == dunedaq::fddetdataformats::ssp::kUSB) {
    ident += "(USB";
    ident += fDeviceId;
    ident += "):";
  } else if (fCommType == dunedaq::fddetdataformats::ssp::kEthernet) {
    boost::asio::ip::address ip = boost::asio::ip::address_v4(fDeviceId);
    std::string ipString = ip.to_string();
    ident += "(";
    ident += ipString;
    ident += "):";
  } else if (fCommType == dunedaq::fddetdataformats::ssp::kEmulated) {
    ident += "(EMULATED";
    ident += fDeviceId;
    ident += "):";
  }
  return ident;
}

unsigned long                   // NOLINT(runtime/int)
dunedaq::sspmodules::DeviceInterface::GetTimestamp(const dunedaq::fddetdataformats::ssp::EventHeader& header)
{
  unsigned long packetTime = 0; // NOLINT(runtime/int)
  TLOG_DEBUG(TLVL_WORK_STEPS) << "fUseExternalTimestamp value: " << std::boolalpha << fUseExternalTimestamp
                              << std::endl;
  TLOG_DEBUG(TLVL_WORK_STEPS) << "fPreTrigLength value: " << fPreTrigLength << std::endl;
  if (fUseExternalTimestamp) {
    for (unsigned int iWord = 0; iWord <= 3; ++iWord) {
      packetTime += ((unsigned long)(header.timestamp[iWord])) << 16 * iWord; // NOLINT(runtime/int)
    }
  } else {
    for (unsigned int iWord = 1; iWord <= 3; ++iWord) {
      packetTime += ((unsigned long)(header.intTimestamp[iWord])) << 16 * (iWord - 1); // NOLINT(runtime/int)
    }
  }
  return packetTime;
}

void
dunedaq::sspmodules::DeviceInterface::SetExternalTimestamp(dunedaq::fddetdataformats::ssp::EventHeader& header, unsigned long newtimestamp)
{
  TLOG_DEBUG(TLVL_WORK_STEPS) << "fUseExternalTimestamp value: " << std::boolalpha << fUseExternalTimestamp
                              << std::endl;
  for (unsigned int iWord = 0; iWord <= 3; ++iWord) {
    TLOG_DEBUG(TLVL_WORK_STEPS) << "header.timestamp [" << iWord << "] set to: " << (unsigned short)((newtimestamp >> 16 * iWord) & 0xFFFF) << std::endl;
    header.timestamp[iWord] = (unsigned short)((newtimestamp >> 16 * iWord) & 0xFFFF); // NOLINT(runtime/int)
    //packetTime += ((unsigned long)(header.timestamp[iWord])) << 16 * iWord; // NOLINT(runtime/int)
  }
  return;
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
