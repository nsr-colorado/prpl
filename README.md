# Policy Routing using Process-Level Identifiers (PRPL)

## Paper Abstract

Enforcing and routing based on network-wide policies remains a crucial
challenge in the operation of large-scale enterprise and datacenter
networks. As current dataplane devices solely rely on layer 2 - layer 4
identifiers to make forwarding decisions, there is no notion of the exact
origin of a packet in terms of the sending user or process. In this paper
we ask the question: Can we go beyond the MAC? That is, can
fine-grained process-level information like user ids, process ids or a
cryptographic hash of the sending executable be semantically used to make
forwarding decisions within the network? Toward this goal, we present a
system enabling such capabilities without the need for modification in
applications or the operating system's networking stack. We implemented an
early prototype leveraging the P4 technology for protocol-independent
packet processing and forwarding in conjunction with on-board tools of the
Linux operating system. We finally evaluate our system with regards to
practicability and discuss the performance-behavior of our implementation.

