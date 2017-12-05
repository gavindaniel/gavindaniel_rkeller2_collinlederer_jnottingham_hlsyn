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
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[]) {
    
    if (argc != 4) {
        cout << "Error: Invalid number of arguments. Usage: dpgen netlistFile verilogFile" << endl;
        return 1;
    }
    else {
        stringstream inputPath;
        stringstream outputPath;
        bool success = true;
        
        inputPath << argv[1];
		int latency = atoi(argv[2]);
        outputPath << argv[3];

		
        
        Controller controller(inputPath.str(), outputPath.str(),latency);
        
        success = controller.readFromFile();
		if(success) success = controller.PerformScheduling(latency);
        if (success) {  controller.writeToFile();   }


        
        return 0;
    }
}



