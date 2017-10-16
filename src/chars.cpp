//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#include <fstream>
#include <iostream>
#include <algorithm>
#include "word.h"
#include "chars.h"

using namespace std;

/**
 * Description: constructor for Chars
 *
 * @param string wordsFilePath relative path of file to get Word information
 * @param string cardsFilePath relative path of file to get Card information
 * @param string outputFilePath relative path of file to write processed cards information
 *
 * @requirement assign parameters to Chars data members of similar name
 */
Chars::Chars(std::string wordsFilePath, std::string cardsFilePath, std::string outputFilePath){
    _wordFilePath = wordsFilePath;
    _cardFilePath = cardsFilePath;
    _outputFilePath = outputFilePath;
}

/**
* Description: Read Cards from a File
*
*
* @requirement output error if _cardsFilePath cannot be opened
* @requirement populate _cards list in the order they are found in the file
* @requirement Every line in the _cardsFilePath is used to create a new Card object using the Card constructor
* @requirement A Card is added to the _cards list only if the Card is valid
* @requirement close the file stream when you are done.
*
*/
void Chars::ReadCardsFromFile() {
    ifstream inputFile(_cardFilePath);
	string lineBuffer;

	if (!inputFile.is_open()) {
        cout << "Could not open cards file " << _cardFilePath << endl;
		return;
	}

	while(!inputFile.eof()) {
        getline(inputFile, lineBuffer);
        Card newCard = Card(lineBuffer);
        if(newCard.IsValid()){
            _cards.push_back(newCard);
        }
	}
    
    inputFile.close();
}

/**
 * Description: Read Word from a File
 *
 *
 * @requirement output error if _wordsFilePath cannot be opened
 * @requirement populate _words list in the order they are found in the file
 * @requirement Every line in the _wordsFilePath is used to create a new Word object using the Word constructor
 * @requirement A Word is added to the _words list only if the Word is valid
 * @requirement close the file stream when you are done.
 *
 */
void Chars::ReadWordsFromFile() {
    ifstream inputFile(_wordFilePath);
    string lineBuffer;
    
    if (!inputFile.is_open()) {
        cout << "Could not open words file " << _wordFilePath << endl;
        return;
    }
    
    while(!inputFile.eof()) {
        getline(inputFile, lineBuffer);
        Word newWord = Word(lineBuffer);
        if(newWord.IsValid()){
            _words.push_back(lineBuffer);
        }
    }
    
    inputFile.close();
}

/**
 * Description: Uses _cards and _words lists to populate blanks inside of Card
 * objects in _cards with Word objects in _words
 *
 * @requirement attempt to fill all blanks in the Card objects in _cards with
 * the content of a Word object in _words with the same length as the blank
 * ReplaceBlanks should be used for replacing blanks
 *
 * @requirement If no Word object in _words is of the correct size for a Card object in _cards, then the Card object is removed from _cards using the erase function of the list<> type
 *
 * @requirement Once a Word in _words is used to replace a blank in a Card object in _cards, it is removed from _words using the erase function of the list<> type
 *
 */
void Chars::ProcessCards() {
    bool foundWord;
    for(list<Card>::iterator card = _cards.begin(); card != _cards.end(); card++){
        foundWord = false;
        for(list<Word>::iterator word = _words.begin(); word != _words.end(); word++){
            if(word->GetContent().length() == card->GetBlankLength()){
//                card->ReplaceBlanks(*word);
//                _words.erase(word);
//                foundWord = true;
//                break;
            }
        }
//        if(!foundWord){
//            _cards.erase(card);
//        }
    }
}

/**
 * Description: Write Cards that have blanks filled with Words to _outputFilePath
 *
 * @requirement output error if _outputFilePath cannot be opened
 * @requirement The _cards list must be sorted according to the length of the card from shortest to longest.
 * if the < operator is properly overloaded, this can be done by calling .sort() on a list
 *
 * @requirement Each Card object in _cards must be output on a new line with a carriage return ("\r\n") at the end
 * @requirement close the file stream when you are done.
 *
 */
void Chars::WriteCardsToFile() {
    ofstream outputFile(_outputFilePath);
    if (!outputFile.is_open()) {
        cout << "Error opening output file " << _outputFilePath << endl;
	}
    
    _cards.sort();
    
    for(Card c : _cards){
        outputFile << c.GetContent() << "\r\n";
    }
    
    outputFile.close();
}
