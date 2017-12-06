//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>


using namespace std;

class Node{
private:
    string _sourceCode;
    string _name;
    int _bitWidth;
    string _varType;
    bool _signed;
	int _latency;
    
    
    
    
public:
    Node();
    Node(string netlist, string name, int bitWidth, string varType, bool s);
    bool IsValid();
    string ParseInput();
    string getSourceCode();
    string getVariableName();
    string getVariableType();
    int getBitWidth();
	int getLatency();
    bool getSigned();
    bool checkOperation(string netlistCode);
    string checkType(string netlistCode);
    string checkSize(string netlistCode);
	void setVariableName(string name);
};
#endif




