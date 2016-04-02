
CFLAGS+=-Wall

all: prpl_agent prpl_switch.json

prpl_agent: prpl_agent.c
	$(CC) $(CFLAGS) $< -o $@

prpl_switch.json: prpl_switch.p4
	p4c-bmv2 --json $@ $<

clean:
	$(RM) *.pcap

spotless: clean
	$(RM) prpl_agent
	$(RM) prpl_switch.json

