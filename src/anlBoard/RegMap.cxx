/**
 * @file RegMap.cxx
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef SSPMODULES_SRC_ANLBOARD_REGMAP_CXX_
#define SSPMODULES_SRC_ANLBOARD_REGMAP_CXX_

#include "RegMap.hpp"

dunedaq::sspmodules::RegMap&
dunedaq::sspmodules::RegMap::Get(void)
{

  static dunedaq::sspmodules::RegMap* instance = nullptr;

  if (!instance) {
    instance = new dunedaq::sspmodules::RegMap;
    // NOTE: All comments about default values, read masks, and write masks are current as of 2/11/2014
    // clang-format off
    // Registers in the Zynq ARM        Address                         Address         Default Value   Read Mask               Write Mask              Code Name
    instance->armStatus                         = 0x00000000;   //      0x0000,         0xABCDEF01              0xFFFFFFFF              0x00000000              rregStatus             
    instance->armError                          = 0x00000004;   //      0x0004,         0xEF012345              0xFFFFFFFF              0x00000000              regError               
    instance->armCommand                        = 0x00000008;   //      0x0008,         0x00000000              0xFFFFFFFF              0xFFFFFFFF              regCommand             
    instance->armVersion                        = 0x0000000C;   //      0x000C,         0x00000001              0xFFFFFFFF              0x00000000              regVersion             
    instance->armTest[0]                        = 0x00000010;   //      0x0010,         0x12345678              0xFFFFFFFF              0xFFFFFFFF              regTest0               
    instance->armTest[1]                        = 0x00000014;   //      0x0014,         0x567890AB              0xFFFFFFFF              0xFFFFFFFF              regTest1               
    instance->armTest[2]                        = 0x00000018;   //      0x0018,         0x90ABCDEF              0xFFFFFFFF              0xFFFFFFFF              regTest2               
    instance->armTest[3]                        = 0x0000001C;   //      0x001C,         0xCDEF1234              0xFFFFFFFF              0xFFFFFFFF              regTest3               
    instance->armRxAddress                      = 0x00000020;   //      0x0020,         0xFFFFFFF0              0xFFFFFFFF              0x00000000              regRxAddress           
    instance->armRxCommand                      = 0x00000024;   //      0x0024,         0xFFFFFFF1              0xFFFFFFFF              0x00000000              regRxCommand           
    instance->armRxSize                         = 0x00000028;   //      0x0028,         0xFFFFFFF2              0xFFFFFFFF              0x00000000              regRxSize              
    instance->armRxStatus                       = 0x0000002C;   //      0x002C,         0xFFFFFFF3              0xFFFFFFFF              0x00000000              regRxStatus            
    instance->armTxAddress                      = 0x00000030;   //      0x0030,         0xFFFFFFF4              0xFFFFFFFF              0x00000000              regTxAddress           
    instance->armTxCommand                      = 0x00000034;   //      0x0034,         0xFFFFFFF5              0xFFFFFFFF              0x00000000              regTxCommand           
    instance->armTxSize                         = 0x00000038;   //      0x0038,         0xFFFFFFF6              0xFFFFFFFF              0x00000000              regTxSize              
    instance->armTxStatus                       = 0x0000003C;   //      0x003C,         0xFFFFFFF7              0xFFFFFFFF              0x00000000              regTxStatus            
    instance->armPackets                        = 0x00000040;   //      0x0040,         0x00000000              0xFFFFFFFF              0x00000000              regPackets             
    instance->armOperMode                       = 0x00000044;   //      0x0044,         0x00000000              0xFFFFFFFF              0x00000000              regOperMode            
    instance->armOptions                        = 0x00000048;   //      0x0048,         0x00000000              0xFFFFFFFF              0x00000000              regOptions             
    instance->armModemStatus                    = 0x0000004C;   //      0x004C,         0x00000000              0xFFFFFFFF              0x00000000              regModemStatus         
    instance->PurgeDDR                          = 0x00000300;   //      0x0300,         0x00000000              0x00000001              0x00000001

    // Registers in the Zynq FPGA       Address                         Address         Default Value   Read Mask               Write Mask              VHDL Name                      
    instance->zynqTest[0]                       = 0x40000000;   //      X"000",         X"33333333",    X"FFFFFFFF",    X"00000000",    regin_test_0                                   
    instance->zynqTest[1]                       = 0x40000004;   //      X"004",         X"44444444",    X"FFFFFFFF",    X"00000000",    unnamed test register                          
    instance->zynqTest[2]                       = 0x40000008;   //      X"008",         X"55555555",    X"FFFFFFFF",    X"FFFF0000",    unnamed test register                          
    instance->zynqTest[3]                       = 0x4000000C;   //      X"00C",         X"66666666",    X"FFFFFFFF",    X"0000FFFF",    unnamed test register                          
    instance->zynqTest[4]                       = 0x40000010;   //      X"010",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_test_1                                     
    instance->zynqTest[5]                       = 0x40000014;   //      X"014",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    unnamed test register                          
    instance->eventDataInterfaceSelect          = 0x40000020;   //      X"020",         X"00000000",    X"000000F8",    X"FFFFFFFF",    reg_fake_control                               
    instance->fakeNumEvents                     = 0x40000024;   //      X"024",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_fake_num_events                            
    instance->fakeEventSize                     = 0x40000028;   //      X"028",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_fake_event_size                            
    instance->fakeBaseline                      = 0x4000002C;   //      X"02C",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_fake_baseline                              
    instance->fakePeakSum                       = 0x40000030;   //      X"030",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_fake_peak_sum                              
    instance->fakePrerise                       = 0x40000034;   //      X"034",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_fake_prerise                               
    instance->timestamp[0]                      = 0x40000038;   //      X"038",         X"00000000",    X"FFFFFFFF",    X"00000000",    regin_timestamp (low bits)                     
    instance->timestamp[1]                      = 0x4000003C;   //      X"03C",         X"00000000",    X"FFFFFFFF",    X"00000000",    regin_timestamp (high bits);                   
    instance->codeErrCounts[0]                  = 0x40000100;   //      X"100",         X"00000000",    X"FFFFFFFF",    X"00000000",    regin_code_err_counts(0);                      
    instance->codeErrCounts[1]                  = 0x40000104;   //      X"104",         X"00000000",    X"FFFFFFFF",    X"00000000",    regin_code_err_counts(1);                      
    instance->codeErrCounts[2]                  = 0x40000108;   //      X"108",         X"00000000",    X"FFFFFFFF",    X"00000000",    regin_code_err_counts(2);                      
    instance->codeErrCounts[3]                  = 0x4000010C;   //      X"10C",         X"00000000",    X"FFFFFFFF",    X"00000000",    regin_code_err_counts(3);                      
    instance->codeErrCounts[4]                  = 0x40000110;   //      X"110",         X"00000000",    X"FFFFFFFF",    X"00000000",    regin_code_err_counts(4);                      
    instance->dispErrCounts[0]                  = 0x40000120;   //      X"120",         X"00000000",    X"FFFFFFFF",    X"00000000",    regin_disp_err_counts(0);                      
    instance->dispErrCounts[1]                  = 0x40000124;   //      X"124",         X"00000000",    X"FFFFFFFF",    X"00000000",    regin_disp_err_counts(1);                      
    instance->dispErrCounts[2]                  = 0x40000128;   //      X"128",         X"00000000",    X"FFFFFFFF",    X"00000000",    regin_disp_err_counts(2)                       
    instance->dispErrCounts[3]                  = 0x4000012C;   //      X"12C",         X"00000000",    X"FFFFFFFF",    X"00000000",    regin_disp_err_counts(3)                       
    instance->dispErrCounts[4]                  = 0x40000130;   //      X"130",         X"00000000",    X"FFFFFFFF",    X"00000000",    regin_disp_err_counts(4);                      
    instance->link_rx_status                    = 0x40000140;   //      X"140",         X"00000000",    X"FFFFFFFF",    X"00000000",    regin_link_status;                             
    instance->eventDataControl                  = 0x40000144;   //      X"144",         X"0020001F",    X"FFFFFFFF",    X"0033001F",    reg_event_data_control;                        
    instance->eventDataPhaseControl             = 0x40000148;   //      X"148",         X"00000000",    X"00000000",    X"00000003",    reg_event_data_phase_control;                  
    instance->eventDataPhaseStatus              = 0x4000014C;   //      X"14C",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_event_data_phase_status;                   
    instance->c2c_master_status                 = 0x40000150;   //      X"150",         X"00000000",    X"FFFFFFFF",    X"00000000",    regin_c2c_status;                              
    instance->c2c_control                        = 0x40000154;   //      X"154",         X"00000007",    X"FFFFFFFF",    X"00000007",    reg_c2c_control;                               
    instance->c2c_master_intr_control            = 0x40000158;   //      X"158",         X"00000000",    X"FFFFFFFF",    X"0000000F",    reg_c2c_intr_control;                          
    instance->dspStatus                         = 0x40000160;   //      X"160",         X"00000000",    X"FFFFFFFF",    X"00000000",    regin_dsp_status                               
    instance->comm_clock_status                       = 0x40000170;   //      X"170",         X"00000000",    X"FFFFFFFF",    X"00000000",    regin_clock_status;                            
    instance->comm_clock_control                      = 0x40000174;   //      X"174",         X"00000001",    X"FFFFFFFF",    X"00000001",    reg_clock_control                              
    instance->comm_led_config                         = 0x40000180;   //      X"180",         X"00000000",    X"FFFFFFFF",    X"00000003",    reg_led_config                                 
    instance->comm_led_input                          = 0x40000184;   //      X"184",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_led_input                                  
    instance->eventDataStatus                   = 0x40000190;   //      X"190",         X"00000000",    X"FFFFFFFF",    X"00000000",    regin_event_data_status                        
    instance->qi_dac_control			= 0x40000200;	//	X"200",		X"00000000",	X"00000000",	X"00000001",	reg_qi_dac_control
    instance->qi_dac_config			= 0x40000204;	//	X"204",		X"00008000",	X"0003FFFF",	X"0003FFFF",	reg_qi_dac_config
	
    instance->bias_control			= 0x40000300;	//	X"300",		X"00000000",	X"00000000",	X"00000001",	reg_bias_control
    instance->bias_status				= 0x40000304;	//	X"304",		X"00000000",	X"00000FFF",	X"00000000",	regin_bias_status

    instance->bias_config[0]			= 0x40000340;	//	X"340",		X"00000000",	X"FFFFFFFF",	X"FFFFFFFF",	reg_bias_dac_config(0)
    instance->bias_config[1]			= 0x40000344;	//	X"344",		X"00000000",	X"FFFFFFFF",	X"FFFFFFFF",	reg_bias_dac_config(1)
    instance->bias_config[2]			= 0x40000348;	//	X"348",		X"00000000",	X"FFFFFFFF",	X"FFFFFFFF",	reg_bias_dac_config(2)
    instance->bias_config[3]			= 0x4000034C;	//	X"34C",		X"00000000",	X"FFFFFFFF",	X"FFFFFFFF",	reg_bias_dac_config(3)
    instance->bias_config[4]			= 0x40000350;	//	X"350",		X"00000000",	X"FFFFFFFF",	X"FFFFFFFF",	reg_bias_dac_config(4)
    instance->bias_config[5]			= 0x40000354;	//	X"354",		X"00000000",	X"FFFFFFFF",	X"FFFFFFFF",	reg_bias_dac_config(5)
    instance->bias_config[6]			= 0x40000358;	//	X"358",		X"00000000",	X"FFFFFFFF",	X"FFFFFFFF",	reg_bias_dac_config(6)
    instance->bias_config[7]			= 0x4000035C;	//	X"35C",		X"00000000",	X"FFFFFFFF",	X"FFFFFFFF",	reg_bias_dac_config(7)
    instance->bias_config[8]			= 0x40000360;	//	X"360",		X"00000000",	X"FFFFFFFF",	X"FFFFFFFF",	reg_bias_dac_config(8)
    instance->bias_config[9]			= 0x40000364;	//	X"364",		X"00000000",	X"FFFFFFFF",	X"FFFFFFFF",	reg_bias_dac_config(9)
    instance->bias_config[10]			= 0x40000368;	//	X"368",		X"00000000",	X"FFFFFFFF",	X"FFFFFFFF",	reg_bias_dac_config(10)
    instance->bias_config[11]			= 0x4000036C;	//	X"36C",		X"00000000",	X"FFFFFFFF",	X"FFFFFFFF",	reg_bias_dac_config(11)

    instance->bias_readback[0]		= 0x40000380;	//	X"380",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_bias_dac_readback(0)
    instance->bias_readback[1]		= 0x40000384;	//	X"384",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_bias_dac_readback(1)
    instance->bias_readback[2]		= 0x40000388;	//	X"388",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_bias_dac_readback(2)
    instance->bias_readback[3]		= 0x4000038C;	//	X"38C",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_bias_dac_readback(3)
    instance->bias_readback[4]		= 0x40000390;	//	X"390",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_bias_dac_readback(4)
    instance->bias_readback[5]		= 0x40000394;	//	X"394",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_bias_dac_readback(5)
    instance->bias_readback[6]		= 0x40000398;	//	X"398",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_bias_dac_readback(6)
    instance->bias_readback[7]		= 0x4000039C;	//	X"39C",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_bias_dac_readback(7)
    instance->bias_readback[8]		= 0x400003A0;	//	X"3A0",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_bias_dac_readback(8)
    instance->bias_readback[9]		= 0x400003A4;	//	X"3A4",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_bias_dac_readback(9)
    instance->bias_readback[10]		= 0x400003A8;	//	X"3A8",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_bias_dac_readback(10)
    instance->bias_readback[11]		= 0x400003AC;	//	X"3AC",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_bias_dac_readback(11)

    instance->vmon_config				= 0x40000400;	//	X"400",		X"0012F000",	X"00FFFFFF",	X"00FFFFFF",	reg_vmon_config
    instance->vmon_select				= 0x40000404;	//	X"404",		X"00FFFF00",	X"FFFFFFFF",	X"FFFFFFFF",	reg_vmon_select
    instance->vmon_gpio				= 0x40000408;	//	X"408",		X"00000000",	X"0000FFFF",	X"0000FFFF",	reg_vmon_gpio
    instance->vmon_config_readback		= 0x40000410;	//	X"410",		X"00000000",	X"00FFFFFF",	X"00000000",	regin_vmon_config_readback
    instance->vmon_select_readback		= 0x40000414;	//	X"414",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_select_readback
    instance->vmon_gpio_readback		= 0x40000418;	//	X"418",		X"00000000",	X"0000FFFF",	X"00000000",	regin_vmon_gpio_readback
    instance->vmon_id_readback			= 0x4000041C;	//	X"41C",		X"00000000",	X"000000FF",	X"00000000",	regin_vmon_id_readback
    instance->vmon_control				= 0x40000420;	//	X"420",		X"00000000",	X"00010100",	X"00010001",	reg_vmon_control & regin_vmon_control
    instance->vmon_status				= 0x40000424;	//	X"424",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_status
	
    instance->vmon_bias[0]				= 0x40000440;	//	X"440",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_bias(0)
    instance->vmon_bias[1]				= 0x40000444;	//	X"444",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_bias(1)
    instance->vmon_bias[2]				= 0x40000448;	//	X"448",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_bias(2)
    instance->vmon_bias[3]				= 0x4000044C;	//	X"44C",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_bias(3)
    instance->vmon_bias[4]				= 0x40000450;	//	X"450",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_bias(4)
    instance->vmon_bias[5]				= 0x40000454;	//	X"454",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_bias(5)
    instance->vmon_bias[6]				= 0x40000458;	//	X"458",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_bias(6)
    instance->vmon_bias[7]				= 0x4000045C;	//	X"45C",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_bias(7)
    instance->vmon_bias[8]				= 0x40000460;	//	X"460",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_bias(8)
    instance->vmon_bias[9]				= 0x40000464;	//	X"464",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_bias(9)
    instance->vmon_bias[10]			= 0x40000468;	//	X"468",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_bias(10)
    instance->vmon_bias[11]			= 0x4000046C;	//	X"46C",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_bias(11)

    instance->vmon_value[0]			= 0x40000480;	//	X"480",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_value(0)
    instance->vmon_value[1]			= 0x40000484;	//	X"484",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_value(1)
    instance->vmon_value[2]			= 0x40000488;	//	X"488",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_value(2)
    instance->vmon_value[3]			= 0x4000048C;	//	X"48C",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_value(3)
    instance->vmon_value[4]			= 0x40000490;	//	X"490",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_value(4)
    instance->vmon_value[5]			= 0x40000494;	//	X"494",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_value(5)
    instance->vmon_value[6]			= 0x40000498;	//	X"498",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_value(6)
    instance->vmon_value[7]			= 0x4000049C;	//	X"49C",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_value(7)
    instance->vmon_value[8]			= 0x400004A0;	//	X"4A0",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_vmon_value(8)


    instance->imon_config				= 0x40000500;	//	X"400",		X"0012F000",	X"00FFFFFF",	X"00FFFFFF",	reg_imon_config
    instance->imon_select				= 0x40000504;	//	X"404",		X"00FFFF00",	X"FFFFFFFF",	X"FFFFFFFF",	reg_imon_select
    instance->imon_gpio				= 0x40000508;	//	X"408",		X"00000000",	X"0000FFFF",	X"0000FFFF",	reg_imon_gpio
    instance->imon_config_readback		= 0x40000510;	//	X"410",		X"00000000",	X"00FFFFFF",	X"00000000",	regin_imon_config_readback
    instance->imon_select_readback		= 0x40000514;	//	X"414",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_select_readback
    instance->imon_gpio_readback		= 0x40000518;	//	X"418",		X"00000000",	X"0000FFFF",	X"00000000",	regin_imon_gpio_readback
    instance->imon_id_readback			= 0x4000051C;	//	X"41C",		X"00000000",	X"000000FF",	X"00000000",	regin_imon_id_readback
    instance->imon_control				= 0x40000520;	//	X"420",		X"00000000",	X"00010100",	X"00010001",	reg_imon_control & regin_imon_control
    instance->imon_status				= 0x40000524;	//	X"424",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_status
	
    instance->imon_bias[0]				= 0x40000540;	//	X"440",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_bias(0)
    instance->imon_bias[1]				= 0x40000544;	//	X"444",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_bias(1)
    instance->imon_bias[2]				= 0x40000548;	//	X"448",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_bias(2)
    instance->imon_bias[3]				= 0x4000054C;	//	X"44C",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_bias(3)
    instance->imon_bias[4]				= 0x40000550;	//	X"450",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_bias(4)
    instance->imon_bias[5]				= 0x40000554;	//	X"454",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_bias(5)
    instance->imon_bias[6]				= 0x40000558;	//	X"458",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_bias(6)
    instance->imon_bias[7]				= 0x4000055C;	//	X"45C",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_bias(7)
    instance->imon_bias[8]				= 0x40000560;	//	X"460",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_bias(8)
    instance->imon_bias[9]				= 0x40000564;	//	X"464",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_bias(9)
    instance->imon_bias[10]			= 0x40000568;	//	X"468",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_bias(10)
    instance->imon_bias[11]			= 0x4000056C;	//	X"46C",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_bias(11)

    instance->imon_value[0]			= 0x40000580;	//	X"480",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_value(0)
    instance->imon_value[1]			= 0x40000584;	//	X"484",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_value(1)
    instance->imon_value[2]			= 0x40000588;	//	X"488",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_value(2)
    instance->imon_value[3]			= 0x4000058C;	//	X"48C",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_value(3)
    instance->imon_value[4]			= 0x40000590;	//	X"490",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_value(4)
    instance->imon_value[5]			= 0x40000594;	//	X"494",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_value(5)
    instance->imon_value[6]			= 0x40000598;	//	X"498",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_value(6)
    instance->imon_value[7]			= 0x4000059C;	//	X"49C",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_value(7)
    instance->imon_value[8]			= 0x400005A0;	//	X"4A0",		X"00000000",	X"FFFFFFFF",	X"00000000",	regin_imon_value(8)


    // Registers in the Artix FPGA      Address                         Default Value   Read Mask               Write Mask              VHDL Name                                      
    instance->board_id                          = 0x80000000;   //      X"000",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_board_id                                   
    instance->fifo_control                      = 0x80000004;   //      X"004",         X"00000000",    X"0FFFFFFF",    X"08000000",    reg_fifo_control                               
    instance->dp_fpga_fw_build                  = 0x80000010;   //      X"004",         X"00000000",    X"0FFFFFFF",    X"08000000",    reg_dp_fpga_build                               
    instance->calib_build                       = 0x80000014;   //      X"004",         X"00000000",    X"0FFFFFFF",    X"08000000",    reg_calib_build                               
    instance->dp_clock_status			= 0x80000020;	//	X"020",		X"00000000",	X"FFFFFFFF",	X"00000000",	reg_dp_clock_status
    instance->module_id                         = 0x80000024;   //      X"024",         X"00000000",    X"00000FFF",    X"00000FFF",    reg_module_id                                  
    instance->c2c_slave_status                  = 0x80000030;   //      X"030",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_c2c_status                                 
    instance->c2c_slave_intr_control            = 0x80000034;   //      X"034",         X"00000000",    X"FFFFFFFF",    X"0000000F",    reg_c2c_intr_control                           
                                                                                                                                                                                       
    instance->channel_control[0]                 = 0x80000040;   //      X"040",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_channel_control(0)                          
    instance->channel_control[1]                 = 0x80000044;   //      X"044",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_channel_control(1)                          
    instance->channel_control[2]                 = 0x80000048;   //      X"048",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_channel_control(2)                          
    instance->channel_control[3]                 = 0x8000004C;   //      X"04C",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_channel_control(3)                          
    instance->channel_control[4]                 = 0x80000050;   //      X"050",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_channel_control(4)                          
    instance->channel_control[5]                 = 0x80000054;   //      X"054",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_channel_control(5)                          
    instance->channel_control[6]                 = 0x80000058;   //      X"058",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_channel_control(6)                          
    instance->channel_control[7]                 = 0x8000005C;   //      X"05C",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_channel_control(7)                          
    instance->channel_control[8]                 = 0x80000060;   //      X"060",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_channel_control(8)                          
    instance->channel_control[9]                 = 0x80000064;   //      X"064",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_channel_control(9)                          
    instance->channel_control[10]                = 0x80000068;   //      X"068",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_channel_control(10)                         
    instance->channel_control[11]                = 0x8000006C;   //      X"06C",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_channel_control(11)                         
                                                                                                                                                                                       
    instance->led_threshold[0]                  = 0x80000080;   //      X"080",         X"00000064",    X"00FFFFFF",    X"00FFFFFF",    reg_led_threshold(0)                           
    instance->led_threshold[1]                  = 0x80000084;   //      X"084",         X"00000064",    X"00FFFFFF",    X"00FFFFFF",    reg_led_threshold(1)                           
    instance->led_threshold[2]                  = 0x80000088;   //      X"088",         X"00000064",    X"00FFFFFF",    X"00FFFFFF",    reg_led_threshold(2)                           
    instance->led_threshold[3]                  = 0x8000008C;   //      X"08C",         X"00000064",    X"00FFFFFF",    X"00FFFFFF",    reg_led_threshold(3)                           
    instance->led_threshold[4]                  = 0x80000090;   //      X"090",         X"00000064",    X"00FFFFFF",    X"00FFFFFF",    reg_led_threshold(4)                           
    instance->led_threshold[5]                  = 0x80000094;   //      X"094",         X"00000064",    X"00FFFFFF",    X"00FFFFFF",    reg_led_threshold(5)                           
    instance->led_threshold[6]                  = 0x80000098;   //      X"098",         X"00000064",    X"00FFFFFF",    X"00FFFFFF",    reg_led_threshold(6)                           
    instance->led_threshold[7]                  = 0x8000009C;   //      X"09C",         X"00000064",    X"00FFFFFF",    X"00FFFFFF",    reg_led_threshold(7)                           
    instance->led_threshold[8]                  = 0x800000A0;   //      X"0A0",         X"00000064",    X"00FFFFFF",    X"00FFFFFF",    reg_led_threshold(8)                           
    instance->led_threshold[9]                  = 0x800000A4;   //      X"0A4",         X"00000064",    X"00FFFFFF",    X"00FFFFFF",    reg_led_threshold(9)                           
    instance->led_threshold[10]                 = 0x800000A8;   //      X"0A8",         X"00000064",    X"00FFFFFF",    X"00FFFFFF",    reg_led_threshold(10)                          
    instance->led_threshold[11]                 = 0x800000AC;   //      X"0AC",         X"00000064",    X"00FFFFFF",    X"00FFFFFF",    reg_led_threshold(11)                          
                                                                                                                                                                                       
    instance->cfd_parameters[0]                 = 0x800000C0;   //      X"0C0",         X"00001800",    X"00001FFF",    X"00001FFF",    reg_cfd_parameters(0)                          
    instance->cfd_parameters[1]                 = 0x800000C4;   //      X"0C4",         X"00001800",    X"00001FFF",    X"00001FFF",    reg_cfd_parameters(1)                          
    instance->cfd_parameters[2]                 = 0x800000C8;   //      X"0C8",         X"00001800",    X"00001FFF",    X"00001FFF",    reg_cfd_parameters(2)                          
    instance->cfd_parameters[3]                 = 0x800000CC;   //      X"0CC",         X"00001800",    X"00001FFF",    X"00001FFF",    reg_cfd_parameters(3)                          
    instance->cfd_parameters[4]                 = 0x800000D0;   //      X"0D0",         X"00001800",    X"00001FFF",    X"00001FFF",    reg_cfd_parameters(4)                          
    instance->cfd_parameters[5]                 = 0x800000D4;   //      X"0D4",         X"00001800",    X"00001FFF",    X"00001FFF",    reg_cfd_parameters(5)                          
    instance->cfd_parameters[6]                 = 0x800000D8;   //      X"0D8",         X"00001800",    X"00001FFF",    X"00001FFF",    reg_cfd_parameters(6)                          
    instance->cfd_parameters[7]                 = 0x800000DC;   //      X"0DC",         X"00001800",    X"00001FFF",    X"00001FFF",    reg_cfd_parameters(7)                          
    instance->cfd_parameters[8]                 = 0x800000E0;   //      X"0E0",         X"00001800",    X"00001FFF",    X"00001FFF",    reg_cfd_parameters(8)                          
    instance->cfd_parameters[9]                 = 0x800000E4;   //      X"0E4",         X"00001800",    X"00001FFF",    X"00001FFF",    reg_cfd_parameters(9)                          
    instance->cfd_parameters[10]                = 0x800000E8;   //      X"0E8",         X"00001800",    X"00001FFF",    X"00001FFF",    reg_cfd_parameters(10)                         
    instance->cfd_parameters[11]                = 0x800000EC;   //      X"0EC",         X"00001800",    X"00001FFF",    X"00001FFF",    reg_cfd_parameters(11)                         
                                                                                                                                                                                       
    instance->readout_pretrigger[0]             = 0x80000100;   //      X"100",         X"00000019",    X"000007FF",    X"000007FF",    reg_readout_pretrigger(0)                      
    instance->readout_pretrigger[1]             = 0x80000104;   //      X"104",         X"00000019",    X"000007FF",    X"000007FF",    reg_readout_pretrigger(1)                      
    instance->readout_pretrigger[2]             = 0x80000108;   //      X"108",         X"00000019",    X"000007FF",    X"000007FF",    reg_readout_pretrigger(2)                      
    instance->readout_pretrigger[3]             = 0x8000010C;   //      X"10C",         X"00000019",    X"000007FF",    X"000007FF",    reg_readout_pretrigger(3)                      
    instance->readout_pretrigger[4]             = 0x80000110;   //      X"110",         X"00000019",    X"000007FF",    X"000007FF",    reg_readout_pretrigger(4)                      
    instance->readout_pretrigger[5]             = 0x80000114;   //      X"114",         X"00000019",    X"000007FF",    X"000007FF",    reg_readout_pretrigger(5)                      
    instance->readout_pretrigger[6]             = 0x80000118;   //      X"118",         X"00000019",    X"000007FF",    X"000007FF",    reg_readout_pretrigger(6)                      
    instance->readout_pretrigger[7]             = 0x8000011C;   //      X"11C",         X"00000019",    X"000007FF",    X"000007FF",    reg_readout_pretrigger(7)                      
    instance->readout_pretrigger[8]             = 0x80000120;   //      X"120",         X"00000019",    X"000007FF",    X"000007FF",    reg_readout_pretrigger(8)                      
    instance->readout_pretrigger[9]             = 0x80000124;   //      X"124",         X"00000019",    X"000007FF",    X"000007FF",    reg_readout_pretrigger(9)                      
    instance->readout_pretrigger[10]            = 0x80000128;   //      X"128",         X"00000019",    X"000007FF",    X"000007FF",    reg_readout_pretrigger(10)                     
    instance->readout_pretrigger[11]            = 0x8000012C;   //      X"12C",         X"00000019",    X"000007FF",    X"000007FF",    reg_readout_pretrigger(11)                     
                                                                                                                                                                                       
    instance->readout_window[0]                 = 0x80000140;   //      X"140",         X"000000C8",    X"000007FE",    X"000007FE",    reg_readout_window(0)                          
    instance->readout_window[1]                 = 0x80000144;   //      X"144",         X"000000C8",    X"000007FE",    X"000007FE",    reg_readout_window(1)                          
    instance->readout_window[2]                 = 0x80000148;   //      X"148",         X"000000C8",    X"000007FE",    X"000007FE",    reg_readout_window(2)                          
    instance->readout_window[3]                 = 0x8000014C;   //      X"14C",         X"000000C8",    X"000007FE",    X"000007FE",    reg_readout_window(3)                          
    instance->readout_window[4]                 = 0x80000150;   //      X"150",         X"000000C8",    X"000007FE",    X"000007FE",    reg_readout_window(4)                          
    instance->readout_window[5]                 = 0x80000154;   //      X"154",         X"000000C8",    X"000007FE",    X"000007FE",    reg_readout_window(5)                          
    instance->readout_window[6]                 = 0x80000158;   //      X"158",         X"000000C8",    X"000007FE",    X"000007FE",    reg_readout_window(6)                          
    instance->readout_window[7]                 = 0x8000015C;   //      X"15C",         X"000000C8",    X"000007FE",    X"000007FE",    reg_readout_window(7)                          
    instance->readout_window[8]                 = 0x80000160;   //      X"160",         X"000000C8",    X"000007FE",    X"000007FE",    reg_readout_window(8)                          
    instance->readout_window[9]                 = 0x80000164;   //      X"164",         X"000000C8",    X"000007FE",    X"000007FE",    reg_readout_window(9)                          
    instance->readout_window[10]                = 0x80000168;   //      X"168",         X"000000C8",    X"000007FE",    X"000007FE",    reg_readout_window(10)                         
    instance->readout_window[11]                = 0x8000016C;   //      X"16C",         X"000000C8",    X"000007FE",    X"000007FE",    reg_readout_window(11)                         
                                                                                                                                                                                       
    instance->p_window[0]                       = 0x80000180;   //      X"180",         X"00000000",    X"000003FF",    X"000003FF",    reg_p_window(0)                                
    instance->p_window[1]                       = 0x80000184;   //      X"184",         X"00000000",    X"000003FF",    X"000003FF",    reg_p_window(1)                                
    instance->p_window[2]                       = 0x80000188;   //      X"188",         X"00000000",    X"000003FF",    X"000003FF",    reg_p_window(2)                                
    instance->p_window[3]                       = 0x8000018C;   //      X"18C",         X"00000000",    X"000003FF",    X"000003FF",    reg_p_window(3)                                
    instance->p_window[4]                       = 0x80000190;   //      X"190",         X"00000000",    X"000003FF",    X"000003FF",    reg_p_window(4)                                
    instance->p_window[5]                       = 0x80000194;   //      X"194",         X"00000000",    X"000003FF",    X"000003FF",    reg_p_window(5)                                
    instance->p_window[6]                       = 0x80000198;   //      X"198",         X"00000000",    X"000003FF",    X"000003FF",    reg_p_window(6)                                
    instance->p_window[7]                       = 0x8000019C;   //      X"19C",         X"00000000",    X"000003FF",    X"000003FF",    reg_p_window(7)                                
    instance->p_window[8]                       = 0x800001A0;   //      X"1A0",         X"00000000",    X"000003FF",    X"000003FF",    reg_p_window(8)                                
    instance->p_window[9]                       = 0x800001A4;   //      X"1A4",         X"00000000",    X"000003FF",    X"000003FF",    reg_p_window(9)                                
    instance->p_window[10]                      = 0x800001A8;   //      X"1A8",         X"00000000",    X"000003FF",    X"000003FF",    reg_p_window(10)                               
    instance->p_window[11]                      = 0x800001AC;   //      X"1AC",         X"00000000",    X"000003FF",    X"000003FF",    reg_p_window(11)                               
                                                                                                                                                                                       
    instance->i2_window[0]                       = 0x800001C0;   //      X"1C0",         X"00000014",    X"000003FF",    X"000003FF",    reg_i2_window(0)                                
    instance->i2_window[1]                       = 0x800001C4;   //      X"1C4",         X"00000014",    X"000003FF",    X"000003FF",    reg_i2_window(1)                                
    instance->i2_window[2]                       = 0x800001C8;   //      X"1C8",         X"00000014",    X"000003FF",    X"000003FF",    reg_i2_window(2)                                
    instance->i2_window[3]                       = 0x800001CC;   //      X"1CC",         X"00000014",    X"000003FF",    X"000003FF",    reg_i2_window(3)                                
    instance->i2_window[4]                       = 0x800001D0;   //      X"1D0",         X"00000014",    X"000003FF",    X"000003FF",    reg_i2_window(4)                                
    instance->i2_window[5]                       = 0x800001D4;   //      X"1D4",         X"00000014",    X"000003FF",    X"000003FF",    reg_i2_window(5)                                
    instance->i2_window[6]                       = 0x800001D8;   //      X"1D8",         X"00000014",    X"000003FF",    X"000003FF",    reg_i2_window(6)                                
    instance->i2_window[7]                       = 0x800001DC;   //      X"1DC",         X"00000014",    X"000003FF",    X"000003FF",    reg_i2_window(7)                                
    instance->i2_window[8]                       = 0x800001E0;   //      X"1E0",         X"00000014",    X"000003FF",    X"000003FF",    reg_i2_window(8)                                
    instance->i2_window[9]                       = 0x800001E4;   //      X"1E4",         X"00000014",    X"000003FF",    X"000003FF",    reg_i2_window(9)                                
    instance->i2_window[10]                      = 0x800001E8;   //      X"1E8",         X"00000014",    X"000003FF",    X"000003FF",    reg_i2_window(10)                               
    instance->i2_window[11]                      = 0x800001EC;   //      X"1EC",         X"00000014",    X"000003FF",    X"000003FF",    reg_i2_window(11)                               
                                                                                                                                                                                       
    instance->m1_window[0]                      = 0x80000200;   //      X"200",         X"0000000A",    X"000003FF",    X"000003FF",    reg_m1_window(0)                               
    instance->m1_window[1]                      = 0x80000204;   //      X"204",         X"0000000A",    X"000003FF",    X"000003FF",    reg_m1_window(1)                               
    instance->m1_window[2]                      = 0x80000208;   //      X"208",         X"0000000A",    X"000003FF",    X"000003FF",    reg_m1_window(2)                               
    instance->m1_window[3]                      = 0x8000020C;   //      X"20C",         X"0000000A",    X"000003FF",    X"000003FF",    reg_m1_window(3)                               
    instance->m1_window[4]                      = 0x80000210;   //      X"210",         X"0000000A",    X"000003FF",    X"000003FF",    reg_m1_window(4)                               
    instance->m1_window[5]                      = 0x80000214;   //      X"214",         X"0000000A",    X"000003FF",    X"000003FF",    reg_m1_window(5)                               
    instance->m1_window[6]                      = 0x80000218;   //      X"218",         X"0000000A",    X"000003FF",    X"000003FF",    reg_m1_window(6)                               
    instance->m1_window[7]                      = 0x8000021C;   //      X"21C",         X"0000000A",    X"000003FF",    X"000003FF",    reg_m1_window(7)                               
    instance->m1_window[8]                      = 0x80000220;   //      X"220",         X"0000000A",    X"000003FF",    X"000003FF",    reg_m1_window(8)                               
    instance->m1_window[9]                      = 0x80000224;   //      X"224",         X"0000000A",    X"000003FF",    X"000003FF",    reg_m1_window(9)                               
    instance->m1_window[10]                     = 0x80000228;   //      X"228",         X"0000000A",    X"000003FF",    X"000003FF",    reg_m1_window(10)                              
    instance->m1_window[11]                     = 0x8000022C;   //      X"22C",         X"0000000A",    X"000003FF",    X"000003FF",    reg_m1_window(11)                              
                                                                                                                                                                                       
    instance->m2_window[0]                      = 0x80000240;   //      X"240",         X"00000014",    X"0000007F",    X"0000007F",    reg_m2_window(0)                               
    instance->m2_window[1]                      = 0x80000244;   //      X"244",         X"00000014",    X"0000007F",    X"0000007F",    reg_m2_window(1)                               
    instance->m2_window[2]                      = 0x80000248;   //      X"248",         X"00000014",    X"0000007F",    X"0000007F",    reg_m2_window(2)                               
    instance->m2_window[3]                      = 0x8000024C;   //      X"24C",         X"00000014",    X"0000007F",    X"0000007F",    reg_m2_window(3)                               
    instance->m2_window[4]                      = 0x80000250;   //      X"250",         X"00000014",    X"0000007F",    X"0000007F",    reg_m2_window(4)                               
    instance->m2_window[5]                      = 0x80000254;   //      X"254",         X"00000014",    X"0000007F",    X"0000007F",    reg_m2_window(5)                               
    instance->m2_window[6]                      = 0x80000258;   //      X"258",         X"00000014",    X"0000007F",    X"0000007F",    reg_m2_window(6)                               
    instance->m2_window[7]                      = 0x8000025C;   //      X"25C",         X"00000014",    X"0000007F",    X"0000007F",    reg_m2_window(7)                               
    instance->m2_window[8]                      = 0x80000260;   //      X"260",         X"00000014",    X"0000007F",    X"0000007F",    reg_m2_window(8)                               
    instance->m2_window[9]                      = 0x80000264;   //      X"264",         X"00000014",    X"0000007F",    X"0000007F",    reg_m2_window(9)                               
    instance->m2_window[10]                     = 0x80000268;   //      X"268",         X"00000014",    X"0000007F",    X"0000007F",    reg_m2_window(10)                              
    instance->m2_window[11]                     = 0x8000026C;   //      X"26C",         X"00000014",    X"0000007F",    X"0000007F",    reg_m2_window(11)                              
                                                                                                                                                                                       
    instance->d_window[0]                       = 0x80000280;   //      X"280",         X"00000014",    X"0000007F",    X"0000007F",    reg_d_window(0)                                
    instance->d_window[1]                       = 0x80000284;   //      X"284",         X"00000014",    X"0000007F",    X"0000007F",    reg_d_window(1)                                
    instance->d_window[2]                       = 0x80000288;   //      X"288",         X"00000014",    X"0000007F",    X"0000007F",    reg_d_window(2)                                
    instance->d_window[3]                       = 0x8000028C;   //      X"28C",         X"00000014",    X"0000007F",    X"0000007F",    reg_d_window(3)                                
    instance->d_window[4]                       = 0x80000290;   //      X"290",         X"00000014",    X"0000007F",    X"0000007F",    reg_d_window(4)                                
    instance->d_window[5]                       = 0x80000294;   //      X"294",         X"00000014",    X"0000007F",    X"0000007F",    reg_d_window(5)                                
    instance->d_window[6]                       = 0x80000298;   //      X"298",         X"00000014",    X"0000007F",    X"0000007F",    reg_d_window(6)                                
    instance->d_window[7]                       = 0x8000029C;   //      X"29C",         X"00000014",    X"0000007F",    X"0000007F",    reg_d_window(7)                                
    instance->d_window[8]                       = 0x800002A0;   //      X"2A0",         X"00000014",    X"0000007F",    X"0000007F",    reg_d_window(8)                                
    instance->d_window[9]                       = 0x800002A4;   //      X"2A4",         X"00000014",    X"0000007F",    X"0000007F",    reg_d_window(9)                                
    instance->d_window[10]                      = 0x800002A8;   //      X"2A8",         X"00000014",    X"0000007F",    X"0000007F",    reg_d_window(10)                               
    instance->d_window[11]                      = 0x800002AC;   //      X"2AC",         X"00000014",    X"0000007F",    X"0000007F",    reg_d_window(11)                               
                                                                                                                                                                                       
    instance->i1_window[0]                       = 0x800002C0;   //      X"2C0",         X"00000010",    X"000003FF",    X"000003FF",    reg_i1_window(0)                                
    instance->i1_window[1]                       = 0x800002C4;   //      X"2C4",         X"00000010",    X"000003FF",    X"000003FF",    reg_i1_window(1)                                
    instance->i1_window[2]                       = 0x800002C8;   //      X"2C8",         X"00000010",    X"000003FF",    X"000003FF",    reg_i1_window(2)                                
    instance->i1_window[3]                       = 0x800002CC;   //      X"2CC",         X"00000010",    X"000003FF",    X"000003FF",    reg_i1_window(3)                                
    instance->i1_window[4]                       = 0x800002D0;   //      X"2D0",         X"00000010",    X"000003FF",    X"000003FF",    reg_i1_window(4)                                
    instance->i1_window[5]                       = 0x800002D4;   //      X"2D4",         X"00000010",    X"000003FF",    X"000003FF",    reg_i1_window(5)                                
    instance->i1_window[6]                       = 0x800002D8;   //      X"2D8",         X"00000010",    X"000003FF",    X"000003FF",    reg_i1_window(6)                                
    instance->i1_window[7]                       = 0x800002DC;   //      X"2DC",         X"00000010",    X"000003FF",    X"000003FF",    reg_i1_window(7)                                
    instance->i1_window[8]                       = 0x800002E0;   //      X"2E0",         X"00000010",    X"000003FF",    X"000003FF",    reg_i1_window(8)                                
    instance->i1_window[9]                       = 0x800002E4;   //      X"2E4",         X"00000010",    X"000003FF",    X"000003FF",    reg_i1_window(9)                                
    instance->i1_window[10]                      = 0x800002E8;   //      X"2E8",         X"00000010",    X"000003FF",    X"000003FF",    reg_i1_window(10)                               
    instance->i1_window[11]                      = 0x800002EC;   //      X"2EC",         X"00000010",    X"000003FF",    X"000003FF",    reg_i1_window(11)                               
                                                                                                                                                                                       
    instance->disc_width[0]                     = 0x80000300;   //      X"300",         X"00000000",    X"0000FFFF",    X"0000FFFF",    reg_disc_width(0)                              
    instance->disc_width[1]                     = 0x80000304;   //      X"304",         X"00000000",    X"0000FFFF",    X"0000FFFF",    reg_disc_width(1)                              
    instance->disc_width[2]                     = 0x80000308;   //      X"308",         X"00000000",    X"0000FFFF",    X"0000FFFF",    reg_disc_width(2)                              
    instance->disc_width[3]                     = 0x8000030C;   //      X"30C",         X"00000000",    X"0000FFFF",    X"0000FFFF",    reg_disc_width(3)                              
    instance->disc_width[4]                     = 0x80000310;   //      X"310",         X"00000000",    X"0000FFFF",    X"0000FFFF",    reg_disc_width(4)                              
    instance->disc_width[5]                     = 0x80000314;   //      X"314",         X"00000000",    X"0000FFFF",    X"0000FFFF",    reg_disc_width(5)                              
    instance->disc_width[6]                     = 0x80000318;   //      X"318",         X"00000000",    X"0000FFFF",    X"0000FFFF",    reg_disc_width(6)                              
    instance->disc_width[7]                     = 0x8000031C;   //      X"31C",         X"00000000",    X"0000FFFF",    X"0000FFFF",    reg_disc_width(7)                              
    instance->disc_width[8]                     = 0x80000320;   //      X"320",         X"00000000",    X"0000FFFF",    X"0000FFFF",    reg_disc_width(8)                              
    instance->disc_width[9]                     = 0x80000324;   //      X"324",         X"00000000",    X"0000FFFF",    X"0000FFFF",    reg_disc_width(9)                              
    instance->disc_width[10]                    = 0x80000328;   //      X"328",         X"00000000",    X"0000FFFF",    X"0000FFFF",    reg_disc_width(10)                             
    instance->disc_width[11]                    = 0x8000032C;   //      X"32C",         X"00000000",    X"0000FFFF",    X"0000FFFF",    reg_disc_width(11)                             
                                                                                                                                                                                       
    instance->baseline_start[0]                 = 0x80000340;   //      X"340",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_baseline_start(0)                          
    instance->baseline_start[1]                 = 0x80000344;   //      X"344",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_baseline_start(1)                          
    instance->baseline_start[2]                 = 0x80000348;   //      X"348",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_baseline_start(2)                          
    instance->baseline_start[3]                 = 0x8000034C;   //      X"34C",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_baseline_start(3)                          
    instance->baseline_start[4]                 = 0x80000350;   //      X"350",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_baseline_start(4)                          
    instance->baseline_start[5]                 = 0x80000354;   //      X"354",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_baseline_start(5)                          
    instance->baseline_start[6]                 = 0x80000358;   //      X"358",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_baseline_start(6)                          
    instance->baseline_start[7]                 = 0x8000035C;   //      X"35C",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_baseline_start(7)                          
    instance->baseline_start[8]                 = 0x80000360;   //      X"360",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_baseline_start(8)                          
    instance->baseline_start[9]                 = 0x80000364;   //      X"364",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_baseline_start(9)                          
    instance->baseline_start[10]                = 0x80000368;   //      X"368",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_baseline_start(10)                         
    instance->baseline_start[11]                = 0x8000036C;   //      X"36C",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_baseline_start(11)                         
    instance->c_window[0]                 = 0x80000380;   //      X"340",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_c_window(0)                          
    instance->c_window[1]                 = 0x80000384;   //      X"344",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_c_window(1)                          
    instance->c_window[2]                 = 0x80000388;   //      X"348",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_c_window(2)                          
    instance->c_window[3]                 = 0x8000038C;   //      X"34C",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_c_window(3)                          
    instance->c_window[4]                 = 0x80000390;   //      X"350",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_c_window(4)                          
    instance->c_window[5]                 = 0x80000394;   //      X"354",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_c_window(5)                          
    instance->c_window[6]                 = 0x80000398;   //      X"358",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_c_window(6)                          
    instance->c_window[7]                 = 0x8000039C;   //      X"35C",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_c_window(7)                          
    instance->c_window[8]                 = 0x800003A0;   //      X"360",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_c_window(8)                          
    instance->c_window[9]                 = 0x800003A4;   //      X"364",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_c_window(9)                          
    instance->c_window[10]                = 0x800003A8;   //      X"368",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_c_window(10)                         
    instance->c_window[11]                = 0x800003AC;   //      X"36C",         X"00002000",    X"00003FFF",    X"00003FFF",    reg_c_window(11)                         
                                                                                                                                                                                       
    instance->trigger_input_delay               = 0x80000400;   //      X"400",         X"00000010",    X"0000FFFF",    X"0000FFFF",    reg_trigger_input_delay                        
    instance->gpio_output_width                 = 0x80000404;   //      X"404",         X"0000000F",    X"0000FFFF",    X"0000FFFF",    reg_gpio_output_width                          
    instance->front_panel_config                       = 0x80000408;   //      X"408",         X"00000111",    X"00730333",    X"00730333",    reg_misc_config                                
    instance->channel_pulsed_control            = 0x8000040C;   //      X"40C",         X"00000000",    X"00000000",    X"FFFFFFFF",    reg_channel_pulsed_control                     
    instance->dsp_led_config                        = 0x80000410;   //      X"410",         X"00000000",    X"FFFFFFFF",    X"00000003",    reg_led_config                                 
    instance->dsp_led_input                         = 0x80000414;   //      X"414",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_led_input                                  
    instance->baseline_delay                    = 0x80000418;   //      X"418",         X"00000019",    X"000000FF",    X"000000FF",    reg_baseline_delay                             
    instance->diag_channel_input                = 0x8000041C;   //      X"41C",         X"00000000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_diag_channel_input                         
    instance->event_data_control                = 0x80000424;   //      X"424",         X"00000001",    X"00020001",    X"00020001",    reg_event_data_control                         
    instance->adc_config                        = 0x80000428;   //      X"428",         X"00010000",    X"FFFFFFFF",    X"FFFFFFFF",    reg_adc_config                                 
    instance->adc_config_load                   = 0x8000042C;   //      X"42C",         X"00000000",    X"00000000",    X"00000001",    reg_adc_config_load                            
    instance->qi_config				= 0x80000430;	//	X"430",		X"0FFF1700",	X"0FFF1F11",	X"0FFF1F11",	reg_qi_config
    instance->qi_delay				= 0x80000434;	//	X"434",		X"00000000",	X"0000007F",	X"0000007F",	reg_qi_delay
    instance->qi_pulse_width			= 0x80000438;	//	X"438",		X"00000000",	X"0000FFFF",	X"0000FFFF",	reg_qi_pulse_width
    instance->qi_pulsed				= 0x8000043C;	//	X"43C",		X"00000000",	X"00000000",	X"00030001",	reg_qi_pulsed
    instance->external_gate_width		= 0x80000440;	//	X"440",		X"00008000",	X"0000FFFF",	X"0000FFFF",	reg_external_gate_width
    instance->pdts_cmd_control[0]		= 0x80000460;	//	X"440",		X"00008000",	X"0000FFFF",	X"0000FFFF",	reg_pdts_cmd_control(0)
    instance->pdts_cmd_control[1]		= 0x80000464;	//	X"440",		X"00008000",	X"0000FFFF",	X"0000FFFF",	reg_pdts_cmd_control(1)
    instance->pdts_cmd_control[2]		= 0x80000468;	//	X"440",		X"00008000",	X"0000FFFF",	X"0000FFFF",	reg_pdts_cmd_control(2)
    instance->lat_timestamp_lsb                 = 0x80000484;   //      X"484",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_lat_timestamp (lsb)                        
    instance->lat_timestamp_msb                 = 0x80000488;   //      X"488",         X"00000000",    X"0000FFFF",    X"00000000",    reg_lat_timestamp (msb)                        
    instance->live_timestamp_lsb                = 0x8000048C;   //      X"48C",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_live_timestamp (lsb)                       
    instance->live_timestamp_msb                = 0x80000490;   //      X"490",         X"00000000",    X"0000FFFF",    X"00000000",    reg_live_timestamp (msb)                       
    instance->sync_period                       = 0x80000494;   //      X"494",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_last_sync_reset_count                      
    instance->sync_delay                        = 0x80000498;   //      X"498",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_external_timestamp (lsb)                   
    instance->sync_count                        = 0x8000049C;   //      X"49C",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_external_timestamp (msb)                   
    instance->pdts_control                      = 0x800004C0;
    instance->pdts_status                       = 0x800004C4;
    instance->pdts_ts_preset[0]                 = 0x800004C8;
    instance->pdts_ts_preset[1]                 = 0x800004CC;
    instance->master_logic_control               = 0x80000500;   //      X"500",         X"00000000",    X"FFFFFFFF",    X"00000073",    reg_master_logic_status                        
    instance->master_logic_status                    = 0x80000504;   //      X"504",         X"00000000",    X"00000003",    X"00000003",    reg_trigger_config                             
    instance->overflow_status                   = 0x80000508;   //      X"508",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_overflow_status                            
    instance->phase_value                       = 0x8000050C;   //      X"50C",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_phase_value                                
    instance->link_tx_status                       = 0x80000510;   //      X"510",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_link_status                                

    instance->dsp_clock_control		= 0x80000520;	//	X"520",		X"00000000",	X"00000713",	X"00000713",	reg_dsp_clock_control		
    instance->dsp_clock_phase_control	= 0x80000524;	//	X"524",		X"00000000",	X"00000000",	X"00000007",	reg_dsp_clock_phase_control
                                                                                                                                                                                       
    instance->code_revision                     = 0x80000600;   //      X"600",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_code_revision                              
    instance->code_date                         = 0x80000604;   //      X"604",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_code_date                                  
                                                                                                                                                                                       
    instance->dropped_event_count[0]            = 0x80000700;   //      X"700",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_dropped_event_count(0)                     
    instance->dropped_event_count[1]            = 0x80000704;   //      X"704",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_dropped_event_count(1)                     
    instance->dropped_event_count[2]            = 0x80000708;   //      X"708",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_dropped_event_count(2)                     
    instance->dropped_event_count[3]            = 0x8000070C;   //      X"70C",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_dropped_event_count(3)                     
    instance->dropped_event_count[4]            = 0x80000710;   //      X"710",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_dropped_event_count(4)                     
    instance->dropped_event_count[5]            = 0x80000714;   //      X"714",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_dropped_event_count(5)                     
    instance->dropped_event_count[6]            = 0x80000718;   //      X"718",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_dropped_event_count(6)                     
    instance->dropped_event_count[7]            = 0x8000071C;   //      X"71C",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_dropped_event_count(7)                     
    instance->dropped_event_count[8]            = 0x80000720;   //      X"720",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_dropped_event_count(8)                     
    instance->dropped_event_count[9]            = 0x80000724;   //      X"724",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_dropped_event_count(9)                     
    instance->dropped_event_count[10]           = 0x80000728;   //      X"728",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_dropped_event_count(10)                    
    instance->dropped_event_count[11]           = 0x8000072C;   //      X"72C",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_dropped_event_count(11)                    
                                                                                                                                                                                       
    instance->accepted_event_count[0]           = 0x80000740;   //      X"740",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_accepted_event_count(0)                    
    instance->accepted_event_count[1]           = 0x80000744;   //      X"744",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_accepted_event_count(1)                    
    instance->accepted_event_count[2]           = 0x80000748;   //      X"748",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_accepted_event_count(2)                    
    instance->accepted_event_count[3]           = 0x8000074C;   //      X"74C",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_accepted_event_count(3)                    
    instance->accepted_event_count[4]           = 0x80000750;   //      X"750",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_accepted_event_count(4)                    
    instance->accepted_event_count[5]           = 0x80000754;   //      X"754",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_accepted_event_count(5)                    
    instance->accepted_event_count[6]           = 0x80000758;   //      X"758",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_accepted_event_count(6)                    
    instance->accepted_event_count[7]           = 0x8000075C;   //      X"75C",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_accepted_event_count(7)                    
    instance->accepted_event_count[8]           = 0x80000760;   //      X"760",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_accepted_event_count(8)                    
    instance->accepted_event_count[9]           = 0x80000764;   //      X"764",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_accepted_event_count(9)                    
    instance->accepted_event_count[10]          = 0x80000768;   //      X"768",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_accepted_event_count(10)                   
    instance->accepted_event_count[11]          = 0x8000076C;   //      X"76C",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_accepted_event_count(11)                   
                                                                                                                                                                                       
    instance->ahit_count[0]                     = 0x80000780;   //      X"780",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_ahit_count(0)                              
    instance->ahit_count[1]                     = 0x80000784;   //      X"784",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_ahit_count(1)                              
    instance->ahit_count[2]                     = 0x80000788;   //      X"788",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_ahit_count(2)                              
    instance->ahit_count[3]                     = 0x8000078C;   //      X"78C",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_ahit_count(3)                              
    instance->ahit_count[4]                     = 0x80000790;   //      X"790",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_ahit_count(4)                              
    instance->ahit_count[5]                     = 0x80000794;   //      X"794",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_ahit_count(5)                              
    instance->ahit_count[6]                     = 0x80000798;   //      X"798",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_ahit_count(6)                              
    instance->ahit_count[7]                     = 0x8000079C;   //      X"79C",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_ahit_count(7)                              
    instance->ahit_count[8]                     = 0x800007A0;   //      X"7A0",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_ahit_count(8)                              
    instance->ahit_count[9]                     = 0x800007A4;   //      X"7A4",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_ahit_count(9)                              
    instance->ahit_count[10]                    = 0x800007A8;   //      X"7A8",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_ahit_count(10)                             
    instance->ahit_count[11]                    = 0x800007AC;   //      X"7AC",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_ahit_count(11)                             
                                                                                                                                                                                       
    instance->disc_count[0]                     = 0x800007C0;   //      X"7C0",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_disc_count(0)                              
    instance->disc_count[1]                     = 0x800007C4;   //      X"7C4",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_disc_count(1)                              
    instance->disc_count[2]                     = 0x800007C8;   //      X"7C8",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_disc_count(2)                              
    instance->disc_count[3]                     = 0x800007CC;   //      X"7CC",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_disc_count(3)                              
    instance->disc_count[4]                     = 0x800007D0;   //      X"7D0",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_disc_count(4)                              
    instance->disc_count[5]                     = 0x800007D4;   //      X"7D4",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_disc_count(5)                              
    instance->disc_count[6]                     = 0x800007D8;   //      X"7D8",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_disc_count(6)                              
    instance->disc_count[7]                     = 0x800007DC;   //      X"7DC",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_disc_count(7)                              
    instance->disc_count[8]                     = 0x800007E0;   //      X"7E0",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_disc_count(8)                              
    instance->disc_count[9]                     = 0x800007E4;   //      X"7E4",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_disc_count(9)                              
    instance->disc_count[10]                    = 0x800007E8;   //      X"7E8",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_disc_count(10)                             
    instance->disc_count[11]                    = 0x800007EC;   //      X"7EC",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_disc_count(11)                             
                                                                                                                                                                                       
    instance->idelay_count[0]                   = 0x80000800;   //      X"800",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_idelay_count(0)                            
    instance->idelay_count[1]                   = 0x80000804;   //      X"804",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_idelay_count(1)                            
    instance->idelay_count[2]                   = 0x80000808;   //      X"808",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_idelay_count(2)                            
    instance->idelay_count[3]                   = 0x8000080C;   //      X"80C",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_idelay_count(3)                            
    instance->idelay_count[4]                   = 0x80000810;   //      X"810",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_idelay_count(4)                            
    instance->idelay_count[5]                   = 0x80000814;   //      X"814",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_idelay_count(5)                            
    instance->idelay_count[6]                   = 0x80000818;   //      X"818",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_idelay_count(6)                            
    instance->idelay_count[7]                   = 0x8000081C;   //      X"81C",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_idelay_count(7)                            
    instance->idelay_count[8]                   = 0x80000820;   //      X"820",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_idelay_count(8)                            
    instance->idelay_count[9]                   = 0x80000824;   //      X"824",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_idelay_count(9)                            
    instance->idelay_count[10]                  = 0x80000828;   //      X"828",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_idelay_count(10)                           
    instance->idelay_count[11]                  = 0x8000082C;   //      X"82C",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_idelay_count(11)                           
                                                                                                                                                                                       
    instance->adc_data_monitor[0]               = 0x80000840;   //      X"840",         X"00000000",    X"0000FFFF",    X"00000000",    reg_adc_data_monitor(0)                        
    instance->adc_data_monitor[1]               = 0x80000844;   //      X"844",         X"00000000",    X"0000FFFF",    X"00000000",    reg_adc_data_monitor(1)                        
    instance->adc_data_monitor[2]               = 0x80000848;   //      X"848",         X"00000000",    X"0000FFFF",    X"00000000",    reg_adc_data_monitor(2)                        
    instance->adc_data_monitor[3]               = 0x8000084C;   //      X"84C",         X"00000000",    X"0000FFFF",    X"00000000",    reg_adc_data_monitor(3)                        
    instance->adc_data_monitor[4]               = 0x80000850;   //      X"850",         X"00000000",    X"0000FFFF",    X"00000000",    reg_adc_data_monitor(4)                        
    instance->adc_data_monitor[5]               = 0x80000854;   //      X"854",         X"00000000",    X"0000FFFF",    X"00000000",    reg_adc_data_monitor(5)                        
    instance->adc_data_monitor[6]               = 0x80000858;   //      X"858",         X"00000000",    X"0000FFFF",    X"00000000",    reg_adc_data_monitor(6)                        
    instance->adc_data_monitor[7]               = 0x8000085C;   //      X"85C",         X"00000000",    X"0000FFFF",    X"00000000",    reg_adc_data_monitor(7)                        
    instance->adc_data_monitor[8]               = 0x80000860;   //      X"860",         X"00000000",    X"0000FFFF",    X"00000000",    reg_adc_data_monitor(8)                        
    instance->adc_data_monitor[9]               = 0x80000864;   //      X"864",         X"00000000",    X"0000FFFF",    X"00000000",    reg_adc_data_monitor(9)                        
    instance->adc_data_monitor[10]              = 0x80000868;   //      X"868",         X"00000000",    X"0000FFFF",    X"00000000",    reg_adc_data_monitor(10)                       
    instance->adc_data_monitor[11]              = 0x8000086C;   //      X"86C",         X"00000000",    X"0000FFFF",    X"00000000",    reg_adc_data_monitor(11)                       
                                                                                                                                                                                       
    instance->adc_status[0]                     = 0x80000880;   //      X"880",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_adc_status(0)                              
    instance->adc_status[1]                     = 0x80000884;   //      X"884",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_adc_status(1)                              
    instance->adc_status[2]                     = 0x80000888;   //      X"888",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_adc_status(2)                              
    instance->adc_status[3]                     = 0x8000088C;   //      X"88C",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_adc_status(3)                              
    instance->adc_status[4]                     = 0x80000890;   //      X"890",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_adc_status(4)                              
    instance->adc_status[5]                     = 0x80000894;   //      X"894",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_adc_status(5)                              
    instance->adc_status[6]                     = 0x80000898;   //      X"898",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_adc_status(6)                              
    instance->adc_status[7]                     = 0x8000089C;   //      X"89C",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_adc_status(7)                              
    instance->adc_status[8]                     = 0x800008A0;   //      X"8A0",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_adc_status(8)                              
    instance->adc_status[9]                     = 0x800008A4;   //      X"8A4",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_adc_status(9)                              
    instance->adc_status[10]                    = 0x800008A8;   //      X"8A8",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_adc_status(10)                             
    instance->adc_status[11]                    = 0x800008AC;   //      X"8AC",         X"00000000",    X"FFFFFFFF",    X"00000000",    reg_adc_status(11)                             

//                      Name                               Address     Read Mask  Write Mask  Size

    instance->fNamed["armStatus"]               =Register(  0x00000000, 0xFFFFFFFF, 0x00000000, 1 );
    instance->fNamed["armError"]                =Register(  0x00000004, 0xFFFFFFFF, 0x00000000, 1 );
    instance->fNamed["armCommand"]              =Register(  0x00000008, 0xFFFFFFFF, 0xFFFFFFFF, 1 );
    instance->fNamed["armVersion"]              =Register(  0x0000000C, 0xFFFFFFFF, 0x00000000, 1 );
    instance->fNamed["armTest"]                 =Register(  0x00000010, 0xFFFFFFFF, 0xFFFFFFFF, 4 );
    instance->fNamed["armRxAddress"]            =Register(  0x00000020, 0xFFFFFFFF, 0x00000000, 1 ); 
    instance->fNamed["armRxCommand"]            =Register(  0x00000024, 0xFFFFFFFF, 0x00000000, 1 );      
    instance->fNamed["armRxSize"]               =Register(  0x00000028, 0xFFFFFFFF, 0x00000000, 1 );      
    instance->fNamed["armRxStatus"]             =Register(  0x0000002C, 0xFFFFFFFF, 0x00000000, 1 );      
    instance->fNamed["armTxAddress"]            =Register(  0x00000030, 0xFFFFFFFF, 0x00000000, 1 );  
    instance->fNamed["armTxCommand"]            =Register(  0x00000034, 0xFFFFFFFF, 0x00000000, 1 );  
    instance->fNamed["armTxSize"]               =Register(  0x00000038, 0xFFFFFFFF, 0x00000000, 1 );      
    instance->fNamed["armTxStatus"]             =Register(  0x0000003C, 0xFFFFFFFF, 0x00000000, 1 );      
    instance->fNamed["armPackets"]              =Register(  0x00000040, 0xFFFFFFFF, 0x00000000, 1 );      
    instance->fNamed["armOperMode"]             =Register(  0x00000044, 0xFFFFFFFF, 0x00000000, 1 );      
    instance->fNamed["armOptions"]              =Register(  0x00000048, 0xFFFFFFFF, 0x00000000, 1 );      
    instance->fNamed["armModemStatus"]          =Register(  0x0000004C, 0xFFFFFFFF, 0x00000000, 1 );  
    instance->fNamed["PurgeDDR"]                =Register(  0x00000300, 0x00000001, 0x00000001, 1 );  
    instance->fNamed["zynqTest"]                =Register(  0x40000000, 0xFFFFFFFF, 0x00000000, 6 );
    instance->fNamed["eventDataInterfaceSelect"]=Register(  0x40000020, 0xFFFFFFFF, 0x00000001, 1 );
    instance->fNamed["fakeNumEvents"]           =Register(  0x40000024, 0xFFFFFFFF, 0xFFFFFFFF, 1 );
    instance->fNamed["fakeEventSize"]           =Register(  0x40000028, 0xFFFFFFFF, 0xFFFFFFFF, 1 );
    instance->fNamed["fakeBaseline"]            =Register(  0x4000002C, 0xFFFFFFFF, 0xFFFFFFFF, 1 );
    instance->fNamed["fakePeakSum"]             =Register(  0x40000030, 0xFFFFFFFF, 0xFFFFFFFF, 1 );
    instance->fNamed["fakePrerise"]             =Register(  0x40000034, 0xFFFFFFFF, 0xFFFFFFFF, 1 );
    instance->fNamed["timestamp"]               =Register(  0x40000038, 0xFFFFFFFF, 0x00000000, 2 );
    instance->fNamed["codeErrCounts"]           =Register(  0x40000100, 0xFFFFFFFF, 0x00000000, 5 );
    instance->fNamed["dispErrCounts"]           =Register(  0x40000120, 0xFFFFFFFF, 0x00000000, 5 );
    instance->fNamed["link_rx_status"]          =Register(  0x40000140, 0xFFFFFFFF, 0x00000000, 1 );
    instance->fNamed["eventDataControl"]        =Register(  0x40000144, 0xFFFFFFFF, 0x0033001F, 1 );     
    instance->fNamed["eventDataPhaseControl"]   =Register(  0x40000148, 0x00000000, 0x00000003, 1 );     
    instance->fNamed["eventDataPhaseStatus"]    =Register(  0x4000014C, 0xFFFFFFFF, 0x00000000, 1 );
    instance->fNamed["c2c_master_status"]       =Register(  0x40000150, 0xFFFFFFFF, 0x00000000, 1 );
    instance->fNamed["c2c_control"]             =Register(  0x40000154, 0xFFFFFFFF, 0x00000007, 1 );
    instance->fNamed["c2c_master_intr_control"] =Register(  0x40000158, 0xFFFFFFFF, 0x0000000F, 1 );     
    instance->fNamed["dspStatus"]               =Register(  0x40000160, 0xFFFFFFFF, 0x00000000, 1 );
    instance->fNamed["comm_clock_status"]       =Register(  0x40000170, 0xFFFFFFFF, 0x00000000, 1 );
    instance->fNamed["comm_clock_control"]      =Register(  0x40000174, 0xFFFFFFFF, 0x00000001, 1 );
    instance->fNamed["comm_led_config"]         =Register(  0x40000180, 0xFFFFFFFF, 0x00000003, 1 );     
    instance->fNamed["comm_led_input"]          =Register(  0x40000184, 0xFFFFFFFF, 0xFFFFFFFF, 1 );     
    instance->fNamed["eventDataStatus"]         =Register(  0x40000190, 0xFFFFFFFF, 0x00000000, 1 );     
    instance->fNamed["qi_dac_control"]          =Register(  0x40000200, 0x00000000, 0x00000001, 1 );    
    instance->fNamed["qi_dac_config"]           =Register(  0x40000204, 0x0003FFFF, 0x0003FFFF, 1 );    
    instance->fNamed["bias_control"]            =Register(  0x40000300, 0x00000000, 0x00000001, 1 );    
    instance->fNamed["bias_status"]             =Register(  0x40000304, 0x00000FFF, 0x00000000, 1 );    
    instance->fNamed["bias_config"]             =Register(  0x40000340, 0xFFFFFFFF, 0x00440FFF, 12);    
    instance->fNamed["bias_readback"]           =Register(  0x40000380, 0xFFFFFFFF, 0x00000000, 12);    
    instance->fNamed["vmon_config"]             =Register(  0x40000400, 0x00FFFFFF, 0x00FFFFFF, 1 );    
    instance->fNamed["vmon_select"]             =Register(  0x40000404, 0xFFFFFFFF, 0xFFFFFFFF, 1 );    
    instance->fNamed["vmon_gpio"]               =Register(  0x40000408, 0x0000FFFF, 0x0000FFFF, 1 );    
    instance->fNamed["vmon_config_readback"]    =Register(  0x40000410, 0x00FFFFFF, 0x00000000, 1 );    
    instance->fNamed["vmon_select_readback"]    =Register(  0x40000414, 0xFFFFFFFF, 0x00000000, 1 );    
    instance->fNamed["vmon_gpio_readback"]      =Register(  0x40000418, 0x0000FFFF, 0x00000000, 1 );    
    instance->fNamed["vmon_id_readback"]        =Register(  0x4000041C, 0x000000FF, 0x00000000, 1 );    
    instance->fNamed["vmon_control"]            =Register(  0x40000420, 0x00010100, 0x00010001, 1 );    
    instance->fNamed["vmon_status"]             =Register(  0x40000424, 0xFFFFFFFF, 0x00000000, 1 );    
    instance->fNamed["vmon_bias"]               =Register(  0x40000440, 0xFFFFFFFF, 0x00000000, 12);    
    instance->fNamed["vmon_value"]              =Register(  0x40000480, 0xFFFFFFFF, 0x00000000, 8 );    
    instance->fNamed["imon_config"]             =Register(  0x40000500, 0x00FFFFFF, 0x00FFFFFF, 1 );    
    instance->fNamed["imon_select"]             =Register(  0x40000504, 0xFFFFFFFF, 0xFFFFFFFF, 1 );    
    instance->fNamed["imon_gpio"]               =Register(  0x40000508, 0x0000FFFF, 0x0000FFFF, 1 );    
    instance->fNamed["imon_config_readback"]    =Register(  0x40000510, 0x00FFFFFF, 0x00000000, 1 );    
    instance->fNamed["imon_select_readback"]    =Register(  0x40000514, 0xFFFFFFFF, 0x00000000, 1 );    
    instance->fNamed["imon_gpio_readback"]      =Register(  0x40000518, 0x0000FFFF, 0x00000000, 1 );    
    instance->fNamed["imon_id_readback"]        =Register(  0x4000051C, 0x000000FF, 0x00000000, 1 );    
    instance->fNamed["imon_control"]            =Register(  0x40000520, 0x00010100, 0x00010001, 1 );    
    instance->fNamed["imon_status"]             =Register(  0x40000524, 0xFFFFFFFF, 0x00000000, 1 );    
    instance->fNamed["imon_bias"]               =Register(  0x40000540, 0xFFFFFFFF, 0x00000000, 12);    
    instance->fNamed["imon_value"]              =Register(  0x40000580, 0xFFFFFFFF, 0x00000000, 8 );    
    instance->fNamed["board_id"]                =Register(  0x80000000, 0xFFFFFFFF, 0x00000000, 1 );
    instance->fNamed["fifo_control"]            =Register(  0x80000004, 0xFFFFFFFF, 0x00000000, 1 );
    instance->fNamed["dp_fpga_fw_build"]        =Register(  0x80000010, 0xFFFFFFFF, 0x00000000, 1 );
    instance->fNamed["calib_build"]             =Register(  0x80000014, 0x0FFFFFFF, 0x08000000, 1 );
    instance->fNamed["dp_clock_status"]         =Register(  0x80000020, 0xFFFFFFFF, 0x00000000, 1 );     
    instance->fNamed["module_id"]               =Register(  0x80000024, 0x00000FFF, 0x00000FFF, 1 );     
    instance->fNamed["c2c_slave_status"]        =Register(  0x80000030, 0xFFFFFFFF, 0x00000000, 1 );     
    instance->fNamed["c2c_slave_intr_control"]  =Register(  0x80000034, 0xFFFFFFFF, 0x0000000F, 1 );
    instance->fNamed["channel_control"]         =Register(  0x80000040, 0xFFFFFFFF, 0xFFFFFFFF, 12);     
    instance->fNamed["led_threshold"]           =Register(  0x80000080, 0x00FFFFFF, 0x00FFFFFF, 12);
    instance->fNamed["cfd_parameters"]          =Register(  0x800000C0, 0x00001FFF, 0x00001FFF, 12);     
    instance->fNamed["readout_pretrigger"]      =Register(  0x80000100, 0x000007FF, 0x000007FF, 12);
    instance->fNamed["readout_window"]          =Register(  0x80000140, 0x000007FE, 0x000007FE, 12);     
    instance->fNamed["p_window"]                =Register(  0x80000180, 0x000003FF, 0x000003FF, 12);     
    instance->fNamed["i2_window"]               =Register(  0x800001C0, 0x000003FF, 0x000003FF, 12);     
    instance->fNamed["m1_window"]               =Register(  0x80000200, 0x000003FF, 0x000003FF, 12);
    instance->fNamed["m2_window"]               =Register(  0x80000240, 0x0000007F, 0x0000007F, 12);
    instance->fNamed["d_window"]                =Register(  0x80000280, 0x0000007F, 0x0000007F, 12);     
    instance->fNamed["i1_window"]               =Register(  0x800002C0, 0x000003FF, 0x000003FF, 12);     
    instance->fNamed["disc_width"]              =Register(  0x80000300, 0x0000FFFF, 0x0000FFFF, 12);
    instance->fNamed["baseline_start"]          =Register(  0x80000340, 0x00003FFF, 0x00003FFF, 12);     
    instance->fNamed["c_window"]                =Register(  0x80000380, 0x00003FFF, 0x00003FFF, 12);     
    instance->fNamed["trigger_input_delay"]     =Register(  0x80000400, 0x0000FFFF, 0x0000FFFF, 1 );     
    instance->fNamed["gpio_output_width"]       =Register(  0x80000404, 0x0000FFFF, 0x0000FFFF, 1 );     
    instance->fNamed["front_panel_config"]      =Register(  0x80000408, 0x00730333, 0x00730333, 1 );     
    instance->fNamed["channel_pulsed_control"]  =Register(  0x8000040C, 0x00000000, 0xFFFFFFFF, 1 );
    instance->fNamed["dsp_led_config"]          =Register(  0x80000410, 0xFFFFFFFF, 0x00000003, 1 );     
    instance->fNamed["dsp_led_input"]           =Register(  0x80000414, 0xFFFFFFFF, 0xFFFFFFFF, 1 );     
    instance->fNamed["baseline_delay"]          =Register(  0x80000418, 0x000000FF, 0x000000FF, 1 );
    instance->fNamed["diag_channel_input"]      =Register(  0x8000041C, 0xFFFFFFFF, 0xFFFFFFFF, 1 );     
    instance->fNamed["event_data_control"]      =Register(  0x80000424, 0x00020001, 0x00020001, 1 );     
    instance->fNamed["adc_config"]              =Register(  0x80000428, 0xFFFFFFFF, 0xFFFFFFFF, 1 );     
    instance->fNamed["adc_config_load"]         =Register(  0x8000042C, 0x00000000, 0x00000001, 1 );
    instance->fNamed["qi_config"]               =Register(  0x80000430, 0x0FFF1F11, 0x0FFF1F11, 1 );
    instance->fNamed["qi_delay"]                =Register(  0x80000434, 0x0000007F, 0x0000007F, 1 );
    instance->fNamed["qi_pulse_width"]          =Register(  0x80000438, 0x0000FFFF, 0x0000FFFF, 1 );
    instance->fNamed["qi_pulsed"]               =Register(  0x8000043C, 0x00000000, 0x00030001, 1 );
    instance->fNamed["external_gate_width"]     =Register(  0x80000440, 0x0000FFFF, 0x0000FFFF, 1 );
    instance->fNamed["pdts_cmd_control"]        =Register(  0x80000460, 0xFFFFFFFF, 0xFFFFFFFF, 3 );
    instance->fNamed["lat_timestamp_lsb"]       =Register(  0x80000484, 0xFFFFFFFF, 0x00000000, 1 );     
    instance->fNamed["lat_timestamp_msb"]       =Register(  0x80000488, 0x0000FFFF, 0x00000000, 1 );     
    instance->fNamed["live_timestamp_lsb"]      =Register(  0x8000048C, 0xFFFFFFFF, 0x00000000, 1 );
    instance->fNamed["live_timestamp_msb"]      =Register(  0x80000490, 0x0000FFFF, 0x00000000, 1 );
    instance->fNamed["sync_period"]             =Register(  0x80000494, 0xFFFFFFFF, 0x00000000, 1 );
    instance->fNamed["sync_delay"]              =Register(  0x80000498, 0xFFFFFFFF, 0x00000000, 1 );
    instance->fNamed["sync_count"]              =Register(  0x8000049C, 0xFFFFFFFF, 0x00000000, 1 ); 
    instance->fNamed["pdts_control"]            =Register(  0x800004C0, 0xFFFFFFFF, 0xFFFFFFFF, 1 );
    instance->fNamed["pdts_status"]             =Register(  0x800004C4, 0x00000000, 0xFFFFFFFF, 1 );
    instance->fNamed["pdts_ts_preset"]          =Register(  0x800004C8, 0x00000000, 0xFFFFFFFF, 2 );
    instance->fNamed["master_logic_control"]    =Register(  0x80000500, 0xFFFFFFFF, 0x00000173, 1 );     
    instance->fNamed["master_logic_status"]     =Register(  0x80000504, 0x00000001, 0x00000000, 1 );
    instance->fNamed["overflow_status"]         =Register(  0x80000508, 0xFFFFFFFF, 0x00000000, 1 );
    instance->fNamed["phase_value"]             =Register(  0x8000050C, 0xFFFFFFFF, 0x00000000, 1 );     
    instance->fNamed["link_tx_status"]          =Register(  0x80000510, 0xFFFFFFFF, 0x00000000, 1 );     
    instance->fNamed["dsp_clock_control"]       =Register(  0x80000520, 0x00000713, 0x00000713, 1 );
    instance->fNamed["dsp_clock_phase_control"] =Register(  0x80000524, 0x00000000, 0x00000007, 1 );
    instance->fNamed["code_revision"]           =Register(  0x80000600, 0xFFFFFFFF, 0x00000000, 1 );
    instance->fNamed["code_date"]               =Register(  0x80000604, 0xFFFFFFFF, 0x00000000, 1 );     
    instance->fNamed["dropped_event_count"]     =Register(  0x80000700, 0xFFFFFFFF, 0x00000000, 12);
    instance->fNamed["accepted_event_count"]    =Register(  0x80000740, 0xFFFFFFFF, 0x00000000, 12);
    instance->fNamed["ahit_count"]              =Register(  0x80000780, 0xFFFFFFFF, 0x00000000, 12);
    instance->fNamed["disc_count"]              =Register(  0x800007C0, 0xFFFFFFFF, 0x00000000, 12);
    instance->fNamed["idelay_count"]            =Register(  0x80000800, 0xFFFFFFFF, 0x00000000, 12);
    instance->fNamed["adc_data_monitor"]        =Register(  0x80000840, 0x0000FFFF, 0x00000000, 12);     
    instance->fNamed["adc_status"]              =Register(  0x80000880, 0xFFFFFFFF, 0x00000000, 12);
    // clang-format on
  }
  return *instance;
} // NOLINT(readability/fn_size)

#endif // SSPMODULES_SRC_ANLBOARD_REGMAP_CXX_
