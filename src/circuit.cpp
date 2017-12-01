//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//


#include <fstream>
//#include <iostream>
#include <algorithm>
#include "circuit.h"
#include "controller.h"
#include "input.h"
#include "node.h"
#include "operation.h"
#include <iomanip>

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

bool Circuit::ForceDirectedScheduling(vector<Operation> _operations, int latency)
{
	//Finds child and parent nodes for each node
	getChildNodes(_operations);
	getParentNodes(_operations);

	//Sorts each node into multiply, divide/modulo, adder/subtractor, logic elements
	SortNodes(_operations, latency);

	//Finds ASAP schedule
	getASAP(_operations, latency);

	//Finds ALAP schedule
	getALAP(_operations, latency);

	//Finds time interval and Width
	getTimeInterval(_operations, latency);

	//Calculates probability matrix and probability sums for each time
	getProbablityMatrix( _operations, latency);

	//Calculate self force
	getSelfForce(_operations, latency);
	return true;

}

void Circuit::getSelfForce(vector<Operation> _operations, int latency)
{

	SelfForce = new double[numOperations*latency];

	for (int ii = 0; ii < numOperations*latency; ii++) SelfForce[ii] = 0;
#ifdef DEBUG_MODE
	cout << endl <<"Node\tLatency\tloop\tSum" << endl;
#endif
	for (int yy = 0; yy < numOperations; yy++)
	{
		for (int xx = Op_Interval[yy * 2]; xx < Op_Interval[yy * 2 + 1] + 1; xx++)
		{
			if (_operations[yy].getOperator() == "*")
			{

				double sum = 0;
				for (int zz = Op_Interval[yy * 2]; zz < Op_Interval[yy * 2+1] + 1; zz++) 
				{

					if (zz == xx)
						sum += ProbMultSum[xx] * (1 - ProbMult[yy*latency + xx]);
					
					else 
						sum += ProbMultSum[zz] * (0 - ProbMult[yy*latency + zz]);

#ifdef DEBUG_MODE
					cout << yy << "\t" << xx << "\t" << zz << "\t" << sum << endl;
#endif
				}

				SelfForce[yy*latency + xx] = sum;

			}
			else if (_operations[yy].getOperator() == "/" || _operations[yy].getOperator() == "%") {

				double sum = 0;
				for (int zz = Op_Interval[yy * 2]; zz < Op_Interval[yy * 2 + 1] + 1; zz++)
				{

					if (zz == xx)
						sum += ProbDivSum[xx] * (1 - ProbDiv[yy*latency + xx]);

					else
						sum += ProbDivSum[zz] * (0 - ProbDiv[yy*latency + zz]);

#ifdef DEBUG_MODE
					cout << yy << "\t" << xx << "\t" << zz << "\t" << sum << endl;
#endif
				}

				SelfForce[yy*latency + xx] = sum;

			}
			else if (_operations[yy].getOperator() == "+" || _operations[yy].getOperator() == "-") {

				double sum = 0;
				for (int zz = Op_Interval[yy * 2]; zz < Op_Interval[yy * 2 + 1] + 1; zz++)
				{

					if (zz == xx)
						sum += ProbAdderSum[xx] * (1 - ProbAdder[yy*latency + xx]);

					else
						sum += ProbAdderSum[zz] * (0 - ProbAdder[yy*latency + zz]);

#ifdef DEBUG_MODE
					cout << yy << "\t" << xx << "\t" << zz << "\t" << sum << endl;
#endif
				}

				SelfForce[yy*latency + xx] = sum;

			}
			else {
				double sum = 0;
				for (int zz = Op_Interval[yy * 2]; zz < Op_Interval[yy * 2 + 1] + 1; zz++)
				{

					if (zz == xx)
						sum += ProbLogicSum[xx] * (1 - ProbLogic[yy*latency + xx]);

					else
						sum += ProbLogicSum[zz] * (0 - ProbLogic[yy*latency + zz]);

#ifdef DEBUG_MODE
					cout << yy << "\t" << xx << "\t" << zz << "\t" << sum << endl;
#endif
				}

				SelfForce[yy*latency + xx] = sum;


			}

		}
	}

#ifdef DEBUG_MODE
	for (int yy = 0; yy < numOperations; yy++) {
		for (int xx = 0; xx < latency; xx++) {
			cout << SelfForce[yy*latency + xx] << "    ";
		}
		cout << endl;
	}
#endif
}


