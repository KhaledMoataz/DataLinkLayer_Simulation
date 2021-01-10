//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __WIRESHARK_GOBACKN_H_
#define __WIRESHARK_GOBACKN_H_

#include <omnetpp.h>
#include <vector>
#include <queue>
#include <string>
#include <fstream>

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class GoBackN : public cSimpleModule
{
private:
    int maxWinSize;
    int seqN;               // The end of the sliding window
    int seqFirst;           // The beginning of the sliding window
    int frameExp;           // Expected frame in the receiver side
    int peer;               // Which node I'm currently communicating with
    int index;              // For Indexing Local Buffer
    int sessionId;          // Id of the current communication session.
    cMessage* lastMessage;  // Last Continue Message for self alerting loop

    std::vector<std::pair<std::string, int>> localBuffer;   // Current sending window
    std::queue<cMessage*> timers;                           // Timers Queue for sent frames
    std::queue<std::string> globalBuffer;                   // All messages buffer
    std::queue<std::string> receivedBuffer;                 // Received messaged buffer

    std::ifstream file;     // File reader for the current node
    int numGeneratedFrames;
    int numDroppedFrames;
    int numRetransmittedFrames;
    int usefulData;
    int totalData;

    void increment(int & x);
    void sendFrame(std::string msg, int seq, bool firstTime=false);
    bool isBusy();              // Checks if the window is full
    int calcSize(int x, int y);
    void printAndClear();       // Prints the received frames and clears everything
    void loopAlert();           // Self message for looping over the global buffer

    // Noisy Channel Functions
    void apply(cMessage*, std::string, int);        // Send a message and apply a random noise
    void normalSend(cMessage*, std::string, int);   // send the message unchanged
    void lose(cMessage*, std::string, int);         // ignore the message
    void replicate(cMessage*, std::string, int);    // send the message twice
    void delay(cMessage*, std::string, int);        // send the message after a random delay
    void modify(cMessage*, std::string, int);       // toggle a single random bit

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
