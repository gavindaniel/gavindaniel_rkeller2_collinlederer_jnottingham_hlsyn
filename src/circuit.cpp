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
#include <stdexcept>

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



bool Circuit::ForceDirectedScheduling(vector<Operation> _operations, int latency, int* _Schedule, int* _ScheduleCount)
{
    //Initialize parameters and allocate arrays
    Initialize(_operations, latency);
    
    //Finds child and parent nodes for each node
    getChildNodes(_operations);
    getDescendantNodes(_operations);
    getParentNodes(_operations);
    getAncestorNodes(_operations);
    fixChildNodes(_operations, latency);
    getDescendantNodes(_operations);
    getAncestorNodes(_operations);
    
    //Sorts each node into multiply, divide/modulo, adder/subtractor, logic elements
    SortNodes(_operations);
    
    //Finds ASAP schedule
    getASAP(_operations, latency);
    
    //Finds ALAP schedule
    getALAP(_operations, latency);
    
    //Finds time interval and Width
    getTimeInterval(_operations);
    
    //Find all of the total forces in each iteration,
    //and choose minimum force. Lock that node done and
    //repeat until all nodes are scheduled.
    while (!FDS_DONE)
    {
        //Calculates probability matrix and probability sums for each time
        getProbablityMatrix(_operations, latency);
        
        //Calculate self force
        getSelfForce(_operations, latency);
        
        //Calculate predecessor forces
        getPredecessorForce(_operations, latency);
        
        //Calculate successor forces
        getSuccessorForce(_operations, latency);
        
        //Calculate successor forces
        getTotalForce(_operations, latency);
    }
    
    
    //Copy to output pointer
	for (int ii = 0; ii < numOps*latency;ii++)
		_Schedule[ii] = Schedule[ii];

	for (int ii = 0; ii < latency;ii++)
		_ScheduleCount[ii] = scheduleCount[ii];
    
    return true;
    
}


void Circuit::Initialize(vector<Operation> _operations, int latency)
{
    numOps = (int)_operations.size();
    Width = (int)numOps - 1;
    FDS_DONE = false;
    _ChildNodes = new int[numOps*Width];
    _DescendantNodes = new int[numOps*Width];
    _ParentNodes = new int[numOps*Width];
    _AncestorNodes = new int[numOps*Width];
    
    for (int ii = 0; ii < numOps*Width; ii++)
    {
        _ChildNodes[ii] = 0;
        _DescendantNodes[ii] = 0;
        _ParentNodes[ii] = 0;
        _AncestorNodes[ii] = 0;
    }
    _childCount = new int[numOps];
    _descendantCount = new int[numOps];
    _parentCount = new int[numOps];
    _ancestorCount = new int[numOps];
    
    _ASAP = new int[numOps*latency];
    _ALAP = new int[numOps*latency];
    
    for (int ii = 0; ii < numOps*latency;ii++)
    {
        _ASAP[ii] = -1;
        _ALAP[ii] = -1;
    }
    
    asapCount = new int[latency];
    alapCount = new int[latency];
    
    for (int ii = 0; ii < latency;ii++)
    {
        asapCount[ii] = 0;
        alapCount[ii] = 0;
    }
    
    
    ASAP_Op_Time = new int[numOps];
    ALAP_Op_Time = new int[numOps];
    FDS_Op_Time  = new int[numOps];
    
    
    
    OperationDelay = new int[numOps];
    Op_Width = new int[numOps];
    Op_Interval = new int[numOps * 2];
    
    ProbMult = new double[latency*numOps];
    ProbDiv = new double[latency*numOps];
    ProbAdder = new double[latency*numOps];
    ProbLogic = new double[latency*numOps];
    
    ProbMultSum = new double[latency];
    ProbDivSum = new double[latency];
    ProbAdderSum = new double[latency];
    ProbLogicSum = new double[latency];
    
    
    SelfForce = new double[numOps*latency];
    PredecessorForce = new double[numOps*latency];
    SuccessorForce = new double[numOps*latency];
    
    TotalForce = new double[numOps*latency];
    Schedule = new int[numOps*latency];
    scheduleCount = new int[latency];
    node_done = new bool[numOps];
    
    for (int ii = 0; ii < numOps;ii++)
    {
        ASAP_Op_Time[ii] = -1;
        ALAP_Op_Time[ii] = -1;
        FDS_Op_Time[ii] = -1;
        node_done[ii] = false;
    }
    for (int ii = 0; ii < latency; ii++) scheduleCount[ii] = 0;
    for (int ii = 0; ii < numOps*latency;ii++) Schedule[ii] = -1;
    for (int ii = 0; ii < numOps; ii++) node_done[ii] = false;
    
    
}

