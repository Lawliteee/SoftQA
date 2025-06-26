#include "grammarrule.h"
#include "QDebug"

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

    if (verb->hasChildWithRel(Aux))
    {
        return true;
    }

    // 1. Проверка наклонения subjunctive
    if (verb->getMood() == Subj) {
        if (verb->getUpos() == VBP) {
            return true;
        } else {
            mistakes.insert(Mistake("Глагол " + verb->getlemma() + " в неправильной форме. В Present Subjunctive используется базовая форма", nsubj->getId(), verb->getId()));
            return false;
        }
    }

    // Проверка для будущего времени (особый случай с have)
    if (verb->isHaveForm() && hasSibling(verb, "will")) {
        if (verb->getlemma() != "have") {
            mistakes.insert(Mistake("Неправильная форма глагола have. В будущем времени употребляется только форма have", nsubj->getId(), verb->getId()));
            return false;
        }
        return true;
    }

    if (nsubj->getlemma() == "I" && verb->isBeForm())
    {
        if (verb->getlemma().toLower() != "am")
        {
            QString message = "Вспомогательный глагол " + verb->getlemma() + " не согласован с подлежащим " +
                              nsubj->getlemma() + ". Глагол должен быть в форме 1-го лица ед. числа";;
            mistakes.insert(Mistake(message, nsubj->getId(), verb->getId()));
            return false;
        }
        else return true;
    }

    // 2. Проверка 3 лица единственного числа
    if (nsubj->getPerson() == 3 && nsubj->getNumber() == 1) {
        if (verb->getUpos() == VBZ) {
            return true;
        } else {
            QString message;
            if (verb->isBeForm() || verb->isHaveForm() || verb->isDoForm()) {
                message = "Вспомогательный глагол " + verb->getlemma() + " не согласован с подлежащим " +
                          nsubj->getlemma() + ". Глагол должен быть в форме 3-го лица ед. числа";
            } else {
                message = "Основной глагол " + verb->getlemma() + " не согласован с подлежащим " +
                          nsubj->getlemma() + ". Глагол должен быть в форме 3-го лица ед. числа";
            }
            mistakes.insert(Mistake(message, nsubj->getId(), verb->getId()));
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
                message = "Вспомогательный глагол " + verb->getlemma() + " не согласован с подлежащим " +
                          nsubj->getlemma() + ". Глагол должен быть в начальной форме";
            } else {
                message = "Основной глагол " + verb->getlemma() + " не согласован с подлежащим " +
                          nsubj->getlemma() + ". Глагол должен быть в начальной форме";
            }
            mistakes.insert(Mistake(message, nsubj->getId(), verb->getId()));
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
    const QSet<PosTag> allowedTagsWord1 = {PRP, NN, NNS, NNP, NNPS, DT, CD, JJ};
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

    QString word1Lemma = word1->getlemma().toLower();
    QString word2Lemma = word2->getlemma().toLower();

    //если тег JJ, допустимы только квантификаторы
    const QSet<QString> quantifiers = {"some", "any", "many", "few", "several", "much", "little"};
    if (word1->getUpos() == JJ) {
        if (!quantifiers.contains(word1Lemma)) {
            throw QString("Invalid part of speech");
        }
    }

    // Особый случай для Subjunctive mood (were для всех лиц)
    if (word2->getMood() == Subj) {
        if (word2->getlemma().toLower() == "were") {
            return true;
        } else {
            mistakes.insert(Mistake("глагол " + word2->getlemma() +
                                    " не используется в Past Subjunctive. "
                                    "Используется were для всех лиц (даже с he/she/it).", word1->getId(),word2->getId()));
            return false;
        }
    }

    // Проверка согласованности
    if (num1 == 0 || num2 == 0 || num1 == num2) {
        return true;
    }


    // Обработка особых случаев для указательных местоимений и квантификаторов
    // Указательные местоимения (this/that/these/those)
    if (word1Lemma == "this" || word1Lemma == "that" ||
        word1Lemma == "these" || word1Lemma == "those") {

        QString correctForm = (num2 == 1) ? "this/that" : "these/those";
        QString currentForm = (word1Lemma == "this" || word1Lemma == "that") ? "this/that" : "these/those";

        if (currentForm != correctForm) {
            QString message = QString("Указательное местоимение %1 не согласовано по числу с существительным %2. "
                                      "Указательное местоимение должно быть в форме %3.")
                                  .arg(word1->getlemma())
                                  .arg(word2->getlemma())
                                  .arg(correctForm);
            mistakes.insert(Mistake(message, word1->getId(),word2->getId()));
            return false;
        }
    }

    // Квантификаторы (some/any/many/few/several/much/little)
    if (quantifiers.contains(word1Lemma)) {
        if (num2 != 2) { // Квантификаторы требуют множественного числа
            QString message = QString("Квантификатор %1 некорректно использован. "
                                      "Нельзя использовать с существительным %2 в единственном числе")
                                  .arg(word1->getlemma())
                                  .arg(word2->getlemma());
            mistakes.insert(Mistake(message, word1->getId(),word2->getId()));
            return false;
        }
    }

    // Обработка артиклей (a/an)
    if (word1->getUpos() == DT && (word1Lemma == "a" || word1Lemma == "an")) {
        if (num2 != 1) { // Артикли a/an требуют единственного числа
            QString message = QString("Артикль %1 не согласован по числу с существительным %2. "
                                      "Требуется артикль the или нулевой артикль.")
                                  .arg(word1->getlemma())
                                  .arg(word2->getlemma());
            mistakes.insert(Mistake(message, word1->getId(),word2->getId()));
            return false;
        }
    }

    // Обработка указательных местоимений (this/that/these/those)
    if (word1Lemma == "this" || word1Lemma == "that" ||
        word1Lemma == "these" || word1Lemma == "those") {

        QString correctForm = (num2 == 1) ? "this/that" : "these/those";
        QString currentForm = (word1Lemma == "this" || word1Lemma == "that") ? "this/that" : "these/those";

        if (currentForm != correctForm) {
            QString message = QString("Указательное местоимение %1 не согласовано по числу с существительным %2. "
                                      "Указательное местоимение должно быть в форме %3.")
                                  .arg(word1->getlemma())
                                  .arg(word2->getlemma())
                                  .arg(correctForm);
            mistakes.insert(Mistake(message, word1->getId(),word2->getId()));
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
            message = QString("глагол %1 не согласован с подлежащим %2. "
                              "Глагол должен быть в форме единственного числа.")
                          .arg(word2->getlemma()).arg(word1->getlemma());
        } else {
            message = QString("глагол %1 не согласован с подлежащим %2. "
                              "Глагол должен быть в форме множественного числа.")
                          .arg(word2->getlemma()).arg(word1->getlemma());
        }
    } else {
        message = QString("%1 %2 не согласовано по числу с %3 %4.")
                      .arg(word1Type).arg(word1->getlemma())
                      .arg(word2Type).arg(word2->getlemma());
    }

    if (message != "")
    {
        mistakes.insert(Mistake(message, word1->getId(),word2->getId()));
        return false;
    }
    else return true;
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
    const QSet<PosTag> allowedMainTags = {VB, VBZ, VBD, VBG, VBN, VBP};

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
                mistakes.insert(Mistake(QString("Глагол %1 не согласован по времени с вспомогательным глаголом 'd").arg(mainVerb->getlemma()), auxVerb->getId(),mainVerb->getId()));
                return false;
            }
        } else if (auxTag == VBD) { // had
            if (mainTag == VBN) {
                return true;
            } else {
                mistakes.insert(Mistake(QString("Глагол %1 не согласован по времени с вспомогательным глаголом 'd").arg(mainVerb->getlemma()), auxVerb->getId(),mainVerb->getId()));
                return false;
            }
        }
    }

    // 2. Проверка модальных глаголов и форм do
    if (auxTag == MD || auxLemma == "do" || auxLemma == "does" || auxLemma == "did") {
        if (mainTag == VB) {
            return true;
        } else {
            mistakes.insert(Mistake(QString("Глагол %1 не согласован по времени с вспомогательным глаголом %2. Требуется начальная форма глагола")
                                        .arg(mainVerb->getlemma())
                                        .arg(auxVerb->getlemma()), auxVerb->getId(),mainVerb->getId()));
            return false;
        }
    }

    // 3. Проверка форм have
    if (auxLemma == "have" || auxLemma == "has" || auxLemma == "had") {
        if (mainTag == VBN) {
            return true;
        } else {
            mistakes.insert(Mistake(QString("Глагол %1 не согласован по времени с вспомогательным глаголом %2")
                                        .arg(mainVerb->getlemma())
                                        .arg(auxVerb->getlemma()), auxVerb->getId(),mainVerb->getId()));
            return false;
        }
    }

    // 4. Проверка форм be
    if (auxLemma == "be" || auxLemma == "am" || auxLemma == "is" || auxLemma == "are" ||
        auxLemma == "was" || auxLemma == "were" || auxLemma == "been") {
        if (mainTag == VBG) {
            return true;
        } else {
            mistakes.insert(Mistake(QString("Глагол %1 не согласован по времени с вспомогательным глаголом %2")
                                        .arg(mainVerb->getlemma())
                                        .arg(auxVerb->getlemma()), auxVerb->getId(),mainVerb->getId()));
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
                                        ". После would/have может быть been/be/have", auxVerb->getId(),mainAuxVerb->getId()));
                return false;
            }
        }

        // Обычные модальные глаголы
        if (mainAuxLemma == "have" || mainAuxLemma == "be") {
            return true;
        } else {
            mistakes.insert(Mistake("Глагол " + mainAuxVerb->getlemma() +
                                    " не согласован по времени с модальным глаголом " +
                                    auxVerb->getlemma() +
                                    ". После модального глагола следует в начальной форме", auxVerb->getId(),mainAuxVerb->getId()));
            return false;
        }
    }

    // 2. Обработка форм have (have/has/had/'ve)
    if (auxLemma == "have" || auxLemma == "has" || auxLemma == "had" || auxLemma == "'ve" || auxLemma == "'d") {
        if (mainAuxLemma == "been") {
            return true;
        } else {
            mistakes.insert(Mistake("Глагол " + mainAuxVerb->getlemma() +
                                    " не согласован по времени с вспомогательным глаголом " +
                                    auxVerb->getlemma() +
                                    ". Требуется форма Past Participle/V3(been).", auxVerb->getId(),mainAuxVerb->getId()));
            return false;
        }
    }

    // Если вспомогательный глагол не распознан
    throw QString("Invalid auxiliary verb type");
};

