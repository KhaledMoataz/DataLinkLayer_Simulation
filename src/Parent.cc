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

#include "Parent.h"

Define_Module(Parent);

void Parent::initialize()
{
    // All nodes area Available
    for(int i=0; i<gateSize("ins"); i++)
        available.push_back(i);

    sessionId = 0;

    numGeneratedFrames = 0;
    numDroppedFrames = 0;
    numRetransmittedFrames = 0;
    usefulData = 0;
    totalData = 0;
    scheduleAt(simTime() + 1.0, new cMessage("self"));
    for (auto &id : finishedSessions)
    {
        id = -1;
    }
}

void Parent::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())      // Select pairs for connections
    {
//        EV <<"in sessionId = "<<sessionId<<'\n';

        // 0 or 1 (50% probability for choosing pairs or not)
        if(intuniform(0, 1))
        {
            // if 1 choose random number between (1, available_size/2)
            int num = intuniform(0, available.size()/2);
//            EV <<"The Parent chooses "<<num<<" pairs"<<'\n'<<'\n';

            int randIndex1,randIndex2,index1Value,index2Value;
            // for each pair choose two random numbers between (0, available_size)
            // (delete the pair we chose )
            for(int i=0; i<num; i++)
            {
                // choose random sender
                randIndex1 = intuniformexcl(0, available.size());
                index1Value = available[randIndex1];
                // delete the sender from available
                available.erase(available.begin() + randIndex1);

                // choose random receiver
                randIndex2 = intuniformexcl(0, available.size());
                index2Value = available[randIndex2];
                // delete the receiver from available
                available.erase(available.begin() + randIndex2);

                // send message to sender
                std::string str =  std::to_string(index2Value)+" "+std::to_string(sessionId);
                send(new cMessage(((char*)(str.c_str()))),"outs",index1Value);
//                EV <<index1Value<<", ";

                // send message to receiver
                str = std::to_string(index1Value)+" "+std::to_string(sessionId);
                send(new cMessage(((char*)(str.c_str()))),"outs",index2Value);
//                EV <<index2Value<<'\n';

                // start session logs
                EV << "New session started between nodes: <" << index1Value << ", " << index2Value << ">\n";

                sessionId = (sessionId + 1) % 256;
            }
        }
        else
//            EV <<"The Parent didn't choose any pairs"<<'\n'<<'\n';

        if (simTime() >= 180 && simTime() < 200)
        {
            EV << "==============Statistics================\n";
            EV << "Num Generated Frames: " << numGeneratedFrames << '\n';
            EV << "Num Dropped Frames: " << numDroppedFrames << '\n';
            EV << "Num Retransmitted Frames: " << numRetransmittedFrames << '\n';
            EV << "Percentage of useful data: " << (100.0 * usefulData) / totalData << "%\n";
        }
        scheduleAt(simTime() + par("sleepTime"), new cMessage("self"));
    }
    else     // Node ended a connection
    {
        int message = atoi(msg->getName());
        int idx = message % 100;
        message /= 100;
        int nGF = message % 100;
        message /= 100;
        int nDF = message % 100;
        message /= 100;
        int nRF = message % 100;
        message /= 100;
        available.push_back(idx);

        int uD = msg->par("usefulData");
        int tD = msg->par("totalData");

        int sessionID = msg->par("sessionID");

        if (finishedSessions[sessionsID] == -1)
        {
            finishedSessions[sessionsID] = idx;
        }
        else
        {
            EV << "Session between nodes: <" << idx << ", " << finishedSessions[sessionsID] << "> has ended.\n";
        }

        numGeneratedFrames += nGF;
        numDroppedFrames += nDF;
        numRetransmittedFrames += nRF;
        usefulData += uD;
        totalData += tD;
    }
    cancelAndDelete(msg);
}
