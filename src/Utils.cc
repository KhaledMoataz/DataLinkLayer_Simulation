//
// Created by ayman on 25/12/2020.
//

#include "Utils.h"
#include "Framing.h"
#include "Hamming.h"
#include <bitset>

string Utils::toBinary(const string &message) {
    string binaryMessage;
    for (char c: message) {
        binaryMessage += bitset<8>(c).to_string();
    }
    return binaryMessage;
}

string Utils::toCharString(string binaryMessage) {
    int extra = 8 - (int) binaryMessage.size() % 8;
    if (extra != 8) {
        for (int j = 0; j < extra; ++j) {
            binaryMessage += "0";
        }
    }
    string message;
    int i = 0;
    while (i < binaryMessage.size()) {
        message += (char) bitset<8>(binaryMessage.substr(i, 8)).to_ulong();
        i += 8;
    }
    return message;
}

char Utils::xor_char(char a, char b) {
    if (a == b)
        return '0';
    return '1';
}

void Utils::char_not(char &c) {
    if (c == '0')
        c = '1';
    else
        c = '0';
}

vector<string> Utils::splitString(string message) {
    vector<string> words;
    size_t pos = 0;
    string token;
    while ((pos = message.find(' ')) != string::npos) {
        token = message.substr(0, pos);
        message.erase(0, pos + 1);
        words.push_back(token);
    }
    words.push_back(message);
    return words;
}

void Utils::getFramesToSend(const string &message, queue<string> &globalBuffer) {
    vector<string> words = splitString(message);
    for (string word:words) {
        word = Utils::toBinary(word);
        word = Framing::bitStuffing(word);
        word = Hamming::hamming(word);
        word = Framing::addFlags(word);
        globalBuffer.push(word);
    }
}

string Utils::decodeFrames(queue<string> receivedFrames, bool &corrected) {
    string message;
    while (!receivedFrames.empty()) {
        string frame = decodeFrame(receivedFrames.front(), corrected);
        receivedFrames.pop();
        message += frame + (!receivedFrames.empty() ? " " : "");
    }
    return message;
}

string Utils::decodeFrame(const string &receivedFrame, bool &corrected) {
    string frame = receivedFrame;
//    frame = Utils::toBinary(frame);
    frame = Framing::removeFlags(frame);
    frame = Hamming::correctError(frame, corrected);
    frame = Framing::bitUnstuffing(frame);
    frame = Utils::toCharString(frame);
    return frame;
}
