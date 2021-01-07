package wireshark;

@license(LGPL);

import ned.IdealChannel;

network Mesh
{
    parameters:
        int n = default(4);
        
		// geometric distribution for the bit to be modified (no modification if out of bounds)
        double pModify = default(0.01);
        // bernoulli distribution for each of loss, replication and delay probability
        double pLoss = default(0.05);
        // replication probability
        double pReplicate = default(0.05);
        // delay probability
        double pDelay = default(0.05);
        // exponential distribution for the duration of the delay
        double lambda = default(0.5);
    submodules:
        nodes[n]: Node;
    connections:
        for i=0..n-1, for j=i+1..n-1
        {
            nodes[i].out++ --> IdealChannel --> nodes[j].in++;
            nodes[i].in++ <-- IdealChannel <-- nodes[j].out++;
        }
}