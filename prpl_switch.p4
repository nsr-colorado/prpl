//
// Policy Routing using Process-level Identifiers (PRPL)
// Oliver Michel (oliver dot michel at colorado dot edu)
// Networking and Security Research Group
// University of Colorado Boulder
//

#include "p4_include/headers.p4"
#include "p4_include/parsers.p4"

header ethernet_t ethernet;
header prpl_t prpl;

metadata intrinsic_metadata_t intrinsic_metadata;

action _drop() { drop(); }
action _nop() { }

action forward(port) {
	modify_field(standard_metadata.egress_port, port);
}

action broadcast() {
	modify_field(intrinsic_metadata.mgid, 1);
}

table prpl {
	reads {
		prpl.token : exact;
	}
	actions { _nop; _drop; }
	size : 128;
}

table dmac {
	reads {
		ethernet.dstAddr : exact;
	}
	actions { forward; broadcast; }
	size : 512;
}

control ingress {
	apply(prpl);
	apply(dmac);
}

