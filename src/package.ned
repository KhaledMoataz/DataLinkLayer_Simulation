package wireshark;

@license(LGPL);

import ned.IdealChannel;

network Test
{
    parameters:
        int n = default(2);
    submodules:
        nodes[n]: GoBackN;
    connections: 
       for i=0..n-1, for j=i+1..n-1
        {
            nodes[i].outs++ --> IdealChannel --> nodes[j].ins++;
            nodes[i].ins++ <-- IdealChannel <-- nodes[j].outs++;
        }
}