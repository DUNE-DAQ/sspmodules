#include "EthernetDevice.h"
#include <cstdlib>
#include <algorithm>
//#include "dune-artdaq/DAQLogger/DAQLogger.hh"
#include "anlExceptions.h"

boost::asio::io_service SSPDAQ::EthernetDevice::fIo_service;

SSPDAQ::EthernetDevice::EthernetDevice(unsigned long ipAddress):
  isOpen(false),
  fCommSocket(fIo_service),fDataSocket(fIo_service),
  fIP(boost::asio::ip::address_v4(ipAddress))
  {}

void SSPDAQ::EthernetDevice::Open(bool slowControlOnly){

  fSlowControlOnly=slowControlOnly;

  //dune::DAQLogger::LogInfo("SSP_EthernetDevice")<<"Looking for SSP Ethernet device at "<<fIP.to_string()<<std::endl;
  boost::asio::ip::tcp::resolver resolver(fIo_service);
  boost::asio::ip::tcp::resolver::query commQuery(fIP.to_string(), slowControlOnly?"55002":"55001");
  boost::asio::ip::tcp::resolver::iterator commEndpointIterator = resolver.resolve(commQuery);
  boost::asio::connect(fCommSocket, commEndpointIterator);

  if(slowControlOnly){
    //dune::DAQLogger::LogInfo("SSP_EthernetDevice")<<"Connected to SSP Ethernet device at "<<fIP.to_string()<<std::endl;
    return;
  }

  boost::asio::ip::tcp::resolver::query dataQuery(fIP.to_string(), "55010");
  boost::asio::ip::tcp::resolver::iterator dataEndpointIterator = resolver.resolve(dataQuery);

  boost::asio::connect(fDataSocket, dataEndpointIterator);

  //Set limited receive buffer size to avoid taxing switch
  //JTH: Remove this since it was causing event read errors. Could try again
  //with a different value if there are more issues which point to switch problems.
  //  boost::asio::socket_base::receive_buffer_size option(16384);
  //  fDataSocket.set_option(option);

  //dune::DAQLogger::LogInfo("SSP_EthernetDevice")<<"Connected to SSP Ethernet device at "<<fIP.to_string()<<std::endl;
}

void SSPDAQ::EthernetDevice::Close(){
  isOpen=false;
  //dune::DAQLogger::LogInfo("SSP_EthernetDevice")<<"Device closed"<<std::endl;
}

void SSPDAQ::EthernetDevice::DevicePurgeComm (void){
  DevicePurge(fCommSocket);
}

void SSPDAQ::EthernetDevice::DevicePurgeData (void){
  DevicePurge(fDataSocket);
}

void SSPDAQ::EthernetDevice::DeviceQueueStatus(unsigned int* numWords){
  unsigned int numBytes=fDataSocket.available();
  (*numWords)=numBytes/sizeof(unsigned int);
}

void SSPDAQ::EthernetDevice::DeviceReceive(std::vector<unsigned int>& data, unsigned int size){
  data.resize(size);
  unsigned int dataReturned=fDataSocket.read_some(boost::asio::buffer(data));
  if(dataReturned<size*sizeof(unsigned int)){
    data.resize(dataReturned/sizeof(unsigned int));
  }
}

//==============================================================================
// Command Functions
//==============================================================================

void SSPDAQ::EthernetDevice::DeviceRead (unsigned int address, unsigned int* value){
 	dunedaq::dataformats::CtrlPacket tx;
	dunedaq::dataformats::CtrlPacket rx;
	unsigned int txSize;
	unsigned int rxSizeExpected;

	tx.header.length	= sizeof(dunedaq::dataformats::CtrlHeader);
	tx.header.address	= address;
	tx.header.command	= dunedaq::dataformats::cmdRead;
	tx.header.size		= 1;
	tx.header.status	= dunedaq::dataformats::statusNoError;
	txSize			= sizeof(dunedaq::dataformats::CtrlHeader);
	rxSizeExpected		= sizeof(dunedaq::dataformats::CtrlHeader) + sizeof(unsigned int);

	SendReceive(tx, rx, txSize, rxSizeExpected, 3);
	*value = rx.data[0];
}

