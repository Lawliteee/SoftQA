#ifndef RELTYPECHECK_H
#define RELTYPECHECK_H

#include <grammarrule.h>

class RelTypeCheck
{
protected:
    GrammarRule* rule;                    // Правило для проверки
    QString message;
public:
    RelTypeCheck();
    virtual void getNodes(const UDNode* mainNode,UDNode* searchNode, const UDNode* parent)
    {
        return;
    };
};

class ParentChild : public RelTypeCheck
{
public:
    void getNodes(const UDNode* mainNode,UDNode* searchNode, const UDNode* parent) override;
};

class ChildChild : public RelTypeCheck
{
    DepRel relatedRel;                      // Тип связи второго узла
    QSet <QString> validWords;       // список допустимых слов второго узла, может быть любое
    QSet <PosTag> validTags;           // список допустимых тегов второго узла, может быть любой
public:
    void getNodes(const UDNode* mainNode,UDNode* searchNode, const UDNode* parent) override;
    void setRelatedRel(DepRel rel) { relatedRel = rel; }
    void setValidWords(const QSet<QString>& words) { validWords = words; }
    void setValidTags(const QSet<PosTag>& tags) { validTags = tags; }
};

#endif // RELTYPECHECK_H
