//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string>
#include <vector>
#include "input.h"
#include "node.h"
#include "operation.h"
#include "condition.h"

using namespace std;

class Controller{
private:
    string _inputFilePath;
    string _outputFilePath;
    vector<string> _linesFromInputFile;
    vector<string> _verilogLines;
    vector<Node> _variables;
    vector<Operation> _operations;
    vector<Condition> _ifstatements;    //new
public:
    Controller(string inputFilePath, string outputFilePath);
    
    vector<Operation> getOperations();
    vector<Node> getVariables();
    vector<string> getVerilogLines();
    
    bool readFromFile();
    bool convertOperations();
    void ProcessInputFile();
    void addNodes(Input inp);
    string writeModule();
    string writeVariables();
    string checkSign(Operation oper, Node var);
    void writeToFile();
    Operation parseOperation(string netlistCode);
    Condition parseIfStatement(string netlistCode); //new
    Node findVariable(string varName);
    void getCriticalPath();
	bool PerformScheduling(int latency);
    string checkForTabs(string netlistCode);
};


#endif

