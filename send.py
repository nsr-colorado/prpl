#!/usr/bin/env python

from scapy.all import *

class PRPL(Packet):
    name = "PRPL "
    fields_dest = [
            XByteField('token', 0)
    ]

p1 = Ether(src='00:00:00:00:00:01', dst='00:00:00:00:00:02')/PRPL('a')
p2 = Ether(src='00:00:00:00:00:02', dst='00:00:00:00:00:01')/PRPL('b')
p3 = Ether(src='00:00:00:00:00:01', dst='00:00:00:00:00:02')/PRPL('c')

sendp(p1, iface='veth0')
sendp(p2, iface='veth2')
sendp(p3, iface='veth0')
