#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>

class Util
{
public:
    Util();

    static std::vector<unsigned int> convert_and_pad(std::string payload);
};

#endif // UTIL_H
