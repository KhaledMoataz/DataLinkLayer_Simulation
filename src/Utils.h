//
// Created by ayman on 25/12/2020.
//

#ifndef UNTITLED1_UTILS_H
#define UNTITLED1_UTILS_H

#include <string>
#include <vector>

using namespace std;


class Utils {
private:
    static vector<string> splitString(string message);
public:
    static string toBinary(const string &message);

    static string toCharString(string binaryMessage);

    static char xor_char(char a, char b);

    static void char_not(char &c);

    static vector<string> getFramesToSend(const string &message);

    static string decodeFrames(vector<string> receivedFrames);

};


#endif //UNTITLED1_UTILS_H
