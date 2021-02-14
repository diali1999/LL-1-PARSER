#include <bits/stdc++.h>
#include<string.h>
using namespace std;

#define INTERMEDIATE string("./Intermediate/")
#define EPSILON string("epsilon")

bool isTerminal(unordered_map<string, vector<vector<string>>> grammar, string symbol){
    if(grammar.find(symbol) == grammar.end())   return true;
    else return false;
}

bool containsEpsilon(set<string> first){
    if(first.find(EPSILON) == first.end())  return false;
    return true;
}

void read(unordered_map<string, vector<vector<string>>> &grammar, string &start){
    ifstream ifs;
    ifs.open("grammar.txt", fstream::in);
    int first = true;
    string symbol, non_terminal;
    bool rule = false;
    int num_rules = 0;
    while(ifs >> symbol){
        if(first){
            start = symbol;
            first = false;
        }
        non_terminal = symbol;
        while(ifs >> symbol){
            if(symbol == "|"){
                grammar[non_terminal].push_back(vector<string>());
                num_rules++;
            }
            else if(symbol == ";"){
                rule = false;
                num_rules = 0;
                break;
            }
            else if(rule){
                grammar[non_terminal][num_rules].push_back(symbol);
            }
            else if(symbol == ":"){
                grammar[non_terminal] = vector<vector<string>>();
                grammar[non_terminal].push_back(vector<string>());
                rule = true;
            }
        }
    }
    ifs.close();
}

void eliminate_left_recursion(unordered_map<string, vector<vector<string>>>& grammar){
    unordered_map<string, vector<vector<string>>> finals;
    for(auto it = grammar.begin(); it != grammar.end(); ++it){
        string non_terminal = it->first;
        vector<vector<string>> productions = it->second;
        vector<vector<string>> alpha;
        vector<vector<string>> beta;
        bool left_recursive = false;
        for(int i = 0; i < productions.size(); ++i){
            if(productions[i][0] == non_terminal){
                left_recursive = true;
                vector<string> _alpha;
                for(int j = 1; j < productions[i].size(); j++){
                    _alpha.push_back(productions[i][j]);
                }
                alpha.push_back(_alpha);
            }
            else{
                beta.push_back(productions[i]);
            }
        }
        if(left_recursive){
            finals[non_terminal+"\'"].push_back(vector<string>({"epsilon"}));
            if(beta.size() == 0){
                finals[non_terminal].push_back(vector<string>({non_terminal+"\'"}));
            }
            for(int i = 0; i < beta.size(); i++){
                vector<string> _beta;
                for(int j = 0; j < beta[i].size(); j++){
                    _beta.push_back(beta[i][j]);
                }
                _beta.push_back(non_terminal+"\'");
                finals[non_terminal].push_back(_beta);
            }
            for(int i = 0; i < alpha.size(); i++){
                vector<string> _alpha;
                for(int j = 0; j < alpha[i].size(); j++){
                    _alpha.push_back(alpha[i][j]);
                }
                _alpha.push_back(non_terminal+"\'");
                finals[non_terminal+"\'"].push_back(_alpha);
            }
        }
        else{
            finals[non_terminal] = productions;
        }
    }
    grammar = finals;
    ofstream ofs;
    ofs.open((INTERMEDIATE+"left_recursion_eliminated.txt"), fstream::out);
    for(auto it = grammar.begin(); it != grammar.end(); ++it){
        vector<vector<string>> productions = it->second;
        ofs << it->first << ":";
        for(int i = 0; i < productions.size();++i){
            for(int j = 0; j < productions[i].size(); j++)
                ofs << productions[i][j];
            if(i!=productions.size()-1)
                ofs << "|";
        }
        ofs<<";\n";
    }
    ofs.close();
}

