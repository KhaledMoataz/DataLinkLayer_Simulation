//
// Created by ayman on 25/12/2020.
//

#include "Framing.h"

string Framing::bitStuffing(const string &binaryMessage) {
    string bitStuffedMessage;
    int numberOfOnes = 0;
    for (char c:binaryMessage) {
        bitStuffedMessage += c;
        if (c == '1') {
            numberOfOnes++;
            if (numberOfOnes == 5) {
                bitStuffedMessage += "0";
                numberOfOnes = 0;
            }
        } else {
            numberOfOnes = 0;
        }
    }
    return bitStuffedMessage;
}

string Framing::bitUnstuffing(const string &binaryMessage) {
    int numberOfOnes = 0;
    string unstuffedMessage;
    for (char c:binaryMessage) {
        if (numberOfOnes == 5) {
            numberOfOnes = 0;
            continue;
        }
        unstuffedMessage += c;
        if (c == '1') {
            numberOfOnes++;
        } else {
            numberOfOnes = 0;
        }
    }
    return unstuffedMessage;
}

string Framing::addFlags(const string &binaryMessage) {
    string flag = "01111110";
    return flag + binaryMessage + flag;
}

string Framing::removeFlags(const string &binaryMessage) {
    if (binaryMessage.size() < 16)
        return "";
    int last = (int) binaryMessage.rfind("01111110");
    return binaryMessage.substr(8, last - 8);
}
