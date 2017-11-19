//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#include "operation.h"
#include <iostream>

using namespace std;

Operation::Operation(){
    _result = Node();
    _a = Node();
    _b = Node();
    _c = Node();
    _operator = 'z';
}

string Operation::getOperator() {   return _operator;   }
Node Operation::getResult() {   return _result;     }
Node Operation::get_varA() {    return _a;      }
Node Operation::get_varB() {    return _b;      }
Node Operation::get_varC() {    return _c;      }

void Operation::setOperator(string op) {    _operator = op;     }
void Operation::setResult(Node r) {     _result = r;    }
void Operation::set_varA(Node a) {  _a = a;   }
void Operation::set_varB(Node b) {  _b = b;   }
void Operation::set_varC(Node c) {  _c = c;   }


string Operation::convertOperation(vector<int>& vCount) {
    
    string verilogCode = "";
    string varName = "";
    string op = "";
    
    int dw, dw_A, dw_B, dw_C, dw_R;
    
    op = getOperator();
    
    // check for signed operation
    if((get_varA().getSigned() == true && get_varA().getVariableName() != "") ||
       (get_varB().getSigned() == true && get_varB().getVariableName() != "") ||
       (get_varC().getSigned() == true && get_varC().getVariableName() != "")){
        verilogCode += "S";
    }
    
    if (op == "=") {  verilogCode += "REG ";   }
    else if (op == "+" && get_varB().getVariableName() != "1") {  verilogCode += "ADD ";  }
    else if (op == "+" && get_varB().getVariableName() == "1"){  verilogCode += "INC ";  }
    else if (op == "-" && get_varB().getVariableName() != "1") { verilogCode += "SUB ";  }
    else if (op == "-" && get_varB().getVariableName() == "1") {  verilogCode += "DEC ";  }
    else if (op == "*") {  verilogCode += "MUL ";  }
    else if (op == ">" || op == "<" || op == "==") {  verilogCode += "COMP ";  }
    else if (op == ":") {  verilogCode += "MUX2x1 ";  }
    else if (op == ">>") { verilogCode += "SHR ";   }
    else if (op == "<<") {  verilogCode += "SHL ";  }
    else if (op == "/") {   verilogCode += "DIV ";  }
    else if (op == "%") {   verilogCode += "MOD ";  }
    else {
        cout << "ERROR: INVALID operator -> " << op << endl;
        return "ERROR";
    }
    
    // add DATAWIDTH
    dw_A = get_varA().getBitWidth();
    dw_B = get_varB().getBitWidth();
    dw_C = get_varC().getBitWidth();
    dw_R = getResult().getBitWidth();
    dw = dw_A;
    
    if(op == "==" || op == ">" || op == "<"){
        if((dw_A >= dw_R) && (dw_A >= dw_B) && (dw_A >= dw_C)){    dw = dw_A;   }
        else if((dw_B >= dw_R) && (dw_B >= dw_C) && (dw_B >= dw_A)){   dw = dw_B;   }
        else if((dw_C >= dw_R) && (dw_C >= dw_B) && (dw_C >= dw_A)){   dw = dw_C;   }
        // check if inputs are greater than output
        if((dw_R < dw)){    dw = dw_R;   }
    }
    else {
        dw = dw_R;
    }
    
    verilogCode += "#(.DATAWIDTH(" + to_string(dw) + ")) ";
    
    if (op == "=") {
        verilogCode += "reg_";
        verilogCode += to_string(vCount.at(0)++);
        verilogCode += "(";
        verilogCode += checkSign(get_varA());
        verilogCode += ",Clk,Rst,";
        verilogCode += getResult().getVariableName();
        verilogCode += ");";
    }
    else if (op == "+")
    {
        if (get_varB().getVariableName() != "1")
        {
            verilogCode += "add_";
            verilogCode += to_string(vCount.at(1)++);
            verilogCode += "(";
            verilogCode += checkSign(get_varA());
            verilogCode += ",";
            verilogCode += checkSign(get_varB());
            verilogCode += ",";
            verilogCode += checkSign(getResult());
            verilogCode += ");";
        }
        else
        {
            verilogCode += "inc_";
            verilogCode += to_string(vCount.at(2)++);
            verilogCode += "(";
            verilogCode += checkSign(get_varA());
            verilogCode += ",";
            verilogCode += checkSign(getResult());
            verilogCode += ");";
            setOperator("++");
        }
    }
    else if (op == "-")
    {
        if (get_varB().getVariableName() != "1")
        {
            verilogCode += "sub_";
            verilogCode += to_string(vCount.at(3)++);
            verilogCode += "(";
            verilogCode += checkSign(get_varA());
            verilogCode += ",";
            verilogCode += checkSign(get_varB());
            verilogCode += ",";
            verilogCode += checkSign(getResult());
            verilogCode += ");";
        }
        else
        {
            verilogCode += "dec_";
            verilogCode += to_string(vCount.at(4)++);
            verilogCode += "(";
            verilogCode += checkSign(get_varA());
            verilogCode += ",";
            verilogCode += checkSign(getResult());
            verilogCode += ");";
            setOperator("--");
        }
    }
    else if (op == "*") {
        verilogCode += "mul_";
        verilogCode += to_string(vCount.at(5)++);
        verilogCode += "(";
        verilogCode += checkSign(get_varA());
        verilogCode += ",";
        verilogCode += checkSign(get_varB());
        verilogCode += ",";
        verilogCode += checkSign(getResult());
        verilogCode += ");";
    }
    else if (op == ">") {
        verilogCode += "comp_";
        verilogCode += to_string(vCount.at(6)++);
        verilogCode += "(";
        verilogCode += checkSign(get_varA());
        verilogCode += ",";
        verilogCode += checkSign(get_varB());
        verilogCode += ",";
        verilogCode += checkSign(getResult());
        verilogCode += ",lt,eq";
        verilogCode += ");";
    }
    else if (op == "<") {
        verilogCode += "comp_";
        verilogCode += to_string(vCount.at(7)++);
        verilogCode += "(";
        verilogCode += checkSign(get_varA());
        verilogCode += ",";
        verilogCode += checkSign(get_varB());
        verilogCode += ",gt,";
        verilogCode += checkSign(getResult());
        verilogCode += ",eq";
        verilogCode += ");";
    }
    else if (op == "==") {
        verilogCode += "comp_";
        verilogCode += to_string(vCount.at(8)++);
        verilogCode += "(";
        verilogCode += checkSign(get_varA());
        verilogCode += ",";
        verilogCode += checkSign(get_varB());
        verilogCode += ",gt,lt,";
        verilogCode += checkSign(getResult());
        verilogCode += ");";
    }
    else if (op == ":") {
        verilogCode += "mux_";
        verilogCode += to_string(vCount.at(9)++);
        verilogCode += "(";
        verilogCode += checkSign(get_varB());
        verilogCode += ",";
        verilogCode += get_varC().getVariableName();
        verilogCode += ",";
        verilogCode += checkSign(get_varA());
        verilogCode += ",";
        verilogCode += checkSign(getResult());
        verilogCode += ");";
    }
    
    else if (op == ">>"){
        verilogCode += "shr_";
        verilogCode += to_string(vCount.at(10)++);
        verilogCode += "(";
        verilogCode += checkSign(get_varA());
        verilogCode += ",";
        verilogCode += checkSign(get_varB());
        verilogCode += ",";
        verilogCode += checkSign(getResult());
        verilogCode += ");";
    }
    
    else if (op == "<<") {
        verilogCode += "shl_";
        verilogCode += to_string(vCount.at(11)++);
        verilogCode += "(";
        verilogCode += checkSign(get_varA());
        verilogCode += ",";
        verilogCode += checkSign(get_varB());
        verilogCode += ",";
        verilogCode += checkSign(getResult());
        verilogCode += ");";
    }
    else if (op == "/") {
        verilogCode += "div_";
        verilogCode += to_string(vCount.at(12)++);
        verilogCode += "(";
        verilogCode += checkSign(get_varA());
        verilogCode += ",";
        verilogCode += checkSign(get_varB());
        verilogCode += ",";
        verilogCode += checkSign(getResult());
        verilogCode += ");";
    }
    
    else if (op == "%") {
        verilogCode += "mod_";
        verilogCode += to_string(vCount.at(13)++);
        verilogCode += "(";
        verilogCode += checkSign(get_varA());
        verilogCode += ",";
        verilogCode += checkSign(get_varB());
        verilogCode += ",";
        verilogCode += checkSign(getResult());
        verilogCode += ");";
    }
    
    return verilogCode;
}


