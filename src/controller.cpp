//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#include <fstream>
#include <iostream>
#include <algorithm>
#include <ctype.h>
#include "controller.h"
#include "input.h"
#include "node.h"
#include "circuit.h"
#include "condition.h"

using namespace std;

/**
 * Description: contrusctor for Controller
 */
Controller::Controller(string inputFilePath, string outputFilePath){
    _inputFilePath = inputFilePath;
    _outputFilePath = outputFilePath;
    
    Node Clk = Node("input Int1 Clk, Rst, Start", "Clk", 1, "input", true);
    Node Rst = Node("input Int1 Clk, Rst, Start", "Rst", 1, "input", true);
    Node Start = Node("input Int1 Clk, Rst, Start", "Start", 1, "input", true);
    Node Done = Node("output reg Int1 Done", "Done", 1, "output", true);
    _variables.push_back(Clk);
    _variables.push_back(Rst);
    _variables.push_back(Start);
    _variables.push_back(Done);
}
/*
 * Description: Getters
 */
vector<Operation> Controller::getOperations() {
    return _operations;
}
vector<string> Controller::getVerilogLines() {
    return _verilogLines;
}
vector<Node> Controller::getVariables() {
    return _variables;
}

/*
 * Description: Read  from a File
 */

bool Controller::readFromFile() {
    ifstream inputFile(_inputFilePath);
    string lineBuffer;
    string verilogCode;
    Operation tempOperation;
    Operation temp;
    Condition tempCondition;
    vector<Condition> parentConditions;
    int num_if_statement_open = 0;
    
    if (!inputFile.is_open()) {
        cout << "Could not open netlist " << _inputFilePath << endl;
        return false;
    }
    
    while(!inputFile.eof()) {
        getline(inputFile, lineBuffer);
        Input newInput = Input(lineBuffer);
        if(newInput.IsValid()){
            _linesFromInputFile.push_back(lineBuffer);
            newInput.ParseInput();
            // found some variables to add 
            if (newInput.getVariables().size() > 0) {
                addNodes(newInput);
            }
            if (newInput.checkOperation(lineBuffer)){
                tempOperation = parseOperation(lineBuffer);
                if (tempOperation.getOperator() == "z") {
                    return false;
                }
                if (num_if_statement_open > 0) { //this operation is inside an if statement
                    tempCondition.addOperation(tempOperation);
                }
                else
                    _operations.push_back(tempOperation);
            }
            if (newInput.checkIfStatement(lineBuffer)) {
                if (tempCondition.getVariable().getVariableName() == "")
                    tempCondition = parseIfStatement(lineBuffer);
                else { // nested if statement
                    parentConditions.push_back(tempCondition);
                    tempCondition = parseIfStatement(lineBuffer);
                }
                if (tempCondition.getVariable().getVariableName() == "") {
                    return false;
                }
                num_if_statement_open++;
                
            }
            if(lineBuffer.find("}") != string::npos) {
                if (!parentConditions.empty()) {
                    temp.setOperator("{");
                    temp.set_ifstatement(tempCondition);
                    parentConditions.back().addCondition(tempCondition);
//                    tempCondition = parentConditions.back();
                    parentConditions.pop_back();
                }
                else { // no parent if statements
                    _ifstatements.push_back(tempCondition);
                    temp.setOperator("{");
                    temp.set_ifstatement(tempCondition);
                    _operations.push_back(temp);
                    tempCondition = Condition();
                }
                num_if_statement_open--;
            }
        }
    }
    
    inputFile.close();
    return true;
}

/*
 * Description: Creates a Node from an Input and adds it to the list of Variables.
 */

void Controller::addNodes(Input inp) {
    
    for (unsigned int i = 0; i < inp.getVariables().size(); i++) {
        Node newNode = Node(inp.getContent(), inp.getVariables().at(i), inp.getSize(), inp.getType(), inp.getSigned());
        _variables.push_back(newNode);
    }
}

string Controller::checkForTabs(string netlistCode) {
    if (netlistCode.find("\t") != string::npos) {
        netlistCode = netlistCode.substr(netlistCode.find_last_of("\t")+1, netlistCode.length());
    }
    return netlistCode;
}
/*
 * Description: Parse netlistCode for an operation and convert it into an Operation (object)
 */
