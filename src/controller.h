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

using namespace std;

class Controller{
private:
    string _inputFilePath;
    string _outputFilePath;
	int _latency;
    vector<string> _linesFromInputFile;
    vector<string> _verilogLines;
    vector<Node> _variables;
    vector<Operation> _operations;
//    vector<Condition> _ifstatements;    //new
    vector<Operation> _ifstatements;    //new
	
	int* _Schedule;
	int* _scheduleCount;
    
public:
    Controller(string inputFilePath, string outputFilePath,int latency);
    
    vector<Operation> getOperations();
    vector<Node> getVariables();
    vector<string> getVerilogLines();
	vector<string> VarNames;
	vector<string> OriginalNames;
	int dummyCount = 0;
	int OriginalNameCount = 0;
    bool readFromFile();
    bool convertOperations();
    void ProcessInputFile();
    void addNodes(Input inp);
    string writeModule();
    string writeVariables();
	string writeStateMachine();
    string checkSign(Operation oper, Node var);
    void writeToFile();
    Operation parseOperation(string netlistCode);
//    Condition parseIfStatement(string netlistCode); //new
    Operation parseIfStatement(string netlistCode); //new
    Node findVariable(string varName);
    void getCriticalPath();
	bool PerformScheduling(int latency);
    string checkForTabs(string netlistCode);

	
};


#endif

