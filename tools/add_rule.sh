iptables -A OUTPUT -t mangle -d 10.0.0.10 -j MARK --set-mark 0x1
iptables -A OUTPUT -t mangle -m owner --uid-owner 0 -j MARK --set-mark 0x99
ip rule add from all fwmark 0x1 lookup prpl1
ip rule add from all fwmark 0x99 lookup main
ip route add default dev tun0 table prpl1
