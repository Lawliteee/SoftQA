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

};
