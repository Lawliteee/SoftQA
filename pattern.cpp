#include "pattern.h"

Pattern::Pattern() {}

/*!
* \brief Функция проверки соответствия узла шаблону
* \param [in] node - проверяемый узел дерева
* \param [in] usedChildren - множество уже использованных дочерних узлов
* \return True – узел соответствует шаблону, false – нет
*/
bool Pattern::matchesPattern(const UDNode* node, QSet<const UDNode*>& usedChildren)
{
    return true;
};

void Pattern::compareMatches(const Pattern* expected, QStringList& errors, const QString& path) const
{
    // Проверка на нулевые указатели
    if (!expected) {
        errors.append(QString("Expected pattern is null at %1").arg(path));
        return;
    }


    // Проверка currentMatch текущего узла
    if (!currentMatch && !expected->currentMatch) {
        // Оба nullptr
        return;
    }
    else if (!currentMatch || !expected->currentMatch) {
        // Только один из них nullptr - ошибка
        errors.append(QString("Match mismatch at %1\n  Expected: %2\n  Actual: %3")
                          .arg(path)
                          .arg(expected->currentMatch ? QString::number(expected->currentMatch->getId()) : "null")
                          .arg(currentMatch ? QString::number(currentMatch->getId()) : "null"));
        return;
    }


    for (auto it = children.constBegin(); it != children.constEnd(); ++it)
    {
        const QString relationName = depRelToString(it.key());

        // Получаем все значения для этого ключа
        const QList<Pattern*> allPatterns = children.values(it.key());
        const QList<Pattern*> allExpected = expected->children.values(it.key());

        for (int i = 0; i < allPatterns.size(); ++i)
        {
            const QString childPath = QString("%1[%2](%3)")
            .arg(path)
                .arg(relationName)
                .arg(i);

            allPatterns[i]->compareMatches(
                allExpected[i],
                errors,
                childPath
                );
        }
    }
}

QStringList Pattern::getUncalledChecks(const QString& path) const
{
    QStringList result;

    // Проверяем checks текущего Pattern
    for (const RelTypeCheck* check : checks) {
        if (!check) {
            result << QString("%1: null check").arg(path);
        } else if (!check->isCalled()) {
            result << QString("%1: uncalled check").arg(path);
        }
    }

    // Рекурсивно проверяем все дочерние Pattern
    for (auto it = children.constBegin(); it != children.constEnd(); ++it) {
        const QString relationName = depRelToString(it.key());
        const QList<Pattern*> childPatterns = children.values(it.key());
        for (int i = 0; i < childPatterns.size(); ++i) {
            const QString childPath = QString("%1[%2](%3)").arg(path).arg(relationName).arg(i);
            if (!childPatterns[i]) {
                result << QString("%1: null pattern").arg(childPath);
            } else {
                result << childPatterns[i]->getUncalledChecks(childPath);
            }
        }
    }

    return result;
}
