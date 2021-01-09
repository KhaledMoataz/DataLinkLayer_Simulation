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
#include "Utils.h"
#include "Framing.h"

Define_Module(GoBackN);

void GoBackN::initialize()
{
    maxWinSize = (1 << par("numBits").intValue()) - 1;
    seqN = 0;
    seqFirst = 0;
    frameExp = 0;
    index = INT_MIN;
    lastMessage = nullptr;
    localBuffer.resize(maxWinSize);
    file.open("../data/"+std::to_string(getIndex()), std::ifstream::in);
}

void GoBackN::handleMessage(cMessage *msg)
{
    if (msg->arrivedOn("ins",gateSize("ins")-1)) // New message from parent module to send a new line
    {
        // TODO: Read from file logic
        // Set peer value
        // transform to cmessages and push to main globalBuffer

        std::string str = msg->getName();
        peer = std::stoi(str.substr(0,str.find(" ")));
        if(peer > getIndex())
            peer--;

        sessionId = std::stoi(str.substr(str.find(" ")+1));


        // if the file ended, open it again
        if(file.peek()==EOF)
            file.open("../data/"+std::to_string(getIndex()), std::ifstream::in);

        // read a line from the file
        std::string line;
        std::getline(file,line,'\n');

        EV<<line<<'\n';

        Utils::getFramesToSend(line,globalBuffer);


         if (globalBuffer.empty())
        {
            cMessage *u = new cMessage("End");
            u->addPar("session");
            u->par("session").setLongValue(sessionId);
            send(u, "outs", peer);
            printAndClear();

        }
        else
        {
            index = 0;
            loopAlert();
        }
        cancelAndDelete(msg);
    }
    else if (strcmp(msg->getName(), "Continue") == 0) // Network Layer Ready, Global Buffer Not Empty
    {
        std::string s = globalBuffer.front();
        globalBuffer.pop();
        EV << "Sending: ";
        sendFrame(s, seqN, true);
        if (!globalBuffer.empty() && !isBusy())
        {
            loopAlert();
        }
        else
        {
            lastMessage = nullptr;
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
        int winSize = calcSize(seqFirst, seqN);
        int timeoutIndex = (index - winSize) % maxWinSize;
        for (int i = 0; i<winSize; i++){
            sendFrame(localBuffer[(timeoutIndex + i) % maxWinSize].first,
                    localBuffer[(timeoutIndex + i) % maxWinSize].second);
        }
    }
    else if (msg->par("session").longValue() != sessionId);
    else if (strcmp(msg->getName(), "End") == 0) // Peer doesn't have any data
    {
        printAndClear();
    }
    else // Frame Arrival from Receiver
    {
        int ack = msg->par("ack").longValue();
        int seq = msg->par("seq").longValue();

        if (seq == frameExp)
        {

            std::string s = Utils::decodeFrame(msg->getName());
            EV << "Received Raw: " << std::string(msg->getName()).size() << ", " << msg->getName() << '\n';
            EV << "Received Bits: " << Utils::toBinary(msg->getName()) << '\n';
            EV << "Received Message: " << s << '\n';
            receivedBuffer.push(msg->getName());
            increment(frameExp);
        }

        // Squeeze the window and cancel all timers in between
        int winSize = calcSize(seqFirst, ack);
        winSize = (winSize > calcSize(seqFirst, seqN)) ? 0 : winSize;
        EV << "Local Buffer Size: " << calcSize(seqFirst, seqN) << '\n';
        EV << "Ack Size: " << calcSize(seqFirst, ack) << '\n';
        if (winSize && isBusy() && !globalBuffer.empty())
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

        if (index > 0 && globalBuffer.empty() && seqFirst == seqN) // I Ended my messages, terminate communication
        {
            cMessage *u = new cMessage("End");
            u->addPar("session");
            u->par("session").setLongValue(sessionId);
            send(u, "outs", peer);
            printAndClear();

        }
        cancelAndDelete(msg);
    }
}

void GoBackN::sendFrame(std::string frame, int seq, bool firstTime)
{
    std::string decodedFrame = Utils::decodeFrame(Framing::addFlags(frame));
    EV << decodedFrame << '\n';

    cMessage* msg = new cMessage((char*)(frame.c_str()));
    EV << "Sending Raw: " << msg->getName() << '\n';
    if (firstTime)
    {
        localBuffer[index++ % maxWinSize] = {frame, seq};
        increment(seqN);
    }
    // Attaches Acknowledge (Piggybacking) & Frame Sequence to the message
    // Then Sends the new message and sets a timer for it
    msg->addPar("seq");
    msg->par("seq").setLongValue(seq);

    msg->addPar("ack");
    msg->par("ack").setLongValue(frameExp);

    msg->addPar("session");
    msg->par("session").setLongValue(sessionId);
    EV << seq << '\n';
    apply(msg, "outs", peer);
    EV << isBusy() << '\n';
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
    std::string conversation = Utils::decodeFrames(receivedBuffer);
    EV << conversation << '\n';
    cancelAndDelete(lastMessage);

    seqN = 0;
    seqFirst = 0;
    frameExp = 0;
    index = INT_MIN;
    sessionId = -1;
    lastMessage = nullptr;
    while(!timers.empty())
    {
        cMessage* timer = timers.front();
        cancelAndDelete(timer);
        timers.pop();
    }

    // send end session to the parent
    std::string myIndex = std::to_string(getIndex());
    send(new cMessage(((char*)(myIndex.c_str()))), "outs", gateSize("ins")-1);
}

void GoBackN::loopAlert()
{
    // Wake my self again after a random interval to send a new frame
    double interFrameDelay = ((double) rand() / (RAND_MAX * 100.0));
    lastMessage = new cMessage("Continue");
    scheduleAt(simTime() + interFrameDelay, lastMessage);
}
