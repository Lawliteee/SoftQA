#ifndef PATTERN_H
#define PATTERN_H

#include <QObject>
#include <reltypecheck.h>



class Pattern
{
    QSet <QString> validWords;                            // список допустимых слов, может быть любое
    QSet <PosTag> validTags;                                // список допустимых тегов, может быть любой
    QMultiMap <DepRel,Pattern> children;         // дети
    QSet <RelTypeCheck*> checks;                        // список проверок
    UDNode* currentMatch;

public:
    Pattern();
    bool matchesPattern(const UDNode* node, QSet<const UDNode*>& usedChildren);
};






#endif // PATTERN_H
