//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#ifndef CIRCUIT_H
#define CIRCUIT_H

#ifndef DEBUG_MODE
//#define DEBUG_MODE
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
    double* PredecessorForce;
	double* SuccessorForce;
	double* TotalForce;
	int* Schedule;
	int* scheduleCount;


    int* _ChildNodes;
    int* _ParentNodes;
    int* _AncestorNodes;
	int* _DescendantNodes;
	bool* node_done;
    int *_childCount;
    int *_parentCount;
    int* _ancestorCount;
	int* _descendantCount;
    double *_VertexLatency;
    int *_Q;
    int *_CriticalPathQ;
    int _CriticalPathLength;
	int* _ASAP;
	int* _ALAP;
	int* asapCount;
	int* alapCount;
	int* OperationDelay;
	int* ASAP_Op_Time;
	int* ALAP_Op_Time;
	int* FDS_Op_Time;
	int *Op_Width;
	int *Op_Interval;
    int numOps, Width;
	bool FDS_DONE;
	int iters = 0;
	int MaxIters = 1000;
    
public:
    Circuit();
    
    void getCriticalPath(vector<Operation> _operations);
    void getParentNodes(vector<Operation> _operations);
    void getChildNodes(vector<Operation> _operations);
    void getDescendantNodes(vector<Operation> _operations);
    double getOperationLatency(Operation op);

	bool ForceDirectedScheduling(vector<Operation> _operations, int latency, int* _Schedule, int* _ScheduleCount);
    
	void getASAP(vector<Operation> _operations, int latency);
	void getALAP(vector<Operation> _operations, int latency);
	void SortNodes(vector<Operation> _operations);
	void getTimeInterval(vector<Operation> _operations);
	void getProbablityMatrix(vector<Operation> _operations, int latency);
	void getSelfForce(vector<Operation> _operations, int latency);
    void getPredecessorForce(vector<Operation> _operations, int latency);
	void getSuccessorForce(vector<Operation> _operations, int latency);
	void getTotalForce(vector<Operation> _operations, int latency);
	void Initialize(vector<Operation> _operations, int latency);
    void addParent(vector<Operation> _operations, int addTo, int add, int* node);
    void fixChildNodes(vector<Operation> _operations, int latency);
    void getAncestorNodes(vector<Operation> _operations);
};
#endif


