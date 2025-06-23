/**
 * @file reltypecheck.h
 * @brief Заголовочный файл, содержащий определения классов для проверки типов отношений между узлами
 */

#ifndef RELTYPECHECK_H
#define RELTYPECHECK_H

#include <grammarrule.h>

/**
 * @class RelTypeCheck
 * @brief Базовый класс для проверки отношений между узлами дерева зависимостей
 *
 * Предоставляет базовую функциональность для проверки грамматических правил
 * между связанными узлами с использованием объектов GrammarRule.
 */
class RelTypeCheck
{
protected:
    GrammarRule* rule;       ///< Указатель на грамматическое правило для проверки
    QString message;         ///< Сообщение об ошибке (если есть)
    bool checkCalled = false;///< Флаг, указывающий была ли выполнена проверка

public:
    /**
     * @brief Конструктор по умолчанию
     */
    RelTypeCheck();

    /**
     * @brief Выполняет проверку грамматического правила между узлами
     * @param[in] node1 Первый узел для проверки
     * @param[in] node2 Второй узел для проверки
     * @param[out] m Множество для записи найденных ошибок
     * @throws QString Может передавать исключения из GrammarRule::check
     *
     * Устанавливает флаг checkCalled в true перед выполнением проверки.
     */
    void callCheck(const UDNode* node1, const UDNode* node2, QSet<Mistake>& m)
    {
        checkCalled = true;
        try {
            if (rule) {
                rule->check(node1, node2, m);
            }
        } catch (const QString& error) {
            throw; // Перебрасываем для обработки в Pattern
        }
    }

    /**
     * @brief Проверяет, была ли выполнена проверка
     * @return true если проверка была выполнена, иначе false
     */
    bool isCalled() const
    {
        return checkCalled;
    }

    /**
     * @brief Устанавливает грамматическое правило для проверки
     * @param[in] r Указатель на объект GrammarRule
     */
    void setRule(GrammarRule* r)
    {
        rule = r;
    }

    /**
     * @brief Виртуальный метод для получения узлов для проверки
     * @param[in] mainNode Основной узел
     * @param[out] searchNode Указатель на искомый узел (будет заполнен)
     * @param[in] parent Родительский узел
     *
     * @note Должен быть переопределен в производных классах
     */
    virtual void getNodes(const UDNode* mainNode, UDNode** searchNode, const UDNode* parent)
    {
        return;
    };
};

/**
 * @class ParentChild
 * @brief Класс для проверки отношений между родительским и дочерним узлами
 *
 * Наследует базовую функциональность RelTypeCheck и реализует
 * специфическую логику поиска узлов для проверки отношений
 * родитель-потомок.
 */
class ParentChild : public RelTypeCheck
{
public:
    /**
     * @brief Получает узлы для проверки отношений родитель-потомок
     * @inheritDoc RelTypeCheck::getNodes
     *
     * Реализует логику поиска дочернего узла относительно родительского.
     */
    void getNodes(const UDNode* mainNode, UDNode** searchNode, const UDNode* parent) override;
};

/**
 * @class ChildChild
 * @brief Класс для проверки отношений между дочерними узлами
 *
 * Наследует базовую функциональность RelTypeCheck и реализует
 * специфическую логику поиска узлов для проверки отношений
 * между дочерними узлами одного родителя.
 */
class ChildChild : public RelTypeCheck
{
    DepRel relatedRel;           ///< Тип зависимости для поиска связанного узла
    QSet<QString> validWords;    ///< Допустимые слова/леммы для связанного узла
    QSet<PosTag> validTags;      ///< Допустимые частеречные теги для связанного узла

public:
    /**
     * @brief Получает узлы для проверки отношений между дочерними узлами
     * @inheritDoc RelTypeCheck::getNodes
     *
     * Ищет связанный узел по заданному типу зависимости, учитывая ограничения
     * на слова и теги.
     */
    void getNodes(const UDNode* mainNode, UDNode** searchNode, const UDNode* parent) override;

    /**
     * @brief Устанавливает тип зависимости для поиска связанного узла
     * @param[in] rel Тип зависимости
     */
    void setRelatedRel(DepRel rel) { relatedRel = rel; }

    /**
     * @brief Устанавливает допустимые слова для связанного узла
     * @param[in] words Множество допустимых слов
     */
    void setValidWords(const QSet<QString>& words) { validWords = words; }

    /**
     * @brief Устанавливает допустимые теги для связанного узла
     * @param[in] tags Множество допустимых частеречных тегов
     */
    void setValidTags(const QSet<PosTag>& tags) { validTags = tags; }
};

#endif // RELTYPECHECK_H
