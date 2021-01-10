//
// Created by ayman on 25/12/2020.
//

#include "Hamming.h"
#include "Utils.h"

int Hamming::redundantBitsCount(int messageSize) {
    int powersOfTwo = 1;
    int redundantBits = 0;
    while (powersOfTwo < messageSize + redundantBits + 1) {
        powersOfTwo *= 2;
        redundantBits += 1;
    }
    return redundantBits;
}

string Hamming::embedDummyHammingBits(const string &binaryMessage) {
    int m = binaryMessage.size();
    int r = redundantBitsCount(m);
    string outputMessage;
    int messageIndex = 0;
    int nextPowerOfTwo = 1;
    for (int i = 0; i < m + r; ++i) {
        if (i + 1 == nextPowerOfTwo) {
            outputMessage += '0';
            nextPowerOfTwo *= 2;
            continue;
        }
        outputMessage += binaryMessage[messageIndex];
        messageIndex++;
    }
    return outputMessage;
}

string Hamming::removeHammingBits(const string &binaryMessage) {
    string outputMessage;
    int nextPowerOfTwo = 1;
    for (int i = 0; i < binaryMessage.size(); ++i) {
        if (i + 1 == nextPowerOfTwo) {
            nextPowerOfTwo *= 2;
            continue;
        }
        outputMessage += binaryMessage[i];
    }
    return outputMessage;
}

void Hamming::incrementIndexForHamming(int take, int &taken, int &i) {
    taken++;
    if (take == taken) {
        taken = 0;
        i += take;
    }
}

string Hamming::hamming(const string &binaryMessage) {
    string outputMessage = embedDummyHammingBits(binaryMessage);
    int totalMessageSize = outputMessage.size();
    int nextPowerOfTwo = 1;
    while (nextPowerOfTwo - 1 < totalMessageSize) {
        int take = nextPowerOfTwo;
        int taken = 0;
        int i = nextPowerOfTwo;
        incrementIndexForHamming(take, taken, i);
        for (; i < totalMessageSize; ++i) {
            outputMessage[nextPowerOfTwo - 1] = Utils::xor_char(outputMessage[nextPowerOfTwo - 1], outputMessage[i]);
            incrementIndexForHamming(take, taken, i);
        }
        nextPowerOfTwo *= 2;
    }
    return outputMessage;
}

string Hamming::correctError(const string &binaryMessage, bool &corrected) {
    int errorPosition = 0;
    int totalMessageSize = binaryMessage.size();
    int nextPowerOfTwo = 1;
    while (nextPowerOfTwo - 1 < totalMessageSize) {
        int take = nextPowerOfTwo;
        int taken = 0;
        int i = nextPowerOfTwo;
        char error = binaryMessage[nextPowerOfTwo - 1];
        incrementIndexForHamming(take, taken, i);
        for (; i < totalMessageSize; ++i) {
            error = Utils::xor_char(error, binaryMessage[i]);
            incrementIndexForHamming(take, taken, i);
        }
        if (error == '1')
            errorPosition += nextPowerOfTwo;
        nextPowerOfTwo *= 2;
    }
    string correctMessage = binaryMessage;
    if (errorPosition != 0 && errorPosition - 1 < correctMessage.length())
    {
        Utils::char_not(correctMessage[errorPosition - 1]);
        corrected = true;
    }
    return removeHammingBits(correctMessage);
}


