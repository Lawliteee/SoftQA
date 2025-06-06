#include "tests.h"
#include <QDebug>
void Tests::testPersonNumberCheck()
{
    QFETCH(const UDNode*, inputNode1);
    QFETCH(const UDNode*, inputNode2);
    QFETCH(bool, expAgreement);
    QFETCH(QSet<Mistake>, expMistakes);

    QSet<Mistake> mistakes;
    PersonNumberAgreement gRule;

    bool agreement = gRule.check(inputNode1,inputNode2,mistakes);

    // Сравнение полученных и ожидаемых ошибок
    QCOMPARE(agreement, expAgreement);
    QCOMPARE(mistakes, expMistakes);
}

void Tests::testPersonNumberCheck_data()
{
    // Добавляем столбцы

    QTest::addColumn<const UDNode*>("inputNode1");
    QTest::addColumn<const UDNode*>("inputNode2");
    QTest::addColumn<bool>("expAgreement");
    QTest::addColumn<QSet<Mistake>>("expMistakes");

    QSet<Mistake> emptyMistakes;         // Множество ошибок для тестов, где слова согласованы
    QSet<Mistake> mistakeSet;    // Множество для хранения ошибок

    // ТЕСТ 1 - Нет ошибки, подлежащее и глагол в 3 лице, ед.числе

    const UDNode* node1_1 = new UDNode("he",PRP,None);
    const UDNode* node1_2 = new UDNode("runs",VBZ,None);
    QTest::newRow("Test 1: no mistake, subject and verb are in the 3rd person, singular") << node1_1 << node1_2 << true << emptyMistakes;

    // ТЕСТ 2 - Нет ошибки, подлежащее и глагол не в 3 лице, ед.числе
    const UDNode* node2_1 = new UDNode("I", PRP, None);
    const UDNode* node2_2 = new UDNode("run", VBP, Ind);
    QTest::newRow("Test 2: no mistake, subject and verb are not in the 3rd person singular")
        << node2_1 << node2_2 << true << emptyMistakes;

    // ТЕСТ 3 - Нет ошибки, подлежащее в 3 лице ед.числе, глагол в subjunctive
    const UDNode* node3_1 = new UDNode("he", PRP, None);
    const UDNode* node3_2 = new UDNode("be", VB, Subj);
    QTest::newRow("Test 3: no mistake, subject is 3rd singular but verb is subjunctive")
        << node3_1 << node3_2 << true << emptyMistakes;

    // ТЕСТ 4 - Есть ошибка, подлежащее и глагол в Subjunctive в 3 лице ед.числе
    const UDNode* node4_1 = new UDNode("he", PRP, None);
    const UDNode* node4_2 = new UDNode("is", VBZ, Subj);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Глагол is в неправильной форме. В Present Subjunctive используется базовая форма")));
    QTest::newRow("Test 4: mistake, subject and verb in subjunctive but 3rd singular")
        << node4_1 << node4_2 << false << mistakeSet;

    // ТЕСТ 5 - Есть ошибка, подлежащее в 3 лице ед.числе, а глагол нет
    const UDNode* node5_1 = new UDNode("he", PRP, None);
    const UDNode* node5_2 = new UDNode("run", VBP, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Основной глагол run несогласован с подлежащим he. Глагол должен быть в форме 3-го лица ед. числа")));
    QTest::newRow("Test 5: mistake, subject is 3rd singular but verb is not")
        << node5_1 << node5_2 << false << mistakeSet;

    // ТЕСТ 6 - Есть ошибка, подлежащее не в 3 лице ед.числе, а глагол да
    const UDNode* node6_1 = new UDNode("you", PRP, None);
    const UDNode* node6_2 = new UDNode("runs", VBZ, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Основной глагол runs несогласован с подлежащим you. Глагол должен быть в начальной форме")));
    QTest::newRow("Test 6: error, subject is not 3rd singular but verb is")
        << node6_1 << node6_2 << false << mistakeSet;

    // ТЕСТ 7-10 - Личные местоимения не 3 лица ед. числа (I, we, you, they) без ошибок
    const UDNode* node7_1 = new UDNode("I", PRP, None);
    const UDNode* node7_2 = new UDNode("run", VBP, Ind);
    QTest::newRow("Test 7: no mistake with 'I'")
        << node7_1 << node7_2 << true << emptyMistakes;

    const UDNode* node8_1 = new UDNode("we", PRP, None);
    const UDNode* node8_2 = new UDNode("run", VBP, Ind);
    QTest::newRow("Test 8: no mistake with 'we'")
        << node8_1 << node8_2 << true << emptyMistakes;

    const UDNode* node9_1 = new UDNode("you", PRP, None);
    const UDNode* node9_2 = new UDNode("run", VBP, Ind);
    QTest::newRow("Test 9: no mistake with 'you'")
        << node9_1 << node9_2 << true << emptyMistakes;

    const UDNode* node10_1 = new UDNode("they", PRP, None);
    const UDNode* node10_2 = new UDNode("run", VBP, Ind);
    QTest::newRow("Test 10: no mistake with 'they'")
        << node10_1 << node10_2 << true << emptyMistakes;

    // ТЕСТ 11-14 - Личные местоимения не 3 лица ед. числа (I, we, you, they) с ошибками
    const UDNode* node11_1 = new UDNode("I", PRP, None);
    const UDNode* node11_2 = new UDNode("runs", VBZ, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Основной глагол runs несогласован с подлежащим I. Глагол должен быть в начальной форме")));
    QTest::newRow("Test 11: mistake with 'I'")
        << node11_1 << node11_2 << false << mistakeSet;

    const UDNode* node12_1 = new UDNode("we", PRP, None);
    const UDNode* node12_2 = new UDNode("runs", VBZ, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Основной глагол runs несогласован с подлежащим we. Глагол должен быть в начальной форме")));
    QTest::newRow("Test 12: mistake with 'we'")
        << node12_1 << node12_2 << false << mistakeSet;

    const UDNode* node13_1 = new UDNode("you", PRP, None);
    const UDNode* node13_2 = new UDNode("runs", VBZ, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Основной глагол runs несогласован с подлежащим you. Глагол должен быть в начальной форме")));
    QTest::newRow("Test 13: mistake with 'you'")
        << node13_1 << node13_2 << false << mistakeSet;

    const UDNode* node14_1 = new UDNode("they", PRP, None);
    const UDNode* node14_2 = new UDNode("runs", VBZ, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Основной глагол runs несогласован с подлежащим they. Глагол должен быть в начальной форме")));
    QTest::newRow("Test 14: mistake with 'they'")
        << node14_1 << node14_2 << false << mistakeSet;

    // ТЕСТ 15-17 - Личные местоимения 3 лица ед. числа (he, she, it) без ошибок
    const UDNode* node15_1 = new UDNode("he", PRP, None);
    const UDNode* node15_2 = new UDNode("runs", VBZ, Ind);
    QTest::newRow("Test 15: no mistake with 'he'")
        << node15_1 << node15_2 << true << emptyMistakes;

    const UDNode* node16_1 = new UDNode("she", PRP, None);
    const UDNode* node16_2 = new UDNode("runs", VBZ, Ind);
    QTest::newRow("Test 16: no mistake with 'she'")
        << node16_1 << node16_2 << true << emptyMistakes;

    const UDNode* node17_1 = new UDNode("it", PRP, None);
    const UDNode* node17_2 = new UDNode("runs", VBZ, Ind);
    QTest::newRow("Test 17: no mistake with 'it'")
        << node17_1 << node17_2 << true << emptyMistakes;

    // ТЕСТ 18-20 - Личные местоимения 3 лица ед. числа (he, she, it) с ошибками
    const UDNode* node18_1 = new UDNode("he", PRP, None);
    const UDNode* node18_2 = new UDNode("are", VBP, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Вспомогательный глагол are несогласован с подлежащим he. Глагол должен быть в форме в форме 3-го числа ед.числа")));
    QTest::newRow("Test 18: mistake with 'he'")
        << node18_1 << node18_2 << false << mistakeSet;

    const UDNode* node19_1 = new UDNode("she", PRP, None);
    const UDNode* node19_2 = new UDNode("run", VBP, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Основной глагол run несогласован с подлежащим she. Глагол должен быть в начальной форме")));
    QTest::newRow("Test 19: mistake with 'she'")
        << node19_1 << node19_2 << false << mistakeSet;

    const UDNode* node20_1 = new UDNode("it", PRP, None);
    const UDNode* node20_2 = new UDNode("run", VBP, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Основной глагол run несогласован с подлежащим it. Глагол должен быть в начальной форме")));
    QTest::newRow("Test 20: mistake with 'it'")
        << node20_1 << node20_2 << false << mistakeSet;

    // ТЕСТ 21-22 - Существительные не 3 лица ед. числа (теги NNS, NNPS) без ошибок
    const UDNode* node21_1 = new UDNode("horses", NNS, None);
    const UDNode* node21_2 = new UDNode("run", VBP, Ind);
    QTest::newRow("Test 21: no mistake with plural noun (NNS)")
        << node21_1 << node21_2 << true << emptyMistakes;

    const UDNode* node22_1 = new UDNode("Speedsters", NNPS, None);
    const UDNode* node22_2 = new UDNode("run", VBP, Ind);
    QTest::newRow("Test 22: no mistake with plural proper noun (NNPS)")
        << node22_1 << node22_2 << true << emptyMistakes;

    // ТЕСТ 23-24 - Существительные 3 лица ед. числа (теги NN, NNP) без ошибок
    const UDNode* node23_1 = new UDNode("dog", NN, None);
    const UDNode* node23_2 = new UDNode("runs", VBZ, Ind);
    QTest::newRow("Test 23: no mistake with singular noun (NN)")
        << node23_1 << node23_2 << true << emptyMistakes;

    const UDNode* node24_1 = new UDNode("Bolt", NNP, None);
    const UDNode* node24_2 = new UDNode("runs", VBZ, Ind);
    QTest::newRow("Test 24: no mistake with singular proper noun (NNP)")
        << node24_1 << node24_2 << true << emptyMistakes;

    // ТЕСТ 25 - Существительное не 3 лица ед. числа с ошибкой
    const UDNode* node25_1 = new UDNode("dogs", NNS, None);
    const UDNode* node25_2 = new UDNode("runs", VBZ, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Основной глагол runs несогласован с подлежащим dogs. Глагол должен быть в начальной форме")));
    QTest::newRow("Test 25: mistake with plural noun")
        << node25_1 << node25_2 << false << mistakeSet;

    // ТЕСТ 26 - Существительное 3 лица ед. числа с ошибкой
    const UDNode* node26_1 = new UDNode("dog", NN, None);
    const UDNode* node26_2 = new UDNode("run", VBP, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Основной глагол run несогласован с подлежащим dog. Глагол должен быть в форме 3-го числа ед.числа")));
    QTest::newRow("Test 26: mistake with singular noun")
        << node26_1 << node26_2 << false << mistakeSet;

    // ТЕСТ 27-28 - Глагол have в настоящих временах
    const UDNode* node27_1 = new UDNode("I", NN, None);
    const UDNode* node27_2 = new UDNode("have", VBZ, Ind);
    QTest::newRow("Test 27: no mistake I + have in present")
        << node27_1 << node27_2 << true << emptyMistakes;

    const UDNode* node28_1 = new UDNode("he", NN, None);
    const UDNode* node28_2 = new UDNode("has", VBZ, Ind);
    QTest::newRow("Test 28: no mistake he + has in present")
        << node28_1 << node28_2 << true << emptyMistakes;

    // ТЕСТ 29 - Существительное 3 лица ед. числа с ошибкой
    const UDNode* node29_1 = new UDNode("I", NN, None);
    const UDNode* node29_2 = new UDNode("has", VBP, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Вспомогательный глагол has несогласован с подлежащим I. Глагол должен быть в форме в начальной форме")));
    QTest::newRow("Test 29: mistake I + has")
        << node29_1 << node29_2 << false << mistakeSet;

    // ТЕСТ 30 - Глагол have в будущих временах(всегда форма have)
    UDNode * node30_0 = new UDNode(4,QString("written"),VBN,0,Root,Ind);
    UDNode * nodeWill = new UDNode(2,QString("will"),MD,4,Aux,Ind);
    node30_0->addChild(nodeWill);
    const UDNode* node30_1 = new UDNode(1,"he", NN, 4,Nsubj,None);
    const UDNode* node30_2 = new UDNode(3,"have", VBZ,4,Aux,Ind);
    node30_0->addChild(node30_1);
    node30_0->addChild(node30_2);
    QTest::newRow("Test 30: no mistake he + have in future")
        << node30_1 << node30_2 << true << emptyMistakes;

}

void Tests::testNumberCheck()
{
    QFETCH(const UDNode*, inputNode1);
    QFETCH(const UDNode*, inputNode2);
    QFETCH(bool, expAgreement);
    QFETCH(QSet<Mistake>, expMistakes);

    QSet<Mistake> mistakes;
    NumberAgreement gRule;

    bool agreement = gRule.check(inputNode1,inputNode2,mistakes);

    // Сравнение полученных и ожидаемых ошибок
    QCOMPARE(agreement, expAgreement);
    QCOMPARE(mistakes, expMistakes);
};

void Tests::testNumberCheck_data()
{
    // Добавляем столбцы

    QTest::addColumn<const UDNode*>("inputNode1");
    QTest::addColumn<const UDNode*>("inputNode2");
    QTest::addColumn<bool>("expAgreement");
    QTest::addColumn<QSet<Mistake>>("expMistakes");

    QSet<Mistake> emptyMistakes;         // Множество ошибок для тестов, где слова согласованы
    QSet<Mistake> mistakeSet;                // Множество для хранения ошибок


    // ТЕСТ 1 - Нет ошибки, 1 и 2 слово в ед. числе

    const UDNode* node1_1 = new UDNode("he",PRP,None);
    const UDNode* node1_2 = new UDNode("runs",VBZ,None);
    QTest::newRow("Test 1: No mistake, 1st and 2nd words are singular") << node1_1 << node1_2 << true << emptyMistakes;

    // ТЕСТ 2 - Нет ошибки, 1 и 2 слово во мн. числе
    const UDNode* node2_1 = new UDNode("we", NNS, None);
    const UDNode* node2_2 = new UDNode("were", VBP, Ind);
    QTest::newRow("Test 2: No mistake, both words are plural")
        << node2_1 << node2_2 << true << emptyMistakes;

    // ТЕСТ 3 - Нет ошибки, 1 слово совместимо с любым числом и 2 слово во мн. числе
    const UDNode* node3_1 = new UDNode("the", NN, None); // sheep может быть и ед. и мн.
    const UDNode* node3_2 = new UDNode("apples", VBP, Ind);
    QTest::newRow("Test 3: No mistake, first word compatible with any number")
        << node3_1 << node3_2 << true << emptyMistakes;

    // ТЕСТ 4 - Есть ошибка: 1 слово в ед. числе, а второе во мн. числе
    const UDNode* node4_1 = new UDNode("I", NN, None);
    const UDNode* node4_2 = new UDNode("were", VBP, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("глагол were несогласован с подлежащим I. Глагол должен быть в форме единственного числа.")));
    QTest::newRow("Test 4: Mistake, first word singular, second plural")
        << node4_1 << node4_2 << false << mistakeSet;

    // ТЕСТ 5-6 - Случай Subjunctive
    const UDNode* node5_1 = new UDNode("I", NN, None);
    const UDNode* node5_2 = new UDNode("were", VB, Subj);
    QTest::newRow("Test 5: Subjunctive mood, no number agreement check I")
        << node5_1 << node5_2 << true << emptyMistakes;

    const UDNode* node6_1 = new UDNode("we", NNS, None);
    const UDNode* node6_2 = new UDNode("were", VB, Subj);
    QTest::newRow("Test 6: Subjunctive mood, no number agreement check We")
        << node6_1 << node6_2 << true << emptyMistakes;

    // ТЕСТ 7-10 - Личные местоимения ед.числа (I, he, she, it)
    const UDNode* node7_1 = new UDNode("I", PRP, None);
    const UDNode* node7_2 = new UDNode("was", VBP, Ind);
    QTest::newRow("Test 7: No mistake with 'I'")
        << node7_1 << node7_2 << true << emptyMistakes;

    const UDNode* node8_1 = new UDNode("he", PRP, None);
    const UDNode* node8_2 = new UDNode("was", VBZ, Ind);
    QTest::newRow("Test 8: No mistake with 'he'")
        << node8_1 << node8_2 << true << emptyMistakes;

    const UDNode* node9_1 = new UDNode("she", PRP, None);
    const UDNode* node9_2 = new UDNode("was", VBZ, Ind);
    QTest::newRow("Test 9: No mistake with 'she'")
        << node9_1 << node9_2 << true << emptyMistakes;

    const UDNode* node10_1 = new UDNode("it", PRP, None);
    const UDNode* node10_2 = new UDNode("was", VBZ, Ind);
    QTest::newRow("Test 10: No mistake with 'it'")
        << node10_1 << node10_2 << true << emptyMistakes;

    // ТЕСТ 11-13 - Личные местоимения мн.числа (You, we, they)
    const UDNode* node11_1 = new UDNode("you", PRP, None);
    const UDNode* node11_2 = new UDNode("were", VBP, Ind);
    QTest::newRow("Test 11: No mistake with 'you'")
        << node11_1 << node11_2 << true << emptyMistakes;

    const UDNode* node12_1 = new UDNode("we", PRP, None);
    const UDNode* node12_2 = new UDNode("were", VBP, Ind);
    QTest::newRow("Test 12: No mistake with 'we'")
        << node12_1 << node12_2 << true << emptyMistakes;

    const UDNode* node13_1 = new UDNode("they", PRP, None);
    const UDNode* node13_2 = new UDNode("were", VBP, Ind);
    QTest::newRow("Test 13: No mistake with 'they'")
        << node13_1 << node13_2 << true << emptyMistakes;

    // ТЕСТ 14-15 - Существительное ед.числа (NN, NNP)
    const UDNode* node14_1 = new UDNode("cat", NN, None);
    const UDNode* node14_2 = new UDNode("was", VBZ, Ind);
    QTest::newRow("Test 14: No mistake with singular noun (NN)")
        << node14_1 << node14_2 << true << emptyMistakes;

    const UDNode* node15_1 = new UDNode("John", NNP, None);
    const UDNode* node15_2 = new UDNode("was", VBZ, Ind);
    QTest::newRow("Test 15: No mistake with singular proper noun (NNP)")
        << node15_1 << node15_2 << true << emptyMistakes;

    // ТЕСТ 16-17 - Существительное мн.числа
    const UDNode* node16_1 = new UDNode("cats", NNS, None);
    const UDNode* node16_2 = new UDNode("were", VBP, Ind);
    QTest::newRow("Test 16: No mistake with plural noun (NNS)")
        << node16_1 << node16_2 << true << emptyMistakes;

    const UDNode* node17_1 = new UDNode("Rockies", NNPS, None);
    const UDNode* node17_2 = new UDNode("were", VBP, Ind);
    QTest::newRow("Test 17: No mistake with plural proper noun (NNPS)")
        << node17_1 << node17_2 << true << emptyMistakes;

    // ТЕСТ 18-19 - The как с ед. числом, так и с мн. числом
    const UDNode* node18_1 = new UDNode("the", DT, None);
    const UDNode* node18_2 = new UDNode("cat", NN, None);
    QTest::newRow("Test 18: No mistake with 'the' + singular noun")
        << node18_1 << node18_2 << true << emptyMistakes;

    const UDNode* node19_1 = new UDNode("the", DT, None);
    const UDNode* node19_2 = new UDNode("cats", NNS, None);
    QTest::newRow("Test 19: No mistake with 'the' + plural noun")
        << node19_1 << node19_2 << true << emptyMistakes;

    // ТЕСТ 20-22 - A, an, one c ед. числом без ошибок
    const UDNode* node20_1 = new UDNode("a", DT, None);
    const UDNode* node20_2 = new UDNode("bottle", NN, None);
    QTest::newRow("Test 20: No mistake with 'a' + singular noun")
        << node20_1 << node20_2 << true << emptyMistakes;

    const UDNode* node21_1 = new UDNode("an", DT, None);
    const UDNode* node21_2 = new UDNode("apple", NN, None);
    QTest::newRow("Test 21: No mistake with 'an' + singular noun")
        << node21_1 << node21_2 << true << emptyMistakes;

    const UDNode* node22_1 = new UDNode("one", CD, None);
    const UDNode* node22_2 = new UDNode("bottle", NN, None);
    QTest::newRow("Test 22: No mistake with 'one' + singular noun")
        << node22_1 << node22_2 << true << emptyMistakes;

    // ТЕСТ 23-25 - A, an, one с мн. числом (ошибка)
    const UDNode* node23_1 = new UDNode("a", DT, None);
    const UDNode* node23_2 = new UDNode("bottles", NNS, None);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("артикль a употреблен ссуществительным bottles множественного числа. Требуется артикль the или нулевой артикль.")));
    QTest::newRow("Test 23: Mistake with 'a' + plural noun")
        << node23_1 << node23_2 << false << mistakeSet;

    const UDNode* node24_1 = new UDNode("an", DT, None);
    const UDNode* node24_2 = new UDNode("apples", NNS, None);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("артикль an употреблен ссуществительным bottles множественного числа. Требуется артикль the или нулевой артикль.")));
    QTest::newRow("Test 24: Mistake with 'an' + plural noun")
        << node24_1 << node24_2 << false << mistakeSet;

    const UDNode* node25_1 = new UDNode("one", DT, None);
    const UDNode* node25_2 = new UDNode("apples", NNS, None);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("существительное apples несогласовано по числу с числительным one. Существительное должно быть в форме единственного числа.")));
    QTest::newRow("Test 25: Mistake with 'one' + plural noun")
        << node25_1 << node25_2 << false << mistakeSet;

    // ТЕСТ 26 - Числительное мн. числа
    const UDNode* node26_1 = new UDNode("two", CD, None);
    const UDNode* node26_2 = new UDNode("apples", NNS, None);
    QTest::newRow("Test 26: No mistake with plural number + plural noun")
        << node26_1 << node26_2 << true << emptyMistakes;

    // ТЕСТ 27 - Числительное мн. числа
    const UDNode* node27_1 = new UDNode("two", CD, None);
    const UDNode* node27_2 = new UDNode("bottle", NNS, None);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("существительное bottle несогласовано по числу с числительным two. Существительное должно быть в форме множественного числа.")));
    QTest::newRow("Test 26: mistake with plural number + NN")
        << node27_1 << node27_2 << true << mistakeSet;
};

void Tests::testMainAuxAgreement()
{
    QFETCH(const UDNode*, inputNode1);
    QFETCH(const UDNode*, inputNode2);
    QFETCH(bool, expAgreement);
    QFETCH(QSet<Mistake>, expMistakes);

    QSet<Mistake> mistakes;
    MainAuxAgreement gRule;

    bool agreement = gRule.check(inputNode1,inputNode2,mistakes);

    // Сравнение полученных и ожидаемых ошибок
    QCOMPARE(agreement, expAgreement);
    QCOMPARE(mistakes, expMistakes);

}

void Tests::testMainAuxAgreement_data()
{
    // Добавляем столбцы

    QTest::addColumn<const UDNode*>("inputNode1");
    QTest::addColumn<const UDNode*>("inputNode2");
    QTest::addColumn<bool>("expAgreement");
    QTest::addColumn<QSet<Mistake>>("expMistakes");

    QSet<Mistake> emptyMistakes;         // Множество ошибок для тестов, где слова согласованы
    QSet<Mistake> mistakeSet;                // Множество для хранения ошибок

    // Тест 1-3: Нет ошибки, вспомогательный глагол форма do/does/did, основной в начальной форме
    const UDNode* node1_1 = new UDNode("do", VBP, Ind);
    const UDNode* node1_2 = new UDNode("play", VB, Ind);
    QTest::newRow("Test 1: do + VB") << node1_1 << node1_2 << true << emptyMistakes;

    const UDNode* node2_1 = new UDNode("does", VBZ, Ind);
    const UDNode* node2_2 = new UDNode("play", VB, Ind);
    QTest::newRow("Test 2: does + VB") << node2_1 << node2_2 << true << emptyMistakes;

    const UDNode* node3_1 = new UDNode("did", VBD, Ind);
    const UDNode* node3_2 = new UDNode("play", VB, Ind);
    QTest::newRow("Test 3: did + VB") << node3_1 << node3_2 << true << emptyMistakes;

    // Тест 4: Есть ошибка, вспомогательный глагол do, основной не в начальной форме
    const UDNode* node4_1 = new UDNode("do", VBP, Ind);
    const UDNode* node4_2 = new UDNode("speaks", VBZ, Ind);
    mistakeSet.insert(Mistake("Глагол speaks несогласован по времени с вспомогательным глаголом do."));
    QTest::newRow("Test 4: do + VBZ (mistake)") << node4_1 << node4_2 << false << mistakeSet;
    mistakeSet.clear();

    // Тест 5-13: Нет ошибки, вспомогательный глагол модальный, основной в начальной форме
    const UDNode* node5_1 = new UDNode("can", MD, Ind);
    const UDNode* node5_2 = new UDNode("go", VB, Ind);
    QTest::newRow("Test 5: can + VB") << node5_1 << node5_2 << true << emptyMistakes;

    const UDNode* node6_1 = new UDNode("could", MD, Ind);
    const UDNode* node6_2 = new UDNode("go", VB, Ind);
    QTest::newRow("Test 6: could + VB") << node6_1 << node6_2 << true << emptyMistakes;

    const UDNode* node7_1 = new UDNode("may", MD, Ind);
    const UDNode* node7_2 = new UDNode("go", VB, Ind);
    QTest::newRow("Test 7: may + VB") << node7_1 << node7_2 << true << emptyMistakes;

    const UDNode* node8_1 = new UDNode("might", MD, Ind);
    const UDNode* node8_2 = new UDNode("go", VB, Ind);
    QTest::newRow("Test 8: might + VB") << node8_1 << node8_2 << true << emptyMistakes;

    const UDNode* node9_1 = new UDNode("must", MD, Ind);
    const UDNode* node9_2 = new UDNode("go", VB, Ind);
    QTest::newRow("Test 9: must + VB") << node9_1 << node9_2 << true << emptyMistakes;

    const UDNode* node10_1 = new UDNode("shall", MD, Ind);
    const UDNode* node10_2 = new UDNode("go", VB, Ind);
    QTest::newRow("Test 10: shall + VB") << node10_1 << node10_2 << true << emptyMistakes;

    const UDNode* node11_1 = new UDNode("should", MD, Ind);
    const UDNode* node11_2 = new UDNode("go", VB, Ind);
    QTest::newRow("Test 11: should + VB") << node11_1 << node11_2 << true << emptyMistakes;

    const UDNode* node12_1 = new UDNode("will", MD, Ind);
    const UDNode* node12_2 = new UDNode("go", VB, Ind);
    QTest::newRow("Test 12: will + VB") << node12_1 << node12_2 << true << emptyMistakes;

    const UDNode* node13_1 = new UDNode("would", MD, Ind);
    const UDNode* node13_2 = new UDNode("go", VB, Ind);
    QTest::newRow("Test 13: would + VB") << node13_1 << node13_2 << true << emptyMistakes;

    // Тест 14: Есть ошибка, вспомогательный глагол модальный, основной не в начальной форме
    const UDNode* node14_1 = new UDNode("should", MD, Ind);
    const UDNode* node14_2 = new UDNode("goes", VBZ, Ind);
    mistakeSet.insert(Mistake("Глагол goes несогласован по времени с модальным глаголом should"));
    QTest::newRow("Test 14: can + VBZ (mistake)") << node14_1 << node14_2 << false << mistakeSet;
    mistakeSet.clear();

    // Тест 15-18: Нет ошибки, вспомогательный глагол форма be, основной в форме герундия
    const UDNode* node15_1 = new UDNode("be", VB, Ind);
    const UDNode* node15_2 = new UDNode("going", VBG, Ind);
    QTest::newRow("Test 15: be + VBG") << node15_1 << node15_2 << true << emptyMistakes;

    const UDNode* node16_1 = new UDNode("was", VBD, Ind);
    const UDNode* node16_2 = new UDNode("going", VBG, Ind);
    QTest::newRow("Test 16: was + VBG") << node16_1 << node16_2 << true << emptyMistakes;

    const UDNode* node17_1 = new UDNode("were", VBD, Ind);
    const UDNode* node17_2 = new UDNode("going", VBG, Ind);
    QTest::newRow("Test 17: were + VBG") << node17_1 << node17_2 << true << emptyMistakes;

    const UDNode* node18_1 = new UDNode("been", VBN, Ind);
    const UDNode* node18_2 = new UDNode("going", VBG, Ind);
    QTest::newRow("Test 18: been + VBG") << node18_1 << node18_2 << true << emptyMistakes;

    // Тест 19: Есть ошибка, вспомогательный глагол was, основной не в форме герундия
    const UDNode* node19_1 = new UDNode("was", VBD, Ind);
    const UDNode* node19_2 = new UDNode("go", VB, Ind);
    mistakeSet.insert(Mistake("Глагол go несогласован по времени с вспомогательным глаголом was."));
    QTest::newRow("Test 19: was + VB (mistake)") << node19_1 << node19_2 << false << mistakeSet;
    mistakeSet.clear();

    // Тест 20-22: Нет ошибки, вспомогательный глагол форма have, основной в форме причастия прошедшего времени
    const UDNode* node20_1 = new UDNode("have", VBP, Ind);
    const UDNode* node20_2 = new UDNode("seen", VBN, Ind);
    QTest::newRow("Test 20: have + VBN") << node20_1 << node20_2 << true << emptyMistakes;

    const UDNode* node21_1 = new UDNode("has", VBZ, Ind);
    const UDNode* node21_2 = new UDNode("seen", VBN, Ind);
    QTest::newRow("Test 21: has + VBN") << node21_1 << node21_2 << true << emptyMistakes;

    const UDNode* node22_1 = new UDNode("had", VBD, Ind);
    const UDNode* node22_2 = new UDNode("seen", VBN, Ind);
    QTest::newRow("Test 22: had + VBN") << node22_1 << node22_2 << true << emptyMistakes;

    // Тест 23: Есть ошибка, вспомогательный глагол форма have, основной не в форме причастия прошедшего времени
    const UDNode* node23_1 = new UDNode("have", VBP, Ind);
    const UDNode* node23_2 = new UDNode("goes", VBZ, Ind);
    mistakeSet.insert(Mistake("Глагол goes несогласован по времени с вспомогательным глаголом have."));
    QTest::newRow("Test 23: have + VBZ (mistake)") << node23_1 << node23_2 << false << mistakeSet;
    mistakeSet.clear();

    // Тест 24-25: Нет ошибки, сокращение 'd (had/would), основной в базовой форме или причастии прошедшего времени
    const UDNode* node24_1 = new UDNode("'d", MD, Ind); // would
    const UDNode* node24_2 = new UDNode("tell", VB, Ind);
    QTest::newRow("Test 24: 'd (would) + VB") << node24_1 << node24_2 << true << emptyMistakes;

    const UDNode* node25_1 = new UDNode("'d", VBD, Ind); // had
    const UDNode* node25_2 = new UDNode("seen", VBN, Ind);
    QTest::newRow("Test 25: 'd (had) + VBN") << node25_1 << node25_2 << true << emptyMistakes;

    // Тест 26: Есть ошибка, сокращение 'd
    const UDNode* node26_1 = new UDNode("'d", MD, Ind); // would
    const UDNode* node26_2 = new UDNode("playing", VBG, Ind);
    mistakeSet.insert(Mistake("Глагол playing несогласован по времени с вспомогательным глаголом 'd."));
    QTest::newRow("Test 26: 'd + VBG (error)") << node26_1 << node26_2 << false << mistakeSet;
    mistakeSet.clear();
}

void Tests::testAuxAuxAgreement()
{
    QFETCH(const UDNode*, inputNode1);
    QFETCH(const UDNode*, inputNode2);
    QFETCH(bool, expAgreement);
    QFETCH(QSet<Mistake>, expMistakes);

    QSet<Mistake> mistakes;
    AuxAuxAgreement gRule;

    bool agreement = gRule.check(inputNode1,inputNode2,mistakes);

    // Сравнение полученных и ожидаемых ошибок
    QCOMPARE(agreement, expAgreement);
    QCOMPARE(mistakes, expMistakes);
}

void Tests::testAuxAuxAgreement_data()
{
    // Добавляем столбцы

    QTest::addColumn<const UDNode*>("inputNode1");
    QTest::addColumn<const UDNode*>("inputNode2");
    QTest::addColumn<bool>("expAgreement");
    QTest::addColumn<QSet<Mistake>>("expMistakes");

    QSet<Mistake> emptyMistakes;         // Множество ошибок для тестов, где слова согласованы
    QSet<Mistake> mistakeSet;                // Множество для хранения ошибок

    // Тест 1-2: Нет ошибки, определяющий will, второй have/be
    const UDNode* node1_1 = new UDNode("will", MD, Ind);
    const UDNode* node1_2 = new UDNode("have", VB, Ind);
    QTest::newRow("Test 1: will + have") << node1_1 << node1_2 << true << emptyMistakes;

    const UDNode* node2_1 = new UDNode("will", MD, Ind);
    const UDNode* node2_2 = new UDNode("be", VB, Ind);
    QTest::newRow("Test 2: will + be") << node2_1 << node2_2 << true << emptyMistakes;

    // Тест 3-8: Есть ошибка, определяющий will, второй не have/be в начальной форме
    const UDNode* node3_1 = new UDNode("will", MD, Ind);
    const UDNode* node3_2 = new UDNode("has", VBZ, Ind);
    mistakeSet.insert(Mistake("неправильная форма глагола have. Требуется  начальная форма."));
    QTest::newRow("Test 3: will + has (mistake)") << node3_1 << node3_2 << false << mistakeSet;
    mistakeSet.clear();

    const UDNode* node4_1 = new UDNode("will", MD, Ind);
    const UDNode* node4_2 = new UDNode("had", VBD, Ind);
    mistakeSet.insert(Mistake("неправильная форма глагола have. Требуется  начальная форма."));
    QTest::newRow("Test 4: will + had (mistake)") << node4_1 << node4_2 << false << mistakeSet;
    mistakeSet.clear();

    const UDNode* node5_1 = new UDNode("will", MD, Ind);
    const UDNode* node5_2 = new UDNode("am", VBP, Ind);
    mistakeSet.insert(Mistake("Глагол am несогласован по времени с вспомогательным глаголом will. После will вспомогательным может быть have"));
    QTest::newRow("Test 5: will + am (mistake)") << node5_1 << node5_2 << false << mistakeSet;
    mistakeSet.clear();

    const UDNode* node6_1 = new UDNode("will", MD, Ind);
    const UDNode* node6_2 = new UDNode("is", VBZ, Ind);
    mistakeSet.insert(Mistake("Глагол am несогласован по времени с вспомогательным глаголом will. После will вспомогательным может быть have"));
    QTest::newRow("Test 6: will + is (mistake)") << node6_1 << node6_2 << false << mistakeSet;
    mistakeSet.clear();

    const UDNode* node7_1 = new UDNode("will", MD, Ind);
    const UDNode* node7_2 = new UDNode("are", VBP, Ind);
    mistakeSet.insert(Mistake("Глагол am несогласован по времени с вспомогательным глаголом will. После will вспомогательным может быть have"));
    QTest::newRow("Test 7: will + are (mistake)") << node7_1 << node7_2 << false << mistakeSet;
    mistakeSet.clear();

    const UDNode* node8_1 = new UDNode("will", MD, Ind);
    const UDNode* node8_2 = new UDNode("been", VBN, Ind);
    mistakeSet.insert(Mistake("Глагол am несогласован по времени с вспомогательным глаголом will. После will вспомогательным может быть have"));
    QTest::newRow("Test 8: will + been (mistake)") << node8_1 << node8_2 << false << mistakeSet;
    mistakeSet.clear();

    // Тест 9: Нет ошибки, определяющий have, второй been
    const UDNode* node9_1 = new UDNode("have", VB, Ind);
    const UDNode* node9_2 = new UDNode("been", VBN, Ind);
    QTest::newRow("Test 9: have + been") << node9_1 << node9_2 << true << emptyMistakes;

    // Тест 10-13: Есть ошибка, определяющий have, второй не been
    const UDNode* node10_1 = new UDNode("have", VB, Ind);
    const UDNode* node10_2 = new UDNode("be", VB, Ind);
    mistakeSet.insert(Mistake("Глагол be несогласован по времени свспомогательным глаголом have. Требуется форма Past Participle/V3(been)."));
    QTest::newRow("Test 10: have + be (mistake)") << node10_1 << node10_2 << false << mistakeSet;
    mistakeSet.clear();

    const UDNode* node11_1 = new UDNode("have", VB, Ind);
    const UDNode* node11_2 = new UDNode("am", VBP, Ind);
    mistakeSet.insert(Mistake("Глагол am несогласован по времени свспомогательным глаголом have. Требуется форма Past Participle/V3(been)."));
    QTest::newRow("Test 11: have + am (mistake)") << node11_1 << node11_2 << false << mistakeSet;
    mistakeSet.clear();

    const UDNode* node12_1 = new UDNode("have", VB, Ind);
    const UDNode* node12_2 = new UDNode("is", VBZ, Ind);
    mistakeSet.insert(Mistake("Глагол is несогласован по времени свспомогательным глаголом have. Требуется форма Past Participle/V3(been)."));
    QTest::newRow("Test 12: have + is (mistake)") << node12_1 << node12_2 << false << mistakeSet;
    mistakeSet.clear();

    const UDNode* node13_1 = new UDNode("have", VB, Ind);
    const UDNode* node13_2 = new UDNode("are", VBP, Ind);
    mistakeSet.insert(Mistake("Глагол are несогласован по времени свспомогательным глаголом have. Требуется форма Past Participle/V3(been)."));
    QTest::newRow("Test 13: have + are (mistake)") << node13_1 << node13_2 << false << mistakeSet;
    mistakeSet.clear();
}

void Tests::testPassiveAgreement()
{
    QFETCH(const UDNode*, inputNode1);
    QFETCH(const UDNode*, inputNode2);
    QFETCH(bool, expAgreement);
    QFETCH(QSet<Mistake>, expMistakes);

    QSet<Mistake> mistakes;
    PassiveAgreement gRule;

    bool agreement = gRule.check(inputNode1,inputNode2,mistakes);

    // Сравнение полученных и ожидаемых ошибок
    QCOMPARE(agreement, expAgreement);
    QCOMPARE(mistakes, expMistakes);
}

void Tests::testPassiveAgreement_data()
{
    // Добавляем столбцы

    QTest::addColumn<const UDNode*>("inputNode1");
    QTest::addColumn<const UDNode*>("inputNode2");
    QTest::addColumn<bool>("expAgreement");
    QTest::addColumn<QSet<Mistake>>("expMistakes");

    QSet<Mistake> emptyMistakes;         // Множество ошибок для тестов, где слова согласованы
    QSet<Mistake> mistakeSet;                // Множество для хранения ошибок

    // Тест №1-8. Нет ошибки, глаголы в пассивном залоге согласованы
    UDNode* node1_0 = new UDNode(3,"delivered",VBN,0,Root,Ind);
    const UDNode* node1_1 = new UDNode(1,"letters",NNS,3,Nsubj_Pass,None);
    node1_0->addChild(node1_1);
    const UDNode* node1_2 = new UDNode(2,"are",VBP,0,Aux_Pass,Ind);
    node1_0->addChild(node1_2);
    QTest::newRow("Test 1: No mistake Passive: are + V3") << node1_2 << static_cast<const UDNode*>(node1_0) << true << emptyMistakes;

    // Тест №9. Есть ошибка, глаголы в пассивном залоге не согласованы(основной глагол не в форме причастия прошедшего времени)
    UDNode* node9_0 = new UDNode(3,"deliver",VB,0,Root,Ind);
    const UDNode* node9_1 = new UDNode(1,"letters",NNS,3,Nsubj_Pass,None);
    node9_0->addChild(node9_1);
    const UDNode* node9_2 = new UDNode(2,"are",VBP,0,Aux_Pass,Ind);
    node9_0->addChild(node9_2);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(" глаголы при построении пассивного залого(Passive Voice) несогласованы."));
    QTest::newRow("Test 9: Mistake Passive: are + VB") << node9_2 << static_cast<const UDNode*>(node9_0) << false << mistakeSet;
}

void Tests::testComplexSentenceAgreement()
{
    QFETCH(const UDNode*, inputNode1);
    QFETCH(const UDNode*, inputNode2);
    QFETCH(bool, expAgreement);
    QFETCH(QSet<Mistake>, expMistakes);

    QSet<Mistake> mistakes;
    ComplexSentenceAgreement gRule;

    bool agreement = gRule.check(inputNode1,inputNode2,mistakes);

    // Сравнение полученных и ожидаемых ошибок
    QCOMPARE(agreement, expAgreement);
    QCOMPARE(mistakes, expMistakes);
}

void Tests::testComplexSentenceAgreement_data()
{
    // Добавляем столбцы

    QTest::addColumn<const UDNode*>("inputNode1");
    QTest::addColumn<const UDNode*>("inputNode2");
    QTest::addColumn<bool>("expAgreement");
    QTest::addColumn<QSet<Mistake>>("expMistakes");

    QSet<Mistake> emptyMistakes;         // Множество ошибок для тестов, где слова согласованы
    QSet<Mistake> mistakeSet;                // Множество для хранения ошибок

    // Тест №1-2. Нет ошибки, главная часть в будущем времени без условных и временных союзов
    UDNode* node1_0 = new UDNode(3,"fail",VB,0,Root,Ind);
    UDNode* node1_1 = new UDNode(9,"studies",VBZ,3,Advcl,None);
    node1_0->addChild(node1_1);
    const UDNode* node1_2 = new UDNode(2,"will",MD,3,Aux,Ind);
    node1_0->addChild(node1_2);
    const UDNode* node1_3 = new UDNode(6,"because",IN,9,Mark,None);
    node1_1->addChild(node1_3);
    QTest::newRow("Test 1: No mistake Future main clause without conditional and temporary conjunctions") << static_cast<const UDNode*>(node1_1) << static_cast<const UDNode*>(node1_0) << true << emptyMistakes;

    // Тест №16-17. Есть ошибка, главная часть в будущем времени с временным с временными или условными союзами(when,if), придаточная не в настоящем времени
    UDNode* node16_0 = new UDNode(3,"call",VB,0,Root,Ind);
    UDNode* node16_1 = new UDNode(8,"arrive",VBZ,3,Advcl,None);
    node16_0->addChild(node16_1);
    const UDNode* node16_2 = new UDNode(2,"will",MD,3,Aux,Ind);
    node16_0->addChild(node16_2);
    const UDNode* node16_3 = new UDNode(7,"will",MD,8,Aux,Ind);
    node16_1->addChild(node16_3);
    const UDNode* node16_4 = new UDNode(5,"when",WRB,8,Advmod,None);
    node16_1->addChild(node16_4);

    mistakeSet.clear();
    mistakeSet.insert(Mistake("Если главноепредложение стоит в будущем времени, а придаточное начинается с when, as soon as, before, after, until, if или unless, то в нем используется одно из настоящих времен."));
    QTest::newRow("Test 16: Mistake, Future main clause with temporary conjunction, dep clause in future") << static_cast<const UDNode*>(node16_1) << static_cast<const UDNode*>(node16_0) << false << mistakeSet;
}

void Tests::testConditionalsAgreement()
{
    QFETCH(const UDNode*, inputNode1);
    QFETCH(const UDNode*, inputNode2);
    QFETCH(bool, expAgreement);
    QFETCH(QSet<Mistake>, expMistakes);

    QSet<Mistake> mistakes;
    ConditionalsAgreement gRule;

    bool agreement = gRule.check(inputNode1,inputNode2,mistakes);

    // Сравнение полученных и ожидаемых ошибок
    QCOMPARE(agreement, expAgreement);
    QCOMPARE(mistakes, expMistakes);
}
void Tests::testConditionalsAgreement_data()
{
    // Добавляем столбцы

    QTest::addColumn<const UDNode*>("inputNode1");
    QTest::addColumn<const UDNode*>("inputNode2");
    QTest::addColumn<bool>("expAgreement");
    QTest::addColumn<QSet<Mistake>>("expMistakes");

    QSet<Mistake> emptyMistakes;         // Множество ошибок для тестов, где слова согласованы
    QSet<Mistake> mistakeSet;                // Множество для хранения ошибок

    // Тест №1. Нет ошибки,  второй тип,  придаточная часть в Past simple
    UDNode* node1_0 = new UDNode(7,"travel",VB,0,Root,Subj);
    UDNode* node1_1 = new UDNode(3,"had",VBD,7,Advcl,None);
    node1_0->addChild(node1_1);
    const UDNode* node1_2 = new UDNode(6,"would",MD,7,Aux,Ind);
    node1_0->addChild(node1_2);
    QTest::newRow("Test 1: No mistake 2nd Conditionals") << static_cast<const UDNode*>(node1_1) << static_cast<const UDNode*>(node1_0) << true << emptyMistakes;

    // Тест №2-3. Есть ошибка,  второй тип,  придаточная часть не в Past Simple
    UDNode* node2_0 = new UDNode(7,"go",VB,0,Root,Subj);
    UDNode* node2_1 = new UDNode(4,"playing",VBG,7,Advcl,None);
    node2_0->addChild(node2_1);
    const UDNode* node2_2 = new UDNode(6,"would",MD,7,Aux,Ind);
    node2_0->addChild(node2_2);

    mistakeSet.clear();
    mistakeSet.insert(Mistake("придаточная частьнесогласована с главной по времени. Во втором типе условных предложений(Second Conditional) следующая формула: If + Past Simple, would + V1. "));
    QTest::newRow("Test 2: Mistake 2nd Conditionals, Past Continuous") << static_cast<const UDNode*>(node2_1) << static_cast<const UDNode*>(node2_0) << false << mistakeSet;
}

void Tests::testCreateNodesFromLines()
{
    QFETCH(QStringList, inputLines);
    typedef QMap<int, UDNode*> NodeMap;
    QFETCH(NodeMap, expNodes);
    QFETCH(QSet<Error>, expErrors);

    QMap<int, UDNode*> actualNodes;
    QSet<Error> actualErrors;
    createNodesFromLines(inputLines, actualNodes, actualErrors);

    // Проверка узлов
    QCOMPARE(actualNodes.size(), expNodes.size());
    for (auto it = expNodes.begin(); it != expNodes.end(); ++it)
    {
        int id = it.key();
        UDNode* expected = it.value();
        UDNode* actual = actualNodes.value(id, nullptr);

        QVERIFY2(actual != nullptr,  ("Actual node with key = " + QString::number(it.key()) + " is NULL").toUtf8());
        if (actual->getId() != expected->getId())
        {
            QVERIFY2(actual != nullptr,  ("Key =" + QString::number(it.key()) +": Actual's node id = " + QString::number(actual->getId()) + ", exp id = "+ QString::number(expected->getId())).toUtf8());
        }
        if (actual->getlemma() != expected->getlemma())
        {
            QVERIFY2(actual != nullptr,  ("Key =" + QString::number(it.key()) +": Actual's node lemma = " + QString(actual->getlemma()) + ", exp lemma = "+ QString(expected->getlemma())).toUtf8());
        }
        QCOMPARE(actual->getUpos(), expected->getUpos());
        if (actual->getHead() != expected->getHead())
        {
            QVERIFY2(actual != nullptr,  ("Key =" + QString::number(it.key()) +": Actual's node Head = " + QString::number(actual->getHead()) + ", exp Head = "+ QString::number(expected->getHead())).toUtf8());
        }
        QCOMPARE(actual->getDepRel(), expected->getDepRel());
        QCOMPARE(actual->getMood(), expected->getMood());
    }

    // Проверка ошибок
    QCOMPARE(actualErrors, expErrors);

}

void addDepRelTest(DepRel rel, const QString& relStr)
{
    QStringList lines;
    QMap<int, UDNode*> nodes;
    QSet<Error> errors;

    lines << QString("1   does VBZ  0 %1 -").arg(relStr);
    nodes.insert(1, new UDNode(1,QString("does"),VBZ,0, rel, Ind));
    QTest::newRow(qPrintable(QString("%1 relation").arg(relStr))) << lines << nodes << errors;
}

void addPosTagTest(PosTag tag, const QString& tagStr)
{
    QStringList lines;
    QMap<int, UDNode*> nodes;
    QSet<Error> errors;

    lines << QString("1   does %1  0 root -").arg(tagStr);
    nodes.insert(1, new UDNode(1,QString("does"),tag,0, Root, Ind));
    QTest::newRow(qPrintable(QString("%1 tag").arg(tagStr))) << lines << nodes << errors;
}

void Tests::testCreateNodesFromLines_data() {
    QTest::addColumn<QStringList>("inputLines");
    QTest::addColumn<QMap<int, UDNode*>>("expNodes");
    QTest::addColumn<QSet<Error>>("expErrors");

    // Тесты для всех типов связей
    addDepRelTest(DepRel::Acl, "acl");
    addDepRelTest(DepRel::Acl_Relcl, "acl:relcl");
    addDepRelTest(DepRel::Advcl, "advcl");
    addDepRelTest(DepRel::Advcl_Relcl, "advcl:relcl");
    addDepRelTest(DepRel::Advmod, "advmod");
    addDepRelTest(DepRel::Advmod_Emph, "advmod:emph");
    addDepRelTest(DepRel::Advmod_Lmod, "advmod:lmod");
    addDepRelTest(DepRel::Amod, "amod");
    addDepRelTest(DepRel::Appos, "appos");
    addDepRelTest(DepRel::Aux, "aux");
    addDepRelTest(DepRel::Aux_Pass, "aux:pass");
    addDepRelTest(DepRel::Case, "case");
    addDepRelTest(DepRel::Cc, "cc");
    addDepRelTest(DepRel::Cc_Preconj, "cc:preconj");
    addDepRelTest(DepRel::Ccomp, "ccomp");
    addDepRelTest(DepRel::Clf, "clf");
    addDepRelTest(DepRel::Compound, "compound");
    addDepRelTest(DepRel::Compound_Lvc, "compound:lvc");
    addDepRelTest(DepRel::Compound_Prt, "compound:prt");
    addDepRelTest(DepRel::Compound_Redup, "compound:redup");
    addDepRelTest(DepRel::Compound_Svc, "compound:svc");
    addDepRelTest(DepRel::Conj, "conj");
    addDepRelTest(DepRel::Cop, "cop");
    addDepRelTest(DepRel::Csubj, "csubj");
    addDepRelTest(DepRel::Csubj_Outer, "csubj:outer");
    addDepRelTest(DepRel::Csubj_Pass, "csubj:pass");
    addDepRelTest(DepRel::Dep, "dep");
    addDepRelTest(DepRel::Det, "det");
    addDepRelTest(DepRel::Det_Numgov, "det:numgov");
    addDepRelTest(DepRel::Det_Nummod, "det:nummod");
    addDepRelTest(DepRel::Det_Poss, "det:poss");
    addDepRelTest(DepRel::Discourse, "discourse");
    addDepRelTest(DepRel::Dislocated, "dislocated");
    addDepRelTest(DepRel::Expl, "expl");
    addDepRelTest(DepRel::Expl_Impers, "expl:impers");
    addDepRelTest(DepRel::Expl_Pass, "expl:pass");
    addDepRelTest(DepRel::Expl_Pv, "expl:pv");
    addDepRelTest(DepRel::Fixed, "fixed");
    addDepRelTest(DepRel::Flat, "flat");
    addDepRelTest(DepRel::Flat_Name, "flat:name");
    addDepRelTest(DepRel::Goeswith, "goeswith");
    addDepRelTest(DepRel::Iobj, "iobj");
    addDepRelTest(DepRel::List, "list");
    addDepRelTest(DepRel::Mark, "mark");
    addDepRelTest(DepRel::Nmod, "nmod");
    addDepRelTest(DepRel::Nmod_Poss, "nmod:poss");
    addDepRelTest(DepRel::Nmod_Tmod, "nmod:tmod");
    addDepRelTest(DepRel::Nsubj, "nsubj");
    addDepRelTest(DepRel::Nsubj_Outer, "nsubj:outer");
    addDepRelTest(DepRel::Nsubj_Pass, "nsubj:pass");
    addDepRelTest(DepRel::Nummod, "nummod");
    addDepRelTest(DepRel::Nummod_Gov, "nummod:gov");
    addDepRelTest(DepRel::Obj, "obj");
    addDepRelTest(DepRel::Obl, "obl");
    addDepRelTest(DepRel::Obl_Agent, "obl:agent");
    addDepRelTest(DepRel::Obl_Arg, "obl:arg");
    addDepRelTest(DepRel::Obl_Lmod, "obl:lmod");
    addDepRelTest(DepRel::Obl_Tmod, "obl:tmod");
    addDepRelTest(DepRel::Obl_Npmod, "obl:npmod");
    addDepRelTest(DepRel::Orphan, "orphan");
    addDepRelTest(DepRel::Parataxis, "parataxis");
    addDepRelTest(DepRel::Punct, "punct");
    addDepRelTest(DepRel::Reparandum, "reparandum");
    addDepRelTest(DepRel::Root, "root");
    addDepRelTest(DepRel::Vocative, "vocative");
    addDepRelTest(DepRel::Xcomp, "xcomp");
    addDepRelTest(DepRel::Other, "Other");

    // Тесты для всех тегов частей речи
    addPosTagTest(PosTag::CC, "CC");    // Сочинительный союз
    addPosTagTest(PosTag::CD, "CD");    // Числительное
    addPosTagTest(PosTag::DT, "DT");    // Определитель
    addPosTagTest(PosTag::EX, "EX");    // Экзистенциальное there
    addPosTagTest(PosTag::FW, "FW");    // Иностранное слово
    addPosTagTest(PosTag::IN, "IN");    // Предлог/подчинительный союз
    addPosTagTest(PosTag::JJ, "JJ");    // Прилагательное
    addPosTagTest(PosTag::JJR, "JJR");  // Прилагательное в сравнительной степени
    addPosTagTest(PosTag::JJS, "JJS");  // Прилагательное в превосходной степени
    addPosTagTest(PosTag::LS, "LS");    // Маркер списка
    addPosTagTest(PosTag::MD, "MD");    // Модальный глагол
    addPosTagTest(PosTag::NN, "NN");    // Существительное единственного числа
    addPosTagTest(PosTag::NNS, "NNS");  // Существительное множественного числа
    addPosTagTest(PosTag::NNP, "NNP");  // Имя собственное единственного числа
    addPosTagTest(PosTag::NNPS, "NNPS");// Имя собственное множественного числа
    addPosTagTest(PosTag::VB, "VB");    // Глагол в базовой форме
    addPosTagTest(PosTag::VBD, "VBD");  // Глагол в прошедшем времени
    addPosTagTest(PosTag::VBG, "VBG");  // Причастие настоящего времени/герундий
    addPosTagTest(PosTag::VBN, "VBN");  // Причастие прошедшего времени
    addPosTagTest(PosTag::VBP, "VBP");  // Глагол в настоящем времени (не 3л. ед.ч.)
    addPosTagTest(PosTag::VBZ, "VBZ");  // Глагол в настоящем времени (3л. ед.ч.)
    addPosTagTest(PosTag::PRP, "PRP");  // Личное местоимение
    addPosTagTest(PosTag::PRP_, "PRP$"); // Притяжательное местоимение
    addPosTagTest(PosTag::WP, "WP");    // Вопросительное местоимение
    addPosTagTest(PosTag::WP_, "WP$");  // Притяжательное вопросительное местоимение
    addPosTagTest(PosTag::WRB, "WRB");  // Вопросительное наречие
    addPosTagTest(PosTag::RB, "RB");    // Наречие
    addPosTagTest(PosTag::RBR, "RBR");  // Наречие в сравнительной степени
    addPosTagTest(PosTag::RBS, "RBS");  // Наречие в превосходной степени
    addPosTagTest(PosTag::RP, "RP");    // Частица
    addPosTagTest(PosTag::SYM, "SYM");  // Символ
    addPosTagTest(PosTag::TO, "TO");    // Частица to
    addPosTagTest(PosTag::UH, "UH");    // Междометие
    addPosTagTest(PosTag::WDT, "WDT");  // Вопросительный определитель
    addPosTagTest(PosTag::PDT, "PDT");  // Предопределитель
    addPosTagTest(PosTag::ANY, "ANY");  // Любой тег (специальный случай)

}

void Tests::testCheckTreeConnectivity()
{
    QFETCH(bool, expConnectivity);
    QFETCH(UDNode*, inputRoot);
    typedef QMap<int, UDNode*> NodeMap;
    QFETCH(NodeMap, inputNodes);
    QFETCH(QSet<Error>, expErrors);

    QSet<Error> actualErrors;
    bool actConnectivity = checkTreeConnectivity(inputRoot,inputNodes,actualErrors);

    QCOMPARE(actConnectivity,expConnectivity);
    QCOMPARE(actualErrors, expErrors);
}

void Tests::testCheckTreeConnectivity_data()
{
    QTest::addColumn<bool>("expConnectivity");
    QTest::addColumn<UDNode*>("inputRoot");
    QTest::addColumn<QMap<int, UDNode*>>("inputNodes");
    QTest::addColumn<QSet<Error>>("expErrors");

    QMap<int, UDNode*> tree;
    QSet<Error> eErrors;
    QSet<int> lines;
    // Тест 1 одно слово
    tree.clear();
    UDNode* node1_0 = new UDNode(1,"Hello",UH,0,Root,None);
    tree.insert(1,node1_0);
    eErrors.clear();
    QTest::newRow("Test 1: Alone word") << true<<node1_0<<tree<<eErrors;

    // Тест 3 Минимальный цикл
    tree.clear();
    UDNode* node3_0 = new UDNode(1,"Stars",NNS,2,Nsubj,None);
    UDNode* node3_1 = new UDNode(2,"twinkle",VBP,0,Root,None);
    UDNode* node3_2 = new UDNode(3,"hide",NN,2,Obj,None);
    UDNode* node3_3 = new UDNode(4,"but",CC,5,Conj,None);
    UDNode* node3_4 = new UDNode(5,"clouds",NNS,4,Conj,None);
    node3_1->addChild(node3_0);
    node3_1->addChild(node3_2);
    node3_3->addChild(node3_4);
    node3_4->addChild(node3_3);

    tree.insert(0,node3_0);
    tree.insert(1,node3_1);
    tree.insert(2,node3_2);
    tree.insert(3,node3_3);
    tree.insert(4,node3_4);

    eErrors.clear();
    lines.clear();
    lines.insert(4);
    lines.insert(5);
    eErrors.insert(Error(lines,CycleInTree,"Ошибка об отсутствии связи с корнем(but, clouds)"));

    QTest::newRow("Test 3: Min cycle") << true<<node1_0<<tree<<eErrors;
}

void Tests::testAddChildren()
{
    typedef QMap<int, UDNode*> NodeMap;
    QFETCH(NodeMap, inputNodes);
    QFETCH(UDNode*, expRoot);
    typedef QMap<int, QSet<UDNode*>> childMap;
    QFETCH(childMap, expChildren);
    QFETCH(QSet<Error>, expErrors);

    QSet<Error> actualErrors;
    UDNode* actRoot = addChildren(inputNodes,actualErrors);

    QCOMPARE(actRoot,expRoot);
    if (actRoot != NULL)
    {
        QCOMPARE(actRoot->getId(),expRoot->getId());
    }
    else
    {
        QVERIFY2(false, (QString(" actual Node is NULL")).toUtf8());
    }
    QCOMPARE(actualErrors, expErrors);
    for (auto it = inputNodes.begin(); it != inputNodes.end(); ++it)
    {
        QSet <UDNode*> tmp;
        it.value()->writeChildren(tmp);
        if (tmp != expChildren.value(it.key()))
        {
            QVERIFY2(false, ("expected children of node with key = " + QString::number(it.key()) + " did not match").toUtf8());
        }
    }
}
void Tests::testAddChildren_data()
{
    QTest::addColumn<QMap<int, UDNode*>>("inputNodes");
    QTest::addColumn<UDNode*>("expRoot");
    QTest::addColumn<QMap<int, QSet<UDNode*>>>("expChildren");
    QTest::addColumn<QSet<Error>>("expErrors");

    QMap<int, UDNode*> tree;
    QMap<int, QSet<UDNode*>> eChildren;
    QSet<UDNode*> tmpChildren;
    QSet<Error> eErrors;
    QSet<int> lines;
    // Тест 1 одно слово
    tree.clear();
    UDNode* node1_0 = new UDNode(1,"the",DT,3,Det,None);
    UDNode* node1_1 = new UDNode(2,"angry",JJ,3,Amod,None);
    UDNode* node1_2 = new UDNode(3,"dog",NN,0,Root,None);
    tmpChildren.clear();
    eChildren.insert(1,tmpChildren);
    eChildren.insert(2,tmpChildren);
    tmpChildren.insert(node1_0);
    tmpChildren.insert(node1_1);
    eChildren.insert(3,tmpChildren);

    tree.insert(1,node1_0);
    tree.insert(2,node1_0);
    tree.insert(3,node1_0);
    eErrors.clear();
    QTest::newRow("Test 1: Alone word") << tree << node1_2 << eChildren << eErrors;
}

void Tests::testGetNodesChild()
{
    QFETCH(ChildChild*, rel);
    QFETCH(const UDNode*, mainNode);
    QFETCH(UDNode*, expSearchNode);
    QFETCH(const UDNode*, parent);

    UDNode* actNode = NULL;
    rel->getNodes(mainNode,actNode,parent);

    if (actNode != NULL)
    {
        QCOMPARE(actNode->getId(),expSearchNode->getId());
    }
    else
    {
        QVERIFY2(false, (QString(" actual Node is NULL")).toUtf8());
    }

}
void Tests::testGetNodesChild_data()
{
    QTest::addColumn<ChildChild*>("rel");
    QTest::addColumn<const UDNode*>("mainNode");
    QTest::addColumn<UDNode*>("expSearchNode");
    QTest::addColumn<const UDNode*>("parent");

    ChildChild* rel = new ChildChild;
    // Тест №1.  У родителя всего 2 ребенка
    rel->setValidWords({});
    rel->setValidTags({});
    rel->setRelatedRel(Other);
    UDNode* node1_0 = new UDNode(1,"she",PRP,3,Nsubj,None);
    UDNode* node1_1 = new UDNode(2,"is",VBZ,3,Aux,None);
    UDNode* node1_2 = new UDNode(3,"singing",VBG,0,Root,None);
    node1_2->addChild(node1_0);
    node1_2->addChild(node1_1);

    QTest::newRow("Test 1: two children") << rel << static_cast<const UDNode*>(node1_0) << node1_1<< static_cast<const UDNode*>(node1_2);
}

/*void Tests::testMatchesPattern()
{
    typedef QMap<int, UDNode*> NodeMap;
    QFETCH(NodeMap, inputNodes);
    QFETCH(int, nodeId);
    //QFETCH(Pattern*, pat);
    QFETCH(bool, expMatch);


}

void Tests::testMatchesPattern_data()
{
    QTest::addColumn<QMap<int, UDNode*>>("inputNodes");
    QTest::addColumn<int>("nodeId");
    QTest::addColumn<Pattern*>("pat");
    QTest::addColumn<bool>("expMatch");

    QMap<int, UDNode*> tree;
    //Тест №1. Узел без детей.
    Pattern* pattern1 = new Pattern({},{});
    tree.clear();
    UDNode* node1_0 = new UDNode(1,"Hello",UH,0,Root,None);
    tree.insert(1,node1_0);
    QTest::newRow("Test 1: Alone word") << tree << 0 << pattern1 << true;


}*/
