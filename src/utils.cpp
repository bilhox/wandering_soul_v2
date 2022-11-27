#include "headers/utils.hpp"
#include <algorithm>

std::vector<std::string> split (std::string str, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = str.find (delimiter, pos_start)) != std::string::npos) {
        token = str.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (str.substr (pos_start));
    return res;
}

std::string strip(std::string str , bool before , bool after){
    
    if(before){
        auto i {std::find_if_not(std::begin(str), std::end(str), isspace)};
        str.erase(std::begin(str) , i);
    }

    if(after){
        std::reverse(std::begin(str) , std::end(str));
        auto h = std::find_if_not(std::begin(str), std::end(str), isspace);
        str.erase(std::begin(str) , h);
        std::reverse(std::begin(str) , std::end(str));
    }

    return str;
    
}