void Circuit::getTimeInterval(vector<Operation> _operations, int latency)
{
	Op_Width = new int[numOperations];
	Op_Interval = new int[numOperations * 2];
#ifdef DEBUG_MODE
	cout << "ii\tWidth\tStart\tEnd" << endl;
#endif
	for (int ii = 0; ii < numOperations; ii++)
	{

		Op_Width[ii] = ALAP_Op_Time[ii] - ASAP_Op_Time[ii] + 1;
		Op_Interval[ii * 2] = ASAP_Op_Time[ii];
		Op_Interval[ii * 2 + 1] = ALAP_Op_Time[ii];
#ifdef DEBUG_MODE
		cout << ii << "\t" << Op_Width[ii] << "\t" << Op_Interval[ii * 2] << "\t" << Op_Interval[ii * 2 + 1] << endl;
#endif
	}
}
void Circuit::SortNodes(vector<Operation> _operations, int latency)
{
	for (int ii = 0; ii < numOperations; ii++)
	{
		if (_operations[ii].getOperator() == "*") MultNodes.push_back(ii);
		else if (_operations[ii].getOperator() == "/" || _operations[ii].getOperator() == "%") DivNodes.push_back(ii);
		else if (_operations[ii].getOperator() == "+" || _operations[ii].getOperator() == "-")  AdderNodes.push_back(ii);
		else LogicNodes.push_back(ii);
	}
}

