//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <vector>

using namespace std;

class Input{
private:
    string _content;
    int _size;
    string _type;
    vector<string> _variables;
    bool _signed;
public:
    Input(string lineFromFile);
    bool IsValid();
    void ParseInput();
    string getContent();
    int getSize();
    string getType();
    vector<string> getVariables();
    bool getSigned();
    bool checkOperation(string netlistCode);
    bool checkIfStatement(string netlistCode);  //new
    bool checkCurlyBrace(string netlistCode);   //new
    string checkType(string netlistCode);
    string checkSize(string netlistCode);
    void checkVariables(string netlistCode);
    void parseOperation(string netlistCode);
    void parseIfStatement(string netlistCode);  //new
};
#endif



