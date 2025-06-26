#include "pattern.h"
#include <QBitArray>
Pattern::Pattern() {}

/*!
* \brief Функция проверки соответствия узла шаблону
* \param [in] node - проверяемый узел дерева
* \param [in] usedChildren - множество уже использованных дочерних узлов
* \return True – узел соответствует шаблону, false – нет
*/
bool Pattern::matchesPattern(const UDNode* node, QSet<const UDNode*>& usedChildren)
{
    // 1. Проверка леммы и тега текущего узла
    if (!validWords.isEmpty() && !validWords.contains(node->getlemma().toLower())) return false;
    if (!validTags.isEmpty() && !validTags.contains(node->getUpos())) return false;

    // 2. Группируем все ожидаемые связи
    QMultiMap<DepRel, Pattern*> expectedChildren = children;

    // 3. Проверяем наличие ВСЕХ требуемых детей
    for (auto relIt = expectedChildren.constBegin(); relIt != expectedChildren.constEnd(); )
    {
        DepRel currentRel = relIt.key();
        int expectedCount = expectedChildren.count(currentRel);

        // Считаем доступных детей для этой связи
        int availableCount = 0;
        for (UDNode* child : node->getChildren()) {
            if ((currentRel == Other || child->getDepRel() == currentRel) &&
                !usedChildren.contains(child)) {
                availableCount++;
            }
        }

        // Если детей меньше чем требуется - несоответствие
        if (availableCount < expectedCount) return false;

        // Переходим к следующей связи
        while (relIt != expectedChildren.constEnd() && relIt.key() == currentRel) {
            ++relIt;
        }
    }

    // 4. Проверяем соответствие каждого конкретного шаблона
    QSet<const UDNode*> tempUsed = usedChildren;
    for (auto relIt = expectedChildren.constBegin(); relIt != expectedChildren.constEnd(); ++relIt)
    {
        DepRel rel = relIt.key();
        Pattern* childPattern = relIt.value();

        bool found = false;
        for (UDNode* child : node->getChildren()) {
            if ((rel == Other || child->getDepRel() == rel) &&
                !tempUsed.contains(child))
            {
                QSet<const UDNode*> patternUsed = tempUsed;
                if (childPattern->matchesPattern(child, patternUsed)) {
                    tempUsed.unite(patternUsed);
                    tempUsed.insert(child);
                    found = true;
                    break;
                }
            }
        }

        if (!found) return false;
    }

    // 5. Все проверки пройдены - обновляем usedChildren
    usedChildren = tempUsed;
    currentMatch = const_cast<UDNode*>(node);
    return true;
}

void Pattern::compareMatches(const Pattern* expected, QStringList& errors, const QString& path) const
{
    // Проверка на нулевые указатели
    if (!expected) {
        errors.append(QString("Expected pattern is null at %1").arg(path));
        return;
    }


    // Проверка currentMatch текущего узла
    if (!currentMatch && !expected->currentMatch) {
        // Оба nullptr
        return;
    }
    else if (!currentMatch || !expected->currentMatch) {
        // Только один из них nullptr - ошибка
        errors.append(QString("Match mismatch at %1\n  Expected: %2\n  Actual: %3")
                          .arg(path)
                          .arg(expected->currentMatch ? QString::number(expected->currentMatch->getId()) : "null")
                          .arg(currentMatch ? QString::number(currentMatch->getId()) : "null"));
        return;
    }


    for (auto it = children.constBegin(); it != children.constEnd(); ++it)
    {
        const QString relationName = depRelToString(it.key());

        // Получаем все значения для этого ключа
        const QList<Pattern*> allPatterns = children.values(it.key());
        const QList<Pattern*> allExpected = expected->children.values(it.key());

        for (int i = 0; i < allPatterns.size(); ++i)
        {
            const QString childPath = QString("%1[%2](%3)")
            .arg(path)
                .arg(relationName)
                .arg(i);

            allPatterns[i]->compareMatches(
                allExpected[i],
                errors,
                childPath
                );
        }
    }
}

