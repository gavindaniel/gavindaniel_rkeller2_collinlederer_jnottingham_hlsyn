//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#ifndef CARD_H
#define CARD_H

#include <string>
#include "word.h"

class Card{
private:
    std::string _content;
    unsigned int _blankIndex;
    unsigned int _blankLength;
public:
    Card();
    Card(std::string lineFromFile);
    void ReplaceBlanks(Word word);
    bool IsValid();
    std::string GetContent() const { return _content; };
    unsigned int GetBlankIndex() const { return _blankIndex; };
    unsigned int GetBlankLength() const { return _blankLength; };
    void SetContent(std::string content) { _content = content; };
    void SetBlankLength(unsigned int length) { length = _blankLength; };
    void SetBlankIndex(unsigned int index) { index = _blankIndex; };
    bool operator<(const Card& rhs) const;
};
#endif

