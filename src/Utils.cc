//
// Created by ayman on 25/12/2020.
//

#include "Utils.h"
#include <bitset>

string Utils::toBinary(const string &message) {
    string binaryMessage;
    for (char c: message) {
        binaryMessage += bitset<8>(c).to_string();
    }
    return binaryMessage;
}

string Utils::toCharString(const string &binaryMessage) {
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
