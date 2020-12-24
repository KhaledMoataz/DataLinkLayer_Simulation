#include "NoisyChannel.h"

Define_Module(NoisyChannel);

void NoisyChannel::initialize()
{
    // nothing to do here
    return;
}

void NoisyChannel::handleMessage(cMessage *msg)
{
    // Noisy channel logic
    std::string data = msg->getName();
    EV << data.size() << " bits: " << data << endl;
    if (bernoulli(par("pLoss").doubleValue()))
    {
        // ignore the message

        EV << "Message lost" << endl;

        delete msg;
        return;
    }
    if (bernoulli(par("pReplicate").doubleValue()))
    {
        // send the message twice
        send(msg, "out");
        send(new cMessage(*msg), "out");

        EV << "Message replicated" << endl;

        return;
    }
    if (bernoulli(par("pDelay").doubleValue()))
    {
        double interval = exponential(1/par("lambda").doubleValue());
        scheduleAt(simTime() + interval, msg);

        EV << "Message delayed for " << interval << " time units" << endl;

        return;
    }
    int position;
    if ((position = geometric(par("pModify").doubleValue())) < data.size())
    {
        data[position] = '1' - data[position] + '0';
        msg->setName((char*)(data.c_str()));
        send(msg, "out");

        EV << "Message modified at " << position << " to '" << data << "'" << endl;

        return;
    }
    // send the message normally
    send(msg, "out");

    EV << "Message is untouched" << endl;
}