void Circuit::getProbablityMatrix(vector<Operation> _operations, int latency)
{

	ProbMult = new double[latency*numOperations];
	ProbDiv = new double[latency*numOperations];
	ProbAdder = new double[latency*numOperations];
	ProbLogic = new double[latency*numOperations];

	ProbMultSum = new double[latency];
	ProbDivSum = new double[latency];
	ProbAdderSum = new double[latency];
	ProbLogicSum = new double[latency];

	for (int ii = 0; ii < latency*numOperations;ii++)
	{
		ProbMult[ii] = 0;
		ProbDiv[ii] = 0;
		ProbAdder[ii] = 0;
		ProbLogic[ii] = 0;
	}


	for (int yy = 0; yy < numOperations; yy++)
	{

		string op = _operations[yy].getOperator();

		if (op == "*")
		{
			for (int xx = Op_Interval[yy * 2];xx < Op_Interval[yy * 2 + 1] + 1; xx++)
			{
				ProbMult[yy*latency + xx] = 1 / (double)Op_Width[yy];
			}
		}
		else if (op == "/" || op == "%")
		{
			for (int xx = Op_Interval[yy * 2];xx < Op_Interval[yy * 2 + 1] + 1; xx++)
			{
				ProbDiv[yy*latency + xx] = 1 / (double)Op_Width[yy];
			}
		}
		else if (op == "+" || op == "-")
		{
			for (int xx = Op_Interval[yy * 2];xx < Op_Interval[yy * 2 + 1] + 1; xx++)
			{
				ProbAdder[yy*latency + xx] = 1 / (double)Op_Width[yy];
			}
		}
		else
		{
			for (int xx = Op_Interval[yy * 2];xx < Op_Interval[yy * 2 + 1] + 1; xx++)
			{
				ProbLogic[yy*latency + xx] = 1 / (double)Op_Width[yy];
			}
		}

	}


	for (int xx = 0; xx < latency; xx++) {
		double sumMult = 0;
		double sumDiv = 0;
		double sumAdder = 0;
		double sumLogic = 0;
		for (int yy = 0; yy < numOperations; yy++) {
			sumMult += ProbMult[yy*latency + xx];
			sumDiv += ProbDiv[yy*latency + xx];
			sumAdder += ProbAdder[yy*latency + xx];
			sumLogic += ProbLogic[yy*latency + xx];
		}
		ProbMultSum[xx] = sumMult;
		ProbDivSum[xx] = sumDiv;
		ProbAdderSum[xx] = sumAdder;
		ProbLogicSum[xx] = sumLogic;
	}


#ifdef DEBUG_MODE
	std::cout << std::fixed;
	std::cout << std::setprecision(3);

	cout << endl << "Prob Mult Matrix" << endl;
	for (int yy = 0; yy < numOperations; yy++) {
		for (int xx = 0; xx < latency; xx++) {
			cout << ProbMult[yy*latency + xx] << "    ";
		}
		cout << endl;
	}

	cout << endl << "Prob Mult Sum" << endl;
	for (int yy = 0; yy < 1; yy++) {
		for (int xx = 0; xx < latency; xx++) {
			cout << ProbMultSum[yy*latency + xx] << "    ";
		}
		cout << endl;
	}

	cout << endl << "Prob Div Matrix" << endl;
	for (int yy = 0; yy < numOperations; yy++) {
		for (int xx = 0; xx < latency; xx++) {
			cout << ProbDiv[yy*latency + xx] << "    ";
		}
		cout << endl;
	}

	cout << endl << "Prob Div Sum" << endl;
	for (int yy = 0; yy < 1; yy++) {
		for (int xx = 0; xx < latency; xx++) {
			cout << ProbDivSum[yy*latency + xx] << "    ";
		}
		cout << endl;
	}

	cout << endl << "Prob Adder Matrix" << endl;
	for (int yy = 0; yy < numOperations; yy++) {
		for (int xx = 0; xx < latency; xx++) {
			cout << ProbAdder[yy*latency + xx] << "    ";
		}
		cout << endl;
	}

	cout << endl << "Prob Adder Sum" << endl;
	for (int yy = 0; yy < 1; yy++) {
		for (int xx = 0; xx < latency; xx++) {
			cout << ProbAdderSum[yy*latency + xx] << "    ";
		}
		cout << endl;
	}
	cout << endl << "Prob Logic Matrix" << endl;
	for (int yy = 0; yy < numOperations; yy++) {
		for (int xx = 0; xx < latency; xx++) {
			cout << ProbLogic[yy*latency + xx] << "    ";
		}
		cout << endl;
	}

	cout << endl << "Prob Logic Sum" << endl;
	for (int yy = 0; yy < 1; yy++) {
		for (int xx = 0; xx < latency; xx++) {
			cout << ProbLogicSum[yy*latency + xx] << "    ";
		}
		cout << endl;
	}
#endif




}

