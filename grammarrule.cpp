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

bool NumberAgreement::check(const UDNode* word1, const UDNode* word2, QSet<Mistake>& mistakes)
{
    // Проверка на нулевые указатели
    if (!word1 || !word2) {
        throw QString("Node pointer is null");
    }

    // Проверка допустимых частей речи
    const QSet<PosTag> allowedTagsWord1 = {PRP, NN, NNS, NNP, NNPS, DT, CD};
    const QSet<PosTag> allowedTagsWord2 = {PRP, NN, NNS, NNP, NNPS, DT, CD, VBD};

    if (!allowedTagsWord1.contains(word1->getUpos())) {
        throw QString("Invalid part of speech");
    }
    if (!allowedTagsWord2.contains(word2->getUpos())) {
        throw QString("Invalid part of speech");
    }

    // Специальная проверка для глаголов - допускаем только was/were
    if (word2->getUpos() == VBD) {
        QString verb = word2->getlemma().toLower();
        if (verb != "was" && verb != "were") {
            throw QString("Verb must be 'was' or 'were'");
        }
    }

    // Получаем числа слов
    int num1 = word1->getNumber();
    int num2 = word2->getNumber();

    // Особый случай для Subjunctive mood (were для всех лиц)
    if (word2->getMood() == Subj) {
        if (word2->getlemma().toLower() == "were") {
            return true;
        } else {
            mistakes.insert(Mistake("глагол " + word2->getlemma() +
                                    " не используется в Past Subjunctive. "
                                    "Используется were для всех лиц (даже с he/she/it)."));
            return false;
        }
    }

    // Проверка согласованности
    if (num1 == 0 || num2 == 0 || num1 == num2) {
        return true;
    }

    // Обработка особых случаев для указательных местоимений и квантификаторов
    QString word1Lemma = word1->getlemma().toLower();
    QString word2Lemma = word2->getlemma().toLower();

    // Указательные местоимения (this/that/these/those)
    if (word1Lemma == "this" || word1Lemma == "that" ||
        word1Lemma == "these" || word1Lemma == "those") {

        QString correctForm = (num2 == 1) ? "this/that" : "these/those";
        QString currentForm = (word1Lemma == "this" || word1Lemma == "that") ? "this/that" : "these/those";

        if (currentForm != correctForm) {
            QString message = QString("Указательное местоимение %1 несогласовано по числу с существительным %2. "
                                      "Указательное местоимение должно быть в форме %3.")
                                  .arg(word1->getlemma())
                                  .arg(word2->getlemma())
                                  .arg(correctForm);
            mistakes.insert(Mistake(message));
            return false;
        }
    }

    // Квантификаторы (some/any/many/few/several/much/little)
    const QSet<QString> quantifiers = {"some", "any", "many", "few", "several", "much", "little"};
    if (quantifiers.contains(word1Lemma)) {
        if (num2 != 2) { // Квантификаторы требуют множественного числа
            QString message = QString("Квантификатор %1 некорректно использован. "
                                      "Нельзя использовать с существительным %2 в единственном числе")
                                  .arg(word1->getlemma())
                                  .arg(word2->getlemma());
            mistakes.insert(Mistake(message));
            return false;
        }
    }

    // Обработка артиклей (a/an)
    if (word1->getUpos() == DT && (word1Lemma == "a" || word1Lemma == "an")) {
        if (num2 != 1) { // Артикли a/an требуют единственного числа
            QString message = QString("Артикль %1 несогласован по числу с существительным %2. "
                                      "Требуется артикль the или нулевой артикль.")
                                  .arg(word1->getlemma())
                                  .arg(word2->getlemma());
            mistakes.insert(Mistake(message));
            return false;
        }
    }

    // Обработка указательных местоимений (this/that/these/those)
    if (word1Lemma == "this" || word1Lemma == "that" ||
        word1Lemma == "these" || word1Lemma == "those") {

        QString correctForm = (num2 == 1) ? "this/that" : "these/those";
        QString currentForm = (word1Lemma == "this" || word1Lemma == "that") ? "this/that" : "these/those";

        if (currentForm != correctForm) {
            QString message = QString("Указательное местоимение %1 несогласовано по числу с существительным %2. "
                                      "Указательное местоимение должно быть в форме %3.")
                                  .arg(word1->getlemma())
                                  .arg(word2->getlemma())
                                  .arg(correctForm);
            mistakes.insert(Mistake(message));
            return false;
        }
    }

    // Общий случай для остальных ошибок
    QString message;
    QString word1Type = word1->getUpos() == PRP ? "Местоимение" :
                            (word1->getUpos() == DT ? "Артикль" :
                                 (word1->getUpos() == CD ? "Числительное" : "Существительное"));

    QString word2Type = word2->getUpos() == PRP ? "местоимением" :
                            (word2->getUpos() == DT ? "артиклем" :
                                 (word2->getUpos() == CD ? "числительным" :
                                      (word2->getUpos() == VBD ? "глаголом" : "существительным")));

    if (word2->getUpos() == VBD) {
        if (num1 == 1) {
            message = QString("глагол %1 несогласован с подлежащим %2. "
                              "Глагол должен быть в форме единственного числа.")
                          .arg(word2->getlemma()).arg(word1->getlemma());
        } else {
            message = QString("глагол %1 несогласован с подлежащим %2. "
                              "Глагол должен быть в форме множественного числа.")
                          .arg(word2->getlemma()).arg(word1->getlemma());
        }
    } else {
        message = QString("%1 %2 несогласовано по числу с %3 %4.")
                      .arg(word1Type).arg(word1->getlemma())
                      .arg(word2Type).arg(word2->getlemma());
    }

    mistakes.insert(Mistake(message));
    return false;
}

