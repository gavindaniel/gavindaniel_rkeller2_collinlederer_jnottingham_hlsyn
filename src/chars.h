//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#ifndef CHARS_H
#define CHARS_H

#include <string>
#include <list>
#include "card.h"
#include "word.h"

class Chars{
private:
    std::string _wordFilePath;
    std::string _cardFilePath;
    std::string _outputFilePath;
    std::list<Word> _words;
    std::list<Card> _cards;
public:
    Chars(std::string wordsFilePath, std::string cardsFilePath, std::string outputFilePath);
    void ReadCardsFromFile();
    void ReadWordsFromFile();
    void ProcessCards();
    void WriteCardsToFile();
};


#endif
