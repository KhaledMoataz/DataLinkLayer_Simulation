package wireshark;

simple NoisyChannel
{
    parameters:
		// geometric distribution for the bit to be modified (no modification if out of bounds)
        double pModify = default(0.01);
        // bernoulli distribution for each of loss, replication and delay probabilities
        double pLoss = default(0.05);
        // replication probability
        double pReplicate = default(0.05);
        // delay probability
        double pDelay = default(0.05);
        // exponential distribution for the duration of the delay
        double lambda = default(0.5);
    gates:
        input in;
        output out;
}