void Circuit::getTotalForce(vector<Operation> _operations, int latency)
{
    //reset total force
    for (int ii = 0; ii < numOps*latency; ii++) TotalForce[ii] = 0;
    
    //Total force is the sum of all the forces
    for (int node = 0; node < numOps;node++)
    {
        for (int time = 0; time < latency;time++)
        {
            TotalForce[node*latency + time] = SelfForce[node*latency + time] + PredecessorForce[node*latency + time] + SuccessorForce[node*latency + time];
        }
    }
    
    //Find and schedule minimum force for each node
    //loop through each time
    double min = 999;
    int min_x = -1;
    int min_y = -1;
    bool alldone = true;
    for (int yy = 0; yy < numOps;yy++)
    {
        if (!node_done[yy]) {
            alldone = false;
            //sort nodes in asap order
            for (int xx = Op_Interval[yy*2]; xx < Op_Interval[yy*2+1]+1; xx++)
            {
                if (TotalForce[yy*latency + xx] < min)
                {
                    min = TotalForce[yy*latency + xx];
                    min_x = xx;
                    min_y = yy;
                }
            }
        }
    }
    
#ifdef DEBUG_MODE
    cout << "\nTotal Forces\n";
    for (int yy = 0; yy < numOps; yy++) {
        cout << "Node = " << yy << "\t: ";
        for (int xx = 0; xx < latency; xx++) {
            cout << TotalForce[yy*latency + xx] << "    ";
        }
        cout << endl;
    }
#endif
    
    if (!alldone)
    {
        if (min_x != -1 && min_y != -1) {
            //schedule the absolute minimum force
            Schedule[min_x*numOps + scheduleCount[min_x]] = min_y;
            scheduleCount[min_x]++;
            
#ifdef DEBUG_MODE
            cout << "\nScheduling node  " << min_y << "  in time " << min_x << endl;
            cout << endl << "Force Directed Schedule" << endl;
            for (int yy = 0; yy < latency; yy++)
            {
                cout << "Time " << yy + 1 << " :\tCount = " << scheduleCount[yy] << " :\tNodes :\t";
                for (int xx = 0; xx < scheduleCount[yy]; xx++)
                {
                    cout << Schedule[yy*numOps + xx] << "  ";
                }
                cout << endl;
            }
#endif
            
            //Since a node has been scheduled, we need to update node interval and width information
            //Loop through all descendants and update start time if it fall before or at current time
#ifdef DEBUG_MODE
            cout << endl;
#endif
            int descendantDiff = min_x - Op_Interval[min_y * 2];
            for (int child = 0; child < _descendantCount[min_y]; child++)
            {
                int CurrentChild = _DescendantNodes[min_y*Width + child];
                if (!node_done[CurrentChild]) {
                    int OldStart = Op_Interval[CurrentChild * 2];
                    int CurrentEnd = Op_Interval[CurrentChild * 2+1];
                    int NewStart = OldStart + descendantDiff;
                    if (NewStart > CurrentEnd) NewStart = CurrentEnd;
                    if (NewStart > OldStart && Op_Width[CurrentChild] > 1) {
#ifdef DEBUG_MODE
                        cout << "Current descendant = " << CurrentChild << "\tOld Start = " << OldStart << "\t New Start = " << NewStart << endl;
#endif
                        Op_Interval[CurrentChild * 2] = NewStart;
                        Op_Width[CurrentChild] = Op_Interval[CurrentChild * 2 + 1] - Op_Interval[CurrentChild * 2] + 1;
                    }
                }
            }
            
            //Loop through all ancestors and update end time if it fall after or at current time
#ifdef DEBUG_MODE
            cout << endl;
#endif
            int minNodeEnd = Op_Interval[min_y * 2 + 1];
            int ancestorDiff = minNodeEnd - min_x;
            for (int parent = 0; parent < _ancestorCount[min_y]; parent++)
            {
                int CurrentParent = _AncestorNodes[min_y*Width + parent];
                if (!node_done[CurrentParent]) {
                    int OldEnd = Op_Interval[CurrentParent * 2 + 1];
                    int CurrentStart = Op_Interval[CurrentParent * 2];
                    int NewEnd = OldEnd - ancestorDiff;
                    if (NewEnd > CurrentStart) NewEnd = CurrentStart;
                    if (NewEnd < OldEnd && Op_Width[CurrentParent] > 1) {
#ifdef DEBUG_MODE
                        cout << "Current ancestor = " << CurrentParent << "\tOld end = " << OldEnd << "\t New End = " << NewEnd << endl;
#endif
                        Op_Interval[CurrentParent * 2 + 1] = NewEnd;
                        Op_Width[CurrentParent] = Op_Interval[CurrentParent * 2 + 1] - Op_Interval[CurrentParent * 2] + 1;
                    }
                }
            }
            
            Op_Interval[min_y * 2] = min_x;
            Op_Interval[min_y * 2 + 1] = min_x;
            Op_Width[min_y] = 1;
            node_done[min_y] = true;
#ifdef DEBUG_MODE
            cout << "\nUpdated Interval\nNode\tWidth\tStart\tEnd\n";
            for (int ii = 0; ii < numOps; ii++)
            {
                cout << ii << "\t" << Op_Width[ii] << "\t" << Op_Interval[ii * 2] << "\t" << Op_Interval[ii * 2 + 1] << endl;
                
            }
            
#endif
        }
        int stophere = 0;
    }
    else
        FDS_DONE = true;
    
    if (iters >= MaxIters) {
        printf("Error: Maximum allowed number of iterations (%d) reached", MaxIters);
        FDS_DONE = true;
    }
    iters++;
    
    
    
}

