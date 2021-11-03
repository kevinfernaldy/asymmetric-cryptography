#include <include/util.h>
#include <sstream>
#include <cstring>
#include <iostream>

Util::Util()
{

}

std::vector<unsigned int> Util::convert_and_pad(std::string payload) {
    // ALPHABET ONLY
//    if (payload.length() % 2 != 0) {
//        payload += 0x5B;
//    }

//    std::vector<unsigned int> payload_block;
//    int i = 0;
//    while (i < payload.length()) {
//        char* temp = (char*) payload.substr(i, 2).c_str();
//        temp[0] -= 0x41;
//        temp[1] -= 0x41;

//        char temp_c = temp[0];
//        temp[0] = temp[1];
//        temp[1] = temp_c;

//        unsigned int temp_block;
//        memmove(&temp_block, temp, sizeof(int));

//        payload_block.push_back(temp_block);

//        i += 2;
//    }

    // ASCII
    if (payload.length() % 2 != 0) {
        payload += (char) 0x00;
    }

    std::vector<unsigned int> payload_block;
    int i = 0;
    while (i < payload.length()) {
        char* temp = (char*) payload.substr(i, 2).c_str();

        unsigned int temp_block = 0;
        temp_block = temp_block + ((unsigned int) temp[0] * 1000);
        temp_block = temp_block + ((unsigned int) temp[1]);

        payload_block.push_back(temp_block);

        i += 2;
    }

    return payload_block;
}
