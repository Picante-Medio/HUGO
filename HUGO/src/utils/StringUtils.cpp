#include "StringUtils.h"

std::vector<std::string_view> StringUtils::split(std::string_view str, char delim)
{
    std::vector<std::string_view> result; 
    auto left = str.begin(); 
    for (auto it = left; it != str.end(); ++it) 
    { 
        if (*it == delim) 
        { 
            result.emplace_back(&*left, it - left); 
            left = it + 1; 
        } 
    } 
    if (left != str.end()) 
        result.emplace_back(&*left, str.end() - left); 
    return result; 
}

std::string StringUtils::killSwitch(std::string& str) {
    std::string temp = str;
    str = "";
    return temp;
}