#ifndef UDNODE_H
#define UDNODE_H

#include <Header.h>

class Pattern;
class Mistake;

class UDNode
{
    int id; //
    QString lemma;                              // слово
    PosTag upos;                                  // POS-тег части речи
    int head;                                        // идентификатор определяющего слова
    DepRel depRel;                               // тип связи к родителю
    VerbMood mood ;                           // наклонение
    QSet <UDNode*> children;             // зависимые слова
    bool connectedToRoot ;                // достижимость до корня
public:
    UDNode();
    UDNode(QString lem, PosTag up, int h, DepRel dp, VerbMood m);
    UDNode(QString lem, PosTag up, VerbMood m);
    void markRelatedDescendants();  // отмечает достижимость потомков до корня
    void checkPattern(const Pattern& pattern, const QMap<int, UDNode*>&nodes,  QSet<Mistake> &Mistakes);     // метод выполнения всех проверок шаблона
    bool isBeForm ();
    bool isModalVerb ();
    bool isHaveForm ();
    bool isDoForm();
    bool isPresentClause ();
    bool isFutureClause ();
    bool isPastClause ();
    bool isCountable();

};

#endif // UDNODE_H
