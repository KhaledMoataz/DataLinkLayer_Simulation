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

    scheduleAt(simTime() + 1.0, new cMessage("self"));
}

void Parent::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())      // Select pairs for connections
    {
        EV <<"in sessionId = "<<sessionId<<'\n';

        // 0 or 1 (50% probability for choosing pairs or not)
        if(intuniform(0, 1))
        {
            // if 1 choose random number between (1, available_size/2)
            int num = intuniform(0, available.size()/2);
            EV <<"The Parent chooses "<<num<<" pairs"<<'\n'<<'\n';

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
                EV <<index1Value<<", ";

                // send message to receiver
                str = std::to_string(index1Value)+" "+std::to_string(sessionId);
                send(new cMessage(((char*)(str.c_str()))),"outs",index2Value);
                EV <<index2Value<<'\n';

                sessionId++;
            }
        }
        else
            EV <<"The Parent didn't choose any pairs"<<'\n'<<'\n';

        scheduleAt(simTime() + par("sleepTime"), new cMessage("self"));
    }
    else     // Node ended a connection
    {
        available.push_back(atoi(msg->getName()));

        EV <<"The Parent received end session from Node "<<atoi(msg->getName())<<'\n'<<'\n';

    }
    cancelAndDelete(msg);
}