void SSPDAQ::EthernetDevice::DeviceReadMask (unsigned int address, unsigned int mask, unsigned int* value)
{
	dunedaq::dataformats::CtrlPacket tx;
	dunedaq::dataformats::CtrlPacket rx;
	unsigned int txSize;
	unsigned int rxSizeExpected;

	tx.header.length	= sizeof(dunedaq::dataformats::CtrlHeader) + sizeof(uint);
	tx.header.address	= address;
	tx.header.command	= dunedaq::dataformats::cmdReadMask;
	tx.header.size		= 1;
	tx.header.status	= dunedaq::dataformats::statusNoError;
	tx.data[0]		= mask;
	txSize			= sizeof(dunedaq::dataformats::CtrlHeader) + sizeof(unsigned int);
	rxSizeExpected		= sizeof(dunedaq::dataformats::CtrlHeader) + sizeof(unsigned int);

	SendReceive(tx, rx, txSize, rxSizeExpected, 3);
	*value = rx.data[0];
}

void SSPDAQ::EthernetDevice::DeviceWrite (unsigned int address, unsigned int value)
{
	dunedaq::dataformats::CtrlPacket tx;
	dunedaq::dataformats::CtrlPacket rx;
	unsigned int txSize;
	unsigned int rxSizeExpected;

	tx.header.length	= sizeof(dunedaq::dataformats::CtrlHeader) + sizeof(uint);
	tx.header.address	= address;
	tx.header.command	= dunedaq::dataformats::cmdWrite;
	tx.header.size		= 1;
	tx.header.status	= dunedaq::dataformats::statusNoError;
	tx.data[0]		= value;
	txSize			= sizeof(dunedaq::dataformats::CtrlHeader) + sizeof(unsigned int);
	rxSizeExpected		= sizeof(dunedaq::dataformats::CtrlHeader);

	SendReceive(tx, rx, txSize, rxSizeExpected, 3);
}

void SSPDAQ::EthernetDevice::DeviceWriteMask (unsigned int address, unsigned int mask, unsigned int value)
{
	dunedaq::dataformats::CtrlPacket tx;
	dunedaq::dataformats::CtrlPacket rx;
	unsigned int txSize;
	unsigned int rxSizeExpected;

	tx.header.length	= sizeof(dunedaq::dataformats::CtrlHeader) + (sizeof(uint) * 2);
	tx.header.address	= address;
	tx.header.command	= dunedaq::dataformats::cmdWriteMask;
	tx.header.size		= 1;
	tx.header.status	= dunedaq::dataformats::statusNoError;
	tx.data[0]		= mask;
	tx.data[1]		= value;
	txSize			= sizeof(dunedaq::dataformats::CtrlHeader) + (sizeof(unsigned int) * 2);
	rxSizeExpected		= sizeof(dunedaq::dataformats::CtrlHeader) + sizeof(unsigned int);

	SendReceive(tx, rx, txSize, rxSizeExpected, 3);
}

void SSPDAQ::EthernetDevice::DeviceSet (unsigned int address, unsigned int mask)
{
	DeviceWriteMask(address, mask, 0xFFFFFFFF);
}

void SSPDAQ::EthernetDevice::DeviceClear (unsigned int address, unsigned int mask)
{
	DeviceWriteMask(address, mask, 0x00000000);
}

void SSPDAQ::EthernetDevice::DeviceArrayRead (unsigned int address, unsigned int size, unsigned int* data)
{
	unsigned int i = 0;
	dunedaq::dataformats::CtrlPacket tx;
	dunedaq::dataformats::CtrlPacket rx;
	unsigned int txSize;
	unsigned int rxSizeExpected;

	tx.header.length	= sizeof(dunedaq::dataformats::CtrlHeader);
	tx.header.address	= address;
	tx.header.command	= dunedaq::dataformats::cmdArrayRead;
	tx.header.size		= size;
	tx.header.status	= dunedaq::dataformats::statusNoError;
	txSize				= sizeof(dunedaq::dataformats::CtrlHeader);
	rxSizeExpected		= sizeof(dunedaq::dataformats::CtrlHeader) + (sizeof(unsigned int) * size);

	SendReceive(tx, rx, txSize, rxSizeExpected, 3);
	for (i = 0; i < rx.header.size; i++) {
	  data[i] = rx.data[i];
	}
}