bool PassiveAgreement::check(const UDNode* auxVerb, const UDNode* mainVerb, QSet<Mistake>& mistakes)
{
    // 1. Проверка нулевых указателей
    if (!auxVerb) throw QString("Auxiliary verb node pointer is null");
    if (!mainVerb) throw QString("Main verb node pointer is null");

    // 2. Проверка типа связи вспомогательного глагола
    if (auxVerb->getDepRel() != Aux_Pass) {
        throw QString("Invalid auxiliary verb relation (should be aux:pass)");
    }

    // 3. Основной глагол должен быть причастием прошедшего времени (VBN)
    if (mainVerb->getUpos() != VBN) {
        mistakes.insert(Mistake("Глаголы при построении пассивного залога (Passive Voice) не согласованы", auxVerb->getId(),mainVerb->getId()));
        return false;
    }

    const QString auxLemma = auxVerb->getlemma().toLower();
    const PosTag auxPos = auxVerb->getUpos();

    // 4. Проверка всех допустимых конструкций пассивного залога

    // Конструкция 1: (am/is/are/was/were) + VBN
    if (auxLemma == "am" || auxLemma == "is" || auxLemma == "are" || auxLemma == "was" || auxLemma == "were") {
        if (mainVerb->hasChildWithRel(Aux))
        {
            mistakes.insert(Mistake("Глаголы при построении пассивного залога (Passive Voice) не согласованы", auxVerb->getId(),mainVerb->getId()));
            return false;
        }
        else return true;
    }

    // Конструкция 2: been (после have/has/had) + VBN
    if (auxLemma == "been" && auxPos == VBN) {
        // Ищем have/has/had среди родителей
        for (const UDNode* child : mainVerb->getChildren()) {
            const QString childLemma = child->getlemma().toLower();
            if ((childLemma == "have" || childLemma == "has" || childLemma == "had") &&
                child->getDepRel() == Aux) {
                return true;
            }
        }
        mistakes.insert(Mistake("Глаголы при построении пассивного залога (Passive Voice) не согласованы", auxVerb->getId(),mainVerb->getId()));
        return false;
    }

    // Конструкция 3: being (после am/is/are/was/were) + VBN
    if (auxLemma == "being" && auxPos == VBG) {
        // Ищем am/is/are/was/were среди родителей
        for (const UDNode* child : mainVerb->getChildren()) {
            const QString childLemma = child->getlemma().toLower();
            if ((childLemma == "am" || childLemma == "is" || childLemma == "are" ||
                 childLemma == "was" || childLemma == "were") &&
                child->getDepRel() == Aux) {
                return true;
            }
        }
        mistakes.insert(Mistake("Глаголы при построении пассивного залога (Passive Voice) не согласованы", auxVerb->getId(),mainVerb->getId()));
        return false;
    }

    // Конструкция 4: be (после модальных глаголов) + VBN
    if (auxLemma == "be" && auxPos == VB) {
        // Ищем модальный глагол среди родителей
        for (const UDNode* child : mainVerb->getChildren()) {
            if (child->getDepRel() == Aux && child->isModalVerb()) {
                return true;
            }
        }
        mistakes.insert(Mistake("Глаголы при построении пассивного залога (Passive Voice) не согласованы", auxVerb->getId(),mainVerb->getId()));
        return false;
    }

    // Если ни одна конструкция не подошла
    mistakes.insert(Mistake("Глаголы при построении пассивного залога (Passive Voice) не согласованы", auxVerb->getId(),mainVerb->getId()));
    return false;
}

