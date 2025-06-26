/**
 * @file GrammarRule.h
 * @brief Заголовочный файл, содержащий объявление классов для проверки грамматических правил
 */

#ifndef GRAMMARRULE_H
#define GRAMMARRULE_H

#include <mistake.h>

/**
 * @class GrammarRule
 * @brief Базовый класс для проверки грамматических правил
 *
 * Класс предоставляет базовую функциональность для проверки различных грамматических правил,
 * а также вспомогательные методы для анализа узлов дерева зависимостей.
 */
class GrammarRule
{
protected:
    QMap<int, UDNode*> allNodes; ///< Карта всех узлов дерева зависимостей (ID -> узел)

public:
    /**
     * @brief Конструктор по умолчанию
     *
     * Создает правило с пустой картой узлов
     */
    GrammarRule() : allNodes(QMap<int, UDNode*>()) {}

    /**
     * @brief Конструктор с инициализацией карты узлов
     * @param nodes Карта узлов дерева зависимостей
     */
    GrammarRule(QMap<int, UDNode*>& nodes) : allNodes(nodes) {};

    /**
     * @brief Устанавливает карту узлов
     * @param n Новая карта узлов
     */
    void writeNodes(QMap<int, UDNode*> n)
    {
        allNodes = n;
    };

    /**
     * @brief Виртуальный метод проверки грамматического правила
     * @param[in] node1 Первый узел для проверки
     * @param[in] node2 Второй узел для проверки
     * @param[out] mistakes Множество для записи найденных ошибок
     * @return true если ошибок не найдено, false в противном случае
     */
    virtual bool check(const UDNode*, const UDNode*, QSet<Mistake>& mistakes);

    /**
     * @brief Получает дочерние узлы указанного узла
     * @param node Узел, для которого нужно получить дочерние узлы
     * @return Константная ссылка на множество дочерних узлов
     */
    const QSet<UDNode*>& getNodeChildren(const UDNode* node) const {
        return node->children;
    }