QStringList Pattern::getUncalledChecks(const QString& path) const
{
    QStringList uncalled;
    // Проверяем checks текущего Pattern
    for (RelTypeCheck* check : checks) {
        if (!check->isCalled()) {
            uncalled << QString("%1: uncalled check").arg(path);
        }
    }

    // Рекурсивно проверяем дочерние Pattern
    for (auto it = children.constBegin(); it != children.constEnd(); ++it) {
        const QString relationName = depRelToString(it.key());
        int childIndex = 0;

        // Перебираем все дочерние шаблоны для данной связи
        const auto childPatterns = children.values(it.key());
        for (Pattern* childPattern : childPatterns) {
            const QString childPath = QString("%1[%2](%3)").arg(path).arg(relationName).arg(childIndex++);
            if (childPattern) {
                uncalled << childPattern->getUncalledChecks(childPath);
            } else {
                uncalled << QString("%1: null pattern").arg(childPath);
            }
        }
    }

    return uncalled;
}


void Pattern::check(const UDNode* node, QSet<Mistake>& mistakes) const {

    for (auto it = children.begin(); it != children.end(); ++it)
    {
        Pattern* childPattern = it.value();
        if (!childPattern->checks.isEmpty())
        {
            for (RelTypeCheck* check : childPattern->checks)
            {
                UDNode* relatedNode = nullptr;
                try {
                    check->getNodes(childPattern->currentMatch, &relatedNode, currentMatch);
                    if (relatedNode) {
                        check->callCheck(childPattern->currentMatch, relatedNode, mistakes);
                    }
                } catch (const QString& error) {
                    throw;
                }
            }
        }
        childPattern->check(childPattern->currentMatch, mistakes);
    }
}