bool ComplexSentenceAgreement::check(const UDNode* depVerb,const UDNode* mainVerb,QSet<Mistake>& mistakes )
{
    // 1. Проверка нулевых указателей
    if (!depVerb) throw QString("Dependent verb node pointer is null");
    if (!mainVerb) throw QString("Main verb node pointer is null");

    // 2. Проверка типа связи (должна быть advcl или acl:relcl)
    if (depVerb->getDepRel() != Advcl && depVerb->getDepRel() != Acl_Relcl) {
        throw QString("Invalid clause relation");
    }

    // 1. Если главная часть в настоящем времени
    if (isPresentClause(mainVerb)) {
        return true;
    }
    // 2. Если главная часть в будущем времени
    else if (isFutureClause(mainVerb)) {
        // 2.1. Проверяем временные/условные союзы
        if (depVerb->hasTemporalConditionalConjunction()) {
            // 2.1.1. Придаточная должна быть в настоящем
            if (isPresentClause(depVerb)) {
                return true;
            }
            else {
                mistakes.insert(Mistake("Придаточная часть не согласована с главной по времени. Если главное предложение стоит в будущем времени, а придаточное начинается с условного или временного союза, то в нем используется одно из настоящих времен.", depVerb->getId(),mainVerb->getId()));
                return false;
            }
        }
        // 2.2. Для других случаев - согласование верное
        return true;
    }
    // 3. Если главная часть в прошедшем времени
    else if (isPastClause(mainVerb)) {
        // 3.1. Придаточная должна быть в прошедшем
        if (isPastClause(depVerb)) {
            return true;
        }
        else {
            mistakes.insert(Mistake("Придаточная часть не согласована с главной по времени. Если главное предложение стоит в прошедшем времени, то и придаточное будет стоять в одном из прошедших времен, если речь не идет о непреложной истине, о фактах.", depVerb->getId(),mainVerb->getId()));
            return false;
        }
    }

    // 4. Для других случаев (например, повелительное наклонение) - согласование верное
    return true;
};