void Circuit::getSelfForce(vector<Operation> _operations, int latency)
{
    
    
    for (int ii = 0; ii < numOps*latency; ii++) SelfForce[ii] = 0;
    
#ifdef DEBUG_MODE
    cout << endl <<"Node\tLatency\tloop\tSum" << endl;
#endif
    for (int yy = 0; yy < numOps; yy++)
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
                    //cout << yy << "\t" << xx << "\t" << zz << "\t" << sum << endl;
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
                    //cout << yy << "\t" << xx << "\t" << zz << "\t" << sum << endl;
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
                    //cout << yy << "\t" << xx << "\t" << zz << "\t" << sum << endl;
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
                    //cout << yy << "\t" << xx << "\t" << zz << "\t" << sum << endl;
#endif
                }
                
                SelfForce[yy*latency + xx] = sum;
            }
        }
    }
    
#ifdef DEBUG_MODE
    cout << "\nSelf Forces\n";
    for (int yy = 0; yy < numOps; yy++) {
        cout << "Node = " << yy << "\t: ";
        for (int xx = 0; xx < latency; xx++) {
            cout << SelfForce[yy*latency + xx] << "    ";
        }
        cout << endl;
    }
#endif
}

void Circuit::getPredecessorForce(vector<Operation> _operations, int latency){
    
    
    
    for (int ii = 0; ii < numOps*latency; ii++) PredecessorForce[ii] = 0;
    
    for (int yy = 0; yy < numOps; yy++)
    {
        for (int xx = Op_Interval[yy * 2]; xx < Op_Interval[yy * 2 + 1] + 1; xx++)
        {
            double parentSum = 0;
            for (int zz = 0; zz < _ancestorCount[yy]; zz++ )
            {
                if(Op_Width[yy] > 1)
                {
                    int parent = _AncestorNodes[yy*(numOps-1)+zz];
                    int endTime = xx-OperationDelay[parent]+1;
                    for(int ChildCount = 0; ChildCount < _childCount[parent];ChildCount++)
                    {
                        for(int ParentCount = 0; ParentCount < _ancestorCount[yy];ParentCount++)
                        {
                            if(_ChildNodes[parent*(numOps-1)+ChildCount] == _AncestorNodes[yy*(numOps-1)+ParentCount])
                            {
                                int newEndTime = endTime - OperationDelay[_ChildNodes[parent*(numOps-1)+ChildCount]];
                                if (newEndTime < endTime)
                                {
                                    endTime = newEndTime;
                                }
                            }
                        }
                    }
                    for(int xxx = Op_Interval[parent*2];xxx<endTime;xxx++)
                    {
                        parentSum += SelfForce[parent * latency + xxx];
                    }
                }
            }
            PredecessorForce[yy*latency+xx] = parentSum;
        }
    }
#ifdef DEBUG_MODE
    cout << "\nPredecessor Forces" << endl;
    for (int yy = 0; yy < numOps; yy++) {
        cout << "Node = " << yy << "\t: ";
        for (int xx = 0; xx < latency; xx++) {
            cout << PredecessorForce[yy*latency + xx] << "    ";
        }
        cout << endl;
    }
#endif
}




