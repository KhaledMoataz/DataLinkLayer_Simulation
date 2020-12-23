package wireshark;

@license(LGPL);

import ned.IdealChannel;

network Test
{
    submodules:
        nodes[2]: GoBackN;
    connections: 
       nodes[0].out --> IdealChannel --> nodes[1].in; 
       nodes[1].out --> IdealChannel --> nodes[0].in; 
}