bool ConditionalsAgreement::check(const UDNode* depVerb,const UDNode* mainVerb,QSet<Mistake>& mistakes )
{
    // 1. Проверка нулевых указателей
    if (!depVerb) throw QString("node pointer is null");
    if (!mainVerb) throw QString("node pointer is null");

    // 2. Проверка типа связи (должна быть advcl)
    if (depVerb->getDepRel() != Advcl) {
        throw QString("Invalid clause relation");
    }

    // 3. Проверка 2-го типа условных предложений
    if (isSecondConditional(mainVerb)) {
        // 3.1. Придаточная должна быть в Past
        if (isPastClause(depVerb)) {
            return true;
        }
        else {
            mistakes.insert(Mistake("придаточная часть не согласована с главной по времени. Во втором типе условных предложений(Second Conditional) следующая формула: If + Past Simple, would + V1.", depVerb->getId(),mainVerb->getId()));
            return false;
        }
    }
    // 4. Проверка 3-го типа условных предложений
    else if (isThirdConditional(mainVerb)) {
        // 4.1. Придаточная должна быть в Past
        if (isPastClause(depVerb)) {
            return true;
        }
        else {
            mistakes.insert(Mistake("придаточная часть не согласована с главной по времени. В третьем типе условных предложений(Third Conditional) следующая формула: If + Past Perfect, would have + V3.", depVerb->getId(),mainVerb->getId()));
            return false;
        }
    }

    // 5. Если главная часть не соответствует ни 2-му, ни 3-му типу
    throw QString("Invalid main clause");
};

