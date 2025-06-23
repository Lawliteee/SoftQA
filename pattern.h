/**
 * @file pattern.h
 * @brief Заголовочный файл, содержащий определение класса Pattern для описания шаблонов грамматических конструкций
 */

#ifndef PATTERN_H
#define PATTERN_H

#include <QObject>
#include <reltypecheck.h>

/**
 * @class Pattern
 * @brief Класс для описания шаблонов грамматических конструкций и их проверки
 *
 * Класс позволяет задавать шаблоны для поиска в дереве разбора (UDNode) с учетом:
 * - Допустимых слов
 * - Допустимых частеречных тегов
 * - Дочерних элементов с определенными типами зависимостей
 * - Набора проверок для найденных конструкций
 */
class Pattern
{
    QSet<QString> validWords;            ///< Множество допустимых слов/лемм для шаблона
    QSet<PosTag> validTags;              ///< Множество допустимых частеречных тегов
    QMultiMap<DepRel, Pattern*> children;///< Дочерние шаблоны с типами зависимостей
    QSet<RelTypeCheck*> checks;          ///< Набор проверок для шаблона
    UDNode* currentMatch;                ///< Текущее совпадение с узлом дерева

public:
    /**
     * @brief Конструктор по умолчанию
     */
    Pattern();

    /**
     * @brief Проверяет соответствие узла шаблону
     * @param[in] node Узел для проверки
     * @param[out] mistakes Множество для записи найденных ошибок
     */
    void check(const UDNode* node, QSet<Mistake>& mistakes) const;

    /**
     * @brief Конструктор с полной инициализацией
     * @param[in] words Множество допустимых слов
     * @param[in] tags Множество допустимых тегов
     * @param[in] childrenPatterns Дочерние шаблоны с типами зависимостей
     */
    Pattern(const QSet<QString>& words, const QSet<PosTag>& tags,
            QMultiMap<DepRel, Pattern*>& childrenPatterns)
    {
        validWords = words;
        validTags = tags;
        children = childrenPatterns;
    }

    /**
     * @brief Конструктор с частичной инициализацией (без дочерних шаблонов)
     * @param[in] words Множество допустимых слов
     * @param[in] tags Множество допустимых тегов
     */
    Pattern(const QSet<QString>& words, const QSet<PosTag>& tags)
    {
        validWords = words;
        validTags = tags;
    }

    /**
     * @brief Добавляет дочерний шаблон
     * @param[in] relation Тип зависимости для дочернего элемента
     * @param[in] childPattern Дочерний шаблон
     */
    void addChildPattern(DepRel relation, Pattern* childPattern)
    {
        children.insert(relation, childPattern);
    }

    /**
     * @brief Добавляет проверку для шаблона
     * @param[in] r Указатель на объект проверки
     */
    void addСheck(RelTypeCheck* r)
    {
        checks.insert(r);
    }

    /**
     * @brief Проверяет соответствие узла шаблону
     * @param[in] node Узел для проверки
     * @param[out] usedChildren Множество уже использованных дочерних узлов
     * @return true если узел соответствует шаблону
     */
    bool matchesPattern(const UDNode* node, QSet<const UDNode*>& usedChildren);

    /**
     * @brief Сравнивает текущее совпадение с ожидаемым шаблоном
     * @param[in] expected Ожидаемый шаблон
     * @param[out] errors Список найденных расхождений
     * @param[in] path Текущий путь в шаблоне (для рекурсивного вызова)
     */
    void compareMatches(const Pattern* expected, QStringList& errors,
                        const QString& path = "root") const;

    /**
     * @brief Возвращает список непроверенных проверок
     * @param[in] path Текущий путь в шаблоне (для рекурсивного вызова)
     * @return Список непроверенных проверок
     */
    QStringList getUncalledChecks(const QString& path) const;

    /**
     * @brief Устанавливает текущее совпадение с узлом
     * @param[in] match Узел, совпадающий с шаблоном
     */
    void setMatch(UDNode* match)
    {
        currentMatch = match;
    }
};

#endif // PATTERN_H
