//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "controller.h"

using namespace std;

int main(int argc, char* argv[]) {
    
    if (argc != 3) {
        cout << "Usage: dpgen netlistFile verilogFile" << endl;
        return 1;
    }
    else {
        stringstream inputPath;
        stringstream outputPath;
        bool success = true;
        
        inputPath << argv[1];
        outputPath << argv[2];
        
        Controller controller(inputPath.str(), outputPath.str());
        
        success = controller.readFromFile();
        if (success) {  success = controller.convertOperations(); controller.getCriticalPath(); }
        if (success) {  controller.writeToFile();   }
        
        return 0;
    }
}



