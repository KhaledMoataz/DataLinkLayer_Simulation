#include "Node.h"
#include "Utils.h"
#include "Framing.h"
#include "Hamming.h"

void Node::apply(cMessage *msg)
{
    // Noisy channel logic
    std::string data = msg->getName();
    EV << data.size() << " bits: " << data << endl;

    double random = uniform(0, 1);
    double cp = 0;

    if (random < (cp += getAncestorPar("pLoss").doubleValue()))
    {
        lose(msg);
        return;
    }
    if (random < (cp += getAncestorPar("pReplicate").doubleValue()))
    {
        replicate(msg);
        return;
    }
    if (random < (cp += getAncestorPar("pDelay").doubleValue()))
    {
        delay(msg);
        return;
    }
    if (random < (cp += getAncestorPar("pModify").doubleValue()))
    {
        modify(msg);
        return;
    }
    normalSend(msg);
}

void Node::normalSend(cMessage* msg)
{
    makeFrame(msg);

    send(msg, "out");

    EV << "Message is untouched" << endl;
}

void Node::lose(cMessage* msg)
{
    delete msg;

    EV << "Message lost" << endl;
}

void Node::replicate(cMessage* msg)
{
    makeFrame(msg);

    send(msg, "out");
    send(new cMessage(*msg), "out");

    EV << "Message replicated" << endl;
}

void Node::delay(cMessage* msg)
{
    makeFrame(msg);

    double delay = exponential(1/getAncestorPar("lambda").doubleValue());
    sendDelayed(msg, delay, "out");

    EV << "Message delayed for " << delay << " time units" << endl;
}

void Node::modify(cMessage* msg)
{
    makeFrame(msg, true);

    send(msg, "out");

    EV << "Message modified" << endl;
}

void Node::makeFrame(cMessage* msg, bool modify)
{
    std::string frame = msg->getName();
    EV << "Frame before: " << frame << endl;
    if (modify)
    {
        int index = intuniformexcl(0, frame.size());
        Utils::char_not(frame[index]);
    }
    EV << "Frame after:  " << frame << endl;
    frame = Framing::addFlags(frame);
    frame = Utils::toCharString(frame);

    msg->setName((char*)(frame.c_str()));
}

