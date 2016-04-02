
header_type ethernet_t {
	fields {
		dstAddr : 48;
		srcAddr : 48;
		etherType : 16;
	}
}

header_type prpl_t {
	fields {
		token : 8;
	}
}

header_type intrinsic_metadata_t {
    fields {
        learn_id : 4;
        mgid : 4;
    }
}
