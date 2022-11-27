#ifndef UTILS_HPP
#define UTILS_HPP
#pragma once

#include <vector>
#include <string>

std::vector<std::string> split(std::string str , std::string delimiter);
std::string strip(std::string str , bool before = true , bool after = true);

#endif // UTILS_HPP