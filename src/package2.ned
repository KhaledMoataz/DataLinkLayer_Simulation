package wireshark;

@license(LGPL);

import ned.IdealChannel;

network Mesh
{
    parameters:
        int n = default(4);
    submodules:
        nodes[n]: Node;
    connections:
        for i=0..n-1, for j=i+1..n-1
        {
            nodes[i].out++ --> IdealChannel --> nodes[j].in++;
            nodes[i].in++ <-- IdealChannel <-- nodes[j].out++;
        }
}