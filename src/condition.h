//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#ifndef CONDITION_H
#define CONDITION_H

#include <string>
#include <vector>
#include "node.h"
#include "operation.h"

using namespace std;

class Condition{
private:
    Node _var;
    vector<Operation> _operations;
    vector<Condition> _ifstatements;
	
    
public:
    Condition();
    string convertIfStatement();
    string checkSign(Node var);
    Node getVariable() { return _var; };
    void setVariable (Node var) { _var = var; };
    void addOperation(Operation newOp) { _operations.push_back(newOp); };
    void addCondition(Condition newIf) { _ifstatements.push_back(newIf); };
};
#endif