bool MainAuxAgreement::check(const UDNode* auxVerb,const UDNode* mainVerb,QSet<Mistake>& mistakes )
{
    // Проверка на нулевые указатели
    if (!auxVerb) {
        throw QString("Auxiliary verb node pointer is null");
    }
    if (!mainVerb) {
        throw QString("Main verb node pointer is null");
    }

    // Проверка допустимых частей речи
    const QSet<PosTag> allowedAuxTags = {MD, VBP, VBZ, VBD, VB, VBN};
    const QSet<PosTag> allowedMainTags = {VB, VBZ, VBD, VBG, VBN};

    if (!allowedAuxTags.contains(auxVerb->getUpos())) {
        throw QString("Invalid auxiliary verb part of speech");
    }
    if (!allowedMainTags.contains(mainVerb->getUpos())) {
        throw QString("Invalid main verb part of speech");
    }

    QString auxLemma = auxVerb->getlemma().toLower();
    PosTag auxTag = auxVerb->getUpos();
    PosTag mainTag = mainVerb->getUpos();

    // 1. Проверка сокращенной формы 'd (would/had)
    if (auxLemma == "'d") {
        if (auxTag == MD) { // would
            if (mainTag == VB) {
                return true;
            } else {
                mistakes.insert(Mistake(QString("Глагол %1 несогласован по времени с вспомогательным глаголом 'd").arg(mainVerb->getlemma())));
                return false;
            }
        } else if (auxTag == VBD) { // had
            if (mainTag == VBN) {
                return true;
            } else {
                mistakes.insert(Mistake(QString("Глагол %1 несогласован по времени с вспомогательным глаголом 'd").arg(mainVerb->getlemma())));
                return false;
            }
        }
    }

    // 2. Проверка модальных глаголов и форм do
    if (auxTag == MD || auxLemma == "do" || auxLemma == "does" || auxLemma == "did") {
        if (mainTag == VB) {
            return true;
        } else {
            mistakes.insert(Mistake(QString("Глагол %1 несогласован по времени с вспомогательным глаголом %2")
                                        .arg(mainVerb->getlemma())
                                        .arg(auxVerb->getlemma())));
            return false;
        }
    }

    // 3. Проверка форм have
    if (auxLemma == "have" || auxLemma == "has" || auxLemma == "had") {
        if (mainTag == VBN) {
            return true;
        } else {
            mistakes.insert(Mistake(QString("Глагол %1 несогласован по времени с вспомогательным глаголом %2")
                                        .arg(mainVerb->getlemma())
                                        .arg(auxVerb->getlemma())));
            return false;
        }
    }

    // 4. Проверка форм be
    if (auxLemma == "be" || auxLemma == "am" || auxLemma == "is" || auxLemma == "are" ||
        auxLemma == "was" || auxLemma == "were" || auxLemma == "been") {
        if (mainTag == VBG) {
            return true;
        } else {
            mistakes.insert(Mistake(QString("Глагол %1 несогласован по времени с вспомогательным глаголом %2")
                                        .arg(mainVerb->getlemma())
                                        .arg(auxVerb->getlemma())));
            return false;
        }
    }

    // Если вспомогательный глагол не распознан
    throw QString("Invalid auxiliary verb type");
};

