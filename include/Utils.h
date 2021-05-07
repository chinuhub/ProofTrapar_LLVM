#include <iostream>
#include <vector>
#include <sstream>


extern int global_i;
class Utils {
public:

    static std::vector<std::string> GetTokens(std::string word);
    static std::string GetLastSymbol(std::string word);
    static std::string ReverseWord(std::string word);
    static std::string RestOfWord(std::vector<std::string> tokens,int i);
    static std::string GetLabel(unsigned number);

};