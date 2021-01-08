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
    index = 0;
    lastMessage = nullptr;
    localBuffer.resize(maxWinSize);
    if (getIndex() == 0)
    {
        scheduleAt(simTime() + 1.0, new cMessage("start"));
    }
    else
    {
        scheduleAt(simTime() + 1.0, new cMessage("start"));
    }
}

void GoBackN::handleMessage(cMessage *msg)
{
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
            globalBuffer.push("Hi200");
            globalBuffer.push("Hi300");
            globalBuffer.push("Hi400");
            globalBuffer.push("Hi500");
            globalBuffer.push("Hi600");
            globalBuffer.push("Bye100");
        }
        if (globalBuffer.empty())
        {
            cMessage *u = new cMessage("End");
            send(u, "outs", peer);
            printAndClear();
        }
        else
        {
            loopAlert();
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
            loopAlert();
        }
        cancelAndDelete(msg);
    }
    else if (strcmp(msg->getName(), "End") == 0) // Peer doesn't have any data
    {
        printAndClear();
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
            receivedBuffer.push(msg->getName());
            increment(frameExp);
        }

        // Squeeze the window and cancel all timers in between
        int winSize = calcSize(seqFirst, ack);
        if (isBusy() && !globalBuffer.empty())
        {
            loopAlert();
        }
        while(!timers.empty() && winSize)
        {
            cMessage* timer = timers.front();
            cancelAndDelete(timer);
            timers.pop();
            increment(seqFirst);
            winSize--;
        }

        if (index && globalBuffer.empty() && seqFirst == seqN) // I Ended my messages, terminate communication
        {
            cMessage *u = new cMessage("End");
            send(u, "outs", peer);
            lastMessage = nullptr;
            printAndClear();
        }
        cancelAndDelete(msg);
    }
}

void GoBackN::sendFrame(std::string frame, bool firstTime)
{
    cMessage* msg = new cMessage((char*)(frame.c_str()));

    if (firstTime)
    {
        localBuffer[index++ % maxWinSize] = frame;
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
    if (firstTime)
    {
        increment(seqN);
    }

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

void GoBackN::printAndClear()
{
    // Prints the received frames and clears everything
    while(!receivedBuffer.empty())
    {
        std::string u = receivedBuffer.front();
        receivedBuffer.pop();
        EV << u << ' ';
    }
    EV << '\n';
    if (!isBusy())
    {
        cancelAndDelete(lastMessage);
    }
    seqN = 0;
    seqFirst = 0;
    frameExp = 0;
    index = 0;
    lastMessage = nullptr;
    while(!timers.empty())
    {
        cMessage* timer = timers.front();
        cancelAndDelete(timer);
        timers.pop();
    }
}

void GoBackN::loopAlert()
{
    // Wake my self again after a random interval to send a new frame
    double interFrameDelay = ((double) rand() / (RAND_MAX * 100.0));
    lastMessage = new cMessage("Continue");
    scheduleAt(simTime() + interFrameDelay, lastMessage);
}
