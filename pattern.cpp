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

    // Проверка currentMatch (защита от nullptr)
    if (!currentMatch || !expected || !expected->currentMatch)
    {
        errors.append(QString("Invalid Pattern or Match at %1").arg(path));
        return; // Прекращаем проверку для этого узла
    }
    // Проверка currentMatch текущего узла
    if (currentMatch->getId() != expected->currentMatch->getId())
    {
        errors.append(QString("Mismatch at %1\n  Expected ID: %2\n  Actual ID: %3")
                          .arg(path)
                          .arg(expected->currentMatch ? expected->currentMatch->getId() : -1)
                          .arg(currentMatch ? currentMatch->getId() : -1));
    }

    for (auto it = children.constBegin(); it != children.constEnd(); ++it)
    {
        const QString relationName = depRelToString(it.key());
        //Pattern* currentPattern = it.value();                               // Получаем один Pattern*

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
