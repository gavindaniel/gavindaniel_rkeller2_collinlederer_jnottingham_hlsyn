//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#ifndef OPERATION_H
#define OPERATION_H

#include <string>
#include <vector>
#include "node.h"

using namespace std;

class Operation{
private:
    Node _result;
    Node _a;
    Node _b;
    Node _c;
    string _operator;
	
    
public:

    Operation();
    Node getResult();
    Node get_varA();
    Node get_varB();
    Node get_varC();
    string getOperator();
    void setResult(Node r);
    void set_varA(Node a);
    void set_varB(Node b);
    void set_varC(Node c);
    void setOperator(string op);
    string convertOperation();
    string checkSign(Node var);
};
#endif





