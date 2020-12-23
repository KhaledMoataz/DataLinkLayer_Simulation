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
    buffer.resize(maxWinSize);
    scheduleAt(simTime() + par("ackTimeout").doubleValue(), new cMessage("Test", 0));
}

void GoBackN::handleMessage(cMessage *msg)
{
    if (msg->getKind() == 0) // New Frame from Network Layer
    {
        buffer[seqN] = msg;
        sendFrame(msg);
    }
    else if (msg->getKind() == 1) // Frame Arrival from Receiver
    {
        EV << "HERE" << '\n';
        int ack = msg->par("ack").longValue();
        int seq = msg->par("seq").longValue();

        if (seq == frameExp)
        {
            send(msg, "internalIn");
            increment(frameExp);
            scheduleAt(simTime() + par("ackTimeout").doubleValue(), new cMessage("", 4));
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
    else if (msg->getKind() == 2) // Timeout
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
            sendFrame(buffer[(seqFirst + i) % maxWinSize]);
        }
    }
    else if (msg->getKind() == 3) // Check for busy
    {
        if (isBusy()) send(new cMessage("1", 0), "internalOut");
        else send(new cMessage("0", 0), "internalOut");
    }
    else if (msg->getKind() == 4) // Send Acknowledge
    {
        sendFrame(new cMessage());
    }
}

void GoBackN::sendFrame(cMessage *msg, bool empty)
{
    // Attaches Acknowledge (Piggybacking) & Frame Sequence to the message
    // Then Sends the new message and sets a timer for it
    if (!empty && msg->findPar("seq") == -1)
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
    send(msg, "out");

    if (!empty)
    {
        increment(seqN);

        // Create a timeout event of type 2
        cMessage* timer = new cMessage("", 2);
        timers.push(timer);
        scheduleAt(simTime() + par("frameTimeout").doubleValue(), timer);
    }
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