void Circuit::getASAP(vector<Operation> _operations,int latency)
{



	int height = latency;
	int width = (int)_operations.size();

	_ASAP = new int[width*height];
	for (int ii = 0; ii < width*height;ii++) _ASAP[ii] = 0;
	int *xIdx = new int[height];
	for (int ii = 0; ii < height;ii++) xIdx[ii] = 0;

	int *StartTime = new int[numOperations];
	int *EndTime = new int[numOperations];
	bool *OpDone = new bool[numOperations];
	ASAP_Op_Time = new int[numOperations];

	OperationDelay = new int[numOperations];


	for (int ii = 0; ii < numOperations;ii++)
	{
		ASAP_Op_Time[ii] = 0;
		StartTime[ii] = -1;
		EndTime[ii] = -1;
		OpDone[ii] = false;
		string op = _operations[ii].getOperator();
		if (op == "*") OperationDelay[ii] = 2;
		else if (op == "/") OperationDelay[ii] = 3;
		else if (op == "%") OperationDelay[ii] = 3;
		else OperationDelay[ii] = 1;

#ifdef DEBUG_MODE
		cout <<"Node = " << ii << "\tOperator = " << op << "\tOperation Delay = " << OperationDelay[ii] << endl;
#endif
	}

#ifdef DEBUG_MODE
	cout << endl << "Parent Nodes" << endl;
	for (int yy = 0; yy < _operations.size(); yy++)
	{
		cout << "Operation " << yy << " :\tCount = " << _parentCount[yy] << " :\tParent Nodes :\t";
		for (int xx = 0; xx < _parentCount[yy]; xx++)
		{
			cout << _ParentNodes[yy*(numOperations - 1) + xx] << "  ";
		}
		cout << endl;
	}

	cout << endl << "Child Nodes" << endl;
	for (int yy = 0; yy < _operations.size(); yy++)
	{
		cout << "Operation " << yy << " :\tCount = " << _childCount[yy] <<" :\tChild Nodes :\t" ;
		for (int xx = 0; xx < _childCount[yy]; xx++)
		{
			cout << _ChildNodes[yy*(numOperations - 1) + xx] << "  ";

		}
		cout << endl;

	}
#endif

	//Loop through every operation
	for (int ii = 0; ii < latency; ii++)
	{
		for (int yy = 0; yy < _operations.size(); yy++)
		{
			if (EndTime[yy] == ii) OpDone[yy] = true;
			if (!OpDone[yy] && StartTime[yy] == -1)
			{
				if (_parentCount[yy] == 0)
				{
					_ASAP[ii*width + xIdx[ii]] = yy;
					xIdx[ii]++;
					StartTime[yy] = ii;
					EndTime[yy] = ii + OperationDelay[yy];
					ASAP_Op_Time[yy] = ii;
				}
				else
				{
					bool AllParentsComplete = true;
					for (int xx = 0; xx < _parentCount[yy]; xx++)
					{
						int Parent = _ParentNodes[yy*(numOperations - 1) + xx];

						if (!OpDone[Parent])
						{
							AllParentsComplete = false;
						}
					}
					if (AllParentsComplete)
					{
						_ASAP[ii*width + xIdx[ii]] = yy;
						xIdx[ii]++;
						StartTime[yy] = ii;
						EndTime[yy] = ii + OperationDelay[yy];
						ASAP_Op_Time[yy] = ii;
					}
				}
			}
			
		}
		
	}
	for (int ii = 0; ii < numOperations; ii++)
	{
		if (EndTime[ii] == latency) OpDone[ii] = true;
		if (!OpDone[ii]) printf("\nError: operation %d could not be scheduled/completed within latency constraint.\n", ii);
	}


#ifdef DEBUG_MODE
	cout << endl << "ASAP Schedule" << endl;
	for (int yy = 0; yy < latency; yy++)
	{
		cout << "Time " << yy+1 << " :\tCount = " << xIdx[yy] << " :\tNodes :\t";
		for (int xx = 0; xx < xIdx[yy]; xx++)
		{
			cout << _ASAP[yy*width + xx] << "  ";
		}
		cout << endl;
	}
#endif	


	delete[] xIdx;
	delete[] StartTime;
	delete[] EndTime;
	delete[] OpDone;

}