void getPatterns(QSet <Pattern*> & s)
{
    // -----------------------------ВСЕ ШАБЛОНЫ ПРАВИЛ----------------------------

    // 1 Настоящее время подлежащее и сказуемое
    Pattern* pat1 = new Pattern({},{VBP,VBZ});
    Pattern* pat1_1 = new Pattern({},{NN,NNS,NNP,NNPS,PRP});
    pat1->addChildPattern(Nsubj,pat1_1);

    ParentChild* check1 = new ParentChild;
    check1->setRule(new PersonNumberAgreement);
    pat1_1->addСheck(check1);
    s.insert(pat1);

    // 2 Настоящее время подлежащее и to be через
    Pattern* pat2 = new Pattern({},{});
    Pattern* pat2_1 = new Pattern({},{NN,NNS,NNP,NNPS,PRP});
    Pattern* pat2_2 = new Pattern({"am","is","are"},{VBP,VBZ});
    pat2->addChildPattern(Nsubj,pat2_1);
    pat2->addChildPattern(Cop,pat2_2);

    ChildChild* check2 = new ChildChild;
    check2->setRule(new PersonNumberAgreement);
    check2->setValidWords({"am","is","are"});
    check2->setRelatedRel(Cop);
    pat2_1->addСheck(check2);
    s.insert(pat2);

    // 3 Прошедшее время подлежащее и сказуемое
    Pattern* pat3 = new Pattern({"was","were"},{VBD});
    Pattern* pat3_1 = new Pattern({},{NN,NNS,NNP,NNPS,PRP});
    pat3->addChildPattern(Nsubj,pat3_1);

    ParentChild* check3 = new ParentChild;
    check3->setRule(new NumberAgreement);
    pat3_1->addСheck(check3);
    s.insert(pat3);

    // 4 Прошедшее время подлежащее и to be через
    Pattern* pat4 = new Pattern({},{});
    Pattern* pat4_1 = new Pattern({},{NN,NNS,NNP,NNPS,PRP});
    Pattern* pat4_2 = new Pattern({"was","were"},{VBD});
    pat4->addChildPattern(Nsubj,pat4_1);
    pat4->addChildPattern(Cop,pat4_2);

    ChildChild* check4 = new ChildChild;
    check4->setRule(new NumberAgreement);
    check4->setValidWords({"was","were"});
    check4->setRelatedRel(Cop);
    pat4_1->addСheck(check4);
    s.insert(pat4);

    // 5 Согласование существительного после числительного
    Pattern* pat5 = new Pattern({},{NN,NNS,NNP,NNPS,PRP});
    Pattern* pat5_1 = new Pattern({},{CD});
    pat5->addChildPattern(Nummod,pat5_1);

    ParentChild* check5 = new ParentChild;
    check5->setRule(new NumberAgreement);
    pat5_1->addСheck(check5);
    s.insert(pat5);

    // 6 	Согласование указательного местоимения по числу с существительным, артикля
    Pattern* pat6 = new Pattern({},{NN,NNS,NNP,NNPS,PRP});
    Pattern* pat6_1 = new Pattern({"a","the","this","these","that","those"},{DT});
    pat6->addChildPattern(Det,pat6_1);

    ParentChild* check6 = new ParentChild;
    check6->setRule(new NumberAgreement);
    pat6_1->addСheck(check6);
    s.insert(pat6);

    // 7 	Согласование квантификаторов с существительным
    Pattern* pat7 = new Pattern({},{NN,NNS,NNP,NNPS});
    Pattern* pat7_1 = new Pattern({"much","many","few","little","several"},{JJ});
    pat7->addChildPattern(Amod,pat7_1);

    ParentChild* check7 = new ParentChild;
    check7->setRule(new NumberAgreement);
    pat7_1->addСheck(check7);
    s.insert(pat7);

    // 8 	Согласование основного глагола с вспомогательным
    Pattern* pat8 = new Pattern({},{VB,VBP,VBZ,VBD,VBN,VBG});
    Pattern* pat8_1 = new Pattern({"do","does","did","have","has","be","am","is","are","was","were","been","will","would"},{});
    pat8->addChildPattern(Aux,pat8_1);

    ParentChild* check8 = new ParentChild;
    check8->setRule(new MainAuxAgreement);
    pat8_1->addСheck(check8);
    s.insert(pat8);

    // 9 Настоящее время подлежащее и вспомогательный
    Pattern* pat9 = new Pattern({},{VB,VBP,VBZ,VBD,VBN,VBG});
    Pattern* pat9_1 = new Pattern({},{NN,NNS,NNP,NNPS,PRP});
    Pattern* pat9_2 = new Pattern({"have","has","am","is","are","do","does" },{VBP,VBZ});
    pat9->addChildPattern(Nsubj,pat9_1);
    pat9->addChildPattern(Aux,pat9_2);

    ChildChild* check9 = new ChildChild;
    check9->setRule(new PersonNumberAgreement);
    check9->setValidWords({"have","has","am","is","are","do","does" });
    check9->setValidTags({VBP,VBZ});
    check9->setRelatedRel(Aux);
    pat9_1->addСheck(check9);
    s.insert(pat9);

    // 10 Согласование 2х Вспомогательных
    Pattern* pat10 = new Pattern({},{VB,VBP,VBZ,VBD,VBN,VBG});
    Pattern* pat10_1 = new Pattern({"have","has","had"},{});
    Pattern* pat10_2 = new Pattern({"be","am","is","are","was","were","been"},{});
    pat10->addChildPattern(Aux,pat10_1);
    pat10->addChildPattern(Aux,pat10_2);

    ChildChild* check10 = new ChildChild;
    check10->setRule(new AuxAuxAgreement);
    check10->setValidWords({"be","am","is","are","was","were","been"});
    check10->setValidTags({});
    check10->setRelatedRel(Aux);
    pat10_1->addСheck(check10);
    s.insert(pat10);

    // 10 Согласование 2х Вспомогательных
    Pattern* pat120 = new Pattern({},{VB,VBP,VBZ,VBD,VBN,VBG});
    Pattern* pat120_1 = new Pattern({"will","would", "'d"},{MD});
    Pattern* pat120_2 = new Pattern({"have","has","had"},{});
    pat120->addChildPattern(Aux,pat120_1);
    pat120->addChildPattern(Aux,pat120_2);

    ChildChild* check120 = new ChildChild;
    check120->setRule(new AuxAuxAgreement);
    check120->setValidWords({"have","has","had","'d" });
    check120->setValidTags({});
    check120->setRelatedRel(Aux);
    pat120_1->addСheck(check120);
    s.insert(pat120);

    // 11 Настоящее в Passive подлежащее и вспомогательный
    Pattern* pat11 = new Pattern({},{VB,VBP,VBZ,VBD,VBN,VBG});
    Pattern* pat11_1 = new Pattern({},{NN,NNS,NNP,NNPS,PRP});
    Pattern* pat11_2 = new Pattern({"am","is","are"},{VBP,VBZ});
    pat11->addChildPattern(Nsubj_Pass,pat11_1);
    pat11->addChildPattern(Aux_Pass,pat11_2);

    ChildChild* check11 = new ChildChild;
    check11->setRule(new PersonNumberAgreement);
    check11->setValidWords({"am","is","are"});
    check11->setValidTags({VBP,VBZ});
    check11->setRelatedRel(Aux_Pass);
    pat11_1->addСheck(check11);
    s.insert(pat11);

    // 12 Прошедшее в Passive подлежащее и вспомогательный
    Pattern* pat12 = new Pattern({},{VB,VBP,VBZ,VBD,VBN,VBG});
    Pattern* pat12_1 = new Pattern({},{NN,NNS,NNP,NNPS,PRP});
    Pattern* pat12_2 = new Pattern({"was","were"},{VBD});
    pat12->addChildPattern(Nsubj_Pass,pat12_1);
    pat12->addChildPattern(Aux_Pass,pat12_2);

    ChildChild* check12 = new ChildChild;
    check12->setRule(new NumberAgreement);
    check12->setValidWords({"was","were"});
    check12->setValidTags({VBD});
    check12->setRelatedRel(Aux_Pass);
    pat12_1->addСheck(check12);
    s.insert(pat12);

    // 13 Согласование в Passive
    Pattern* pat13 = new Pattern({},{VB,VBP,VBZ,VBD,VBN,VBG});
    Pattern* pat13_1 = new Pattern({},{});
    pat13->addChildPattern(Aux_Pass,pat13_1);

    ParentChild* check13 = new ParentChild;
    check13->setRule(new PassiveAgreement);
    pat13_1->addСheck(check13);
    s.insert(pat13);

    // 14 Согласование в complex advcl
    Pattern* pat14 = new Pattern({},{});
    Pattern* pat14_1 = new Pattern({},{});
    pat14->addChildPattern(Advcl,pat14_1);

    ParentChild* check14 = new ParentChild;
    check14->setRule(new ComplexSentenceAgreement);
    pat14_1->addСheck(check14);
    s.insert(pat14);

    // 15 Согласование в complex acl:relcl
    Pattern* pat15 = new Pattern({},{VB,VBP,VBZ,VBD,VBN,VBG});
    Pattern* pat15_1 = new Pattern({},{});
    pat15->addChildPattern(Acl_Relcl,pat15_1);

    ParentChild* check15 = new ParentChild;
    check15->setRule(new ComplexSentenceAgreement);
    pat15_1->addСheck(check15);
    s.insert(pat15);

    // 16 Conditionals 2-3
    Pattern* pat16 = new Pattern({},{VB});
    Pattern* pat16_1 = new Pattern({},{});
    Pattern* pat16_2 = new Pattern({"would"},{});
    pat16->addChildPattern(Advcl,pat16_1);
    pat16->addChildPattern(Aux,pat16_2);

    ParentChild* check16 = new ParentChild;
    check16->setRule(new ConditionalsAgreement);
    pat16_1->addСheck(check16);
    s.insert(pat16);

    // 17 Собственные местоимения
    Pattern* pat17 = new Pattern({},{});
    Pattern* pat17_1 = new Pattern({"he","she","it"},{});
    Pattern* pat17_2 = new Pattern({"himself","herself","itself"},{});
    pat17->addChildPattern(Nsubj,pat17_1);
    pat17->addChildPattern(Obl,pat17_2);

    ChildChild* check17 = new ChildChild;
    GrammarRule* rule17 = new GenderAgreement;
    check17->setValidWords({"himself","herself","itself"});
    check17->setValidTags({PRP});
    check17->setRelatedRel(Obl);
    check17->setRule(rule17);
    pat17_1->addСheck(check17);
    s.insert(pat17);
}


