#ifndef NODE_H_
#define NODE_H_

#include <omnetpp.h>
using namespace omnetpp;

class Node: public cSimpleModule {
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
  private:
    // Noisy channel logic
    void apply(cMessage*);
    void normalSend(cMessage*); // send the message unchanged
    void lose(cMessage*); // ignore the message
    void replicate(cMessage*);  // send the message twice
    void delay(cMessage*);  // send the message after a random delay
    void modify(cMessage*); // toggle a single random bit
    void makeFrame(cMessage*, bool = false);
};

#endif /* NODE_H_ */
