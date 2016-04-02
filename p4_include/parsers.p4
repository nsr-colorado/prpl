
parser start {
    return parse_ethernet;
}

parser parse_ethernet {
    extract(ethernet);
	return parse_prpl;
}

parser parse_prpl {
	extract(prpl);
	return ingress;
}

