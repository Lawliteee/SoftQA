#ifndef ERROR_H
#define ERROR_H

#include <Header.h>

class Error
{
    enum ErrorType {NoError, InputFile, OutputFile, IncorrectId, duplicateIds, IncorrectWord,
                     IncorrectHeadId, headIdNotFound, IncorrectDepRel, ExtraData, NoRoot,
                     MultipleRoots, IncorrectRootId, IncorrectMood, IncorrectTag, CycleInTree,
                     depNotConnected
    };
    QSet <int> line;
    void printMessage();
public:
    Error();
};

#endif // ERROR_H
