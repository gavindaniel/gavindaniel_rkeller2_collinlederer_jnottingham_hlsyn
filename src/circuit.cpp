//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//


#include <fstream>
#include <iostream>
#include <algorithm>
#include "circuit.h"
#include "controller.h"
#include "input.h"
#include "node.h"
#include "operation.h"

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
Circuit::Circuit() {
    
}


void Circuit::getChildNodes(vector<Operation> _operations)
{
    
    Operation operation;
    numOperations = _operations.size();
    Width = numOperations - 1;
    _ChildNodes = new int[numOperations*Width];
    
    for (int ii = 0; ii < numOperations*Width; ii++) _ChildNodes[ii] = 0;
    
    _childCount = new int[numOperations];
    string A, B, C, R;
    
    
    
    
    for (unsigned int ii = 0; ii < _operations.size(); ii++)
    {
        operation = _operations.at(ii);
        
        string outputVar = operation.getResult().getVariableName();
        
        int node_count = 0;
        
        
        //Look for all instances of outputVar further below vertex ii
        for (unsigned int xx = 0; xx < _operations.size(); xx++)
        {
            if (xx != ii)
            {
                Operation NextOperation = _operations.at(xx);
                A = NextOperation.get_varA().getVariableName();
                B = NextOperation.get_varB().getVariableName();
                C = NextOperation.get_varC().getVariableName();
                //R = NextOperation.getResult().getVariableName();
                
                if (outputVar == A || outputVar == B || outputVar == C)
                {
                    _ChildNodes[ii*Width + node_count] = xx;
                    node_count++;
                }
                
                
            }
        }
        _childCount[ii] = node_count;
        
    }
    
}

void Circuit::getParentNodes(vector<Operation> _operations)
{
    
    Operation operation;
    numOperations = _operations.size();
    Width = numOperations - 1;
    _ParentNodes = new int[numOperations*Width];
    for (int ii = 0; ii < numOperations*Width; ii++) _ParentNodes[ii] = 0;
    _parentCount = new int[numOperations];
    string A, B, C, R;
    
    
    
    
    for (unsigned int ii = 0; ii < _operations.size(); ii++)
    {
        operation = _operations.at(ii);
        
        string outputVar = operation.getResult().getVariableName();
        A = operation.get_varA().getVariableName();
        B = operation.get_varB().getVariableName();
        C = operation.get_varC().getVariableName();
        
        int node_count = 0;
        
        
        //Look for all instances of outputVar further below vertex ii
        for (unsigned int xx = 0; xx < _operations.size(); xx++)
        {
            if (xx != ii)
            {
                Operation NextOperation = _operations.at(xx);
                
                R = NextOperation.getResult().getVariableName();
                //If any of the inputs to the iith operation match an output
                //to the xxth operation, then the xxth operation is a parent node
                if (R == A || R == B || R == C)
                {
                    _ParentNodes[ii*Width + node_count] = xx;
                    node_count++;
                }
                
                
            }
        }
        _parentCount[ii] = node_count;
        
    }
    
}



