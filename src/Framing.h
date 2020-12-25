//
// Created by ayman on 25/12/2020.
//

#ifndef UNTITLED1_FRAMING_H
#define UNTITLED1_FRAMING_H

#include <string>

using namespace std;


class Framing {
public:
    static string bitStuffing(const string &binaryMessage);

    static string bitUnstuffing(const string &binaryMessage);

    static string addFlags(const string &binaryMessage);

    static string removeFlags(const string &binaryMessage);

};

#endif //UNTITLED1_FRAMING_H