void Circuit::getSuccessorForce(vector<Operation> _operations, int latency)
{
    
    for (int ii = 0; ii < numOps*latency; ii++) SuccessorForce[ii] = 0;
    
    /*
     for (int yy = 0; yy < numOps; yy++)
     {
     for (int xx = Op_Interval[yy * 2]; xx < Op_Interval[yy * 2 + 1] + 1; xx++)
     {
     double parentSum = 0;
     for (int zz = 0; zz < _ancestorCount[yy]; zz++ )
     {
     */
    //Loop over each node
    for (int yy = 0; yy < numOps; yy++)
    {
        //Loop over current nodes time frame
        for (int xx = Op_Interval[yy * 2]; xx < Op_Interval[yy * 2 + 1] + 1; xx++)
        {
            
            
            double childSum = 0;
            //Loop through al of the descendants of current node
            for (int zz = 0; zz < _descendantCount[yy]; zz++)
            {
                /*
                 if(Op_Width[yy] > 1)
                 {
                 int parent = _AncestorNodes[yy*(numOps-1)+zz];
                 int endTime = xx-OperationDelay[parent]+1;
                 */
                if (Op_Width[yy] > 1)
                {
                    //Current descendant node
                    int descendant = _DescendantNodes[yy*(numOps - 1) + zz];
                    
                    //Start at next node
                    int startTime = xx + 1;
                    
                    /*
                     for(int ChildCount = 0; ChildCount < _childCount[parent];ChildCount++)
                     {
                     for(int ParentCount = 0; ParentCount < _ancestorCount[yy];ParentCount++)
                     {
                     if(_ChildNodes[parent*(numOps-1)+ChildCount] == _AncestorNodes[yy*(numOps-1)+ParentCount])
                     {
                     */
                    
                    for (int ParentIdx = 0; ParentIdx < _parentCount[descendant];ParentIdx++)
                    {
                        for (int ChildIdx = 0; ChildIdx < _descendantCount[yy];ChildIdx++)
                        {
                            if (_ParentNodes[descendant*(numOps - 1) + ParentIdx] == _DescendantNodes[yy*(numOps - 1) + ChildIdx])
                            {
                                /*
                                 int newEndTime = endTime - OperationDelay[_ChildNodes[parent*(numOps-1)+ChildCount]];
                                 if (newEndTime < endTime)
                                 {
                                 endTime = newEndTime;
                                 }
                                 }
                                 }
                                 }
                                 */
                                int newStartTime = startTime + OperationDelay[_ParentNodes[descendant*(numOps - 1) + ParentIdx]];
                                if (newStartTime > startTime)
                                {
                                    startTime = newStartTime;
                                }
                                
                            }
                        }
                    }
                    for (int xxx = startTime; xxx < Op_Interval[descendant * 2+1]+1;xxx++)
                    {
                        childSum += SelfForce[descendant * latency + xxx];
                    }
                }
            }
            SuccessorForce[yy*latency + xx] = childSum;
        }
    }
#ifdef DEBUG_MODE
    cout << "\nSucessor Forces" << endl;
    for (int yy = 0; yy < numOps; yy++) {
        cout << "Node = " << yy << "\t: ";
        for (int xx = 0; xx < latency; xx++) {
            cout << SuccessorForce[yy*latency + xx] << "    ";
        }
        cout << endl;
    }
#endif
}






void Circuit::getTimeInterval(vector<Operation> _operations)
{
    
#ifdef DEBUG_MODE
    cout << "ii\tWidth\tStart\tEnd" << endl;
#endif
    for (int ii = 0; ii < numOps; ii++)
    {
        
        Op_Width[ii] = ALAP_Op_Time[ii] - ASAP_Op_Time[ii] + 1;
        Op_Interval[ii * 2] = ASAP_Op_Time[ii];
        Op_Interval[ii * 2 + 1] = ALAP_Op_Time[ii];
#ifdef DEBUG_MODE
        cout << ii << "\t" << Op_Width[ii] << "\t" << Op_Interval[ii * 2] << "\t" << Op_Interval[ii * 2 + 1] << endl;
#endif
    }
}
void Circuit::SortNodes(vector<Operation> _operations)
{
    for (int ii = 0; ii < numOps; ii++)
    {
        if (_operations[ii].getOperator() == "*") MultNodes.push_back(ii);
        else if (_operations[ii].getOperator() == "/" || _operations[ii].getOperator() == "%") DivNodes.push_back(ii);
        else if (_operations[ii].getOperator() == "+" || _operations[ii].getOperator() == "-")  AdderNodes.push_back(ii);
        else LogicNodes.push_back(ii);
    }
}

