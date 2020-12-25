//
// Created by ayman on 25/12/2020.
//

#ifndef UNTITLED1_UTILS_H
#define UNTITLED1_UTILS_H

#include <string>

using namespace std;


class Utils {
public:
    static string toBinary(const string &message);

    static string toCharString(const string &binaryMessage);

    static char xor_char(char a, char b);

    static void char_not(char &c);

};


#endif //UNTITLED1_UTILS_H
