//----------------------------------------------------------------------//
// Author:
// Net ID:
// Date:
//
//----------------------------------------------------------------------//

#ifndef WORD_H
#define WORD_H

#include <string>

class Word{
private:
    std::string _content;
public:
    Word(std::string lineFromFile);
    bool IsValid();
    std::string GetContent() const { return _content; };
    void SetContent(std::string content) { _content = content; };
};
#endif

