// The schema used by classes in the appfwk code tests.
//
// It is an example of the lowest layer schema below that of the "cmd"
// and "app" and which defines the final command object structure as
// consumed by instances of specific DAQModule implementations (ie,
// the test/ssp* modules).

local moo = import "moo.jsonnet";

// A schema builder in the given path (namespace)
local ns = "dunedaq.sspmodules.sspledcalibmodule";
local s = moo.oschema.schema(ns);

// Object structure used by the test/fake producer module
local sspledcalibmodule = {
    count  : s.number("Count", "u4",
                      doc="Count of things"),

    id : s.number("Identifier", "i4",
                  doc="An ID of a thingy"),

    choice : s.boolean("Choice"),

    name : s.string("Name"),

    hexvalues : s.sequence("HexValues", self.count,
    	           doc="The values to be stored in the array of registers"),

    registervalues : s.record("RegisterValues", [
    	     s.field("regname",self.name,""),
	     s.field("hexvalues",self.hexvalues),
	     ], doc="The name of regsiter(s) and sequence of hex values that register(s) should be set to. configuration does a switch looking for ALL, ARR, channel_control, and literal."),

    hardwareconfiguration : s.sequence("RegisterValuesSequence", self.registervalues,
    		    doc="Sequence of register name and values that are to be written to the SSP"),

    conf: s.record("Conf", [
        s.field("card_id", self.id, 0,
                doc="Physical card identifier (in the same host)"),

 	s.field("board_id", self.count, 0,
		doc="Board ID used for configuration and metric tracking"),

 	s.field("module_id", self.count, 0,
		doc="Board ID used for configuration and metric tracking"),

        s.field("interface_type", self.id, 1,
                doc="connection interface type, 0 is USB, 1 is ethernet, 2 is emulated"),

 	s.field("board_ip", self.name, "default",
		doc="For ethernet interfaces the IP address of the board, otherwise 0"),

        s.field("partition_number", self.count, 0,
                doc="the partition number of the SSP board"),

        s.field("timing_address", self.count, 0,
                doc="timing address of the SSP board"),

        s.field("number_channels", self.count, 12,
                doc="decimal number of the number of channels in the card, should be either 5 or 12"),

	s.field("channel_mask", self.count, 4095,
                doc="decimal number for the 12-bit channel mask where 1 is on, e.g. 4095 is all channels on "),

        s.field("pulse_mode", self.name, "default",
                doc="chose of the pulse mode either single, double, or burst"),

	s.field("burst_count", self.count, 1,
                doc="number of LED calib pulses to send in burst mode"),

	s.field("double_pulse_delay_ticks", self.count, 1,
                doc="number of ticks between first and second pulse in double pulse mode"),

	s.field("pulse1_width_ticks", self.count, 1,
                doc="width of the pulse in tick"),

	s.field("pulse2_width_ticks", self.count, 1,
                doc="width of the pulse in tick"),

	s.field("pulse_bias_percent_270nm", self.count, 0,
                doc="the fraction of bias to be applied to the 270nm LEDs"),

	s.field("pulse_bias_percent_367nm", self.count, 0,
                doc="the fraction of bias to be applied to the 367nm LEDs"),

	s.field("hardware_configuration",self.hardwareconfiguration,
		doc="Hardware configuration for the SSP board."),

    ], doc="SSP LED Calib DAQ Module Configuration"),

};

moo.oschema.sort_select(sspledcalibmodule, ns)