    /**
     * @brief Проверяет наличие сиблинга с заданной леммой
     * @param node Узел, для которого проверяются сиблинги
     * @param siblingLemma Лемма искомого сиблинга
     * @return true если сиблинг найден, false в противном случае
     */
    bool hasSibling(const UDNode* node, const QString& siblingLemma) const {
        if (!node || node->getHead() == 0 || node->getHead() == -1) {
            return false;
        }

        UDNode* parent = allNodes.value(node->getHead(), nullptr);
        if (!parent) return false;

        const QSet<UDNode*>& siblings = getNodeChildren(parent);

        for (UDNode* child : siblings) {
            if (child != node &&
                child->getlemma().compare(siblingLemma, Qt::CaseInsensitive) == 0) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Проверяет, является ли клауза настоящим временем
     * @param main Главный узел клаузы
     * @return true если клауза в настоящем времени, false в противном случае
     */
    bool isPresentClause(const UDNode* main) const;

    /**
     * @brief Проверяет, является ли клауза будущим временем
     * @param main Главный узел клаузы
     * @return true если клауза в будущем времени, false в противном случае
     */
    bool isFutureClause(const UDNode* main) const;

    /**
     * @brief Проверяет, является ли узел вспомогательным глаголом будущего времени
     * @param node Узел для проверки
     * @return true если узел является вспомогательным глаголом будущего времени
     */
    bool isFutureAuxiliary(const UDNode* node) const;

    /**
     * @brief Проверяет, является ли узел основным глаголом будущего времени
     * @param node Узел для проверки
     * @return true если узел является основным глаголом будущего времени
     */
    bool isFutureMainVerb(const UDNode* node) const;

    /**
     * @brief Проверяет наличие дочернего узла с указанной частью речи
     * @param node Родительский узел
     * @param pos Искомая часть речи
     * @return true если найден дочерний узел с указанной частью речи
     */
    bool hasChildWithPos(const UDNode* node, PosTag pos) const;

    /**
     * @brief Проверяет, является ли клауза прошедшим временем
     * @param main Главный узел клаузы
     * @return true если клауза в прошедшем времени, false в противном случае
     */
    bool isPastClause(const UDNode* main) const;
};

/**
 * @class PersonNumberAgreement
 * @brief Класс для проверки согласования по лицу и числу
 */
class PersonNumberAgreement : public GrammarRule
{
public:
    using GrammarRule::GrammarRule;

    /**
     * @brief Проверяет согласование по лицу и числу
     * @inheritDoc GrammarRule::check
     */
    bool check(const UDNode*, const UDNode*, QSet<Mistake>& mistakes) override;
};

/**
 * @class NumberAgreement
 * @brief Класс для проверки согласования по числу
 */
class NumberAgreement : public GrammarRule
{
public:
    /**
     * @brief Проверяет согласование по числу
     * @inheritDoc GrammarRule::check
     */
    bool check(const UDNode*, const UDNode*, QSet<Mistake>& mistakes) override;
};

/**
 * @class AuxAuxAgreement
 * @brief Класс для проверки согласования между вспомогательными глаголами
 */
class AuxAuxAgreement : public GrammarRule
{
public:
    /**
     * @brief Проверяет согласование между вспомогательными глаголами
     * @inheritDoc GrammarRule::check
     */
    bool check(const UDNode*, const UDNode*, QSet<Mistake>& mistakes) override;
};

/**
 * @class MainAuxAgreement
 * @brief Класс для проверки согласования между основным и вспомогательным глаголами
 */
class MainAuxAgreement : public GrammarRule
{
public:
    /**
     * @brief Проверяет согласование между основным и вспомогательным глаголами
     * @inheritDoc GrammarRule::check
     */
    bool check(const UDNode*, const UDNode*, QSet<Mistake>& mistakes) override;
};

/**
 * @class PassiveAgreement
 * @brief Класс для проверки согласования в пассивных конструкциях
 */
class PassiveAgreement : public GrammarRule
{
public:
    /**
     * @brief Проверяет согласование в пассивных конструкциях
     * @inheritDoc GrammarRule::check
     */
    bool check(const UDNode*, const UDNode*, QSet<Mistake>& mistakes) override;
};

/**
 * @class ComplexSentenceAgreement
 * @brief Класс для проверки согласования в сложных предложениях
 */
class ComplexSentenceAgreement : public GrammarRule
{
public:
    /**
     * @brief Проверяет согласование в сложных предложениях
     * @inheritDoc GrammarRule::check
     */
    bool check(const UDNode*, const UDNode*, QSet<Mistake>& mistakes) override;
};

/**
 * @class ConditionalsAgreement
 * @brief Класс для проверки согласования в условных предложениях
 */
class ConditionalsAgreement : public GrammarRule
{
public:
    /**
     * @brief Проверяет согласование в условных предложениях
     * @inheritDoc GrammarRule::check
     */
    bool check(const UDNode*, const UDNode*, QSet<Mistake>& mistakes) override;

private:
    /**
     * @brief Проверяет, является ли конструкция условным предложением 2-го типа
     * @param mainVerb Главный глагол для проверки
     * @return true если конструкция соответствует 2-му типу условных предложений
     */
    bool isSecondConditional(const UDNode* mainVerb) const
    {
        if (!mainVerb) return false;

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

    /**
     * @brief Проверяет, является ли конструкция условным предложением 3-го типа
     * @param mainVerb Главный глагол для проверки
     * @return true если конструкция соответствует 3-му типу условных предложений
     */
    bool isThirdConditional(const UDNode* mainVerb) const
    {
        if (!mainVerb) return false;

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


/**
 * @class NumberAgreement
 * @brief Класс для проверки согласования по роду
 */
class GenderAgreement : public GrammarRule
{
public:
    /**
     * @brief Проверяет согласование по роду
     * @inheritDoc GrammarRule::check
     */
    bool check(const UDNode*, const UDNode*, QSet<Mistake>& mistakes) override;
};
#endif // GRAMMARRULE_H
