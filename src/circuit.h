//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <fstream>
#include <iostream>
#include <algorithm>
#include "circuit.h"
#include "controller.h"
#include "input.h"
#include "node.h"
#include "operation.h"

using namespace std;

class Circuit{
private:
    
    std::vector<string> _lines;
    
    int* _ChildNodes;
    int* _ParentNodes;
    int *_childCount;
    int *_parentCount;
    double *_VertexLatency;
    int *_Q;
    int *_CriticalPathQ;
    int _CriticalPathLength;
    
    int numOperations, Width;
    
public:
    Circuit();
    
    void getCriticalPath(vector<Operation> _operations);
    void getParentNodes(vector<Operation> _operations);
    void getChildNodes(vector<Operation> _operations);
    double getOperationLatency(Operation op);
    
};
#endif


