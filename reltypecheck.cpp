#include "reltypecheck.h"

RelTypeCheck::RelTypeCheck() {}

/*!
* \brief Функция для построения дерева из связей
* \param [in] mainNode –узел, для которого поиск поиск пары
* \param [in] parent – родительский узел
* \param [out] searchNode – найден второй узел
*/
void ChildChild::getNodes(const UDNode* mainNode,UDNode** searchNode, const UDNode* parent)
{
    QSet<UDNode*> children = parent->getChildren();

    QList<UDNode*> matchingChildren;

    for (UDNode* child : children)
    {
        bool isSameNode = (child->getId() == mainNode->getId());
        bool hasValidRel = (relatedRel == Other || child->getDepRel() == relatedRel);
        bool hasValidTag = validTags.isEmpty() || validTags.contains(child->getUpos());
        bool hasValidWord = validWords.isEmpty() || validWords.contains(child->getlemma());

        if (!isSameNode && hasValidRel && hasValidTag && hasValidWord)
        {
            matchingChildren.append(child);
        }
    }

    if (matchingChildren.isEmpty())
    {
        throw QString("Not enough children or no match");
    }

    if (matchingChildren.size() > 1)
    {
        throw QString("Several matching children");
    }

    *searchNode = matchingChildren.first();
};

/*!
* \brief Функция для построения дерева из связей
* \param [in] mainNode –узел, для которого поиск поиск пары
* \param [in] parent – родительский узел
* \param [out] searchNode – найден второй узел
*/
void ParentChild:: getNodes(const UDNode* mainNode,UDNode** searchNode,const UDNode* parent)
{
    if (mainNode->getHead() == 0) {
        throw QString("mainNode is root");
    }

    // Просто возвращаем родительский узел
    *searchNode = const_cast<UDNode*>(parent);
};