void FOLLOW(unordered_map<string, set<string>> &follow, unordered_map<string, vector<vector<string>>> grammar, unordered_map<string, set<string>> first, string start){
    bool changed = false;
    follow[start].insert("$");
    do{
        changed = false;
        for(auto it = grammar.begin(); it!=grammar.end(); it++){
            vector<vector<string>> productions = it->second;
            for(int i = 0; i < productions.size(); i++){
                for(int j = 0; j < productions[i].size(); j++){
                    if(!isTerminal(grammar, productions[i][j])){
                        if(j==productions[i].size() -1){
                            for(auto itr: follow[it->first]){
                                changed = changed || follow[productions[i][j]].insert(itr).second;
                            }
                        } 
                        else{
                            std::pair<std::set<string>::iterator,bool> ret;
                            for(int k = j+1; k < productions[i].size(); k++){
                                for(auto itr: first[productions[i][k]]){
                                    changed = changed || follow[productions[i][j]].insert(itr).second;
                                }
                                if(k==productions[i].size()-1 && containsEpsilon(first[productions[i][k]])){
                                    for(auto itr: follow[it->first]){
                                        changed = changed || follow[productions[i][j]].insert(itr).second;
                                    }
                                }
                                if(!containsEpsilon(first[productions[i][k]])){
                                    break;
                                }
                            }
                        }
                    }  
                }
            }
        }
    }while(changed);
    for(auto it = follow.begin(); it!=follow.end(); it++){
        it->second.erase(EPSILON);
    }
    ofstream ofs;
    ofs.open(INTERMEDIATE+"follow.txt", fstream::out);
    for(auto it = grammar.begin(); it!=grammar.end(); it++){
        set<string> first_non_terminal = follow[it->first];
        ofs << (it->first) << "->{";
        for(auto first_elem: first_non_terminal){
            ofs << first_elem << " ";
        }
        ofs << "}\n";
    }
    ofs.close();
}

void parsing_table( unordered_map<string, set<string>> first , unordered_map<string, set<string>> follow , unordered_map<string, vector<vector<string>>> grammar, map<pair<string,string>, vector<string>>& parse_table){
    for(auto it = grammar.begin(); it != grammar.end(); it++){
        string key = it->first;
        vector<vector<string>>productions = it->second;
        for(int j = 0 ; j < productions.size() ; j++){
            set<string> first_values = first[productions[j][0]];
            if(first_values.find(EPSILON)!=first_values.end())
            {
                set<string>follow_values = follow[key];
                for(auto k = follow_values.begin(); k != follow_values.end(); k++)
                {
                    if(parse_table.find({key, *k}) != parse_table.end())
                        {
                            cout<<"ERROR : NOT A LL1 parser."<<"\n";
                            return;
                        }
                        else{
                            parse_table[{key, *k }].insert(parse_table[{key, *k }].end() , productions[j].begin(), productions[j].end());
                        }
                }
            }
            for(auto k = first_values.begin(); k != first_values.end(); k++)
            {
                if(*k != EPSILON)
                {
                    if(parse_table.find({key, *k}) != parse_table.end())
                        {
                            cout<<"ERROR : NOT A LL1 parser."<<"\n";
                        }
                        else{
                            parse_table[{key, *k }].insert(parse_table[{key, *k }].end() , productions[j].begin(), productions[j].end());
                        }
                    }
            }
        }
    }
    ofstream ofs;
    ofs.open(INTERMEDIATE+"parsing_table.txt");
    ofs<< "Parsing table:"<<"\n";
    for(auto it = parse_table.begin(); it != parse_table.end(); it++)
    {
        ofs<<"("<<(it->first).first<<" "<<(it->first).second<<")";
        vector<string>v = it->second;
        for(int j=0; j < v.size(); j++)
         ofs <<v[j];
         ofs<<"\n";
    }
}

void create_predictive_parser(vector<string> input, string start, unordered_map<string, vector<vector<string>>> grammar, map<pair<string,string>, vector<string>>parse_table){
    ofstream ofs;
    ofs.open(INTERMEDIATE+"parser_op", fstream::out);
    input.push_back("$");
    int ip = 0;
    stack<string> production_stack;
    production_stack.push("$");
    production_stack.push(start);
    string X = production_stack.top();
    while(X != "$"){
        if(X==EPSILON){production_stack.pop();}
        else if(X == input[ip]) {
            production_stack.pop();
            cout << "Match " << input[ip] << "\n";
            ofs << "Match " << input[ip] << "\n";
            ip++;
        }
        else if(isTerminal(grammar, X)){
            cout << "error" << "\n";
            cout << "Does not belong to grammar defined: " << X;
            return;
        }
        else if(parse_table.find({X, input[ip]}) == parse_table.end()){
            cout << "error" << "\n";
            cout << "Transition not allowed for: " << X;
            return;
        }
        else{
            cout << X << "->";
            ofs << X << "->";
            vector<string> production = parse_table[{X, input[ip]}];
            production_stack.pop();
            for(int i = 0; i < production.size(); i++){
                cout << production[i];
                ofs << production[i];
            }
            cout << "\n";
            ofs << "\n";
            for(int i = production.size() - 1; i >= 0; i--){
                production_stack.push(production[i]);
            }
        }
        X = production_stack.top();
    }
    ofs.close();
}

