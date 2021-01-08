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
    void apply(cMessage*, std::string, int);
    void normalSend(cMessage*, std::string, int); // send the message unchanged
    void lose(cMessage*, std::string, int); // ignore the message
    void replicate(cMessage*, std::string, int);  // send the message twice
    void delay(cMessage*, std::string, int);  // send the message after a random delay
    void modify(cMessage*, std::string, int); // toggle a single random bit
    void makeFrame(cMessage*);
};

#endif /* NODE_H_ */
