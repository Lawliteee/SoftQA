#ifndef GRAMMARRULE_H
#define GRAMMARRULE_H


#include <mistake.h>

class GrammarRule
{
protected:
    QMap<int, UDNode*> allNodes; // Ссылка на мапу всех узлов
public:
    // Конструктор с пустой мапой по умолчанию
    GrammarRule() : allNodes(QMap<int, UDNode*>()) {}  // Явное создание пустой мапы

    GrammarRule(QMap<int, UDNode*>& nodes)
        : allNodes(nodes) {}
        ;
    void writeNodes(QMap<int, UDNode*> n)
    {
        allNodes = n;
    };

    virtual bool check(const UDNode*,const UDNode*,QSet<Mistake>& mistakes );

    // Метод для доступа к children (только для наследников)
    const QSet<UDNode*>& getNodeChildren(const UDNode* node) const {
        return node->children; // Доступно благодаря friend
    }

    // Вспомогательный метод для проверки наличия сиблинга
    bool hasSibling(const UDNode* node, const QString& siblingLemma) const {
        if (!node || node->getHead() == 0 || node->getHead() == -1) {
            return false;
        }

        UDNode* parent = allNodes.value(node->getHead(), nullptr);
        if (!parent) return false;

        // Используем метод базового класса для доступа к children
        const QSet<UDNode*>& siblings = getNodeChildren(parent);

        for (UDNode* child : siblings) {
            if (child != node &&
                child->getlemma().compare(siblingLemma, Qt::CaseInsensitive) == 0) {
                return true;
            }
        }
        return false;
    }

    bool isPresentClause (const UDNode* main)const;

    bool isFutureClause (const UDNode* main)const;
    bool isFutureAuxiliary(const UDNode* node) const;
    bool isFutureMainVerb(const UDNode* node) const;
    bool hasChildWithPos(const UDNode* node, PosTag pos) const;

    bool isPastClause (const UDNode* main)const;

};

class PersonNumberAgreement : public GrammarRule
{
public:
    using GrammarRule::GrammarRule;
    bool check(const UDNode*,const UDNode*,QSet<Mistake>& mistakes ) override;
};

class NumberAgreement : public GrammarRule
{
public:
    bool check(const UDNode*,const UDNode*,QSet<Mistake>& mistakes ) override;
};

class AuxAuxAgreement : public GrammarRule
{
public:
    bool check(const UDNode*,const UDNode*,QSet<Mistake>& mistakes ) override;
};

class MainAuxAgreement : public GrammarRule
{
public:
    bool check(const UDNode*,const UDNode*,QSet<Mistake>& mistakes ) override;
};

class PassiveAgreement : public GrammarRule
{
public:
    bool check(const UDNode*,const UDNode*,QSet<Mistake>& mistakes ) override;
};

class ComplexSentenceAgreement : public GrammarRule
{
public:
    bool check(const UDNode*,const UDNode*,QSet<Mistake>& mistakes ) override;
};

class ConditionalsAgreement : public GrammarRule
{
public:
    bool check(const UDNode*,const UDNode*,QSet<Mistake>& mistakes ) override;
private:
    // Проверка 2-го типа условных (would + V1)
    bool isSecondConditional(const UDNode* mainVerb) const
    {
        if (!mainVerb) return false;

        // Ищем would среди детей
        bool hasWould = false;
        bool hasBaseVerb = (mainVerb->getUpos() == VB);

        for (const UDNode* child : mainVerb->getChildren()) {
            if (child->getDepRel() == Aux &&
                child->getlemma().toLower() == "would") {
                hasWould = true;
                break;
            }
        }

        return hasWould && hasBaseVerb;
    }

    // Проверка 3-го типа условных (would have + V3)
    bool isThirdConditional(const UDNode* mainVerb) const
    {
        if (!mainVerb) return false;

        // Ищем would и have среди детей
        bool hasWould = false;
        bool hasHave = false;
        bool hasPastParticiple = (mainVerb->getUpos() == VBN);

        for (const UDNode* child : mainVerb->getChildren()) {
            if (child->getDepRel() == Aux) {
                QString lemma = child->getlemma().toLower();
                if (lemma == "would") hasWould = true;
                if (lemma == "have") hasHave = true;
            }
        }

        return hasWould && hasHave && hasPastParticiple;
    }
};

#endif // GRAMMARRULE_H