Condition Controller::parseIfStatement(string netlistCode) {
    Condition newCondition = Condition();
    Node tempNode;
    string varName;
    // check for a comment
    if (netlistCode.find("//") != string::npos) {   netlistCode = netlistCode.substr(0,netlistCode.find("//"));    }
    netlistCode = checkForTabs(netlistCode);
    if (netlistCode.find(" )") != string::npos)
        cout << netlistCode.find(" )") << endl;
    cout << netlistCode.length();
    netlistCode = netlistCode.substr(netlistCode.find("if ( ")+5, netlistCode.length()); //(netlistCode.length() - netlistCode.find(" )"))+1
    for (int i = 0; i < netlistCode.length(); i++) {
        if (netlistCode.at(i) == ' ')
            break;
        varName += netlistCode.at(i);
    }
    tempNode = findVariable(varName);
    newCondition.setVariable(tempNode);
    return newCondition;
}
/*
 * Description: Parse netlistCode for an operation and convert it into an Operation (object)
 */
Operation Controller::parseOperation(string netlistCode) {
    
    Operation newOperation = Operation();
    Node tempNode;
    string varName = "";
    string op = "";
    
    // check for a comment
    if (netlistCode.find("//") != string::npos) {   netlistCode = netlistCode.substr(0,netlistCode.find("//"));    }
    
    netlistCode = checkForTabs(netlistCode);
    
    for (unsigned int i = 0; i < netlistCode.length(); i++) {
        // check for punctuation / spaces
        
        if (!ispunct(netlistCode[i]) && netlistCode[i] != ' ') {
            varName += netlistCode[i];
        }
        else if (ispunct(netlistCode[i])){
            if (netlistCode[i] == '=' && netlistCode[i+1] != '=' && netlistCode[i-1] != '=') {
                newOperation.setOperator("=");
            }
            else if (netlistCode[i] == '=' && netlistCode[i+1] == '=') {
                newOperation.setOperator("==");
            }
            else if (netlistCode[i] != '=') {
                op += netlistCode[i];
            }
        }
        else {
            if ((varName != "")){
                if (varName != "1") {
                    tempNode = findVariable(varName);
                
                    if (tempNode.getVariableName() == ""){
                        cout << "ERROR: variable NOT FOUND -> " << varName << endl;
                        return Operation();
                    }
                }
                else {
                    tempNode = Node(netlistCode, "1", 1, "temp", false);
                }
                
                if (newOperation.getResult().getVariableName() == "") {
                    newOperation.setResult(tempNode);
                }
                else if (newOperation.get_varA().getVariableName() == "") {
                    newOperation.set_varA(tempNode);
                }
                else if (newOperation.get_varB().getVariableName() == "") {
                    newOperation.set_varB(tempNode);
                }
                else {
                    newOperation.set_varC(tempNode);
                }
                varName = "";
            }
            if (op != "") {
                newOperation.setOperator(op);
                op = "";
            }
        }
    }
    if (varName != ""){
        if (varName != "1") {
            tempNode = findVariable(varName);
            if (tempNode.getVariableName() == ""){
                cout << "ERROR: variable NOT FOUND -> " << varName << endl;
                return Operation();
            }
        }
        else {
            tempNode = Node(netlistCode, "1", 1, "temp", false);
        }
        
        if (newOperation.get_varA().getVariableName() == "") {
            newOperation.set_varA(tempNode);
        }
        else if (newOperation.get_varB().getVariableName() == "") {
            newOperation.set_varB(tempNode);
        }
        else {
            newOperation.set_varC(tempNode);
        }
        varName = "";
    }
    
    if (newOperation.getOperator() == ">") {
        if (findVariable("lt").getVariableName() == "") {
            _variables.push_back(Node("", "lt", newOperation.getResult().getBitWidth(), "wire", true));
        }
        if (findVariable("eq").getVariableName() == "") {
            _variables.push_back(Node("", "eq", newOperation.getResult().getBitWidth(), "wire", true));
        }
    }
    else if (newOperation.getOperator() == "<") {
        if (findVariable("gt").getVariableName() == "") {
            _variables.push_back(Node("", "gt", newOperation.getResult().getBitWidth(), "wire", true));
        }
        if (findVariable("eq").getVariableName() == "") {
            _variables.push_back(Node("", "eq", newOperation.getResult().getBitWidth(), "wire", true));
        }
    }
    else if (newOperation.getOperator() == "==") {
        if (findVariable("gt").getVariableName() == "") {
            _variables.push_back(Node("", "gt", newOperation.getResult().getBitWidth(), "wire", true));
        }
        if (findVariable("lt").getVariableName() == "") {
            _variables.push_back(Node("", "lt", newOperation.getResult().getBitWidth(), "wire", true));
        }
    }
    
    return newOperation;
    
}

