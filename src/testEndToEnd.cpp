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
#include "testEndToEnd.h"
#include "chars.h"

using namespace std;

void EndToEndTester::RunTests(){
    for(int i = 1; i < 11; i++){
        if(testFile(i)) cout << "File " << i << " passed." << endl;
    }
}


/*
 * @requirement pass if all lines in files are the exact same AND output files can be opened properly
 *
 * @note test_files should be traversed from 1 to 10
 *
 * @return pass: 1, fail: 0
 */
bool EndToEndTester::testFile(int testNum){
    int myFileIsFinished = 0;
    ostringstream testWordsPath;
    ostringstream testCardsPath;
    ostringstream myOutputFilePath;
    ostringstream correctOutputFilePath;
    testWordsPath << TEST_FILE_RELATIVE_PATH << "/inputWords" << testNum << ".txt";
    testCardsPath << TEST_FILE_RELATIVE_PATH << "/inputCards" << testNum << ".txt";
    myOutputFilePath << TEST_FILE_RELATIVE_PATH << "/myOutput" << testNum << ".txt";
    correctOutputFilePath << TEST_FILE_RELATIVE_PATH << "/output" << testNum << ".txt";
    
    Chars chars(testWordsPath.str(), testCardsPath.str(), myOutputFilePath.str());
    chars.ReadCardsFromFile();
    chars.ReadWordsFromFile();
    chars.ProcessCards();
    chars.WriteCardsToFile();

    ifstream myOutputFile(myOutputFilePath.str());
    ifstream correctOutputFile(correctOutputFilePath.str());
    
    if(!myOutputFile.is_open() || !correctOutputFile.is_open()){
        cout << "Could not open output files.\n" << endl;
        return false;
    }
    
    //check to see if any line is different
    string correctLineBuffer;
    string myLineBuffer;
    int lineNumber = 0;
    while(!correctOutputFile.eof()){
        getline(correctOutputFile, correctLineBuffer);
        getline(myOutputFile, myLineBuffer);
        if(myLineBuffer.compare(correctLineBuffer) != 0){
            cout << "Line Number " << lineNumber << endl
            << "Correct Line: " << correctLineBuffer << endl
            << "Your line: " << myLineBuffer << endl;
            return false;
        }
        lineNumber++;
    }
    
    myFileIsFinished = myOutputFile.eof();
    myOutputFile.close();
    correctOutputFile.close();
    return myFileIsFinished;
}
