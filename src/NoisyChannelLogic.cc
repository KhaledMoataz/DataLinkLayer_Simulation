#include "GoBackN.h"
#include "Utils.h"
#include "Framing.h"

void GoBackN::apply(cMessage *msg, std::string gate, int port)
{
    // Noisy channel logic
    std::string data = msg->getName();
    EV << data.size() << " bits: " << data << endl;

    double random = uniform(0, 1);
    double cp = 0;

    if (random < (cp += getAncestorPar("pLoss").doubleValue()))
    {
        lose(msg, gate, port);
        return;
    }
    if (random < (cp += getAncestorPar("pReplicate").doubleValue()))
    {
        replicate(msg, gate, port);
        return;
    }
    if (random < (cp += getAncestorPar("pDelay").doubleValue()))
    {
        delay(msg, gate, port);
        return;
    }
    if (random < (cp += getAncestorPar("pModify").doubleValue()))
    {
        modify(msg, gate, port);
        return;
    }
    normalSend(msg, gate, port);
}

void GoBackN::normalSend(cMessage* msg, std::string gate, int port)
{
    makeFrame(msg);

    send(msg, gate.c_str(), port);

    EV << "Message is untouched" << endl;
}

void GoBackN::lose(cMessage* msg, std::string gate, int port)
{
    delete msg;

    EV << "Message lost" << endl;
}

void GoBackN::replicate(cMessage* msg, std::string gate, int port)
{
    makeFrame(msg);

    send(msg, gate.c_str(), port);
    send(new cMessage(*msg), gate.c_str(), port);

    EV << "Message replicated" << endl;
}

void GoBackN::delay(cMessage* msg, std::string gate, int port)
{
    makeFrame(msg);

    double delay = exponential(1/getAncestorPar("lambda").doubleValue());
    sendDelayed(msg, delay, gate.c_str(), port);

    delayedMessages.push_back(msg);

    EV << "Message delayed for " << delay << " time units" << endl;
}

void GoBackN::modify(cMessage* msg, std::string gate, int port)
{
    std::string frame = msg->getName();
    int index = intuniformexcl(0, frame.size());
    Utils::char_not(frame[index]);
    msg->setName((char*)(frame.c_str()));

    makeFrame(msg);

    send(msg, gate.c_str(), port);

    EV << "Message modified at " << index << " to " << frame << endl;
}

void GoBackN::makeFrame(cMessage* msg)
{
    std::string frame = msg->getName();
    frame = Framing::addFlags(frame);
//    frame = Utils::toCharString(frame);
    msg->setName((char*)(frame.c_str()));
}