void Circuit::getProbablityMatrix(vector<Operation> _operations, int latency)
{
    
    
    
    for (int ii = 0; ii < latency*numOps;ii++)
    {
        ProbMult[ii] = 0;
        ProbDiv[ii] = 0;
        ProbAdder[ii] = 0;
        ProbLogic[ii] = 0;
    }
    
    for (int yy = 0; yy < numOps; yy++)
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
        for (int yy = 0; yy < numOps; yy++) {
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
    for (int yy = 0; yy < numOps; yy++) {
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
    for (int yy = 0; yy < numOps; yy++) {
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
    for (int yy = 0; yy < numOps; yy++) {
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
    for (int yy = 0; yy < numOps; yy++) {
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
    
    
    
    int *StartTime = new int[numOps];
    int *EndTime = new int[numOps];
    bool *OpDone = new bool[numOps];
    
    
    
    for (int ii = 0; ii < numOps;ii++)
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
                    _ASAP[ii*numOps + asapCount[ii]] = yy;
                    asapCount[ii]++;
                    StartTime[yy] = ii;
                    EndTime[yy] = ii + OperationDelay[yy];
                    ASAP_Op_Time[yy] = ii;
                }
                else
                {
                    bool AllParentsComplete = true;
                    for (int xx = 0; xx < _parentCount[yy]; xx++)
                    {
                        int Parent = _ParentNodes[yy*(numOps - 1) + xx];
                        
                        if (!OpDone[Parent])
                        {
                            AllParentsComplete = false;
                        }
                    }
                    if (AllParentsComplete)
                    {
                        _ASAP[ii*numOps + asapCount[ii]] = yy;
                        asapCount[ii]++;
                        StartTime[yy] = ii;
                        EndTime[yy] = ii + OperationDelay[yy];
                        ASAP_Op_Time[yy] = ii;
                    }
                }
            }
            
        }
        
    }
    for (int ii = 0; ii < numOps; ii++)
    {
        if (EndTime[ii] == latency) OpDone[ii] = true;
        if (!OpDone[ii]) {
            printf("\nError: operation %d could not be scheduled/completed within latency constraint.\n", ii);
            throw ;
        }
    }
    
    
#ifdef DEBUG_MODE
    cout << endl << "ASAP Schedule" << endl;
    for (int yy = 0; yy < latency; yy++)
    {
        cout << "Time " << yy+1 << " :\tCount = " << asapCount[yy] << " :\tNodes :\t";
        for (int xx = 0; xx < asapCount[yy]; xx++)
        {
            cout << _ASAP[yy*numOps + xx] << "  ";
        }
        cout << endl;
    }
#endif
    
    
    
    delete[] StartTime;
    delete[] EndTime;
    delete[] OpDone;
    
}


void Circuit::getALAP(vector<Operation> _operations, int latency)
{
    
    int *StartTime = new int[numOps];
    int *EndTime = new int[numOps];
    bool *OpDone = new bool[numOps];
    
    
#ifdef DEBUG_MODE
    cout << endl << "ALAP Scheduling" << endl;
#endif
    for (int ii = 0; ii < numOps;ii++)
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
                        int Child = _ChildNodes[yy*(numOps - 1) + xx];
                        
                        if (!OpDone[Child] || EndTime[Child] == ii)
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
                _ALAP[ii*numOps + alapCount[ii]] = yy;
                alapCount[ii]++;
            }
        }
        
    }
    for (int ii = 0; ii < numOps; ii++)
    {
        if (EndTime[ii] == latency) OpDone[ii] = true;
        if (!OpDone[ii]) printf("\nError: operation %d could not be scheduled/completed within latency constraint.\n", ii);
    }
    
    
#ifdef DEBUG_MODE
    cout << endl << "ALAP Schedule" << endl;
    for (int yy = 0; yy < latency; yy++)
    {
        cout << "Time " << yy + 1 << " :\tCount = " << alapCount[yy] << " :\tNodes :\t";
        for (int xx = 0; xx < alapCount[yy]; xx++)
        {
            cout << _ALAP[yy*numOps + xx] << "  ";
        }
        cout << endl;
    }
    for (int zz = 0; zz <numOps; zz++){
        cout << ALAP_Op_Time[zz] << endl;
    }
