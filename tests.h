#pragma once
//#define WORD_DIVISION_TEST
bool divideIntoWordsTest();
bool extendDataTest();
bool warpCompressionTest();
bool blockCompressionTest();
bool blockMergeTest();
bool blockMergeWithOnesStartsTest();
bool blockMergeAlternatingTest();
bool blockMergeFinalLiterals();
bool blockMergeWanderingLiterals();
bool multiBlockTest();
bool compressAndDecompressTest();
bool randomDataTest();
void generateRandomData(unsigned int* tab, unsigned int size, unsigned int everyN);
