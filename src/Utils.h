//
// Created by ayman on 25/12/2020.
//

#ifndef UNTITLED1_UTILS_H
#define UNTITLED1_UTILS_H

#include <string>
#include <vector>
#include <queue>
#include <omnetpp.h>
#include "GoBackN.h"

using namespace std;


class Utils {
private:
    static vector<string> splitString(string message);
public:
    static string toBinary(const string &message);

    static string toCharString(string binaryMessage);

    static char xor_char(char a, char b);

    static void char_not(char &c);

    static void getFramesToSend(const string &message, queue<string> &globalBuffer, const GoBackN *node=nullptr);

    static string decodeFrames(queue<string> receivedFrames, bool &corrected);

    static string decodeFrame(const string &frame, bool &corrected);
};


#endif //UNTITLED1_UTILS_H
