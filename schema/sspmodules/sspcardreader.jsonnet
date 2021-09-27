// The schema used by classes in the appfwk code tests.
//
// It is an example of the lowest layer schema below that of the "cmd"
// and "app" and which defines the final command object structure as
// consumed by instances of specific DAQModule implementations (ie,
// the test/ssp* modules).

local moo = import "moo.jsonnet";

// A schema builder in the given path (namespace)
local ns = "dunedaq.sspmodules.sspcardreader";
local s = moo.oschema.schema(ns);

// Object structure used by the test/fake producer module
local sspcardreader = {
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

	//s.field("", self.name, "", doc=""),
        //s.field("", self.id, 0, doc=""),
        //s.field("", self.count, 0, doc=""),
        //s.field("", self.choice, 0, doc=""),

 	s.field("board_id", self.name, "",
		doc="Board ID used for configuration and metric tracking"),

        s.field("interface_type", self.count, 0,
                doc="connection interface type, 0 is USB, 1 is ethernet, 2 is emulated"),

 	s.field("board_ip", self.name, "",
		doc="For ethernet interfaces the IP address of the board, otherwise 0"),

        s.field("partition_number", self.count, 0,
                doc="the partition number of the SSP board"),

        s.field("timing_address", self.count, 0,
                doc="timing address of the SSP board"),

        s.field("pre_trig_length", self.count, 0,
                doc="Window length in ticks for packets to be included in a fragment. Length of the window before the trigger timestamp in ticks."),

        s.field("post_trig_length", self.count, 0,
                doc="Length of the window after the trigger timestamp in ticks."),

        s.field("use_external_timestamp", self.count, 0,
                doc="Whether to use the external timestamp to determine whether to include packets in fragment. Both timestamps are stored in the packets anyway. 0 is front panel, 1 is NOvA style"),

        s.field("trigger_write_delay", self.count, 0,
                doc="timing delay for when the data should be written out"),

        s.field("trigger_latency", self.count, 0,
                doc="uncertain what this does"),

        s.field("dummy_period", self.id, 0,
                doc="uncertain what this does"),

        s.field("hardware_clock_rate_in_MHz", self.count, 0,
                doc="clock rate on the hardware in MHz"),

        s.field("trigger_mask", self.count, 0,
                doc="this is normally given as a hex number for what triggers to mask on or off"),

        s.field("fragment_timestamp_offset", self.id, 0,
                doc="offset for the timestamp put into the data stream of this SSP"),

	s.field("hardware_configuration",self.hardwareconfiguration,
		doc="Hardware configuration for the SSP board."),

    ], doc="Upstream SSP CardReader DAQ Module Configuration"),

};

moo.oschema.sort_select(sspcardreader, ns)

