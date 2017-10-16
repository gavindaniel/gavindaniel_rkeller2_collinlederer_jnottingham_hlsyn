//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#ifndef TESTENDTOEND_H
#define TESTENDTOEND_H

#ifdef __linux__
    #define TEST_FILE_RELATIVE_PATH "../../test_files" // definitely do not modify this, this will cause trouble
#elif __APPLE__
    #define TEST_FILE_RELATIVE_PATH "../../../test_files"
#else
    #define TEST_FILE_RELATIVE_PATH "../test_files" //you can change this if you really need to do so
#endif

class EndToEndTester{
private:
public:
    void RunTests();
    bool testFile(int testNum);
};



#endif /* testEndToEnd_h */
