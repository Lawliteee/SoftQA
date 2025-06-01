#include "udnode.h"

UDNode::UDNode() {}

/*!
* \brief Рекурсивная функция для выполнения всех проверок шаблона
* \param [in] pattern – шаблон правила
* \param [in] nodes – ассоциативный контейнер связей по идентификатору
* \param [in,out] mistakes – ошибки согласования
*/
void UDNode::checkPattern(const Pattern& pattern, const QMap<int, UDNode*>&nodes,  QSet<Mistake> &Mistakes)
{

};