#endif
    
    
    delete[] StartTime;
    delete[] EndTime;
    delete[] OpDone;
    
}


void Circuit::getChildNodes(vector<Operation> _operations)
{
    
    Operation operation;
    
    
    string A, B, C, R, D1;
    
    
    
    
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
                
                if (outputVar != "" && (outputVar == A || outputVar == B || outputVar == C))
                {
                    bool there_is_if = false;
                    for(int parent_checker = 0; parent_checker < node_count;parent_checker++)
                    {
                        if(_operations.at(_ChildNodes[ii*Width+ parent_checker]).getOperator() == "{")
                        {
                            for(int parent_checker2 = 0;parent_checker2 < _operations.at(_ChildNodes[ii*Width+ parent_checker]).getOperations().size();parent_checker2++)
                            {
                                if(_operations.at(_ChildNodes[ii*Width+ parent_checker]).getOperations().at(parent_checker2).getResult().getVariableName() == outputVar)
                                {
                                    there_is_if = true;
                                }
                            }
                        }
                    }
                    if(there_is_if == false){
                    _ChildNodes[ii*Width + node_count] = xx;
                    node_count++;
                    }
                }
                for (int if_child = 0; if_child < _operations.at(xx).getOperations().size();if_child++)
                {
                    D1 = _operations.at(xx).getOperations().at(if_child).getResult().getVariableName();
                    if (outputVar == D1 && ii - _operations.at(xx).getOperations().size() <= xx)
                    {
                        _ChildNodes[ii*Width + node_count] = xx;
                        node_count++;
                    }
                }
                
                
            }
        }
        _childCount[ii] = node_count;
        
    }
#ifdef DEBUG_MODE
    cout << endl << "Child Nodes" << endl;
    for (int yy = 0; yy < _operations.size(); yy++)
    {
        cout << "Operation " << yy << " :\tCount = " << _childCount[yy] << " :\tChild Nodes :\t";
        for (int xx = 0; xx < _childCount[yy]; xx++)
        {
            cout << _ChildNodes[yy*(numOps - 1) + xx] << "  ";
        }
        cout << endl;
    }
