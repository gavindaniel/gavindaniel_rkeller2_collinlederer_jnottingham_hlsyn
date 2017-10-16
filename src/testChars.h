//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#ifndef TESTCHARS_H
#define TESTCHARS_H


class CharsTester{
private:
public:
    void RunTests();
    bool testConstructWordWithCarriageReturn();
    bool testConstructWordWithNewLine();
    bool testIsWordValidWithSpace();
    bool testIsWordValidPercentSign();
    bool testIsWordValidShortWord();
    bool testConstructCardWithCarriageReturn();
    bool testConstructCardWithNewLine();
    bool testIsCardValidNoBlank();
    bool testIsCardValidShortBlank();
    bool testCorrectBlankLengthAndIndex();
    bool testCardIsLessThan();
};


#endif /* testWords_h */
