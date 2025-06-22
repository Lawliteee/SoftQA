#include "pattern.h"

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
void UDNode::checkPattern(const Pattern* pattern, QSet<Mistake>& mistakes) const {
    if (pattern) {
        pattern->check(this, mistakes);
    }
}

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

void UDNode::markRelatedDescendants() {
    // Если уже посещен, выходим чтобы избежать бесконечной рекурсии
    if (connectedToRoot) {
        return;
    }

    connectedToRoot = true;

    // Рекурсивно помечаем всех детей
    for (UDNode* child : children) {
        child->markRelatedDescendants();
    }
}

bool UDNode::isPresentSimple() const {
    return (upos == VBP || upos == VBZ) &&
           !isBeForm() && !isHaveForm(); // Исключаем вспомогательные глаголы
}

// Present Continuous (am/is/are + VBG)
bool UDNode::isPresentContinuous() const {
    if (!isPresentBe()) return false;

    for (UDNode* child : children) {
        if ((child->depRel == Xcomp || child->depRel == Ccomp) &&
            child->upos == VBG) {
            return true;
        }
    }
    return false;
}

bool UDNode::isPresentPerfect() const {
    if (!isPresentHave()) return false;

    for (UDNode* child : children) {
        if ((child->depRel == Xcomp || child->depRel == Ccomp) &&
            child->upos == VBN) {
            return true;
        }
    }
    return false;
}

bool UDNode::isPresentPerfectContinuous() const {
    if (!isPresentHave()) return false;

    for (UDNode* child : children) {
        if (child->upos == VBN &&
            child->lemma.compare("be", Qt::CaseInsensitive) == 0) {
            for (UDNode* vbgChild : child->children) {
                if (vbgChild->upos == VBG) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Именное сказуемое (I am [student])
bool UDNode::isNominalPredicate() const {

    for (UDNode* child : children) {
        if (child->isPresentBe() &&
            (child->depRel == Cop || child->depRel == Aux)) {
            return true;
        }
    }
    return false;
}

bool UDNode::isPresentBe() const {
    QString lowerLemma = lemma.toLower();
    return (lowerLemma == "be" || lowerLemma == "am" ||
            lowerLemma == "is" || lowerLemma == "are") &&
           (upos == VBP || upos == VBZ);
}

bool UDNode::isPresentModal() const {
    if (upos != MD) return false;

    QString lowerLemma = lemma.toLower();
    // Основные модальные глаголы настоящего времени
    if (lowerLemma == "can" || lowerLemma == "may" ||
        lowerLemma == "must" || lowerLemma == "shall") {
        // Проверяем наличие инфинитива
        for (UDNode* child : children) {
            if ((child->depRel == Xcomp || child->depRel == Ccomp) &&
                child->upos == VB) {
                return true;
            }
        }
    }
    return false;
}

bool UDNode::isPresentHave() const {
    QString lowerLemma = lemma.toLower();
    return (lowerLemma == "have" || lowerLemma == "has") && (upos == VBP || upos == VBZ);
}

// Past Simple (VBD)
bool UDNode::isPastSimple() const {
    return upos == VBD;
}

// Past Continuous (was/were + VBG)
bool UDNode::isPastContinuous() const {
    if (!isPastBe()) return false;

    for (UDNode* child : children) {
        if ((child->depRel == Xcomp || child->depRel == Ccomp) &&
            child->upos == VBG) {
            return true;
        }
    }
    return false;
}

// Past Perfect (had + VBN)
bool UDNode::isPastPerfect() const {
    if (!isPastHave()) return false;

    for (UDNode* child : children) {
        if ((child->depRel == Xcomp || child->depRel == Ccomp) &&
            child->upos == VBN) {
            return true;
        }
    }
    return false;
}

// Past Perfect Continuous (had been + VBG)
bool UDNode::isPastPerfectContinuous() const {
    if (!isPastHave()) return false;

    for (UDNode* child : children) {
        if (child->upos == VBN &&
            child->lemma.compare("be", Qt::CaseInsensitive) == 0) {
            for (UDNode* vbgChild : child->children) {
                if (vbgChild->upos == VBG) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Именное сказуемое в прошедшем (I was [student])
bool UDNode::isPastNominalPredicate() const {

    for (UDNode* child : children) {
        if (child->isPastBe() &&
            (child->depRel == Cop || child->depRel == Aux)) {
            return true;
        }
    }
    return false;
}

// Вспомогательные методы
bool UDNode::isPastBe() const {
    QString lowerLemma = lemma.toLower();
    return (lowerLemma == "was" || lowerLemma == "were") &&
           (upos == VBD);
}

bool UDNode::isPastHave() const {
    QString lowerLemma = lemma.toLower();
    return (lowerLemma == "had" || lowerLemma == "'d") &&
           upos == VBD;
}

bool UDNode::isPastModal() const {
    if (!isModalVerb()) return false;

    QString lowerLemma = lemma.toLower();
    // Основные модалы прошедшего времени
    return (lowerLemma == "could" || lowerLemma == "might" ||
            lowerLemma == "would") &&
           hasInfinitiveChild();
}

bool UDNode::hasChildWithRel(DepRel rel) const
{
    for (UDNode* child : children) {
        if (child->depRel == rel) {
            return true;
        }
    }
    return false;
}

// Проверка наличия временного/условного союза
bool UDNode::hasTemporalConditionalConjunction() const
{

    // Список временных и условных союзов
    static const QSet<QString> conjunctions = {
        "when", "while", "as", "after",
        "until", "till", "since", "once",
        "if", "unless", "case", "provided",
        "assuming", "whether"
    };

    // Ищем маркер среди детей зависимого глагола
    for (const UDNode* child : children) {
        if (child->getDepRel() == Mark &&
            conjunctions.contains(child->getlemma().toLower())) {
            return true;
        }
    }
    return false;
}
