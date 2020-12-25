//
// Created by ayman on 25/12/2020.
//

#ifndef UNTITLED1_HAMMING_H
#define UNTITLED1_HAMMING_H

#include <string>

using namespace std;


class Hamming {
private:
    static string embedDummyHammingBits(const string &binaryMessage);

    static string removeHammingBits(const string &binaryMessage);

    static void incrementIndexForHamming(int take, int &taken, int &i);

public:
    static int redundantBitsCount(int messageSize);
    static string hamming(const string &binaryMessage);
    static string correctError(const string &binaryMessage);

    };


#endif //UNTITLED1_HAMMING_H
