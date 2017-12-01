//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#ifndef CIRCUIT_H
#define CIRCUIT_H

#ifndef DEBUG_MODE
#define DEBUG_MODE
#endif

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
	std::vector<int> MultNodes;
	std::vector<int> DivNodes;
	std::vector<int> AdderNodes;
	std::vector<int> LogicNodes;
	
	double *ProbMultSum;
	double *ProbDivSum;
	double *ProbAdderSum;
	double *ProbLogicSum;

	double* ProbMult;
	double* ProbDiv;
	double* ProbAdder;
	double* ProbLogic;

	double* SelfForce;


    int* _ChildNodes;
    int* _ParentNodes;
    int *_childCount;
    int *_parentCount;
    double *_VertexLatency;
    int *_Q;
    int *_CriticalPathQ;
    int _CriticalPathLength;
	int* _ASAP;
	int* _ALAP;
	int* OperationDelay;
	int* ASAP_Op_Time;
	int* ALAP_Op_Time;
	int *Op_Width;
	int *Op_Interval;
    int numOperations, Width;
    
public:
    Circuit();
    
    void getCriticalPath(vector<Operation> _operations);
    void getParentNodes(vector<Operation> _operations);
    void getChildNodes(vector<Operation> _operations);
    double getOperationLatency(Operation op);

	bool ForceDirectedScheduling(vector<Operation> _operations, int latency);
    
	void getASAP(vector<Operation> _operations, int latency);
	void getALAP(vector<Operation> _operations, int latency);
	void SortNodes(vector<Operation> _operations, int latency);
	void getTimeInterval(vector<Operation> _operations, int latency);
	void getProbablityMatrix(vector<Operation> _operations, int latency);
	void getSelfForce(vector<Operation> _operations, int latency);
};
#endif