void getPatternsNew(QSet <Pattern*> & s, const QMap<int, UDNode*>& nodes)
{
    // -----------------------------ВСЕ ШАБЛОНЫ ПРАВИЛ----------------------------

    // -----------------------------ВСЕ ШАБЛОНЫ ПРАВИЛ----------------------------

    // 1 Настоящее время подлежащее и сказуемое
    Pattern* pat1 = new Pattern({},{VBP,VBZ});
    Pattern* pat1_1 = new Pattern({},{NN,NNS,NNP,NNPS,PRP});
    pat1->addChildPattern(Nsubj,pat1_1);

    ParentChild* check1 = new ParentChild;
    GrammarRule* rule1 = new PersonNumberAgreement;
    rule1->writeNodes(nodes);
    check1->setRule(rule1);
    pat1_1->addСheck(check1);
    s.insert(pat1);

    // 2 Настоящее время подлежащее и to be через
    Pattern* pat2 = new Pattern({},{});
    Pattern* pat2_1 = new Pattern({},{NN,NNS,NNP,NNPS,PRP});
    Pattern* pat2_2 = new Pattern({"am","is","are"},{VBP,VBZ});
    pat2->addChildPattern(Nsubj,pat2_1);
    pat2->addChildPattern(Cop,pat2_2);

    ChildChild* check2 = new ChildChild;
    GrammarRule* rule2 = new PersonNumberAgreement;
    rule2->writeNodes(nodes);
    check2->setRule(rule2);
    check2->setValidWords({"am","is","are"});
    check2->setRelatedRel(Cop);
    pat2_1->addСheck(check2);
    s.insert(pat2);

    // 3 Прошедшее время подлежащее и сказуемое
    Pattern* pat3 = new Pattern({"was","were"},{VBD});
    Pattern* pat3_1 = new Pattern({},{NN,NNS,NNP,NNPS,PRP});
    pat3->addChildPattern(Nsubj,pat3_1);

    ParentChild* check3 = new ParentChild;
    GrammarRule* rule3 = new NumberAgreement;
    rule3->writeNodes(nodes);
    check3->setRule(rule3);
    pat3_1->addСheck(check3);
    s.insert(pat3);

    // 4 Прошедшее время подлежащее и to be через
    Pattern* pat4 = new Pattern({},{});
    Pattern* pat4_1 = new Pattern({},{NN,NNS,NNP,NNPS,PRP});
    Pattern* pat4_2 = new Pattern({"was","were"},{VBD});
    pat4->addChildPattern(Nsubj,pat4_1);
    pat4->addChildPattern(Cop,pat4_2);

    ChildChild* check4 = new ChildChild;
    GrammarRule* rule4 = new NumberAgreement;
    rule4->writeNodes(nodes);
    check4->setRule(rule4);
    check4->setValidWords({"was","were"});
    check4->setRelatedRel(Cop);
    pat4_1->addСheck(check4);
    s.insert(pat4);

    // 5 Согласование существительного после числительного
    Pattern* pat5 = new Pattern({},{NN,NNS,NNP,NNPS,PRP});
    Pattern* pat5_1 = new Pattern({},{CD});
    pat5->addChildPattern(Nummod,pat5_1);

    ParentChild* check5 = new ParentChild;
    GrammarRule* rule5 = new NumberAgreement;
    rule5->writeNodes(nodes);
    check5->setRule(rule5);
    pat5_1->addСheck(check5);
    s.insert(pat5);

    // 6 Согласование указательного местоимения по числу с существительным, артикля
    Pattern* pat6 = new Pattern({},{NN,NNS,NNP,NNPS,PRP});
    Pattern* pat6_1 = new Pattern({"a","the","this","these","that","those"},{DT});
    pat6->addChildPattern(Det,pat6_1);

    ParentChild* check6 = new ParentChild;
    GrammarRule* rule6 = new NumberAgreement;
    rule6->writeNodes(nodes);
    check6->setRule(rule6);
    pat6_1->addСheck(check6);
    s.insert(pat6);

    // 7 Согласование квантификаторов с существительным
    Pattern* pat7 = new Pattern({},{NN,NNS,NNP,NNPS});
    Pattern* pat7_1 = new Pattern({"much","many","few","little","several"},{JJ});
    pat7->addChildPattern(Amod,pat7_1);

    ParentChild* check7 = new ParentChild;
    GrammarRule* rule7 = new NumberAgreement;
    rule7->writeNodes(nodes);
    check7->setRule(rule7);
    pat7_1->addСheck(check7);
    s.insert(pat7);

    // 8 Согласование основного глагола с вспомогательным
    Pattern* pat8 = new Pattern({},{VB,VBP,VBZ,VBD,VBN,VBG});
    Pattern* pat8_1 = new Pattern({"do","does","did","have","has","be","am","is","are","was","were","been","will","would"},{});
    pat8->addChildPattern(Aux,pat8_1);

    ParentChild* check8 = new ParentChild;
    GrammarRule* rule8 = new MainAuxAgreement;
    rule8->writeNodes(nodes);
    check8->setRule(rule8);
    pat8_1->addСheck(check8);
    s.insert(pat8);

    // 9 Настоящее время подлежащее и вспомогательный
    Pattern* pat9 = new Pattern({},{VB,VBP,VBZ,VBD,VBN,VBG});
    Pattern* pat9_1 = new Pattern({},{NN,NNS,NNP,NNPS,PRP});
    Pattern* pat9_2 = new Pattern({"have","has","am","is","are","do","does" },{VBP,VBZ});
    pat9->addChildPattern(Nsubj,pat9_1);
    pat9->addChildPattern(Aux,pat9_2);

    ChildChild* check9 = new ChildChild;
    GrammarRule* rule9 = new PersonNumberAgreement;
    rule9->writeNodes(nodes);
    check9->setRule(rule9);
    check9->setValidWords({"have","has","am","is","are","do","does" });
    check9->setValidTags({VBP,VBZ});
    check9->setRelatedRel(Aux);
    pat9_1->addСheck(check9);
    s.insert(pat9);

    // 10 Согласование 2х Вспомогательных
    Pattern* pat10 = new Pattern({},{VB,VBP,VBZ,VBD,VBN,VBG});
    Pattern* pat10_1 = new Pattern({"have","has","had"},{});
    Pattern* pat10_2 = new Pattern({"be","am","is","are","was","were","been"},{});
    pat10->addChildPattern(Aux,pat10_1);
    pat10->addChildPattern(Aux,pat10_2);

    ChildChild* check10 = new ChildChild;
    GrammarRule* rule10 = new AuxAuxAgreement;
    rule10->writeNodes(nodes);
    check10->setRule(rule10);
    check10->setValidWords({"be","am","is","are","was","were","been"});
    check10->setValidTags({});
    check10->setRelatedRel(Aux);
    pat10_1->addСheck(check10);
    s.insert(pat10);

    // 10 Согласование 2х Вспомогательных
    Pattern* pat120 = new Pattern({},{VB,VBP,VBZ,VBD,VBN,VBG});
    Pattern* pat120_1 = new Pattern({"will","would", "'d"},{MD});
    Pattern* pat120_2 = new Pattern({"have","has","had"},{});
    pat120->addChildPattern(Aux,pat120_1);
    pat120->addChildPattern(Aux,pat120_2);

    ChildChild* check120 = new ChildChild;
    GrammarRule* rule120 = new AuxAuxAgreement;
    rule120->writeNodes(nodes);
    check120->setRule(rule120);
    check120->setValidWords({"have","has","had","'d" });
    check120->setValidTags({});
    check120->setRelatedRel(Aux);
    pat120_1->addСheck(check120);
    s.insert(pat120);

    // 11 Настоящее в Passive подлежащее и вспомогательный
    Pattern* pat11 = new Pattern({},{VB,VBP,VBZ,VBD,VBN,VBG});
    Pattern* pat11_1 = new Pattern({},{NN,NNS,NNP,NNPS,PRP});
    Pattern* pat11_2 = new Pattern({"am","is","are"},{VBP,VBZ});
    pat11->addChildPattern(Nsubj_Pass,pat11_1);
    pat11->addChildPattern(Aux_Pass,pat11_2);

    ChildChild* check11 = new ChildChild;
    GrammarRule* rule11 = new PersonNumberAgreement;
    rule11->writeNodes(nodes);
    check11->setRule(rule11);
    check11->setValidWords({"am","is","are"});
    check11->setValidTags({VBP,VBZ});
    check11->setRelatedRel(Aux_Pass);
    pat11_1->addСheck(check11);
    s.insert(pat11);

    // 12 Прошедшее в Passive подлежащее и вспомогательный
    Pattern* pat12 = new Pattern({},{VB,VBP,VBZ,VBD,VBN,VBG});
    Pattern* pat12_1 = new Pattern({},{NN,NNS,NNP,NNPS,PRP});
    Pattern* pat12_2 = new Pattern({"was","were"},{VBD});
    pat12->addChildPattern(Nsubj_Pass,pat12_1);
    pat12->addChildPattern(Aux_Pass,pat12_2);

    ChildChild* check12 = new ChildChild;
    GrammarRule* rule12 = new NumberAgreement;
    rule12->writeNodes(nodes);
    check12->setRule(rule12);
    check12->setValidWords({"was","were"});
    check12->setValidTags({VBD});
    check12->setRelatedRel(Aux_Pass);
    pat12_1->addСheck(check12);
    s.insert(pat12);

    // 13 Согласование в Passive
    Pattern* pat13 = new Pattern({},{VB,VBP,VBZ,VBD,VBN,VBG});
    Pattern* pat13_1 = new Pattern({},{});
    pat13->addChildPattern(Aux_Pass,pat13_1);

    ParentChild* check13 = new ParentChild;
    GrammarRule* rule13 = new PassiveAgreement;
    rule13->writeNodes(nodes);
    check13->setRule(rule13);
    pat13_1->addСheck(check13);
    s.insert(pat13);

    // 14 Согласование в complex advcl
    Pattern* pat14 = new Pattern({},{});
    Pattern* pat14_1 = new Pattern({},{});
    pat14->addChildPattern(Advcl,pat14_1);

    ParentChild* check14 = new ParentChild;
    GrammarRule* rule14 = new ComplexSentenceAgreement;
    rule14->writeNodes(nodes);
    check14->setRule(rule14);
    pat14_1->addСheck(check14);
    s.insert(pat14);

    // 15 Согласование в complex acl:relcl
    Pattern* pat15 = new Pattern({},{VB,VBP,VBZ,VBD,VBN,VBG});
    Pattern* pat15_1 = new Pattern({},{});
    pat15->addChildPattern(Acl_Relcl,pat15_1);

    ParentChild* check15 = new ParentChild;
    GrammarRule* rule15 = new ComplexSentenceAgreement;
    rule15->writeNodes(nodes);
    check15->setRule(rule15);
    pat15_1->addСheck(check15);
    s.insert(pat15);

    // 16 Conditionals 2-3
    Pattern* pat16 = new Pattern({},{VB});
    Pattern* pat16_1 = new Pattern({},{});
    Pattern* pat16_2 = new Pattern({"would"},{});
    pat16->addChildPattern(Advcl,pat16_1);
    pat16->addChildPattern(Aux,pat16_2);

    ParentChild* check16 = new ParentChild;
    GrammarRule* rule16 = new ConditionalsAgreement;
    rule16->writeNodes(nodes);
    check16->setRule(rule16);
    pat16_1->addСheck(check16);
    s.insert(pat16);

    // 17 Собственные местоимения obl
    Pattern* pat17 = new Pattern({},{});
    Pattern* pat17_1 = new Pattern({"he","she","it"},{});
    Pattern* pat17_2 = new Pattern({"himself","herself","itself"},{});
    pat17->addChildPattern(Nsubj,pat17_1);
    pat17->addChildPattern(Obl,pat17_2);

    ChildChild* check17 = new ChildChild;
    GrammarRule* rule17 = new GenderAgreement;
    rule17->writeNodes(nodes);
    check17->setValidWords({"himself","herself","itself"});
    check17->setRule(rule17);
    check17->setRelatedRel(Obl);
    pat17_1->addСheck(check17);
    s.insert(pat17);

    // 18 Собственные местоимения obj
    Pattern* pat18 = new Pattern({},{});
    Pattern* pat18_1 = new Pattern({"he","she","it"},{});
    Pattern* pat18_2 = new Pattern({"himself","herself","itself"},{});
    pat18->addChildPattern(Nsubj,pat18_1);
    pat18->addChildPattern(Obj,pat18_2);

    ChildChild* check18 = new ChildChild;
    GrammarRule* rule18 = new GenderAgreement;
    rule18->writeNodes(nodes);
    check18->setValidWords({"himself","herself","itself"});
    check18->setRule(rule18);
    check18->setRelatedRel(Obj);
    pat18_1->addСheck(check18);
    s.insert(pat18);

}