#endif
}
void Circuit::fixChildNodes(vector<Operation> _operations, int latency)
{
    int x = 0, y = 0, z=0;
    for(x = 0;x<_operations.size();x++)
    {
        _childCount[x] = 0;
    }
    for(x=0;x<_operations.size();x++)
    {
        for(y =0;y<latency;y++)
        {
            _ChildNodes[x*Width+latency] = 0;
        }
    }
    for(x=0;x<_operations.size();x++)
    {

        int node_count = 0;
        for(y=0;y<_operations.size();y++)
        {
            if(x != y)
            {
                for(z=0;z<_parentCount[y];z++)
                {
                    if(_ParentNodes[y*Width+z] == x)
                    {
                        _ChildNodes[x*Width+node_count] = y;
                        node_count++;
                    }
                }
            }
        }
        _childCount[x] = node_count;
    }
#ifdef DEBUG_MODE
    cout << endl << "Fix Child Nodes" << endl;
    for (int yy = 0; yy < _operations.size(); yy++)
    {
        cout << "Operation " << yy << " :\tCount = " << _childCount[yy] << " :\tChild Nodes :\t";
        for (int xx = 0; xx < _childCount[yy]; xx++)
        {
            cout << _ChildNodes[yy*(numOps - 1) + xx] << "  ";
        }
        cout << endl;
    }
#endif
}
void Circuit::getDescendantNodes(vector<Operation> _operations)
{
    cout << "/nStarting Get Descendant" << endl;
    bool end = false;
    for (int ii = 0; ii < numOps*Width; ii++) {
        _DescendantNodes[ii] = _ChildNodes[ii];
    }
    for (int jj = 0; jj < _operations.size(); jj++) {
        _descendantCount[jj] = _childCount[jj];
    }
    while (end == false) {
        bool notdone = true;
        for (int xx = 0; xx < _operations.size(); xx++)
        {
            for (int yy = 0; yy < _descendantCount[xx];yy++)
            {
                int currentDescendant = _DescendantNodes[xx*Width + yy];
                int DescendantChildcount = _childCount[_DescendantNodes[xx*Width + yy]];
                for (int zz = 0; zz < DescendantChildcount;zz++)
                {
                    bool checker = true;
                    int CurrentChild = _ChildNodes[currentDescendant*Width + zz];
                    for (int xxx = 0; xxx < _descendantCount[xx];xxx++)
                    {
                        int descendant = _DescendantNodes[xx*Width + xxx];
                        if (descendant == CurrentChild)
                        {
                            checker = false;
                        }
                    }
                    if (checker == true) {
                        _DescendantNodes[xx*Width + _descendantCount[xx]] = CurrentChild;
                        _descendantCount[xx]++;
                        notdone = false;
                    }
                }
            }
        }
        if (notdone == true)
            end = true;
    }
#ifdef DEBUG_MODE
    cout << endl << "Child Nodes" << endl;
    for (int yy = 0; yy < _operations.size(); yy++)
    {
        cout << "Operation " << yy << " :\tCount = " << _childCount[yy] << " :\tChild Nodes :\t";
        for (int xx = 0; xx < _childCount[yy]; xx++)
        {
            cout << _ChildNodes[yy*(numOps - 1) + xx] << "  ";
        }
        cout << endl;
    }
#endif
#ifdef DEBUG_MODE
    cout << endl << "Descandant Nodes" << endl;
    for (int yy = 0; yy < _operations.size(); yy++)
    {
        cout << "Operation " << yy << " :\tCount = " << _descendantCount[yy] << " :\tDescendant Nodes :\t";
        for (int xx = 0; xx < _descendantCount[yy]; xx++)
        {
            cout << _DescendantNodes[yy*(numOps - 1) + xx] << "  ";
        }
        cout << endl;
    }
#endif
}
void Circuit::addParent(vector<Operation> _ops, int addTo, int add, int* node){
    bool duplicate = false;
    bool handle_if = false;
    int x = 0;
    for(x = 0;x<(*node);x++)
    {
        if(_ops.at(_ParentNodes[addTo*Width+x]).getResult().getVariableName() == _ops.at(add).getResult().getVariableName())
        {
            if(_ops.at(addTo).getOperator() != "{")
            {
                duplicate = true;
                break;
            }
        }
        if(_ops.at(_ParentNodes[addTo*Width+x]).getOperator() == "{")
        {
            for(int y = 0;y < _ops.at(_ParentNodes[addTo*Width+x]).getOperations().size();y++)
            {
                if(_ops.at(add).getResult().getVariableName() == _ops.at(_ParentNodes[addTo*Width+x]).getOperations().at(y).getResult().getVariableName() )
                {
                    handle_if = true;
                    break;
                }
            }
        }
        if( handle_if == true)
            break;
    }
    if(duplicate == false)
    {
        if(handle_if == true)
        {
            _ParentNodes[addTo*Width+ x] = add;
        }
        else{
            _ParentNodes[addTo*Width + *node] = add;
            (*node)++;
        }
    }
    else
    {
        _ParentNodes[add*Width+ _parentCount[add]] = _ParentNodes[addTo*Width+x];
        _parentCount[add]++;
        _ParentNodes[addTo*Width + x] = add;
    }
}

