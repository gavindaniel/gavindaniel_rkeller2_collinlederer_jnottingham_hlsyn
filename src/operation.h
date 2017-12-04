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

//class Condition;

class Operation{
private:
    Node _result;
    Node _a;
    Node _b;
    Node _c;
    string _operator;
//    Operation _ifstatement;
    
    //new
    vector<Operation> _operations;
//    vector<Operation> _ifstatements;
    
public:

    Operation();
    string convertOperation();
    string checkSign(Node var);
    
    string getOperator();
    Node getResult();
    Node get_varA();
    Node get_varB();
    Node get_varC();
    Operation get_ifstatement();
    void setOperator(string op);
    void setResult(Node r);
    void set_varA(Node a);
    void set_varB(Node b);
    void set_varC(Node c);
    void set_ifstatement(Operation newIf);

//    string getOperator() {   return _operator;   };
//    Node getResult() {   return _result;     };
//    Node get_varA() {    return _a;      };
//    Node get_varB() {    return _b;      };
//    Node get_varC() {    return _c;      };
////    Condition get_ifstatement() {   return _ifstatement;    };
//    void setOperator(string op) {    _operator = op;     };
//    void setResult(Node r) {     _result = r;    };
//    void set_varA(Node a) {  _a = a;   };
//    void set_varB(Node b) {  _b = b;   };
//    void set_varC(Node c) {  _c = c;   };
////    void set_ifstatement(Condition newIf) { _ifstatement = newIf; };
    
    
    //new
    void addOperation(Operation newOp);
    void addCondition(Operation newIf);
    Node getVariable() { return _a; };
    void setVariable(Node var) { _a = var; };
};
#endif





