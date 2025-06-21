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
};

#endif // GRAMMARRULE_H
