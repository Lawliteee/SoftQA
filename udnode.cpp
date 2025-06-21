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
void UDNode::checkPattern(const Pattern* pattern,  QSet<Mistake> &Mistakes) const
{

};

bool UDNode::isBeForm() const
{
    QString lowerLemma = lemma.toLower();
    return lowerLemma == "be"   || lowerLemma == "am"  || lowerLemma == "is" ||
           lowerLemma == "are"  || lowerLemma == "was" || lowerLemma == "were" ||
           lowerLemma == "been" || lowerLemma == "being";
}

bool UDNode::isModalVerb() const
{
    return upos == MD;
}

bool UDNode::isHaveForm() const
{
    QString lowerLemma = lemma.toLower();
    return lowerLemma == "have" || lowerLemma == "has" ||
           lowerLemma == "had"  || lowerLemma == "having" || lowerLemma == "'d";
}

bool UDNode::isDoForm() const
{
    QString lowerLemma = lemma.toLower();
    return lowerLemma == "do" || lowerLemma == "does" ||
           lowerLemma == "did" || lowerLemma == "doing" ||
           lowerLemma == "done";
}

int UDNode::getPerson() const
{
    // 1. Проверка существительных: NN, NNS, NNP, NNPS
    if (upos == PosTag::NN || upos == PosTag::NNS ||
        upos == PosTag::NNP || upos == PosTag::NNPS)
    {
        return 3; // Все существительные - 3 лицо
    }

    // 2. Проверка личных местоимений (PRP)
    if (upos == PosTag::PRP)
    {
        QString lowerLemma = lemma.toLower();

        // 1 лицо: I, we
        if (lowerLemma == "i" || lowerLemma == "we") {
            return 1;
        }

        // 2 лицо: you
        if (lowerLemma == "you") {
            return 2;
        }

        // 3 лицо: he, she, it, they
        if (lowerLemma == "he" || lowerLemma == "she" ||
            lowerLemma == "it" || lowerLemma == "they") {
            return 3;
        }
    }

    // 3. Для всех остальных случаев возвращаем -1 (неопределённое лицо)
    return -1;
}

int UDNode::getNumber() const
{
    QString lowerLemma = lemma.toLower();

    // 1. Проверка на артикль "the" (может быть с любым числом)
    if (lowerLemma == "the") {
        return 0;
    }

    // 2. Проверка на единственное число
    if (upos == PosTag::NN || upos == PosTag::NNP ||  // Существительные в ед. числе
        (upos == PosTag::PRP && (lowerLemma == "i" || lowerLemma == "he" ||
                                 lowerLemma == "she" || lowerLemma == "it")) || // Местоимения
        lowerLemma == "was" || lowerLemma == "this" || lowerLemma == "that" || // Глаголы и указательные
        lowerLemma == "a" || lowerLemma == "an" || lowerLemma == "one")       // Артикли и числительные
    {
        return 1;
    }

    // 3. Все остальные случаи считаем множественным числом
    return 2;
}
