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

using namespace std;

/**
 * Description: contrusctor for Controller
 */
Controller::Controller(string inputFilePath, string outputFilePath){
    _inputFilePath = inputFilePath;
    _outputFilePath = outputFilePath;
    
    Node Clk = Node("input Int1 Clk, Rst", "Clk", 1, "input", true);
    Node Rst = Node("input Int1 Clk, Rst", "Rst", 1, "input", true);
    _variables.push_back(Clk);
    _variables.push_back(Rst);
    _modCount = vector<int>(14,0);
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
vector<int> Controller::getModCount() {   return _modCount;   }
void Controller::setModCount(vector<int> c) {   _modCount = c;  }

/*
 * Description: Read Cards from a File
 */

bool Controller::readFromFile() {
    ifstream inputFile(_inputFilePath);
    string lineBuffer;
    string verilogCode;
    Operation tempOperation;
    
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
                _operations.push_back(tempOperation);
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
 * Description: Write to the Output file
 */
string Controller::writeModule() {
    
    string moduleDef;
    string circuitName = _inputFilePath.substr(_inputFilePath.find_last_of("/")+1);
    
    moduleDef = "`timescale 1ns / 1ps\n\n";
    
    circuitName = circuitName.substr(0,circuitName.find(".txt"));
    moduleDef += "module z" + circuitName + "(";
    
    for (unsigned int i = 0; i < _variables.size(); i++){
        if (i == 0) {
            if (_variables.at(i).getVariableType() == "input" || _variables.at(i).getVariableType() == "output"){
                moduleDef += _variables.at(i).getVariableName();
            }
        }
        else {
            if (_variables.at(i).getVariableType() == "input" || _variables.at(i).getVariableType() == "output"){
                moduleDef += ", " + _variables.at(i).getVariableName();
            }
        }
    }
    
    moduleDef += ");";
    
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
    
    return verilogVariables;
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
    }
    outputFile << "endmodule";
    
    outputFile.close();
}

/*
 * Description: loops through the list of Operations and tries to convert each Operation
 */
bool Controller::convertOperations() {
    string verilog;
    for (Operation op : _operations) {
        verilog = op.convertOperation(_modCount);
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