void Circuit::getALAP(vector<Operation> _operations, int latency)
{


	int height = latency;
	int width = (int)_operations.size();

	_ALAP = new int[width*height];
	for (int ii = 0; ii < width*height;ii++) _ASAP[ii] = 0;
	int *xIdx = new int[height];
	for (int ii = 0; ii < height;ii++) xIdx[ii] = 0;
	//int *yIdx = new int[latency];

	int *StartTime = new int[numOperations];
	int *EndTime = new int[numOperations];
	bool *OpDone = new bool[numOperations];
	//OperationDelay = new int[numOperations];
	ALAP_Op_Time = new int[numOperations];
#ifdef DEBUG_MODE
	cout << endl << "ALAP Scheduling" << endl;
#endif
	for (int ii = 0; ii < numOperations;ii++)
	{
		ALAP_Op_Time[ii] = 0;
		StartTime[ii] = -1;
		EndTime[ii] = -1;
		OpDone[ii] = false;
		string op = _operations[ii].getOperator();
		if (op == "*") OperationDelay[ii] = 2;
		else if (op == "/") OperationDelay[ii] = 3;
		else if (op == "%") OperationDelay[ii] = 3;
		else OperationDelay[ii] = 1;
#ifdef DEBUG_MODE
		cout << "Node = " << ii << "\tOperator = " << op << "\tOperation Delay = " << OperationDelay[ii] << endl;
#endif
	}
#ifdef DEBUG_MODE
	cout << endl << "Parent Nodes" << endl;
	for (int yy = 0; yy < _operations.size(); yy++)
	{
		cout << "Operation " << yy << " :\tCount = " << _parentCount[yy] << " :\tParent Nodes :\t";
		for (int xx = 0; xx < _parentCount[yy]; xx++)
		{
			cout << _ParentNodes[yy*(numOperations - 1) + xx] << "  ";
		}
		cout << endl;
	}

	cout << endl << "Child Nodes" << endl;
	for (int yy = 0; yy < _operations.size(); yy++)
	{
		cout << "Operation " << yy << " :\tCount = " << _childCount[yy] << " :\tChild Nodes :\t";
		for (int xx = 0; xx < _childCount[yy]; xx++)
		{
			cout << _ChildNodes[yy*(numOperations - 1) + xx] << "  ";
		}
		cout << endl;
	}
#endif
	//Loop through every operation
	for (int ii = latency-1; ii >= 0; ii--)
	{
		for (int yy = 0; yy < _operations.size(); yy++)
		{
			//if (StartTime[yy] == ii) OpDone[yy] = true;
			if (!OpDone[yy] && StartTime[yy] == -1)
			{
				if (_childCount[yy] == 0)
				{
					//_ALAP[ii*width + xIdx[ii]] = yy;
					//xIdx[ii]++;
					StartTime[yy] = ii - OperationDelay[yy]+1;
					EndTime[yy] = ii ;
					//ALAP_Op_Time[yy] = ii;
				}
				else
				{
					bool AllChildrenComplete = true;
					for (int xx = 0; xx < _childCount[yy]; xx++)
					{
						int Child = _ChildNodes[yy*(numOperations - 1) + xx];

						if (!OpDone[Child])
						{
							AllChildrenComplete = false;
						}
					}
					if (AllChildrenComplete)
					{
						
						
						StartTime[yy] = ii - OperationDelay[yy]+1;
						EndTime[yy] = ii ;
						
					}
				}
				
			}
			if (StartTime[yy] == ii) {
				OpDone[yy] = true;
				ALAP_Op_Time[yy] = ii;
				_ALAP[ii*width + xIdx[ii]] = yy;
				xIdx[ii]++;
			}
		}

	}
	for (int ii = 0; ii < numOperations; ii++)
	{
		if (EndTime[ii] == latency) OpDone[ii] = true;
		if (!OpDone[ii]) printf("\nError: operation %d could not be scheduled/completed within latency constraint.\n", ii);
	}


#ifdef DEBUG_MODE
	cout << endl << "ALAP Schedule" << endl;
	for (int yy = 0; yy < latency; yy++)
	{
		cout << "Time " << yy + 1 << " :\tCount = " << xIdx[yy] << " :\tNodes :\t";
		for (int xx = 0; xx < xIdx[yy]; xx++)
		{
			cout << _ALAP[yy*width + xx] << "  ";
		}
		cout << endl;
	}
#endif


	delete[] xIdx;
	delete[] StartTime;
	delete[] EndTime;
	delete[] OpDone;

}


void Circuit::getChildNodes(vector<Operation> _operations)
{
    
    Operation operation;
    numOperations = (int)_operations.size();
    Width =(int) numOperations - 1;
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
    numOperations = (int)_operations.size();
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
    
	delete[] xIdx;
    
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