bool GrammarRule::isFutureClause(const UDNode* main) const
{
    if (!main) return false;

    // Случай 1: will/shall как вспомогательный глагол (обычный случай)
    for (UDNode* child : main->getChildren()) {
        if (isFutureAuxiliary(child)) {
            return true;
        }
    }

    // Случай 2: will/shall как основной глагол (редкий, но возможный случай)
    if (isFutureMainVerb(main)) {
        return true;
    }

    return false;
}

bool GrammarRule::isFutureAuxiliary(const UDNode* node) const
{
    if (!node) return false;

    QString lemma = node->getlemma().toLower();
    return (lemma == "will" || lemma == "shall" || lemma == "'ll" ) &&
           node->getUpos() == MD &&
           node->getDepRel() == Aux;
}

bool GrammarRule::isFutureMainVerb(const UDNode* node) const
{
    if (!node) return false;

    QString lemma = node->getlemma().toLower();
    return (lemma == "will" || lemma == "shall") &&
           node->getUpos() == MD &&
           hasChildWithPos(node, VB);  // Должен управлять инфинитивом
}

bool GrammarRule::hasChildWithPos(const UDNode* node, PosTag pos) const
{
    if (!node) return false;

    for (UDNode* child : node->getChildren()) {
        if (child->getUpos() == pos) {
            return true;
        }
    }
    return false;
}

bool GrammarRule::isPresentClause(const UDNode* main) const {
    return main && main->isPresentTense();
}

bool GrammarRule::isPastClause(const UDNode* main) const {
    return main && main->isPastTense();
}

bool GenderAgreement::check(const UDNode* word1,const UDNode* word2,QSet<Mistake>& mistakes )
{
    QString word1Lemma = word1->getlemma().toLower();
    QString word2Lemma = word2->getlemma().toLower();
    QSet <QString> male = {"himself"};
    QSet <QString> female = {"herself"};
    QSet <QString> itMale = {"itself"};


    if (word1Lemma == "he")
    {
        if (!male.contains(word2Lemma))
        {
            mistakes.insert(Mistake("Местоимение " + word2Lemma +
                                        " не согласованно по роду с местоимением " + word1Lemma, word1->getId(),word2->getId()));
            return false;
        }
        else return true;
    }
    else if (word1Lemma == "she")
    {
        if (!female.contains(word2Lemma))
        {
            mistakes.insert(Mistake("Местоимение " + word2Lemma +
                                        " не согласованно по роду с местоимением " + word1Lemma, word1->getId(),word2->getId()));
            return false;
        }
        else return true;
    }
    else if (word1Lemma == "it")
    {
        if (!itMale.contains(word2Lemma))
        {
            mistakes.insert(Mistake("Местоимение " + word2Lemma +
                                        " не согласованно по роду с местоимением " + word1Lemma, word1->getId(),word2->getId()));
            return false;
        }
        else return true;
    }



}