set<string> FIRST(string current, unordered_map<string, vector<vector<string>>> grammar){
    if(grammar.find(current) == grammar.end()) {
        return set<string>({current});
    }
    vector<vector<string>> productions = grammar[current];
    set<string> s;
    for(int i = 0; i < productions.size(); i++){
        for(int j = 0; j < productions[i].size(); j++){
            set<string> foo = FIRST(productions[i][j], grammar);
            s.insert(foo.begin(), foo.end());
            if(foo.find(EPSILON) == foo.end()){
                break;
            }
        }
    }
    return s;
}

void get_first_non_terminals(unordered_map<string, set<string>> &first, set<string> non_terminals, unordered_map<string, vector<vector<string>>> grammar){
    ofstream ofs;
    ofs.open(INTERMEDIATE+"first_non_terminals.txt", fstream::out);
    for(auto it = non_terminals.begin(); it!= non_terminals.end(); it++){
        first[*it] = FIRST(*it, grammar);
    }
    for(auto it = non_terminals.begin(); it!=non_terminals.end(); it++){
        set<string> first_non_terminal = first[*it];
        ofs << (*it) << "->{";
        for(auto first_elem: first_non_terminal){
            ofs << first_elem << " ";
        }
        ofs << "}\n";
    }
    ofs.close();
}

void get_first_terminals(unordered_map<string, set<string>> &first, set<string> terminals){
    ofstream ofs;
    ofs.open(INTERMEDIATE+"first_terminals.txt", fstream::out);
    for(auto it = terminals.begin(); it != terminals.end(); it++){
        first[*it] = set<string>({*it});
        ofs << (*it) << "->{" << (*it) << "}\n";
    }
    ofs.close();
}

set<string> get_non_terminals(unordered_map<string, vector<vector<string>>> grammar){
    ofstream ofs;
    ofs.open(INTERMEDIATE+"non_terminals.txt", fstream::out);
    set<string> non_terminals;
    for(auto it = grammar.begin(); it != grammar.end(); it++){
        non_terminals.insert(it->first);
        ofs << (it->first) << "\n";
    }
    ofs.close();
    return non_terminals;
}

set<string> get_terminals(unordered_map<string, vector<vector<string>>> grammar){
    ofstream ofs;
    ofs.open(INTERMEDIATE+"terminals.txt", fstream::out);
    set<string> terminals;
    for(auto it = grammar.begin(); it != grammar.end(); it++){
        vector<vector<string>> productions = it->second;
        for(int i = 0; i < productions.size(); i++){
            for(int j = 0; j < productions[i].size(); j++){
                if(grammar.find(productions[i][j]) == grammar.end()){
                    ofs << productions[i][j] << "\n";
                    terminals.insert(productions[i][j]);
                }
            }
        }
    }
    ofs.close();
    return terminals;
}

int main(){
    unordered_map<string, vector<vector<string>>> grammar;
    string start;
    read(grammar, start);
    eliminate_left_recursion(grammar);
    set<string> non_terminals = get_non_terminals(grammar);
    set<string> terminals = get_terminals(grammar);
    unordered_map<string, set<string>> first;
    unordered_map<string, set<string>> follow;
    map<pair<string,string>, vector<string>>parse_table;
    get_first_terminals(first, terminals);
    get_first_non_terminals(first, non_terminals, grammar);
    FOLLOW(follow, grammar, first, start);
    parsing_table(first, follow, grammar, parse_table);
    vector<string> v;
    string symbol = "";
    for(;symbol!="$";){
        cin >> symbol;
        v.push_back(symbol);
    }
    create_predictive_parser(v, start, grammar, parse_table);
    return 0;
}