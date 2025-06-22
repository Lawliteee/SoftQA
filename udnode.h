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
    friend class GrammarRule;

    UDNode();
    UDNode(int i, QString lem, PosTag up, int h, DepRel dp, VerbMood m);
    UDNode(QString lem, PosTag up, VerbMood m);

    void markRelatedDescendants();  // отмечает достижимость потомков до корня
    const QSet<UDNode*>& getChildren() const { return children; }
    bool isConnectedToRoot() const { return connectedToRoot; }
    void setConnectedToRoot(bool connected) { connectedToRoot = connected; }


    void checkPattern(const Pattern* pattern,  QSet<Mistake> &Mistakes) const;     // метод выполнения всех проверок шаблона

    bool isBeForm () const;
    bool isModalVerb ()const;
    bool isHaveForm ()const;
    bool isDoForm()const;

    bool hasChildWithRel(DepRel rel) const;
    bool hasTemporalConditionalConjunction() const;

    bool isPresentSimple() const;
    bool isPresentContinuous() const;
    bool isPresentPerfect() const;
    bool isPresentPerfectContinuous() const;
    bool isNominalPredicate() const;
    bool isPresentBe() const;
    bool isPresentModal() const;
    bool isPresentHave() const;
    bool isPresentTense() const {
        return isPresentSimple() ||
               isPresentContinuous() ||
               isPresentPerfect() ||
               isPresentPerfectContinuous() ||
               isNominalPredicate() ||
               isPresentModal();  // Добавлена проверка модальных глаголов
    }

    bool isPastSimple() const;
    bool isPastContinuous() const;
    bool isPastPerfect() const;
    bool isPastPerfectContinuous() const;
    bool isPastNominalPredicate() const;
    bool isPastModal() const;
    bool isPastBe() const;
    bool isPastHave() const;
    bool isPastTense() const {
        return isPastSimple() ||
               isPastContinuous() ||
               isPastPerfect() ||
               isPastPerfectContinuous() ||
               isPastNominalPredicate() ||
               isPastModal();
    }

    bool isCountable()const;

    int getPerson() const;
    int getNumber() const;

    void addChild(const UDNode*);

    // Перегрузка оператора сравнения
    bool operator==(const UDNode& other) const
    {
        return id == other.id && lemma == other.lemma && upos == other.upos && head == other.head && depRel == other.depRel && mood == other.mood;
    }

    //gets
    int getId() const;
    QString getlemma() const;
    PosTag getUpos() const;
    int getHead() const;
    DepRel getDepRel() const;
    VerbMood getMood() const;

    void writeChildren(QSet<UDNode*>& ch)
    {
        ch = children;
    }
private:
    // Вспомогательный метод для проверки инфинитива
    bool hasInfinitiveChild() const {
        for (UDNode* child : children) {
            if ((child->depRel == Xcomp || child->depRel == Ccomp) &&
                child->upos == VB) {
                return true;
            }
        }
        return false;
    }

};
#endif // UDNODE_H
