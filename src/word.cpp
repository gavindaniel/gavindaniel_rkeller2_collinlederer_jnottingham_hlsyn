//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#include "word.h"

using namespace std;

/**
 * Parse a line from a file and create a word
 *
 * @param lineFromFile A string from a file
 *
 * @requirement carriage returns "\r" and newlines "\n" must be removed
 * @requirement after removal of carriage returns and newline, content of line are assigned to _content
 *
 */
Word::Word(string lineFromFile){
    unsigned long int carriageReturnPos = lineFromFile.find("\r");
    if(carriageReturnPos != string::npos) lineFromFile = lineFromFile.substr(0, carriageReturnPos);
    
    unsigned long int newLinePos = lineFromFile.find("\n");
    if(newLinePos != string::npos) lineFromFile = lineFromFile.substr(0, newLinePos);
    
    _content = lineFromFile;
}

/**
 * Validate Word
 *
 * @param Pointer to Word to validate
 *
 * @requirement every character in _content must pass the condition:
 * ispunct(character) || isalnum(character)
 * @requirement The Word length must be greater than 2
 *
 * @return 1 if valid, 0 otherwise
 */
bool Word::IsValid(){
    
    for(unsigned int i = 0; i < _content.length(); i++){
        if(!ispunct(_content[i]) && !isalnum(_content[i])){
            return false;
        }
    }
    
    return _content.length() >= 3;
}
