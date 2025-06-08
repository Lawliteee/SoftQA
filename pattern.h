#ifndef PATTERN_H
#define PATTERN_H

#include <QObject>
#include <reltypecheck.h>



class Pattern
{
    QSet <QString> validWords;                            // список допустимых слов, может быть любое
    QSet <PosTag> validTags;                                // список допустимых тегов, может быть любой
    QMultiMap <DepRel,Pattern*> children;         // дети
    QSet <RelTypeCheck*> checks;                        // список проверок
    UDNode* currentMatch;

public:
    Pattern();
    Pattern(const QSet<QString>& words, const QSet<PosTag>& tags, QMultiMap<DepRel, Pattern*>& childrenPatterns)
    {
        validWords = words;
        validTags = tags;
        children = childrenPatterns;
    }
    Pattern(const QSet<QString>& words, const QSet<PosTag>& tags)
    {
        validWords = words;
        validTags = tags;
    }
    void addChildPattern(DepRel relation, Pattern* childPattern)
    {
        children.insert(relation, childPattern);
    }
    bool matchesPattern(const UDNode* node, QSet<const UDNode*>& usedChildren);
    void compareMatches(const Pattern* expected, QStringList& errors, const QString& path = "root") const;

    void setMatch(UDNode* match)
    {
        currentMatch = match;
    }
};






#endif // PATTERN_H