void Circuit::getCriticalPath(vector<Operation> _operations)
{
    
    getChildNodes(_operations);
    getParentNodes(_operations);
    
    _VertexLatency = new double[numOperations];
    _Q = new int[numOperations];
    _CriticalPathQ = new int[numOperations];
    int *xIdx = new int[numOperations];
    for (int ii = 0; ii < numOperations;ii++)
    {
        xIdx[ii] = 0;
        _Q[ii] = 0;
    }
    
    for (int ii = 0; ii < numOperations; ii++)
    {
        _VertexLatency[ii] = getOperationLatency(_operations.at(ii));
    }
    
    
    
    double CurrentPath = 0;
    double CriticalPath = 0;
    for (int ii = 0; ii < numOperations;ii++)
    {
        CurrentPath = 0;
        if (_parentCount[ii] == 0)
        {
            int qIdx = 0;
            int yy = ii;
            _Q[qIdx] = yy;
            qIdx++;
            CurrentPath += _VertexLatency[ii];
            
            bool paths_complete = false;
            while (!paths_complete)
            {
                if (xIdx[yy] < _childCount[yy])
                {
                    _Q[qIdx] = _ChildNodes[yy*Width + xIdx[yy]];
                    xIdx[yy]++;
                    yy = _Q[qIdx];
                    CurrentPath += _VertexLatency[yy];
                    qIdx++;
                }
                else
                {
                    
                    if (CurrentPath > CriticalPath)
                    {
                        CriticalPath = CurrentPath;
                        _CriticalPathLength = qIdx;
                        for (int zz = 0;zz < numOperations; zz++) _CriticalPathQ[zz] = _Q[zz];
                    }
                    
                    xIdx[yy] = 0;
                    qIdx--;
                    
                    CurrentPath -= _VertexLatency[yy];
                    
                    if (qIdx > 0)
                    {
                        
                        yy = _Q[qIdx - 1];
                        
                        _Q[qIdx] = 0;
                        
                    }
                    else if (qIdx == 0) yy = ii;
                    else
                    {
                        paths_complete = true;
                    }
                }
            }
        }
        
    }
    
    
    printf("\n\nCritical Path : %.3f ns\n\n", CriticalPath);
    
    
}



