#include "grammarrule.h"


/*!
* \brief Функция проверки согласованности по лицу и числу
* \param [in] node1 – узел определяющий правило
* \param [in] node2 - узел проверяемый на согласованность
* \param [in,out]  mistakes- множество ошибок
* \return true если слова согласованны, иначе false
*/
bool GrammarRule::check(const UDNode* node1,const UDNode* node2,QSet<Mistake>& mistakes )
{
    return true;
};

bool PersonNumberAgreement::check(const UDNode* nsubj, const UDNode* verb, QSet<Mistake>& mistakes)
{
    // Проверка на nullptr
    if (!nsubj || !verb) {
        throw QString("Node pointer is null");
    }

    // Проверка частей речи
    static const QSet<PosTag> validSubjectTags = {NN, NNS, NNP, NNPS, PRP};
    static const QSet<PosTag> validVerbTags = {VBP, VBZ};

    if (!validSubjectTags.contains(nsubj->getUpos())) {
        throw QString("Invalid part of speech");
    }
    if (!validVerbTags.contains(verb->getUpos())) {
        throw QString("Invalid part of speech");
    }

    // 1. Проверка наклонения subjunctive
    if (verb->getMood() == Subj) {
        if (verb->getUpos() == VBP) {
            return true;
        } else {
            mistakes.insert(Mistake("Глагол " + verb->getlemma() + " в неправильной форме. В Present Subjunctive используется базовая форма"));
            return false;
        }
    }

    // Проверка для будущего времени (особый случай с have)
    if (verb->isHaveForm() && hasSibling(verb, "will")) {
        if (verb->getlemma() != "have") {
            mistakes.insert(Mistake("Неправильная форма глагола have. В будущем времени употребляется только форма have"));
            return false;
        }
        return true;
    }

    // 2. Проверка 3 лица единственного числа
    if (nsubj->getPerson() == 3 && nsubj->getNumber() == 1) {
        if (verb->getUpos() == VBZ) {
            return true;
        } else {
            QString message;
            if (verb->isBeForm() || verb->isHaveForm() || verb->isDoForm()) {
                message = "Вспомогательный глагол " + verb->getlemma() + " несогласован с подлежащим " +
                          nsubj->getlemma() + ". Глагол должен быть в форме 3-го лица ед. числа";
            } else {
                message = "Основной глагол " + verb->getlemma() + " несогласован с подлежащим " +
                          nsubj->getlemma() + ". Глагол должен быть в форме 3-го лица ед. числа";
            }
            mistakes.insert(Mistake(message));
            return false;
        }
    }
    // 3. Проверка для всех остальных случаев
    else {
        if (verb->getUpos() == VBP) {
            return true;
        } else {
            QString message;
            if (verb->isBeForm() || verb->isHaveForm() || verb->isDoForm()) {
                message = "Вспомогательный глагол " + verb->getlemma() + " несогласован с подлежащим " +
                          nsubj->getlemma() + ". Глагол должен быть в начальной форме";
            } else {
                message = "Основной глагол " + verb->getlemma() + " несогласован с подлежащим " +
                          nsubj->getlemma() + ". Глагол должен быть в начальной форме";
            }
            mistakes.insert(Mistake(message));
            return false;
        }
    }


    return true;
}

bool NumberAgreement::check(const UDNode* node1,const UDNode* node2,QSet<Mistake>& mistakes )
{
    return true;
};

bool MainAuxAgreement::check(const UDNode* node1,const UDNode* node2,QSet<Mistake>& mistakes )
{
    return true;
};

bool AuxAuxAgreement::check(const UDNode* node1,const UDNode* node2,QSet<Mistake>& mistakes )
{
    return true;
};

bool PassiveAgreement::check(const UDNode* node1,const UDNode* node2,QSet<Mistake>& mistakes )
{
    return true;
};

bool ComplexSentenceAgreement::check(const UDNode* node1,const UDNode* node2,QSet<Mistake>& mistakes )
{
    return true;
};

bool ConditionalsAgreement::check(const UDNode* node1,const UDNode* node2,QSet<Mistake>& mistakes )
{
    return true;
};