string Operation::checkSign(Node var) {
    string returnString;
    if (var.getSigned()){
        if (var.getVariableName() != getResult().getVariableName()) {
            returnString += "$signed(";
            if((var.getBitWidth() > getResult().getBitWidth()) && getOperator() != "==" && getOperator() != ">" && getOperator() != "<"){
                returnString += var.getVariableName();
                returnString += "[" + to_string(getResult().getBitWidth()-1) + ":0]";
            }
            else if(var.getBitWidth() < getResult().getBitWidth()){
                returnString += "{{" + to_string(getResult().getBitWidth() - var.getBitWidth()) + "{" + var.getVariableName() + "[" + to_string(var.getBitWidth() - 1) + "]}}," + var.getVariableName() + "}";
            }
            else{
                returnString += var.getVariableName();
            }
            returnString += ")";
        }
        else {
            if((var.getBitWidth() > getResult().getBitWidth()) && getOperator() != "=="  && getOperator() != ">" && getOperator() != "<"){
                returnString = var.getVariableName();
                returnString += "[" + to_string(getResult().getBitWidth()-1) + ":0]";
            }
            else if(var.getBitWidth() < getResult().getBitWidth()){
                returnString += "{{" + to_string(getResult().getBitWidth() - var.getBitWidth()) + "{" + var.getVariableName() + "[" + to_string(var.getBitWidth() - 1) + "]}}," + var.getVariableName() + "}";
            }
            else{
                returnString += var.getVariableName();
            }
        }
        return returnString;
    }
    else {
        if (var.getVariableName() != getResult().getVariableName()) {
            if((var.getBitWidth() > getResult().getBitWidth()) && getOperator() != "==" && getOperator() != ">" && getOperator() != "<"){
                returnString += var.getVariableName();
                returnString += "[" + to_string(getResult().getBitWidth()-1) + ":0]";
            }
            else if(var.getBitWidth() < getResult().getBitWidth()){
                returnString += "{{" + to_string(getResult().getBitWidth() - var.getBitWidth()) + "{" + var.getVariableName() + "[" + to_string(var.getBitWidth() - 1) + "]}}," + var.getVariableName() + "}";
            }
            else{
                returnString += var.getVariableName();
            }
        }
        else {
            if((var.getBitWidth() > getResult().getBitWidth()) && getOperator() != "=="  && getOperator() != ">" && getOperator() != "<"){
                returnString = var.getVariableName();
                returnString += "[" + to_string(getResult().getBitWidth()-1) + ":0]";
            }
            else if(var.getBitWidth() < getResult().getBitWidth()){
                returnString += "{{" + to_string(getResult().getBitWidth() - var.getBitWidth()) + "{" + var.getVariableName() + "[" + to_string(var.getBitWidth() - 1) + "]}}," + var.getVariableName() + "}";
            }
            else{
                returnString = var.getVariableName();
            }
            
        }
        return returnString;
    }
}








