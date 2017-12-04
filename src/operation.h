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
#include "condition.h"

using namespace std;

class Operation{
private:
    Node _result;
    Node _a;
    Node _b;
    Node _c;
    string _operator;
    Condition _ifstatement;
    
public:

    Operation();
    string convertOperation();
    string checkSign(Node var);
    
    
    string getOperator() {   return _operator;   }
    Node getResult() {   return _result;     }
    Node get_varA() {    return _a;      }
    Node get_varB() {    return _b;      }
    Node get_varC() {    return _c;      }
    void setOperator(string op) {    _operator = op;     };
    void setResult(Node r) {     _result = r;    };
    void set_varA(Node a) {  _a = a;   };
    void set_varB(Node b) {  _b = b;   };
    void set_varC(Node c) {  _c = c;   };
    void set_ifstatement(Condition newIf) { _ifstatement = newIf; };
    
};
#endif





