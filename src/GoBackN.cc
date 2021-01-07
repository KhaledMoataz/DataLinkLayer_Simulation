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

#include "GoBackN.h"

Define_Module(GoBackN);

void GoBackN::initialize()
{
    maxWinSize = (1 << par("numBits").intValue()) - 1;
    seqN = 0;
    seqFirst = 0;
    frameExp = 0;
    localBuffer.resize(maxWinSize);
}

void GoBackN::handleMessage(cMessage *msg)
{
    if (msg->arrivedOn("ins", 0)) // New message from parent module to send a new line
    {
        // TODO: Read from file logic
        // Set peer value
        // transform to cmessages and push to main globalBuffer
        while (!globalBuffer.empty())
        {
            cMessage *msg = globalBuffer.front();
            globalBuffer.pop_back();
            while (isBusy());
            sendFrame(msg, true);
        }
    }
    else if (msg->isSelfMessage()) // Timeout
    {
        // Delete all old timers
        while(!timers.empty())
        {
            cMessage* timer = timers.front();
            cancelAndDelete(timer);
            timers.pop();
        }

        // Re-send Frames from seqFrist to N
        int winSize = calcSize(seqFirst, seqN);
        for (int i = 0; i<winSize; i++){
            sendFrame(localBuffer[(seqFirst + i) % maxWinSize]);
        }
    }
    else // Frame Arrival from Receiver
    {
        int ack = msg->par("ack").longValue();
        int seq = msg->par("seq").longValue();

        if (seq == frameExp)
        {
            receivedBuffer.push_back(msg);
            increment(frameExp);
        }

        // Squeeze the window and cancel all timers in between
        int winSize = calcSize(seqFirst, ack);
        while(!timers.empty() && winSize)
        {
            cMessage* timer = timers.front();
            cancelAndDelete(timer);
            timers.pop();
            increment(seqFirst);
            winSize--;
        }
    }

}

void GoBackN::sendFrame(cMessage *msg, bool firstTime)
{
    if (firstTime)
    {
        localBuffer[seqN] = msg;
    }
    // Attaches Acknowledge (Piggybacking) & Frame Sequence to the message
    // Then Sends the new message and sets a timer for it
    if (msg->findPar("seq") == -1)
    {
        msg->addPar("seq");
        msg->par("seq").setLongValue(seqN);
    }

    if (msg->findPar("ack") == -1)
    {
        msg->addPar("ack");
    }
    msg->par("ack").setLongValue(frameExp);

    msg->setKind(1);
    send(msg, "outs", peer);
    increment(seqN);

    // Create a timeout event
    cMessage* timer = new cMessage("");
    timers.push(timer);
    scheduleAt(simTime() + par("frameTimeout").doubleValue(), timer);
}

int GoBackN::calcSize(int x, int y)
{
    return (y >= x)? y - x : maxWinSize - x + y + 1;
}

bool GoBackN::isBusy()
{
    int winSize = calcSize(seqFirst, seqN);
    return winSize >= maxWinSize;
}

void GoBackN::increment(int & x)
{
    x = (x + 1) % (maxWinSize + 1);
}