void SSPDAQ::EthernetDevice::DeviceArrayWrite (unsigned int address, unsigned int size, unsigned int* data)
{
	unsigned int i = 0;
 	dunedaq::dataformats::CtrlPacket tx;
	dunedaq::dataformats::CtrlPacket rx;
	unsigned int txSize;
	unsigned int rxSizeExpected;

	tx.header.length	= sizeof(dunedaq::dataformats::CtrlHeader) + (sizeof(uint) * size);
	tx.header.address	= address;
	tx.header.command	= dunedaq::dataformats::cmdArrayWrite;
	tx.header.size		= size;
	tx.header.status	= dunedaq::dataformats::statusNoError;
	txSize				= sizeof(dunedaq::dataformats::CtrlHeader) + (sizeof(unsigned int) * size);
	rxSizeExpected		= sizeof(dunedaq::dataformats::CtrlHeader);

	for (i = 0; i < size; i++) {
		tx.data[i] = data[i];
	}

	SendReceive(tx, rx, txSize, rxSizeExpected, 3);
}

//==============================================================================
// Support Functions
//==============================================================================

void SSPDAQ::EthernetDevice::SendReceive(dunedaq::dataformats::CtrlPacket& tx, dunedaq::dataformats::CtrlPacket& rx,
				   unsigned int txSize, unsigned int rxSizeExpected, unsigned int retryCount)
{
  unsigned int timesTried=0;
  bool success=false;

  while(!success){
    try{
      SendEthernet(tx,txSize);
      // Insert small delay between send and receive on Linux
      usleep(100);
      ReceiveEthernet(rx,rxSizeExpected);
      usleep(2000);
      success=true;
    }
    catch(ETCPError&){
      if(timesTried<retryCount){
	DevicePurgeComm();
	++timesTried;
	//dune::DAQLogger::LogWarning("SSP_EthernetDevice")<<"Send/receive failed "<<timesTried<<" times on Ethernet link, retrying..."<<std::endl;
      }
      else{
	try {
	  //dune::DAQLogger::LogError("SSP_EthernetDevice")<<"Send/receive failed on Ethernet link, giving up."<<std::endl;
	} catch (...) {}
	throw;
      }
    }
  }
}

void SSPDAQ::EthernetDevice::SendEthernet(dunedaq::dataformats::CtrlPacket& tx, unsigned int txSize)
{
  unsigned int txSizeWritten=fCommSocket.write_some(boost::asio::buffer((void*)(&tx),txSize));
  if(txSizeWritten!=txSize){
    throw(ETCPError(""));
  }

}

void SSPDAQ::EthernetDevice::ReceiveEthernet(dunedaq::dataformats::CtrlPacket& rx, unsigned int rxSizeExpected)
{
  unsigned int rxSizeReturned=fCommSocket.read_some(boost::asio::buffer((void*)(&rx),rxSizeExpected));
  if(rxSizeReturned!=rxSizeExpected){
    throw(ETCPError(""));
  }
}

void SSPDAQ::EthernetDevice::DevicePurge(boost::asio::ip::tcp::socket& socket){
  bool done = false;
  unsigned int bytesQueued = 0;
  unsigned int sleepTime = 0;

  //Keep getting data from channel until queue is empty
  do{
    bytesQueued=socket.available();

    //Read data from device, up to 256 bytes
    if(bytesQueued!=0){
      sleepTime=0;
      unsigned int bytesToGet=std::min((unsigned int)256,bytesQueued);
      std::vector<char> junkBuf(bytesToGet);
      socket.read_some(boost::asio::buffer(junkBuf,bytesToGet));
    }
    //If queue is empty, wait a bit and check that it hasn't filled up again, then return
    else{
      usleep(1000);	// 1ms
      sleepTime+=1000;
      bytesQueued=socket.available();
      if (bytesQueued == 0&&sleepTime>1000000) {
	done = 1;
      }
    }
  }
  while (!done);

}