bool AuxAuxAgreement::check(const UDNode* auxVerb,const UDNode* mainAuxVerb,QSet<Mistake>& mistakes )
{
    // Проверка на нулевые указатели
    if (!auxVerb) {
        throw QString("Auxiliary verb node pointer is null");
    }
    if (!mainAuxVerb) {
        throw QString("Main auxiliary verb node pointer is null");
    }

    // Проверка допустимых частей речи
    const QSet<PosTag> allowedAuxTags = {MD, VBP, VBZ, VBD, VB, VBN};
    const QSet<PosTag> allowedMainAuxTags = {VB, VBZ, VBP, VBD, VBN};

    if (!allowedAuxTags.contains(auxVerb->getUpos())) {
        throw QString("Invalid auxiliary verb part of speech");
    }
    if (!allowedMainAuxTags.contains(mainAuxVerb->getUpos())) {
        throw QString("Invalid main auxiliary verb part of speech");
    }
    if (!(mainAuxVerb->isBeForm() || mainAuxVerb->isHaveForm()))
    {
        throw QString("Invalid main auxiliary verb type");
    }
    QString auxLemma = auxVerb->getlemma().toLower();
    QString mainAuxLemma = mainAuxVerb->getlemma().toLower();
    PosTag auxTag = auxVerb->getUpos();

    // 1. Обработка модальных глаголов (will/would/should/can/may/must и др.)
    if (auxTag == MD) {
        // Специальная обработка сокращенной формы 'd (would/had)
        if (auxLemma == "'d") {
            if (mainAuxLemma == "have" || mainAuxLemma == "be" || mainAuxLemma == "been") {
                return true;
            } else {
                mistakes.insert(Mistake("неправильная форма глагола " + mainAuxVerb->getlemma() +
                                        ". После would/have может быть been/be/have"));
                return false;
            }
        }

        // Обычные модальные глаголы
        if (mainAuxLemma == "have" || mainAuxLemma == "be") {
            return true;
        } else {
            mistakes.insert(Mistake("Глагол " + mainAuxVerb->getlemma() +
                                    " несогласован по времени с модальным глаголом " +
                                    auxVerb->getlemma() +
                                    ". После модального глагола следует в начальной форме"));
            return false;
        }
    }

    // 2. Обработка форм have (have/has/had/'ve)
    if (auxLemma == "have" || auxLemma == "has" || auxLemma == "had" || auxLemma == "'ve" || auxLemma == "'d") {
        if (mainAuxLemma == "been") {
            return true;
        } else {
            mistakes.insert(Mistake("Глагол " + mainAuxVerb->getlemma() +
                                    " несогласован по времени с вспомогательным глаголом " +
                                    auxVerb->getlemma() +
                                    ". Требуется форма Past Participle/V3(been)."));
            return false;
        }
    }

    // Если вспомогательный глагол не распознан
    throw QString("Invalid auxiliary verb type");
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