/*
 * Description: Searches the list of Variables (vector<Node>) for a Variable (Node) based on a given variable Name (string)
 */
Node Controller::findVariable(string varName) {
    for (unsigned int i = 0; i < _variables.size(); i++){
        if (_variables.at(i).getVariableName() == varName) {
            return _variables.at(i);
        }
    }
    return Node();
}


/*
 * Description: Writes to the Output File
 */
void Controller::writeToFile() {
    ofstream outputFile(_outputFilePath);
    
    if (!outputFile.is_open()) {
        cout << "Error opening output file " << _outputFilePath << endl;
    }
    
    outputFile << writeModule() << "\r\n";
    outputFile << writeVariables() << "\r\n";
    
    for(string line : _verilogLines){
        outputFile << "\t" << line << "\r\n";
        cout << "\t" << line << "\r\n";
    }
    outputFile << "endmodule";
    
    outputFile.close();
}

/*
 * Description: Write to the Output file
 */
string Controller::writeModule() {
    
    string moduleDef;
    
    moduleDef = "`timescale 1ns / 1ps\n\n";
    moduleDef += "module HLSM (";
    for (Node variable : _variables){
        if (variable.getVariableName() == _variables.at(0).getVariableName())
            moduleDef += variable.getVariableName();
        else
            if (variable.getVariableType() == "input" || variable.getVariableType() == "output"){
                moduleDef += ", " + variable.getVariableName();
            }
    }
    moduleDef += ");\n";
    
    cout << moduleDef;
    
    return moduleDef;
}
/*
 * Description: Writes the list of Variables to a string
 */
string Controller::writeVariables() {
    
    string verilogVariables;
    string type;
    int bitWidth;
    unsigned int index = 1;
    
    type = _variables.at(0).getVariableType();
    bitWidth = _variables.at(0).getBitWidth();
    
    if (bitWidth > 1){
        verilogVariables += "\t" + type + " [" + to_string(bitWidth-1) + ":0] " + _variables.at(0).getVariableName();
    }
    else {
        verilogVariables += "\t" + type + " " + _variables.at(0).getVariableName();
    }
    
    
    while(index < _variables.size()) {
        
        if (_variables.at(index).getVariableType() == type && _variables.at(index).getBitWidth() == bitWidth) {
            verilogVariables += ", " + _variables.at(index).getVariableName();
        }
        else {
            type = _variables.at(index).getVariableType();
            bitWidth = _variables.at(index).getBitWidth();
            
            if (type == "output") {
                type = "output wire";
            }
            
            if (bitWidth > 1){
                verilogVariables += ";\n\t" + type + " [" + to_string(bitWidth-1) + ":0] " + _variables.at(index).getVariableName();
            }
            else {
                verilogVariables += ";\n\t" + type + " " + _variables.at(index).getVariableName();
            }
        }
        
        index++;
        
    }
    verilogVariables += ";\n";
    
    cout << verilogVariables;
    
    return verilogVariables;
}
/*
 * Description: loops through the list of Operations and tries to convert each Operation
 */
bool Controller::convertOperations() {
    string verilog;
    for (Operation op : _operations) {
        if (op.getOperator() != "{") {
            verilog = op.convertOperation();
        }
        else { // if statement
            verilog = _ifstatements.front().convertIfStatement();
            
        }
        if (verilog != "ERROR") {
            _verilogLines.push_back(verilog);
        }
        else {
            return false;
        }
    }
    return true;
}

/*
 * Description: prints the critical path of the current controller
 */
void Controller::getCriticalPath()
{
    Circuit circuit;
    circuit.getCriticalPath(_operations);
}


bool Controller::PerformScheduling(int latency)
{
	Circuit circuit;
	bool ret = circuit.ForceDirectedScheduling(_operations, latency);
	return ret;
}