void Circuit::getParentNodes(vector<Operation> _operations)
{
    
    Operation operation;
    
    string A, B, C, R, D1;
    
    
    
    
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
                if (R != "" && (R == A || R == B || R == C))
                {
                    int child_counter = 0;
                    bool mux_op = false;
                    for (child_counter = 0; child_counter < _childCount[xx];child_counter++)
                    {
                        if(_operations.at(_ChildNodes[xx*Width+child_counter]).getOperator() == "{")
                        {
                            for (int inner_counter = 0; inner_counter < _operations.at(_ChildNodes[xx*Width+child_counter]).getOperations().size();inner_counter++)
                            {
                                if (_operations.at(_ChildNodes[xx*Width+child_counter]).getOperations().at(inner_counter).getResult().getVariableName() == R)
                                {
                                    mux_op = true;
                                    break;
                                }
                            }
                            if(mux_op == true){break; };
                        }
                    }
                    if(mux_op == false)
                    {
                        addParent(_operations, ii, xx, &node_count);
                        //_ParentNodes[ii*Width + node_count] = xx;
                        //node_count++;
                    }
                    else
                    {
                        int temp = _ChildNodes[xx*Width+child_counter];
                        bool duplicate = false;
                        for (int duplicate_check = 0;duplicate_check < node_count;duplicate_check++)
                        {
                            if(_ParentNodes[ii*Width+duplicate_check] == temp){ duplicate = true;};
                        }
                        if(duplicate == false){
                            addParent(_operations, ii, temp, &node_count);
                            //_ParentNodes[ii*Width + node_count] = temp;
                            //node_count++;
                        }
                    }
                }
                for (int if_parent = 0; if_parent < _operations.at(ii).getOperations().size();if_parent++)
                {
                    D1 = _operations.at(ii).getOperations().at(if_parent).getResult().getVariableName();
                    if (R != "" && (R == D1) && xx - _operations.at(ii).getOperations().size() <= ii)
                    {
                        addParent(_operations, ii, xx, &node_count);
                        //_ParentNodes[ii*Width + node_count] = xx;
                        //node_count++;
                    }
                }
                
                
            }
        }
        _parentCount[ii] = node_count;
        
    }
}
void Circuit::getAncestorNodes(vector<Operation> _operations){
    cout << "/nStarting Get Ancestor Nodes" << endl;
    bool end = false;
    for (int ii = 0; ii < numOps*Width; ii++){
        _AncestorNodes[ii] = _ParentNodes[ii];
    }
    for (int jj = 0; jj < _operations.size(); jj++){
        _ancestorCount[jj] = _parentCount[jj];
    }
    while(end == false){
        bool notdone = true;
        for(int xx = 0; xx < _operations.size(); xx++)
        {
            for(int yy = 0; yy < _ancestorCount[xx];yy++)
            {
                int currentAncestor =_AncestorNodes[xx*Width+yy];
                int AncestorParentcount = _parentCount[_AncestorNodes[xx*Width+yy]];
                for(int zz = 0; zz < AncestorParentcount;zz++)
                {
                    bool checker = true;
                    int CurrentParent = _ParentNodes[currentAncestor*Width+zz];
                    for(int xxx = 0; xxx < _ancestorCount[xx];xxx++)
                    {
                        int ancestor =_AncestorNodes[xx*Width+xxx];
                        if(ancestor == CurrentParent)
                        {
                            checker = false;
                        }
                    }
                    if(checker == true){
                        _AncestorNodes[xx*Width+_ancestorCount[xx]]= CurrentParent;
                        _ancestorCount[xx]++;
                        notdone = false;
                    }
                }
            }
        }
        if(notdone == true)
            end = true;
    }
    for(int aa = 0;aa < _operations.size();aa++)
    {
        if(_operations.at(aa).getOperator()== "{")
        {
            for(int fix_children = 0;fix_children < _operations.size();fix_children++)
            {
                for(int bb = 0;bb<_parentCount[fix_children];bb++)
                {
                    if(_ParentNodes[fix_children*Width+bb] == aa)
                    {
                        _ChildNodes[aa*Width+_childCount[aa]] = fix_children;
                        _childCount[aa]+=1;
                    }
                }
            }
        }
    }

#ifdef DEBUG_MODE
    cout << endl << "Parent Nodes" << endl;
    for (int yy = 0; yy < _operations.size(); yy++)
    {
        cout << "Operation " << yy << " :\tCount = " << _parentCount[yy] << " :\tParent Nodes :\t";
        for (int xx = 0; xx < _parentCount[yy]; xx++)
        {
            cout << _ParentNodes[yy*(numOps - 1) + xx] << "  ";
        }
        cout << endl;
    }
    
    cout << endl << "Ancestor Nodes" << endl;
    for (int yy = 0; yy < _operations.size(); yy++)
    {
        cout << "Operation " << yy << " :\tCount = " << _ancestorCount[yy] << " :\tAncestor Nodes :\t";
        for (int xx = 0; xx < _ancestorCount[yy]; xx++)
        {
            cout << _AncestorNodes[yy*(numOps - 1) + xx] << "  ";
        }
        cout << endl;
    }
#endif
}



void Circuit::getCriticalPath(vector<Operation> _operations)
{
    
    getChildNodes(_operations);
    getParentNodes(_operations);
    
    _VertexLatency = new double[numOps];
    _Q = new int[numOps];
    _CriticalPathQ = new int[numOps];
    int *xIdx = new int[numOps];
    for (int ii = 0; ii < numOps;ii++)
    {
        xIdx[ii] = 0;
        _Q[ii] = 0;
    }
    
    for (int ii = 0; ii < numOps; ii++)
    {
        _VertexLatency[ii] = getOperationLatency(_operations.at(ii));
    }
    
    
    
    double CurrentPath = 0;
    double CriticalPath = 0;
    for (int ii = 0; ii < numOps;ii++)
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
                        for (int zz = 0;zz < numOps; zz++) _CriticalPathQ[zz] = _Q[zz];
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