double Circuit::getOperationLatency(Operation op)
{
    double time = 0;
    string operation = op.getOperator();
    if (op.getOperator() == "=") { //REG
        if (op.getResult().getBitWidth() == 1) {
            time = 2.616;
        }
        if (op.getResult().getBitWidth() == 2) {
            time = 2.644;
        }
        if (op.getResult().getBitWidth() == 8) {
            time = 2.879;
        }
        if (op.getResult().getBitWidth() == 16) {
            time = 3.061;
        }
        if (op.getResult().getBitWidth() == 32) {
            time = 3.602;
        }
        if (op.getResult().getBitWidth() == 64) {
            time = 3.966;
        }
    }
    else if (op.getOperator() == "+") { //ADD
        if (op.getResult().getBitWidth() == 1) {
            time = 2.704;
        }
        if (op.getResult().getBitWidth() == 2) {
            time = 3.713;
        }
        if (op.getResult().getBitWidth() == 8) {
            time = 4.924;
        }
        if (op.getResult().getBitWidth() == 16) {
            time = 5.638;
        }
        if (op.getResult().getBitWidth() == 32) {
            time = 7.270;
        }
        if (op.getResult().getBitWidth() == 64) {
            time = 9.566;
        }
    }
    else if (op.getOperator() == "-") { //SUB
        if (op.getResult().getBitWidth() == 1) {
            time = 3.024;
        }
        if (op.getResult().getBitWidth() == 2) {
            time = 3.412;
        }
        if (op.getResult().getBitWidth() == 8) {
            time = 4.890;
        }
        if (op.getResult().getBitWidth() == 16) {
            time = 5.569;
        }
        if (op.getResult().getBitWidth() == 32) {
            time = 7.253;
        }
        if (op.getResult().getBitWidth() == 64) {
            time = 9.566;
        }
    }
    else if (op.getOperator() == "*") { //MUL
        if (op.getResult().getBitWidth() == 1) {
            time = 2.438;
        }
        if (op.getResult().getBitWidth() == 2) {
            time = 3.651;
        }
        if (op.getResult().getBitWidth() == 8) {
            time = 7.453;
        }
        if (op.getResult().getBitWidth() == 16) {
            time = 7.811;
        }
        if (op.getResult().getBitWidth() == 32) {
            time = 12.395;
        }
        if (op.getResult().getBitWidth() == 64) {
            time = 15.354;
        }
    }
    else if (op.getOperator() == ">" || op.getOperator() == "<" || op.getOperator() == "==") { //COMP
        if (op.getResult().getBitWidth() == 1) {
            time = 3.031;
        }
        if (op.getResult().getBitWidth() == 2) {
            time = 3.934;
        }
        if (op.getResult().getBitWidth() == 8) {
            time = 7.453;
        }
        if (op.getResult().getBitWidth() == 16) {
            time = 7.811;
        }
        if (op.getResult().getBitWidth() == 32) {
            time = 12.395;
        }
        if (op.getResult().getBitWidth() == 64) {
            time = 15.354;
        }
    }
    else if (op.getOperator() == ":") { //MUX
        if (op.getResult().getBitWidth() == 1) {
            time = 4.083;
        }
        if (op.getResult().getBitWidth() == 2) {
            time = 4.115;
        }
        if (op.getResult().getBitWidth() == 8) {
            time = 4.815;
        }
        if (op.getResult().getBitWidth() == 16) {
            time = 5.623;
        }
        if (op.getResult().getBitWidth() == 32) {
            time = 8.079;
        }
        if (op.getResult().getBitWidth() == 64) {
            time = 8.766;
        }
    }
    else if (op.getOperator() == ">>") { //SHR
        if (op.getResult().getBitWidth() == 1) {
            time = 3.644;
        }
        if (op.getResult().getBitWidth() == 2) {
            time = 4.007;
        }
        if (op.getResult().getBitWidth() == 8) {
            time = 5.178;
        }
        if (op.getResult().getBitWidth() == 16) {
            time = 6.460;
        }
        if (op.getResult().getBitWidth() == 32) {
            time = 8.819;
        }
        if (op.getResult().getBitWidth() == 64) {
            time = 11.095;
        }
    }
    else if (op.getOperator() == "<<") { //SHL
        if (op.getResult().getBitWidth() == 1) {
            time = 3.614;
        }
        if (op.getResult().getBitWidth() == 2) {
            time = 3.980;
        }
        if (op.getResult().getBitWidth() == 8) {
            time = 5.152;
        }
        if (op.getResult().getBitWidth() == 16) {
            time = 6.549;
        }
        if (op.getResult().getBitWidth() == 32) {
            time = 8.565;
        }
        if (op.getResult().getBitWidth() == 64) {
            time = 11.220;
        }
    }
    else if (op.getOperator() == "/") { //DIV
        if (op.getResult().getBitWidth() == 1) {
            time = 0.619;
        }
        if (op.getResult().getBitWidth() == 2) {
            time = 2.144;
        }
        if (op.getResult().getBitWidth() == 8) {
            time = 15.439;
        }
        if (op.getResult().getBitWidth() == 16) {
            time = 33.093;
        }
        if (op.getResult().getBitWidth() == 32) {
            time = 86.312;
        }
        if (op.getResult().getBitWidth() == 64) {
            time = 243.233;
        }
    }
    else if (op.getOperator() == "%") { //MOD
        if (op.getResult().getBitWidth() == 1) {
            time = 0.758;
        }
        if (op.getResult().getBitWidth() == 2) {
            time = 2.149;
        }
        if (op.getResult().getBitWidth() == 8) {
            time = 16.078;
        }
        if (op.getResult().getBitWidth() == 16) {
            time = 35.563;
        }
        if (op.getResult().getBitWidth() == 32) {
            time = 88.142;
        }
        if (op.getResult().getBitWidth() == 64) {
            time = 250.583;
        }
    }
    
    else if (op.getOperator() == "++") { //INC
        if (op.getResult().getBitWidth() == 1) {
            time = 1.792;
        }
        if (op.getResult().getBitWidth() == 2) {
            time = 2.218;
        }
        if (op.getResult().getBitWidth() == 8) {
            time = 3.111;
        }
        if (op.getResult().getBitWidth() == 16) {
            time = 3.471;
        }
        if (op.getResult().getBitWidth() == 32) {
            time = 4.347;
        }
        if (op.getResult().getBitWidth() == 64) {
            time = 6.200;
        }
    }
    else if (op.getOperator() == "--") { //DEC
        if (op.getResult().getBitWidth() == 1) {
            time = 1.792;
        }
        if (op.getResult().getBitWidth() == 2) {
            time = 2.218;
        }
        if (op.getResult().getBitWidth() == 8) {
            time = 3.108;
        }
        if (op.getResult().getBitWidth() == 16) {
            time = 3.701;
        }
        if (op.getResult().getBitWidth() == 32) {
            time = 4.685;
        }
        if (op.getResult().getBitWidth() == 64) {
            time = 6.503;
        }
    }
    return time;
}








