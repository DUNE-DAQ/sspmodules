/**
 * @file RegMap.h
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef SSPMODULES_SRC_ANLBOARD_REGMAP_H_
#define SSPMODULES_SRC_ANLBOARD_REGMAP_H_

#include "dataformats/ssp/SSPTypes.hpp"

#include "anlExceptions.h"

#include <iostream>
//#include "dune-artdaq/DAQLogger/DAQLogger.hh"
#include <map>

namespace dunedaq {
namespace sspmodules {

//Singleton containing human-readable names for SSP registers.
//Note that Zynq registers are in camelCase, and Artix registers
//are spaced_with_underscores.  
class RegMap{
public:

  //Get a reference to the instance of RegMap
  static RegMap& Get();

  class Register{
  public:
    Register(unsigned int address, unsigned int readMask, unsigned int writeMask,
             unsigned int size=1, unsigned int offset=0, unsigned int bits=32):
             fAddress(address),
             fReadMask(readMask),
             fWriteMask(writeMask),
             fSize(size),
             fOffset(offset),
             fBits(bits){}

             Register():
             fAddress(0x00000000),
             fReadMask(0xFFFFFFFF),
             fWriteMask(0xFFFFFFFF),
             fSize(1),
             fOffset(0),
             fBits(32){}

             //Allow implicit conversion to unsigned int for scalar registers
             operator unsigned int(){
      if(fSize>1){
        try {
          //dune::DAQLogger::LogError("SSP_RegMap")<<"Attempt to access SSP register array at "
          // <<std::hex<<fAddress<<std::dec<<" as scalar!"<<std::endl;
        } catch (...) {}
        throw(std::invalid_argument(""));
      }
      return fAddress;
    }

    //Indexing returns another register with correct address offset and size 1
    Register operator[](unsigned int i) const{
      if(i>=fSize){
        try {
          //dune::DAQLogger::LogError("SSP_RegMap")<<"Attempt to access SSP register at "
          // <<std::hex<<fAddress<<std::dec<<" index "<<i
          //<<", beyond end of array (size is "<<fSize<<")"<<std::endl;
        } catch (...) {}
      }
      return Register(fAddress+0x4*i,fReadMask,fWriteMask,fOffset,1);
    }

    //Getters and setters

    inline unsigned int ReadMask() const{
      return fReadMask;
    }

    inline unsigned int WriteMask() const{
      return fWriteMask;
    }

    inline unsigned int Offset() const{
      return fOffset;
    }

    inline unsigned int Bits() const{
      return fBits;
    }

    inline unsigned int Size() const{
      return fSize;
    }

  private:

    //Address of register in SSP space
    unsigned int fAddress;

    //Readable/writable bits in this register for calling code to check
    //that read/write requests make sense
    unsigned int fReadMask;
    unsigned int fWriteMask;

    unsigned int fSize;

    //Bit offset of relevant quantity relative to start of addressed word.
    //Not currently used but we could use this to "virtually" address logical quantities
    //which are assigned only part of a 32-bit word
    unsigned int fOffset;

    //Number of bits assigned to relevant quantity. Not currently used (see above)
    unsigned int fBits;
  };

  //Get registers using variable names...
  Register operator[](std::string name){
    if(fNamed.find(name)==fNamed.end()){
      try {
        //dune::DAQLogger::LogError("SSP_RegMap")<<"Attempt to access named SSP register "<<name
        //<<", which does not exist!"<<std::endl;
      } catch (...) {}
      throw(std::invalid_argument(""));
    }
    return fNamed[name];
  }

  // Registers in the ARM Processor
  unsigned int armStatus;
  unsigned int armError;
  unsigned int armCommand;
  unsigned int armVersion;
  unsigned int armTest[4];
  unsigned int armRxAddress;	// 0x00000020
  unsigned int armRxCommand;	// 0x00000024
  unsigned int armRxSize;		// 0x00000028
  unsigned int armRxStatus;	// 0x0000002C
  unsigned int armTxAddress;	// 0x00000030
  unsigned int armTxCommand;	// 0x00000034
  unsigned int armTxSize;		// 0x00000038
  unsigned int armTxStatus;	// 0x0000003C
  unsigned int armPackets;	// 0x00000040
  unsigned int armOperMode;	// 0x00000044
  unsigned int armOptions;	// 0x00000048
  unsigned int armModemStatus;// 0x0000004C
  unsigned int PurgeDDR;        // 0x00000300

  // Registers in the Zynq FPGA
  unsigned int zynqTest[6];
  unsigned int eventDataInterfaceSelect;
  unsigned int fakeNumEvents;
  unsigned int fakeEventSize;
  unsigned int fakeBaseline;
  unsigned int fakePeakSum;
  unsigned int fakePrerise;
  unsigned int timestamp[2];
  unsigned int codeErrCounts[5];
  unsigned int dispErrCounts[5];
  unsigned int link_rx_status;
  unsigned int eventDataControl;
  unsigned int eventDataPhaseControl;
  unsigned int eventDataPhaseStatus;
  unsigned int c2c_master_status;
  unsigned int c2c_control;
  unsigned int c2c_master_intr_control;
  unsigned int dspStatus;
  unsigned int comm_clock_status;
  unsigned int comm_clock_control;
  unsigned int comm_led_config;
  unsigned int comm_led_input;
  unsigned int eventDataStatus;
  unsigned int qi_dac_control;			
  unsigned int qi_dac_config;			
  
  unsigned int bias_control;			
  unsigned int bias_status;			
  unsigned int bias_config[12];	
  unsigned int bias_readback[12];	
  
  unsigned int vmon_config;			
  unsigned int vmon_select;			
  unsigned int vmon_gpio;			
  unsigned int vmon_config_readback;		
  unsigned int vmon_select_readback;		
  unsigned int vmon_gpio_readback;		
  unsigned int vmon_id_readback;			
  unsigned int vmon_control;			
  unsigned int vmon_status;			
  unsigned int vmon_bias[12];		
  unsigned int vmon_value[9];			

  unsigned int imon_config;			
  unsigned int imon_select;			
  unsigned int imon_gpio;			
  unsigned int imon_config_readback;		
  unsigned int imon_select_readback;		
  unsigned int imon_gpio_readback;		
  unsigned int imon_id_readback;			
  unsigned int imon_control;			
  unsigned int imon_status;			
  unsigned int imon_bias[12];		
  unsigned int imon_value[9];			

  // Registers in the Artix FPGA
  unsigned int board_id;
  unsigned int fifo_control;
  unsigned int dp_fpga_fw_build;
  unsigned int calib_build;
  unsigned int dp_clock_status;
  unsigned int module_id;
  unsigned int c2c_slave_status;
  unsigned int c2c_slave_intr_control;

  unsigned int channel_control[12];
  unsigned int led_threshold[12];
  unsigned int cfd_parameters[12];
  unsigned int readout_pretrigger[12];
  unsigned int readout_window[12];

  unsigned int p_window[12];
  unsigned int i2_window[12];
  unsigned int m1_window[12];
  unsigned int m2_window[12];
  unsigned int d_window[12];
  unsigned int i1_window[12];
  unsigned int disc_width[12];
  unsigned int baseline_start[12];
  unsigned int c_window[12];

  unsigned int trigger_input_delay;
  unsigned int gpio_output_width;
  unsigned int front_panel_config;
  unsigned int channel_pulsed_control;
  unsigned int dsp_led_config;
  unsigned int dsp_led_input;
  unsigned int baseline_delay;
  unsigned int diag_channel_input;
  unsigned int event_data_control;
  unsigned int adc_config;
  unsigned int adc_config_load;
  unsigned int qi_config;
  unsigned int qi_delay;
  unsigned int qi_pulse_width;
  unsigned int qi_pulsed;
  unsigned int external_gate_width;
  unsigned int pdts_cmd_control[3];
  unsigned int lat_timestamp_lsb;
  unsigned int lat_timestamp_msb;
  unsigned int live_timestamp_lsb;
  unsigned int live_timestamp_msb;
  unsigned int sync_period;
  unsigned int sync_delay;
  unsigned int sync_count;
  unsigned int pdts_control;
  unsigned int pdts_status;
  unsigned int pdts_ts_preset[2];
  unsigned int master_logic_control;
  unsigned int master_logic_status;
  unsigned int overflow_status;
  unsigned int phase_value;
  unsigned int link_tx_status;
  unsigned int dsp_clock_control;
  unsigned int dsp_clock_phase_control;
	
  unsigned int code_revision;
  unsigned int code_date;

  unsigned int dropped_event_count[12];
  unsigned int accepted_event_count[12];
  unsigned int ahit_count[12];
  unsigned int disc_count[12];
  unsigned int idelay_count[12];
  unsigned int adc_data_monitor[12];
  unsigned int adc_status[12];

private:
  RegMap(){};
  RegMap(RegMap const&); //Don't implement
  void operator=(RegMap const&); //Don't implement
  std::map<std::string, Register> fNamed;
};

} // namespace sspmodules
} // namespace dunedaq

#endif // SSPMODULES_SRC_ANLBOARD_REGMAP_H_
