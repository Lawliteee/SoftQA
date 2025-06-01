#include "udnode.h"

UDNode::UDNode()
    : id(0), lemma(""), upos(PosTag::NN), head(0),
    depRel(DepRel::Other), mood(VerbMood::None),
    children(), connectedToRoot(false)
{}
UDNode::UDNode(QString lem, PosTag up, VerbMood m)
{
    id = 0;
    lemma = lem;
    upos = up;
    head = 0;
    depRel = Other;
    mood = m;
    children = {};
    connectedToRoot = false;
}

/*!
* \brief Рекурсивная функция для выполнения всех проверок шаблона
* \param [in] pattern – шаблон правила
* \param [in] nodes – ассоциативный контейнер связей по идентификатору
* \param [in,out] mistakes – ошибки согласования
*/
void UDNode::checkPattern(const Pattern& pattern, const QMap<int, UDNode*>&nodes,  QSet<Mistake> &Mistakes)
{

};
