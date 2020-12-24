#ifndef __LAB02_HUB_H_
#define __LAB02_HUB_H_

#include <omnetpp.h>

using namespace omnetpp;

class NoisyChannel : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
