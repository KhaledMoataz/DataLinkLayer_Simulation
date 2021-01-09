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

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class GoBackN : public cSimpleModule
{
private:
    int maxWinSize, seqN, seqFirst;
    int frameExp;
    int peer; // Which node I'm currently communicating with
    int index; // For Indexing Local Buffer
    cMessage* lastMessage; // Last Continue Message

    std::vector<std::string> localBuffer;
    std::queue<cMessage*> timers;
    std::queue<std::string> globalBuffer, receivedBuffer;

    void increment(int & x);
    void sendFrame(std::string msg, bool firstTime=false);
    bool isBusy();
    int calcSize(int x, int y);
    void printAndClear();
    void loopAlert();
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
