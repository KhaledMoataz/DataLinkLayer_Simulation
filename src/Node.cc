#include "Node.h"

Define_Module(Node);

void Node::initialize()
{
    double interval = exponential(1 / par("lambda").doubleValue());
    scheduleAt(simTime() + interval, new cMessage(""));
}


void Node::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) { //Host wants to send

            int rand, dest;
            do { //Avoid sending to yourself
                rand = uniform(0, gateSize("out"));
            } while(rand == getIndex());

            //Calculate appropriate gate number
            dest = rand;
            if (rand > getIndex())
                dest--;

            std::stringstream ss;
            ss << rand;
            EV << "Sending "<< ss.str() <<" from source " << getIndex() << "\n";
            delete msg;
            msg = new cMessage(ss.str().c_str());
            // send(msg, "out", dest);
//            apply(msg, "out", dest);

            double interval = exponential(1 / par("lambda").doubleValue());
            EV << ". Scheduled a new packet after " << interval << "s";
            scheduleAt(simTime() + interval, new cMessage(""));
        }
        else {
            //atoi functions converts a string to int
            //Check if this is the proper destination
            if (atoi(msg->getName()) == getIndex())
                bubble("Message received");
            else
                bubble("Wrong destination");
            delete msg;
        }
}



/*
#########################################
        Noisy Channel Logic
#########################################

    TODO: refactor from NoisyChannelLogic.cc
*/
