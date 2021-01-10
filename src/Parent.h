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

#ifndef __WIRESHARK_PARENT_H_
#define __WIRESHARK_PARENT_H_

#include <omnetpp.h>
#include <vector>

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Parent : public cSimpleModule
{
public:
    std::vector<int> available;
    int sessionId;
    int numGeneratedFrames;
    int numDroppedFrames;
    int numRetransmittedFrames;
    int usefulData;
    int totalData;
    int finishedSessions[256];

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
