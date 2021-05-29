#ifndef AFA_UTILS_H_
#define AFA_UTILS_H_
#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <set>


extern int global_i;
class Utils {
public:

    static std::vector<std::string> GetTokens(std::string word);
    static std::string GetLastSymbol(std::string word);
    static std::string ReverseWord(std::string word);
    static std::string RestOfWord(std::vector<std::string> tokens,int i);
    static std::string GetLabel(unsigned number);
    template< typename TV>
    static std::set<std::string> extract_keys(std::map<std::string, TV>& input_map);
};

#endif