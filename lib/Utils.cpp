#include "Utils.h"
#include "AFA/AFAut.h"

int global_i=0;
std::vector<std::string> Utils::GetTokens(std::string word) {
    std::stringstream temp(word);
    std::vector<std::string> tokens;
    std::string get_tokens;
    while (getline(temp, get_tokens, 'L')) {
        tokens.push_back("L"+get_tokens);
    }
    return tokens;
}


std::string Utils::GetLastSymbol(std::string word) {

    std::vector<std::string> tokens = GetTokens(word);
    std::string lastsym = tokens[tokens.size()-1];
    return lastsym;
}


std::string Utils::ReverseWord(std::string word) {

    std::vector<std::string> tokens = GetTokens(word);

    std::string rev="";
    for(int i = tokens.size()-1; i >0; i--) {
        rev=rev+tokens[i];
    }

    return rev;
}


std::string Utils::RestOfWord(std::vector<std::string> tokens,int i) {

    std::string rest="";

    for(int j=1;j<i;j++){
        rest= rest + tokens[j];
    }

    return rest;
}


std::string Utils::GetLabel(unsigned number){

    std::string label="L";
    label= label + std::to_string(number+1);
    return label;
}


template<typename TV>
std::set<std::string> extract_keys(std::map<std::string, TV>& input_map) {
    std::set<std::string> retval;
    for (auto const& element : input_map) {
        retval.insert(element.first);
    }
    return retval;
}

template std::set<std::string> extract_keys<AFAStatePtr>(std::map<std::string, AFAStatePtr>& input_map);
template std::set<std::string> extract_keys<SetAFAStatesPtr>(std::map<std::string, SetAFAStatesPtr>& input_map);
