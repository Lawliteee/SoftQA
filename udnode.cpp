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

void UDNode::addChild(const UDNode* child)
{
    children.insert(const_cast<UDNode*>(child));
}

UDNode::UDNode(int i, QString lem, PosTag up, int h, DepRel dp, VerbMood m)
{
    id = i;
    lemma = lem;
    upos = up;
    head = h;
    depRel = dp;
    mood = m;
    children = {};
    connectedToRoot = false;
}
int UDNode::getId()const
{
    return id;
}
QString UDNode::getlemma()const
{
    return lemma;
}
PosTag UDNode::getUpos()const
{
    return upos;
}
int UDNode::getHead()const
{
    return head;
}
DepRel UDNode::getDepRel()const
{
    return depRel;
}
VerbMood UDNode::getMood()const
{
    return mood;
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
