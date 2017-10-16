//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#include <iostream>
#include "testChars.h"
#include "word.h"
#include "card.h"

using namespace std;

void CharsTester::RunTests() {
    testConstructWordWithCarriageReturn();
    testConstructWordWithNewLine();
    testIsWordValidWithSpace();
    testIsWordValidPercentSign();
    testIsWordValidShortWord();
    testConstructCardWithCarriageReturn();
    testConstructCardWithNewLine();
    testIsCardValidNoBlank();
    testIsCardValidShortBlank();
    testCorrectBlankLengthAndIndex();
    testCardIsLessThan();
}

/*
 * @requirement pass if "\r\n" is discarded by Word constructor
 * @return pass: true, fail: false
 */
bool CharsTester::testConstructWordWithCarriageReturn() {
    Word newWord("This\r\n");
    
    if (newWord.GetContent().compare("This") == 0)
    {
        return true;
    }
    else {
        cout << "testConstructWordWithCarriageReturn failed" << endl;
        return false;
    }
}

/*
 * @requirement pass if "\n" is discarded by Word constructor
 * @return pass: true, fail: false
 */
bool CharsTester::testConstructWordWithNewLine() {
    Word newWord("This\n");
    
    if (newWord.GetContent().compare("This") == 0)
    {
        return true;
    }
    else {
        cout << "testConstructWordWithNewLine failed" << endl;
        return false;
    }
}

/*
 * @requirement pass if Word created using constructor with string that has a space is considered invalid
 * @return pass: true, fail: false
 */
bool CharsTester::testIsWordValidWithSpace() {
    Word newWord("This Word");
    
    if (!newWord.IsValid())
    {
        return true;
    }
    else {
        cout << "testIsWordValidWithSpace failed" << endl;
        return false;
    }
}

/*
 * @requirement pass if Word created using constructor with string that has a percent sign is considered valid
 * @return pass: true, fail: false
 */
bool CharsTester::testIsWordValidPercentSign() {
    Word newWord("Word%");
    
    if (newWord.IsValid())
    {
        return true;
    }
    else {
        cout << "testIsWordValidPercentSign failed" << endl;
        return false;
    }
}

/*
 * @requirement pass if Word created using constructor with string that is less than 3 characters is considered invalid
 * @return pass: true, fail: false
 */
bool CharsTester::testIsWordValidShortWord() {
    Word newWord("t");
    
    if (!newWord.IsValid())
    {
        return true;
    }
    else {
        cout << "testConstructWordWithNewLine failed" << endl;
        return false;
    }
}

/*
 * @requirement pass if "\r\n" is discarded by Card constructor
 * @return pass: true, fail: false
 */
bool CharsTester::testConstructCardWithCarriageReturn() {
    Card newCard("This\r\n");
    
    if (newCard.GetContent().compare("This") == 0)
    {
        return true;
    }
    else {
        cout << "testConstructCardWithCarriageReturn failed" << endl;
        return false;
    }
}

/*
 * @requirement pass if "\n" is discarded by Card constructor
 * @return pass: true, fail: false
 */
bool CharsTester::testConstructCardWithNewLine() {
    Card newCard("This\n");
    
    if (newCard.GetContent().compare("This") == 0)
    {
        return true;
    }
    else {
        cout << "testConstructWordWithNewLine failed" << endl;
        return false;
    }
}

/*
 * @requirement pass if Card created using constructor with string that has no underscores is invalid
 * @return pass: true, fail: false
 */
bool CharsTester::testIsCardValidNoBlank() {
    Card newCard("This is a senstence");
    
    if (!newCard.IsValid())
    {
        return true;
    }
    else {
        cout << "testIsCardValidShortWord failed" << endl;
        return false;
    }
}

/*
 * @requirement pass if Card created using constructor with string that has a blank that is only two characters long is invalid
 * @return pass: true, fail: false
 */
bool CharsTester::testIsCardValidShortBlank() {
    Card newCard("This is a sensten__");
    
    if (!newCard.IsValid())
    {
        return true;
    }
    else {
        cout << "testIsCardValidShortWord failed" << endl;
        return false;
    }
}

/*
 * @requirement pass if blankLength and blankIndex is correct in properly formatted Card object with a blank yet to be filled.
 * @return pass: true, fail: false
 */
bool CharsTester::testCorrectBlankLengthAndIndex() {
    Card newCard("This is a ____\n");
    
    if (newCard.GetBlankIndex() == 10 &&
        newCard.GetBlankLength() == 4)
    {
        return true;
    }
    else {
        cout << "testCorrectBlankLengthAndIndex failed" << endl;
        return false;
    }
}

/*
 * @requirement pass if Card with contents that have fewer characters than another Card is considered "less than"
 * @return pass: true, fail: false
 */
bool CharsTester::testCardIsLessThan() {
    Card newCard("This is a card");
    Card newCard2("This is another card.");
    
    if (newCard < newCard2)
    {
        return true;
    }
    else {
        cout << "testCardIsLessThan failed" << endl;
        return false;
    }
}

