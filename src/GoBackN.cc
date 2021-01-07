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
    if (getIndex() == 0)
    {
        scheduleAt(simTime() + 1.0, new cMessage("start"));
    }
    else
    {
        scheduleAt(simTime() + 1.5, new cMessage("start"));
    }
}

void GoBackN::handleMessage(cMessage *msg)
{
    EV << "NEW MESSAGE" << '\n';

    if (strcmp(msg->getName(), "start") == 0) // New message from parent module to send a new line
    {
        // TODO: Read from file logic
        // Set peer value
        // transform to cmessages and push to main globalBuffer
        if (getIndex() == 0)
        {
            peer = 0;
            globalBuffer.push("Hi");
            globalBuffer.push("Hi2");
            globalBuffer.push("Hi3");
            globalBuffer.push("Hi4");
            globalBuffer.push("Hi5");
            globalBuffer.push("Hi6");
            globalBuffer.push("Bye");
        }
        else
        {
            peer = 0;
            globalBuffer.push("Hi100");
        }
        if (!globalBuffer.empty())
        {
            scheduleAt(simTime() + 0.00001, new cMessage("Continue"));
        }
        cancelAndDelete(msg);
    }
    else if (strcmp(msg->getName(), "Continue") == 0) // Network Layer Ready, Global Buffer Not Empty
    {
        std::string s = globalBuffer.front();
        globalBuffer.pop();
        EV << "Sending.." << s << '\n';
        sendFrame(s, true);
        if (!globalBuffer.empty() && !isBusy())
        {
            scheduleAt(simTime() + 0.00001, new cMessage("Continue"));
            EV << "Scheduled!!" << '\n';
        }
        cancelAndDelete(msg);
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
        int winSize = calcSize(seqFirst, seqN % (maxWinSize + 1));
        EV << winSize << '\n';
        for (int i = 0; i<winSize; i++){
            sendFrame(localBuffer[(seqFirst + i) % maxWinSize]);
        }
    }
    else // Frame Arrival from Receiver
    {
        EV << "Received" << '\n';
        int ack = msg->par("ack").longValue();
        int seq = msg->par("seq").longValue();

        if (seq == frameExp)
        {
            receivedBuffer.push(msg->getName());
            increment(frameExp);
        }

        // Squeeze the window and cancel all timers in between
        int winSize = calcSize(seqFirst, ack);
        if (isBusy() && !globalBuffer.empty())
        {
            EV << "Was Busy" << '\n';
            scheduleAt(simTime() + 0.00001, new cMessage("Continue"));
        }
        while(!timers.empty() && winSize)
        {
            cMessage* timer = timers.front();
            cancelAndDelete(timer);
            timers.pop();
            increment(seqFirst);
            winSize--;
        }
        EV << "FREE" << '\n';
        cancelAndDelete(msg);
    }
}

void GoBackN::sendFrame(std::string frame, bool firstTime)
{
    cMessage* msg = new cMessage((char*)(frame.c_str()));

    if (firstTime)
    {
        localBuffer[seqN % maxWinSize] = frame;
    }
    // Attaches Acknowledge (Piggybacking) & Frame Sequence to the message
    // Then Sends the new message and sets a timer for it
    if (msg->findPar("seq") == -1)
    {
        msg->addPar("seq");
        msg->par("seq").setLongValue(seqN % (maxWinSize + 1));
    }

    if (msg->findPar("ack") == -1)
    {
        msg->addPar("ack");
    }
    msg->par("ack").setLongValue(frameExp);

    msg->setKind(1);

    send(msg, "outs", peer);
    if (firstTime)
    {
        seqN++;
    }

    // Create a timeout event
    cMessage* timer = new cMessage("");
    timers.push(timer);
    EV << simTime() << '\n';
    scheduleAt(simTime() + par("frameTimeout").doubleValue(), timer);
}

int GoBackN::calcSize(int x, int y)
{
    return (y >= x)? y - x : maxWinSize - x + y + 1;
}

bool GoBackN::isBusy()
{
    int winSize = calcSize(seqFirst, seqN % (maxWinSize + 1));
    return winSize >= maxWinSize;
}

void GoBackN::increment(int & x)
{
    x = (x + 1) % (maxWinSize + 1);
}
