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
    Mistake personNumberMistake(QString("глагол несогласован с подлежащим. Глагол должен быть в  начальной форме/форме 3-го лица единственного числа."));
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
    mistakeSet.insert(personNumberMistake);
    QTest::newRow("Test 4: mistake, subject and verb in subjunctive but 3rd singular")
        << node4_1 << node4_2 << false << mistakeSet;

    // ТЕСТ 5 - Есть ошибка, подлежащее в 3 лице ед.числе, а глагол нет
    const UDNode* node5_1 = new UDNode("he", PRP, None);
    const UDNode* node5_2 = new UDNode("run", VBP, Ind);
    mistakeSet.clear();
    mistakeSet.insert(personNumberMistake);
    QTest::newRow("Test 5: mistake, subject is 3rd singular but verb is not")
        << node5_1 << node5_2 << false << mistakeSet;

    // ТЕСТ 6 - Есть ошибка, подлежащее не в 3 лице ед.числе, а глагол да
    const UDNode* node6_1 = new UDNode("he", PRP, None);
    const UDNode* node6_2 = new UDNode("run", VBZ, Ind);
    mistakeSet.clear();
    mistakeSet.insert(personNumberMistake);
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
    mistakeSet.insert(personNumberMistake);
    QTest::newRow("Test 11: mistake with 'I'")
        << node11_1 << node11_2 << false << mistakeSet;

    const UDNode* node12_1 = new UDNode("we", PRP, None);
    const UDNode* node12_2 = new UDNode("runs", VBZ, Ind);
    QTest::newRow("Test 12: mistake with 'we'")
        << node12_1 << node12_2 << false << mistakeSet;

    const UDNode* node13_1 = new UDNode("you", PRP, None);
    const UDNode* node13_2 = new UDNode("runs", VBZ, Ind);
    QTest::newRow("Test 13: mistake with 'you'")
        << node13_1 << node13_2 << false << mistakeSet;

    const UDNode* node14_1 = new UDNode("they", PRP, None);
    const UDNode* node14_2 = new UDNode("runs", VBZ, Ind);
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
    const UDNode* node18_2 = new UDNode("run", VBP, Ind);
    mistakeSet.clear();
    mistakeSet.insert(personNumberMistake);
    QTest::newRow("Test 18: mistake with 'he'")
        << node18_1 << node18_2 << false << mistakeSet;

    const UDNode* node19_1 = new UDNode("she", PRP, None);
    const UDNode* node19_2 = new UDNode("run", VBP, Ind);
    QTest::newRow("Test 19: mistake with 'she'")
        << node19_1 << node19_2 << false << mistakeSet;

    const UDNode* node20_1 = new UDNode("it", PRP, None);
    const UDNode* node20_2 = new UDNode("run", VBP, Ind);
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
    mistakeSet.insert(personNumberMistake);
    QTest::newRow("Test 25: mistake with plural noun")
        << node25_1 << node25_2 << false << mistakeSet;

    // ТЕСТ 26 - Существительное 3 лица ед. числа с ошибкой
    const UDNode* node26_1 = new UDNode("dog", NN, None);
    const UDNode* node26_2 = new UDNode("run", VBP, Ind);
    mistakeSet.clear();
    mistakeSet.insert(personNumberMistake);
    QTest::newRow("Test 26: mistake with singular noun")
        << node26_1 << node26_2 << false << mistakeSet;
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
    Mistake numberMistake(QString("Слова не согласованы по числу"));

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
    mistakeSet.insert(numberMistake);
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
    mistakeSet.insert(numberMistake);
    QTest::newRow("Test 23: Mistake with 'a' + plural noun")
        << node23_1 << node23_2 << false << mistakeSet;

    const UDNode* node24_1 = new UDNode("an", DT, None);
    const UDNode* node24_2 = new UDNode("apples", NNS, None);
    mistakeSet.clear();
    mistakeSet.insert(numberMistake);
    QTest::newRow("Test 24: Mistake with 'an' + plural noun")
        << node24_1 << node24_2 << false << mistakeSet;

    const UDNode* node25_1 = new UDNode("one", DT, None);
    const UDNode* node25_2 = new UDNode("apples", NNS, None);
    mistakeSet.clear();
    mistakeSet.insert(numberMistake);
    QTest::newRow("Test 25: Mistake with 'one' + plural noun")
        << node25_1 << node25_2 << false << mistakeSet;

    // ТЕСТ 26 - Числительное мн. числа
    const UDNode* node26_1 = new UDNode("two", CD, None);
    const UDNode* node26_2 = new UDNode("books", NNS, None);
    QTest::newRow("Test 26: No mistake with plural number + plural noun")
        << node26_1 << node26_2 << true << emptyMistakes;
};
