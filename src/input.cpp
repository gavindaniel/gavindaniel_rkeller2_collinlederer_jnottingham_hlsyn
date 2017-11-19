//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#include "input.h"
#include "operation.h"
#include <iostream>

using namespace std;

/**
 * Description: Parse a line from a file and create a card
 *
 * @param lineFromFile A string from a file
 *
 * @requirement carriage returns "\r" and newlines "\n" must be removed
 * @requirement a valid blank is at least three consecutive underscores
 * @requirement blankLength and blankIndex are zero if a valid blank is not found
 * @requirement blankIndex is the index that makes card->GetContent()[card->blankIndex] be the first underscore
 * @requirement blankLength is the number of underscores in the blank
 *
 */
Input::Input(string lineFromFile){
    
    _content = lineFromFile;
    
}

string Input::getContent() {    return _content;    }
int Input::getSize() {    return _size;    }
string Input::getType() {    return _type;    }
vector<string> Input::getVariables() {  return _variables;  }
bool Input::getSigned() {   return _signed; }

/**
 * Validate Card
 *
 * @requirement blankLength must be at least 3
 *
 */
bool Input::IsValid() {
    if (_content != ""){
        return true;
    }
    else {
        return false;
    }
}

void Input::ParseInput() {
    string netlistCode = this->getContent();
    
    // check if variable definition or operation
    if (!checkOperation(netlistCode)) { // variable definition
        // checks variable type
        netlistCode = checkType(netlistCode);
        netlistCode = checkSize(netlistCode);
        checkVariables(netlistCode);
    }
    else { // operation definition
        
    }
    
}

bool Input::checkOperation(string netlistCode) {
    if (netlistCode.find("=") != string::npos) {
        return true;
    }
    else {
        return false;
    }
}
// pre-condition: must be a variable definition
string Input::checkType(string netlistCode) {
    // Check for input / output / wire
    if (netlistCode.find("input") != string::npos) {
        _type = "input";
    }
    else if (netlistCode.find("output") != string::npos) {
        _type = "output";
    }
    else if (netlistCode.find("wire") != string::npos) {
        _type = "wire";
    }
    else if (netlistCode.find("register") != string::npos) {
        _type = "wire";
    }
    netlistCode = netlistCode.substr(netlistCode.find(" ") + 1);
    
    return netlistCode;
}

string Input::checkSize(string netlistCode) {
    // Check for size of variable
    
    if (netlistCode.find("Int8") != string::npos) {
        _size = 8;
    }
    else if (netlistCode.find("Int16") != string::npos) {
        _size = 16;
    }
    else if (netlistCode.find("Int32") != string::npos) {
        _size = 32;
    }
    else if (netlistCode.find("Int64") != string::npos) {
        _size = 64;
    }
    else if (netlistCode.find("Int1") != string::npos) {
        _size = 1;
    }
    
    if (netlistCode.find("UInt") != string::npos) {
        _signed = false;
    }
    else {
        _signed = true;
    }
    
    netlistCode = netlistCode.substr(netlistCode.find(" ") + 1);
    
    return netlistCode;
}

void Input::checkVariables(string netlistCode) {
    // check for variables
    string varName = "";
    
    // check for a comment
    if (netlistCode.find("//") != string::npos) {   netlistCode = netlistCode.substr(0,netlistCode.find("//"));    }
    
    for (unsigned int i = 0; i < netlistCode.length(); i++) {
        // check for punctuation / spaces
        
            if (netlistCode[i] != ',' && netlistCode[i] != ' ') {
                varName += netlistCode[i];
            }
            else {
                if (varName != ""){
                    _variables.push_back(varName);
                    varName = "";
                }
            }
        
    }
    // catch the last variable
    if (varName != ""){
        _variables.push_back(varName);
  //      varName = "";
    }
}







