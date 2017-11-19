//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#include "node.h"
#include <iostream>

using namespace std;

Node::Node(){
    _sourceCode = "";
    _name = "";
    _bitWidth = 0;
    _varType = "";
    _signed = true;
}

Node::Node(string netlist, string name, int bitWidth, string varType, bool s){
    _sourceCode = netlist;
    _name = name;
    _bitWidth = bitWidth;
    _varType = varType;
    _signed = s;
}

string Node::getSourceCode() {
    return _sourceCode;
}

string Node::getVariableName() {
    return _name;
}

int Node::getBitWidth() {
    return _bitWidth;
}

bool Node::getSigned() {
    return _signed;
}

string Node::getVariableType() {
    return _varType;
}






