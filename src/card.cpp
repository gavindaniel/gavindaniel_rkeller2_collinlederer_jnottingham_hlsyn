//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#include "card.h"
#include <iostream>

using namespace std;

/**
 * Description: Parse a line from a file and create a card
 *
 * @param lineFromFile A string from a file
 *
 * @requirement carriage returns "\r" and newlines "\n" must be removed
 * @requirement a valid blank is at least three consecutive underscores
 * @requirement blankLength and blankIndex are zero if a valid blank is not found
 * @requirement blankIndex is the index that makes card->GetContent()[card->blankIndex] be the first underscore
 * @requirement blankLength is the number of underscores in the blank
 *
 */
Card::Card(string lineFromFile){
    unsigned long int carriageReturnPos = lineFromFile.find("\r");
    if(carriageReturnPos != string::npos) lineFromFile = lineFromFile.substr(0, carriageReturnPos);
    
    unsigned long int newLinePos = lineFromFile.find("\n");
    if(newLinePos != string::npos) lineFromFile = lineFromFile.substr(0, newLinePos);
    
    _content = lineFromFile;
    
    _blankLength = 0;
    _blankIndex = 0;
    if(_content.find("___") != string::npos){
        _blankIndex = _content.find("___");
        while(_content[_blankIndex + _blankLength] == '_' &&
              _blankIndex + _blankLength < _content.length())
        {
            _blankLength++;
        }
    }
}

/**
 * Description: Replace a blank in a card with a Word object
 *
 * @param word Word with which to replace the blank inside the card
 *
 * @requirement Card _content has its blank replaced with the _content of word
 *
 */
void Card::ReplaceBlanks(Word word){
    _content.replace(_blankIndex, _blankLength, word.GetContent());
}

/**
 * Validate Card
 *
 * @requirement blankLength must be at least 3
 *
 */
bool Card::IsValid() {
    return _blankLength >= 3;
}

/**
 * Description: < operator overload
 *
 * @requirement return false if _content length of current Card is less than content of rhs
 *
 * @note This is necessary in order to use the .sort() function on a list properly
 */
bool Card::operator<(const Card& rhs) const{
    return rhs.GetContent().length() > _content.length();
}
