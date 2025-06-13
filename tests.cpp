#include "tests.h"
#include <QDebug>
void Tests::testPersonNumberCheck()
{
    QFETCH(const UDNode*, inputNode1);
    QFETCH(const UDNode*, inputNode2);
    QFETCH(bool, expAgreement);
    QFETCH(QSet<Mistake>, expMistakes);
    QFETCH(bool, expectException);
    QFETCH(QString, exceptionMessage);

    QSet<Mistake> mistakes;
    PersonNumberAgreement gRule;
    bool agreement = false;
    bool exceptionThrown = false;
    QString actualExceptionMessage;

    try {
        agreement = gRule.check(inputNode1, inputNode2, mistakes);
    }
    catch (const QString& e) {
        exceptionThrown = true;
        actualExceptionMessage = e;
    }
    catch (...) {
        exceptionThrown = true;
        actualExceptionMessage = "Unexpected exception type";
    }

    // Проверка исключений
    if (expectException) {
        QVERIFY2(exceptionThrown, "Expected an exception, but none was thrown");
        QCOMPARE(actualExceptionMessage, exceptionMessage);
    } else {
        QVERIFY2(!exceptionThrown, QString("Unexpected exception thrown: %1").arg(actualExceptionMessage).toUtf8());

        // Сравнение результата
        QCOMPARE(agreement, expAgreement);

        // Подробное сравнение ошибок
        if (mistakes != expMistakes) {
            qDebug() << "Discrepancy found in mistakes:";

            QSet<Mistake> unexpectedMistakes = mistakes - expMistakes;
            if (!unexpectedMistakes.isEmpty()) {
                qDebug() << "Unexpected mistakes found (" << unexpectedMistakes.size() << "):";
                for (const Mistake& mistake : unexpectedMistakes) {
                    qDebug() << "  -" << mistake.getMessage();
                }
            }

            QSet<Mistake> missingMistakes = expMistakes - mistakes;
            if (!missingMistakes.isEmpty()) {
                qDebug() << "Missing expected mistakes (" << missingMistakes.size() << "):";
                for (const Mistake& mistake : missingMistakes) {
                    qDebug() << "  +" << mistake.getMessage();
                }
            }
        }
        QCOMPARE(mistakes, expMistakes);
    }
}

void Tests::testPersonNumberCheck_data()
{
    // Добавляем столбцы

    QTest::addColumn<const UDNode*>("inputNode1");
    QTest::addColumn<const UDNode*>("inputNode2");
    QTest::addColumn<bool>("expAgreement");
    QTest::addColumn<QSet<Mistake>>("expMistakes");
    QTest::addColumn<bool>("expectException");
    QTest::addColumn<QString>("exceptionMessage");

    QSet<Mistake> emptyMistakes;         // Множество ошибок для тестов, где слова согласованы
    QSet<Mistake> mistakeSet;    // Множество для хранения ошибок

    // ТЕСТ 1 - Нет ошибки, подлежащее и глагол в 3 лице, ед.числе

    const UDNode* node1_1 = new UDNode("he",PRP,None);
    const UDNode* node1_2 = new UDNode("runs",VBZ,None);
    QTest::newRow("Test 1: no mistake, subject and verb are in the 3rd person, singular")
        << node1_1 << node1_2 << true << emptyMistakes << false << "";

    // ТЕСТ 2 - Нет ошибки, подлежащее и глагол не в 3 лице, ед.числе
    const UDNode* node2_1 = new UDNode("I", PRP, None);
    const UDNode* node2_2 = new UDNode("run", VBP, Ind);
    QTest::newRow("Test 2: no mistake, subject and verb are not in the 3rd person singular")
        << node2_1 << node2_2 << true << emptyMistakes << false << "";

    // ТЕСТ 3- Есть ошибка, подлежащее и глагол в Subjunctive в 3 лице ед.числе
    const UDNode* node4_1 = new UDNode("he", PRP, None);
    const UDNode* node4_2 = new UDNode("is", VBZ, Subj);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Глагол is в неправильной форме. В Present Subjunctive используется базовая форма")));
    QTest::newRow("Test 3: mistake, subject and verb in subjunctive but 3rd singular")
        << node4_1 << node4_2 << false << mistakeSet<< false << "";

    // ТЕСТ 4- Есть ошибка, подлежащее в 3 лице ед.числе, а глагол нет
    const UDNode* node5_1 = new UDNode("he", PRP, None);
    const UDNode* node5_2 = new UDNode("run", VBP, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Основной глагол run несогласован с подлежащим he. Глагол должен быть в форме 3-го лица ед. числа")));
    QTest::newRow("Test 4: mistake, subject is 3rd singular but verb is not")
        << node5_1 << node5_2 << false << mistakeSet<< false << "";

    // ТЕСТ 5 - Есть ошибка, подлежащее не в 3 лице ед.числе, а глагол да
    const UDNode* node6_1 = new UDNode("you", PRP, None);
    const UDNode* node6_2 = new UDNode("runs", VBZ, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Основной глагол runs несогласован с подлежащим you. Глагол должен быть в начальной форме")));
    QTest::newRow("Test 5: error, subject is not 3rd singular but verb is")
        << node6_1 << node6_2 << false << mistakeSet<< false << "";

    // ТЕСТ 6-9 - Личные местоимения не 3 лица ед. числа (I, we, you, they) без ошибок
    const UDNode* node7_1 = new UDNode("I", PRP, None);
    const UDNode* node7_2 = new UDNode("run", VBP, Ind);
    QTest::newRow("Test 6: no mistake with 'I'")
        << node7_1 << node7_2 << true << emptyMistakes<< false << "";

    const UDNode* node8_1 = new UDNode("we", PRP, None);
    const UDNode* node8_2 = new UDNode("run", VBP, Ind);
    QTest::newRow("Test 7: no mistake with 'we'")
        << node8_1 << node8_2 << true << emptyMistakes<< false << "";

    const UDNode* node9_1 = new UDNode("you", PRP, None);
    const UDNode* node9_2 = new UDNode("run", VBP, Ind);
    QTest::newRow("Test 8: no mistake with 'you'")
        << node9_1 << node9_2 << true << emptyMistakes<< false << "";

    const UDNode* node10_1 = new UDNode("they", PRP, None);
    const UDNode* node10_2 = new UDNode("run", VBP, Ind);
    QTest::newRow("Test 9: no mistake with 'they'")
        << node10_1 << node10_2 << true << emptyMistakes<< false << "";

    // ТЕСТ 10-13 - Личные местоимения не 3 лица ед. числа (I, we, you, they) с ошибками
    const UDNode* node11_1 = new UDNode("I", PRP, None);
    const UDNode* node11_2 = new UDNode("runs", VBZ, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Основной глагол runs несогласован с подлежащим I. Глагол должен быть в начальной форме")));
    QTest::newRow("Test 10: mistake with 'I'")
        << node11_1 << node11_2 << false << mistakeSet<< false << "";

    const UDNode* node12_1 = new UDNode("we", PRP, None);
    const UDNode* node12_2 = new UDNode("runs", VBZ, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Основной глагол runs несогласован с подлежащим we. Глагол должен быть в начальной форме")));
    QTest::newRow("Test 11: mistake with 'we'")
        << node12_1 << node12_2 << false << mistakeSet<< false << "";

    const UDNode* node13_1 = new UDNode("you", PRP, None);
    const UDNode* node13_2 = new UDNode("runs", VBZ, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Основной глагол runs несогласован с подлежащим you. Глагол должен быть в начальной форме")));
    QTest::newRow("Test 12: mistake with 'you'")
        << node13_1 << node13_2 << false << mistakeSet<< false << "";

    const UDNode* node14_1 = new UDNode("they", PRP, None);
    const UDNode* node14_2 = new UDNode("runs", VBZ, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Основной глагол runs несогласован с подлежащим they. Глагол должен быть в начальной форме")));
    QTest::newRow("Test 13: mistake with 'they'")
        << node14_1 << node14_2 << false << mistakeSet<< false << "";

    // ТЕСТ 14-16 - Личные местоимения 3 лица ед. числа (he, she, it) без ошибок
    const UDNode* node15_1 = new UDNode("he", PRP, None);
    const UDNode* node15_2 = new UDNode("runs", VBZ, Ind);
    QTest::newRow("Test 14: no mistake with 'he'")
        << node15_1 << node15_2 << true << emptyMistakes<< false << "";

    const UDNode* node16_1 = new UDNode("she", PRP, None);
    const UDNode* node16_2 = new UDNode("runs", VBZ, Ind);
    QTest::newRow("Test 15: no mistake with 'she'")
        << node16_1 << node16_2 << true << emptyMistakes<< false << "";

    const UDNode* node17_1 = new UDNode("it", PRP, None);
    const UDNode* node17_2 = new UDNode("runs", VBZ, Ind);
    QTest::newRow("Test 16: no mistake with 'it'")
        << node17_1 << node17_2 << true << emptyMistakes<< false << "";

    // ТЕСТ 17-19 - Личные местоимения 3 лица ед. числа (he, she, it) с ошибками
    const UDNode* node18_1 = new UDNode("he", PRP, None);
    const UDNode* node18_2 = new UDNode("are", VBP, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Вспомогательный глагол are несогласован с подлежащим he. Глагол должен быть в форме в форме 3-го числа ед.числа")));
    QTest::newRow("Test 17: mistake with 'he'")
        << node18_1 << node18_2 << false << mistakeSet<< false << "";

    const UDNode* node19_1 = new UDNode("she", PRP, None);
    const UDNode* node19_2 = new UDNode("run", VBP, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Основной глагол run несогласован с подлежащим she. Глагол должен быть в начальной форме")));
    QTest::newRow("Test 18: mistake with 'she'")
        << node19_1 << node19_2 << false << mistakeSet<< false << "";

    const UDNode* node20_1 = new UDNode("it", PRP, None);
    const UDNode* node20_2 = new UDNode("run", VBP, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Основной глагол run несогласован с подлежащим it. Глагол должен быть в начальной форме")));
    QTest::newRow("Test 19: mistake with 'it'")
        << node20_1 << node20_2 << false << mistakeSet<< false << "";

    // ТЕСТ 20-21 - Существительные не 3 лица ед. числа (теги NNS, NNPS) без ошибок
    const UDNode* node21_1 = new UDNode("horses", NNS, None);
    const UDNode* node21_2 = new UDNode("run", VBP, Ind);
    QTest::newRow("Test 20: no mistake with plural noun (NNS)")
        << node21_1 << node21_2 << true << emptyMistakes<< false << "";

    const UDNode* node22_1 = new UDNode("Speedsters", NNPS, None);
    const UDNode* node22_2 = new UDNode("run", VBP, Ind);
    QTest::newRow("Test 21: no mistake with plural proper noun (NNPS)")
        << node22_1 << node22_2 << true << emptyMistakes<< false << "";

    // ТЕСТ 22-23 - Существительные 3 лица ед. числа (теги NN, NNP) без ошибок
    const UDNode* node23_1 = new UDNode("dog", NN, None);
    const UDNode* node23_2 = new UDNode("runs", VBZ, Ind);
    QTest::newRow("Test 22: no mistake with singular noun (NN)")
        << node23_1 << node23_2 << true << emptyMistakes<< false << "";

    const UDNode* node24_1 = new UDNode("Bolt", NNP, None);
    const UDNode* node24_2 = new UDNode("runs", VBZ, Ind);
    QTest::newRow("Test 23: no mistake with singular proper noun (NNP)")
        << node24_1 << node24_2 << true << emptyMistakes<< false << "";

    // ТЕСТ 24 - Существительное не 3 лица ед. числа с ошибкой
    const UDNode* node25_1 = new UDNode("dogs", NNS, None);
    const UDNode* node25_2 = new UDNode("runs", VBZ, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Основной глагол runs несогласован с подлежащим dogs. Глагол должен быть в начальной форме")));
    QTest::newRow("Test 24: mistake with plural noun")
        << node25_1 << node25_2 << false << mistakeSet<< false << "";

    // ТЕСТ 25 - Существительное 3 лица ед. числа с ошибкой
    const UDNode* node26_1 = new UDNode("dog", NN, None);
    const UDNode* node26_2 = new UDNode("run", VBP, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Основной глагол run несогласован с подлежащим dog. Глагол должен быть в форме 3-го числа ед.числа")));
    QTest::newRow("Test 25: mistake with singular noun")
        << node26_1 << node26_2 << false << mistakeSet<< false << "";

    // ТЕСТ 26-27 - Глагол have в настоящих временах
    const UDNode* node27_1 = new UDNode("I", NN, None);
    const UDNode* node27_2 = new UDNode("have", VBP, Ind);
    QTest::newRow("Test 26: no mistake I + have in present")
        << node27_1 << node27_2 << true << emptyMistakes<< false << "";

    const UDNode* node28_1 = new UDNode("he", NN, None);
    const UDNode* node28_2 = new UDNode("has", VBZ, Ind);
    QTest::newRow("Test 27: no mistake he + has in present")
        << node28_1 << node28_2 << true << emptyMistakes<< false << "";

    // ТЕСТ 28 - Существительное 3 лица ед. числа с ошибкой
    const UDNode* node29_1 = new UDNode("I", NN, None);
    const UDNode* node29_2 = new UDNode("has", VBZ, Ind);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Вспомогательный глагол has несогласован с подлежащим I. Глагол должен быть в форме в начальной форме")));
    QTest::newRow("Test 28: mistake I + has")
        << node29_1 << node29_2 << false << mistakeSet<< false << "";

    // ТЕСТ 29 - Глагол have в будущих временах(всегда форма have)
    UDNode * node30_0 = new UDNode(4,QString("written"),VBN,0,Root,Ind);
    UDNode * nodeWill = new UDNode(2,QString("will"),MD,4,Aux,Ind);
    node30_0->addChild(nodeWill);
    const UDNode* node30_1 = new UDNode(1,"he", NN, 4,Nsubj,None);
    const UDNode* node30_2 = new UDNode(3,"has", VBZ,4,Aux,Ind);
    node30_0->addChild(node30_1);
    node30_0->addChild(node30_2);
    mistakeSet.clear();
    mistakeSet.insert(Mistake(QString("Неправильная форма глагола have. в будущем времени употребляется только форма have")));
    QTest::newRow("Test 29: mistake he + has in future")
        << node30_1 << node30_2 << false << mistakeSet<< false << "";

    // ТЕСТ 30-32 Ошибочный вызов, неверные части речи
    const UDNode* node31_1 = new UDNode("hello", UH, None);
    const UDNode* node31_2 = new UDNode("goes", VBZ, Ind);
    QTest::newRow("Test 30: exception invalid tag 1st")
        << node31_1 << node31_2 << true << emptyMistakes<< true << "Invalid part of speech";

    const UDNode* node32_1 = new UDNode("I", PRP, None);
    const UDNode* node32_2 = new UDNode("seen", VBN, Ind);
    QTest::newRow("Test 31: exception invalid tag 2nd")
        << node32_1 << node32_2 << true << emptyMistakes<< true << "Invalid part of speech";

    const UDNode* node33_1 = new UDNode("hello", UH, None);
    const UDNode* node33_2 = new UDNode("there", RB, Ind);
    QTest::newRow("Test 32: exception invalid tag 1st and 2nd")
        << node33_1 << node33_2 << true << emptyMistakes<< true << "Invalid part of speech";

    // ТЕСТ 33-34 Ошибочный вызов, нулевой указатель
    const UDNode* node34_1 = nullptr;
    const UDNode* node34_2 = new UDNode("goes", VBZ, Ind);
    QTest::newRow("Test 33: exception nullptr 1st")
        << node34_1 << node34_2 << true << emptyMistakes<< true << "Node pointer is null";

    const UDNode* node35_1 = new UDNode("hello", UH, None);
    const UDNode* node35_2 = nullptr;
    QTest::newRow("Test 34: exception nullptr 2nd")
        << node35_1 << node35_2 << true << emptyMistakes<< true << "Node pointer is null";

}

void Tests::testNumberCheck()
{
    QFETCH(const UDNode*, inputNode1);
    QFETCH(const UDNode*, inputNode2);
    QFETCH(bool, expAgreement);
    QFETCH(QSet<Mistake>, expMistakes);
    QFETCH(bool, expectException);
    QFETCH(QString, exceptionMessage);

    QSet<Mistake> mistakes;
    NumberAgreement gRule;
    bool agreement = false;
    bool exceptionThrown = false;
    QString actualExceptionMessage;

    try {
        agreement = gRule.check(inputNode1, inputNode2, mistakes);
    }
    catch (const QString& e) {
        exceptionThrown = true;
        actualExceptionMessage = e;
    }
    catch (...) {
        exceptionThrown = true;
        actualExceptionMessage = "Unexpected exception type";
    }

    // Проверка исключений
    if (expectException) {
        QVERIFY2(exceptionThrown, "Expected an exception, but none was thrown");
        QCOMPARE(actualExceptionMessage, exceptionMessage);
    } else {
        QVERIFY2(!exceptionThrown, QString("Unexpected exception thrown: %1").arg(actualExceptionMessage).toUtf8());

        // Сравнение результата
        QCOMPARE(agreement, expAgreement);

        // Подробное сравнение ошибок
        if (mistakes != expMistakes) {
            qDebug() << "Discrepancy found in mistakes:";

            QSet<Mistake> unexpectedMistakes = mistakes - expMistakes;
            if (!unexpectedMistakes.isEmpty()) {
                qDebug() << "Unexpected mistakes found (" << unexpectedMistakes.size() << "):";
                for (const Mistake& mistake : unexpectedMistakes) {
                    qDebug() << "  -" << mistake.getMessage();
                }
            }

            QSet<Mistake> missingMistakes = expMistakes - mistakes;
            if (!missingMistakes.isEmpty()) {
                qDebug() << "Missing expected mistakes (" << missingMistakes.size() << "):";
                for (const Mistake& mistake : missingMistakes) {
                    qDebug() << "  +" << mistake.getMessage();
                }
            }
        }
        QCOMPARE(mistakes, expMistakes);
    }
};

void Tests::testNumberCheck_data()
{
    // Добавляем столбцы

    QTest::addColumn<const UDNode*>("inputNode1");
    QTest::addColumn<const UDNode*>("inputNode2");
    QTest::addColumn<bool>("expAgreement");
    QTest::addColumn<QSet<Mistake>>("expMistakes");
    QTest::addColumn<bool>("expectException");
    QTest::addColumn<QString>("exceptionMessage");

    QSet<Mistake> emptyMistakes;         // Множество ошибок для тестов, где слова согласованы
    QSet<Mistake> mistakeSet;                // Множество для хранения ошибок


    // Тест 1: Нет ошибки, 1 и 2 слово в ед. числе
    const UDNode* node1_1 = new UDNode("I", PRP, None);
    const UDNode* node1_2 = new UDNode("was", VBD, Ind);
    QTest::newRow("Test 1: No mistake, 1st and 2nd words are singular") << node1_1 << node1_2 << true << emptyMistakes << false << "";

    // Тест 2: Нет ошибки, 1 и 2 слово во мн. числе
    const UDNode* node2_1 = new UDNode("they", PRP, None);
    const UDNode* node2_2 = new UDNode("were", VBD, Ind);
    QTest::newRow("Test 2: No mistake, 1st and 2nd words are plural") << node2_1 << node2_2 << true << emptyMistakes << false << "";

    // Тест 3: Нет ошибки, 1 слово совместимо с любым числом и 2 слово во мн. числе
    const UDNode* node3_1 = new UDNode("the", DT, None);
    const UDNode* node3_2 = new UDNode("apples", NNS, None);
    QTest::newRow("Test 3: No mistake, 1st word compatible with any number") << node3_1 << node3_2 << true << emptyMistakes << false << "";

    // Тест 4: Есть ошибка - 1 слово в ед. числе, а второе во мн. числе
    const UDNode* node4_1 = new UDNode("he", PRP, None);
    const UDNode* node4_2 = new UDNode("were", VBD, Ind);
    mistakeSet.insert(Mistake(QString("глагол were несогласован с подлежащим I. Глагол должен быть в форме единственного числа.")));
    QTest::newRow("Test 4: Mistake, 1st word singular, 2nd plural") << node4_1 << node4_2 << false << mistakeSet << false << "";
    mistakeSet.clear();


    // Тесты 5-11: Subjunctive mood (I, he, she, it, we, they, you) - correct
    const UDNode* node5_1 = new UDNode("I", PRP, None);
    const UDNode* node5_2 = new UDNode("were", VBD, Subj);
    QTest::newRow("Test 5: Subjunctive mood (I were)") << node5_1 << node5_2 << true << emptyMistakes << false << "";

    const UDNode* node6_1 = new UDNode("he", PRP, None);
    const UDNode* node6_2 = new UDNode("were", VBD, Subj);
    QTest::newRow("Test 6: Subjunctive mood (he were)") << node6_1 << node6_2 << true << emptyMistakes << false << "";

    const UDNode* node7_1 = new UDNode("she", PRP, None);
    const UDNode* node7_2 = new UDNode("were", VBD,  Subj);
    QTest::newRow("Test 7: Subjunctive mood (she were)") << node7_1 << node7_2 << true << emptyMistakes << false << "";

    const UDNode* node8_1 = new UDNode("it", PRP, None);
    const UDNode* node8_2 = new UDNode("were", VBD,  Subj);
    QTest::newRow("Test 8: Subjunctive mood (it were)") << node8_1 << node8_2 << true << emptyMistakes << false << "";

    const UDNode* node9_1 = new UDNode("we", PRP, None);
    const UDNode* node9_2 = new UDNode("were", VBD, Subj);
    QTest::newRow("Test 9: Subjunctive mood (we were)") << node9_1 << node9_2 << true << emptyMistakes << false << "";

    const UDNode* node10_1 = new UDNode("they", PRP, None);
    const UDNode* node10_2 = new UDNode("were", VBD,  Subj);
    QTest::newRow("Test 10: Subjunctive mood (they were)") << node10_1 << node10_2 << true << emptyMistakes << false << "";

    const UDNode* node11_1 = new UDNode("you", PRP, None);
    const UDNode* node11_2 = new UDNode("were", VBD,  Subj);
    QTest::newRow("Test 11: Subjunctive mood (you were)") << node11_1 << node11_2 << true << emptyMistakes << false << "";

    // Тесты 12-15: Subjunctive mood - ошибка (I, he, she, it)
    const UDNode* node12_1 = new UDNode("I", PRP, None);
    const UDNode* node12_2 = new UDNode("was", VBD,  Subj);
    mistakeSet.insert(Mistake(QString("глагол was не используется в Past Subjunctive. Используется were для всех лиц (даже с he/she/it).")));
    QTest::newRow("Test 12: Subjunctive mood error (I was)") << node12_1 << node12_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node13_1 = new UDNode("he", PRP, None);
    const UDNode* node13_2 = new UDNode("was", VBD, Subj);
    mistakeSet.insert(Mistake(QString("глагол was не используется в Past Subjunctive. Используется were для всех лиц (даже с he/she/it).")));
    QTest::newRow("Test 13: Subjunctive mood error (he was)") << node13_1 << node13_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node14_1 = new UDNode("she", PRP, None);
    const UDNode* node14_2 = new UDNode("was", VBD, Subj);
    mistakeSet.insert(Mistake(QString("глагол was не используется вPast Subjunctive. Используется were для всех лиц (даже с he/she/it).")));
    QTest::newRow("Test 14: Subjunctive mood error (she was)") << node14_1 << node14_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node15_1 = new UDNode("it", PRP, None);
    const UDNode* node15_2 = new UDNode("was", VBD, Subj);
    mistakeSet.insert(Mistake(QString("глагол was не используется в Past Subjunctive. Используется were для всех лиц (даже с he/she/it).")));
    QTest::newRow("Test 15: Subjunctive mood error (it was)") << node15_1 << node15_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    // Тесты 16-19: Личные местоимения ед.числа (I, he, she, it)
    const UDNode* node16_1 = new UDNode("I", PRP, None);
    const UDNode* node16_2 = new UDNode("was", VBD, Ind);
    QTest::newRow("Test 16: Singular pronoun (I was)") << node16_1 << node16_2 << true << emptyMistakes << false << "";

    const UDNode* node17_1 = new UDNode("he", PRP, None);
    const UDNode* node17_2 = new UDNode("was", VBD, Ind);
    QTest::newRow("Test 17: Singular pronoun (he was)") << node17_1 << node17_2 << true << emptyMistakes << false << "";

    const UDNode* node18_1 = new UDNode("she", PRP, None);
    const UDNode* node18_2 = new UDNode("was", VBD, Ind);
    QTest::newRow("Test 18: Singular pronoun (she was)") << node18_1 << node18_2 << true << emptyMistakes << false << "";

    const UDNode* node19_1 = new UDNode("it", PRP, None);
    const UDNode* node19_2 = new UDNode("was", VBD, Ind);
    QTest::newRow("Test 19: Singular pronoun (it was)") << node19_1 << node19_2 << true << emptyMistakes << false << "";

    // Тесты 20-22: Личные местоимения мн.числа (you, we, they)
    const UDNode* node20_1 = new UDNode("you", PRP, None);
    const UDNode* node20_2 = new UDNode("were", VBD, Ind);
    QTest::newRow("Test 20: Plural pronoun (you were)") << node20_1 << node20_2 << true << emptyMistakes << false << "";

    const UDNode* node21_1 = new UDNode("we", PRP, None);
    const UDNode* node21_2 = new UDNode("were", VBD, Ind);
    QTest::newRow("Test 21: Plural pronoun (we were)") << node21_1 << node21_2 << true << emptyMistakes << false << "";

    const UDNode* node22_1 = new UDNode("they", PRP, None);
    const UDNode* node22_2 = new UDNode("were", VBD, Ind);
    QTest::newRow("Test 22: Plural pronoun (they were)") << node22_1 << node22_2 << true << emptyMistakes << false << "";

    // Тесты 23-24: Существительное ед.числа (NN, NNP)
    const UDNode* node23_1 = new UDNode("cat", NN, None);
    const UDNode* node23_2 = new UDNode("was", VBD, Ind);
    QTest::newRow("Test 23: Singular noun (cat was)") << node23_1 << node23_2 << true << emptyMistakes << false << "";

    const UDNode* node24_1 = new UDNode("John", NNP, None);
    const UDNode* node24_2 = new UDNode("was", VBD, Ind);
    QTest::newRow("Test 24: Singular proper noun (John was)") << node24_1 << node24_2 << true << emptyMistakes << false << "";


    // Тесты 25-26: Существительное мн.числа
    const UDNode* node25_1 = new UDNode("cats", NNS, None);
    const UDNode* node25_2 = new UDNode("were", VBD, Ind);
    QTest::newRow("Test 25: Plural noun (cats were)") << node25_1 << node25_2 << true << emptyMistakes << false << "";

    const UDNode* node26_1 = new UDNode("Rockies", NNPS, None);
    const UDNode* node26_2 = new UDNode("were", VBD, Ind);
    QTest::newRow("Test 26: Plural proper noun (Rockies were)") << node26_1 << node26_2 << true << emptyMistakes << false << "";

    // Тесты 27-28: The как с ед. числом, так и с мн. числом
    const UDNode* node27_1 = new UDNode("the", DT, None);
    const UDNode* node27_2 = new UDNode("cat", NN, None);
    QTest::newRow("Test 27: 'the' with singular noun") << node27_1 << node27_2 << true << emptyMistakes << false << "";

    const UDNode* node28_1 = new UDNode("the", DT, None);
    const UDNode* node28_2 = new UDNode("cats", NNS, None);
    QTest::newRow("Test 28: 'the' with plural noun") << node28_1 << node28_2 << true << emptyMistakes << false << "";

    // Тесты 29-31: A, an, one c ед. числом без ошибок
    const UDNode* node29_1 = new UDNode("a", DT, None);
    const UDNode* node29_2 = new UDNode("bottle", NN, None);
    QTest::newRow("Test 29: 'a' with singular noun") << node29_1 << node29_2 << true << emptyMistakes << false << "";

    const UDNode* node30_1 = new UDNode("an", DT, None);
    const UDNode* node30_2 = new UDNode("apple", NN, None);
    QTest::newRow("Test 30: 'an' with singular noun") << node30_1 << node30_2 << true << emptyMistakes << false << "";

    const UDNode* node31_1 = new UDNode("one", CD, None);
    const UDNode* node31_2 = new UDNode("bottle", NN, None);
    QTest::newRow("Test 31: 'one' with singular noun") << node31_1 << node31_2 << true << emptyMistakes << false << "";

    // Тесты 32-34: A, an, one c мн. числом (ошибка)
    const UDNode* node32_1 = new UDNode("a", DT, None);
    const UDNode* node32_2 = new UDNode("bottles", NNS, None);
    mistakeSet.insert(Mistake(QString("артикль a употреблен ссуществительным bottles множественного числа. Требуется артикль the или нулевой артикль.")));
    QTest::newRow("Test 32: 'a' with plural noun (mistake)") << node32_1 << node32_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node33_1 = new UDNode("an", DT, None);
    const UDNode* node33_2 = new UDNode("apples", NNS, None);
    mistakeSet.insert(Mistake(QString("артикль an употреблен ссуществительным bottles множественного числа. Требуется артикль the или нулевой артикль.")));
    QTest::newRow("Test 33: 'an' with plural noun (mistake)") << node33_1 << node33_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node34_1 = new UDNode("one", CD, None);
    const UDNode* node34_2 = new UDNode("bottles", NNS, None);
    mistakeSet.insert(Mistake(QString("существительное apples несогласовано по числу с числительным one. Существительное должно быть в форме единственного числа.")));
    QTest::newRow("Test 34: 'one' with plural noun (mistake)") << node34_1 << node34_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    // Тест 35: Числительное мн. числа
    const UDNode* node35_1 = new UDNode("two", CD, None);
    const UDNode* node35_2 = new UDNode("apples", NNS, None);
    QTest::newRow("Test 35: Plural number with plural noun") << node35_1 << node35_2 << true << emptyMistakes << false << "";

    // Тест 36: Числительное мн. числа с ошибкой
    const UDNode* node36_1 = new UDNode("two", CD, None);
    const UDNode* node36_2 = new UDNode("bottle", NN, None);
    mistakeSet.insert(Mistake(QString("существительное bottle несогласовано по числу с числительным two. Существительное должно быть в форме множественного числа.")));
    QTest::newRow("Test 36: Plural number with singular noun (mistake)") << node36_1 << node36_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    // Тесты 37-41: Личные местоимения и существительные ед. числа ошибки
    const UDNode* node37_1 = new UDNode("he", PRP, None);
    const UDNode* node37_2 = new UDNode("were", VBD, Ind);
    mistakeSet.insert(Mistake(QString("глагол were несогласован с подлежащим he. Глагол должен быть в форме единственного числа.")));
    QTest::newRow("Test 37: Singular pronoun with plural verb (mistake)") << node37_1 << node37_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node38_1 = new UDNode("she", PRP, None);
    const UDNode* node38_2 = new UDNode("were", VBD, Ind);
    mistakeSet.insert(Mistake(QString("глагол were несогласован с подлежащим she. Глагол должен быть в форме единственного числа.")));
    QTest::newRow("Test 38: Singular pronoun with plural verb (mistake)") << node38_1 << node38_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node39_1 = new UDNode("it", PRP, None);
    const UDNode* node39_2 = new UDNode("were", VBD, Ind);
    mistakeSet.insert(Mistake(QString("глагол were несогласован с подлежащим it. Глагол должен быть в форме единственного числа.")));
    QTest::newRow("Test 39: Singular pronoun with plural verb (mistake)") << node39_1 << node39_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node40_1 = new UDNode("cat", NN, None);
    const UDNode* node40_2 = new UDNode("were", VBD, Ind);
    mistakeSet.insert(Mistake(QString("глагол were несогласован с подлежащим cat. Глагол должен быть в форме единственного числа.")));
    QTest::newRow("Test 40: Singular noun with plural verb (mistake)") << node40_1 << node40_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node41_1 = new UDNode("John", NNP, None);
    const UDNode* node41_2 = new UDNode("were", VBD, Ind);
    mistakeSet.insert(Mistake(QString("глагол were несогласован с подлежащим John. Глагол должен быть в форме единственного числа.")));
    QTest::newRow("Test 41: Singular proper noun with plural verb (mistake)") << node41_1 << node41_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    // Тесты 42-46: Личные местоимения и существительные мн. числа ошибки
    const UDNode* node42_1 = new UDNode("we", PRP, None);
    const UDNode* node42_2 = new UDNode("was", VBD, Ind);
    mistakeSet.insert(Mistake(QString("глагол was несогласован с подлежащим we. Глагол должен быть в форме множественного числа.")));
    QTest::newRow("Test 42: Plural pronoun with singular verb (mistake)") << node42_1 << node42_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node43_1 = new UDNode("they", PRP, None);
    const UDNode* node43_2 = new UDNode("was", VBD, Ind);
    mistakeSet.insert(Mistake(QString("глагол was несогласован с подлежащим they. Глагол должен быть в форме множественного числа.")));
    QTest::newRow("Test 43: Plural pronoun with singular verb (mistake)") << node43_1 << node43_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node44_1 = new UDNode("you", PRP, None);
    const UDNode* node44_2 = new UDNode("was", VBD, Ind);
    mistakeSet.insert(Mistake(QString("глагол was несогласован с подлежащим you. Глагол должен быть в форме множественного числа.")));
    QTest::newRow("Test 44: Plural pronoun with singular verb (mistake)") << node44_1 << node44_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node45_1 = new UDNode("cats", NNS, None);
    const UDNode* node45_2 = new UDNode("was", VBD, Ind);
    mistakeSet.insert(Mistake(QString("глагол was несогласован с подлежащим cats. Глагол должен быть в форме множественного числа.")));
    QTest::newRow("Test 45: Plural noun with singular verb (mistake)") << node45_1 << node45_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node46_1 = new UDNode("Rockies", NNPS, None);
    const UDNode* node46_2 = new UDNode("was", VBD, Ind);
    mistakeSet.insert(Mistake(QString("глагол was несогласован с подлежащим Rockies. Глагол должен быть в форме множественного числа.")));
    QTest::newRow("Test 46: Plural proper noun with singular verb (mistake)") << node46_1 << node46_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    // Тесты 47-48: Указательные местоимения для ед. числа (this/that)
    const UDNode* node47_1 = new UDNode("this", DT, None);
    const UDNode* node47_2 = new UDNode("apple", NN, None);
    QTest::newRow("Test 47: 'this' with singular noun") << node47_1 << node47_2 << true << emptyMistakes << false << "";

    const UDNode* node48_1 = new UDNode("that", DT, None);
    const UDNode* node48_2 = new UDNode("apple", NN, None);
    QTest::newRow("Test 48: 'that' with singular noun") << node48_1 << node48_2 << true << emptyMistakes << false << "";

    // Тесты 49-50: Указательные местоимения для ед. числа с ошибками
    const UDNode* node49_1 = new UDNode("this", DT, None);
    const UDNode* node49_2 = new UDNode("apples", NNS, None);
    mistakeSet.insert(Mistake(QString("Указательное местоимение this несогласовано по числу с существительным apples. Указательное местоимение должно быть в форме множественного числа.")));
    QTest::newRow("Test 49: 'this' with plural noun (mistake)") << node49_1 << node49_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node50_1 = new UDNode("that", DT, None);
    const UDNode* node50_2 = new UDNode("apples", NNS, None);
    mistakeSet.insert(Mistake(QString("Указательное местоимение that несогласовано по числу с существительным apples. Указательное местоимение должно быть в форме множественного числа.")));
    QTest::newRow("Test 50: 'that' with plural noun (mistake)") << node50_1 << node50_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    // Тесты 51-59: Указательные местоимения для мн. числа и квантификаторы
    const UDNode* node51_1 = new UDNode("these", DT, None);
    const UDNode* node51_2 = new UDNode("apples", NNS, None);
    QTest::newRow("Test 51: 'these' with plural noun") << node51_1 << node51_2 << true << emptyMistakes << false << "";

    const UDNode* node52_1 = new UDNode("those", DT, None);
    const UDNode* node52_2 = new UDNode("apples", NNS, None);
    QTest::newRow("Test 52: 'those' with plural noun") << node52_1 << node52_2 << true << emptyMistakes << false << "";

    const UDNode* node53_1 = new UDNode("some", DT, None);
    const UDNode* node53_2 = new UDNode("apples", NNS, None);
    QTest::newRow("Test 53: 'some' with plural noun") << node53_1 << node53_2 << true << emptyMistakes << false << "";

    const UDNode* node54_1 = new UDNode("any", DT, None);
    const UDNode* node54_2 = new UDNode("apples", NNS, None);
    QTest::newRow("Test 54: 'any' with plural noun") << node54_1 << node54_2 << true << emptyMistakes << false << "";

    const UDNode* node55_1 = new UDNode("many", DT, None);
    const UDNode* node55_2 = new UDNode("apples", NNS, None);
    QTest::newRow("Test 55: 'many' with plural noun") << node55_1 << node55_2 << true << emptyMistakes << false << "";

    const UDNode* node56_1 = new UDNode("few", DT, None);
    const UDNode* node56_2 = new UDNode("apples", NNS, None);
    QTest::newRow("Test 56: 'few' with plural noun") << node56_1 << node56_2 << true << emptyMistakes << false << "";

    const UDNode* node57_1 = new UDNode("several", DT, None);
    const UDNode* node57_2 = new UDNode("apples", NNS, None);
    QTest::newRow("Test 57: 'several' with plural noun") << node57_1 << node57_2 << true << emptyMistakes << false << "";

    const UDNode* node58_1 = new UDNode("little", DT, None);
    const UDNode* node58_2 = new UDNode("apples", NNS, None);
    QTest::newRow("Test 58: 'little' with plural noun") << node58_1 << node58_2 << true << emptyMistakes << false << "";

    const UDNode* node59_1 = new UDNode("much", DT, None);
    const UDNode* node59_2 = new UDNode("apples", NNS, None);
    QTest::newRow("Test 59: 'much' with plural noun") << node59_1 << node59_2 << true << emptyMistakes << false << "";

    // Тесты 60-68: Указательные местоимения для мн. числа с ошибками
    const UDNode* node60_1 = new UDNode("these", DT, None);
    const UDNode* node60_2 = new UDNode("apple", NN, None);
    mistakeSet.insert(Mistake(QString("Указательное местоимение these несогласовано по числу с существительным apple. Указательное местоимение должно быть в форме единственного числа.")));
    QTest::newRow("Test 60: 'these' with singular noun (mistake)") << node60_1 << node60_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node61_1 = new UDNode("those", DT, None);
    const UDNode* node61_2 = new UDNode("apple", NN, None);
    mistakeSet.insert(Mistake(QString("Указательное местоимение those несогласовано по числу с существительным apple. Указательное местоимение должно быть в форме единственного числа.")));
    QTest::newRow("Test 61: 'those' with singular noun (mistake)") << node61_1 << node61_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node62_1 = new UDNode("some", DT, None);
    const UDNode* node62_2 = new UDNode("apple", NN, None);
    mistakeSet.insert(Mistake(QString("квантификатор some некорректно использован. Нельзя использовать с существительным apple в единственном числе")));
    QTest::newRow("Test 62: 'some' with singular noun (mistake)") << node62_1 << node62_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node63_1 = new UDNode("any", DT, None);
    const UDNode* node63_2 = new UDNode("apple", NN, None);
    mistakeSet.insert(Mistake(QString("квантификатор any некорректно использован. Нельзя использовать с существительным apple в единственном числе")));
    QTest::newRow("Test 63: 'any' with singular noun (mistake)") << node63_1 << node63_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node64_1 = new UDNode("many", DT, None);
    const UDNode* node64_2 = new UDNode("apple", NN, None);
    mistakeSet.insert(Mistake(QString("квантификатор many некорректно использован. Нельзя использовать с существительным apple в единственном числе")));
    QTest::newRow("Test 64: 'many' with singular noun (mistake)") << node64_1 << node64_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node65_1 = new UDNode("few", DT, None);
    const UDNode* node65_2 = new UDNode("apple", NN, None);
    mistakeSet.insert(Mistake(QString("квантификатор few некорректно использован. Нельзя использовать с существительным apple в единственном числе")));
    QTest::newRow("Test 65: 'few' with singular noun (mistake)") << node65_1 << node65_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node66_1 = new UDNode("several", DT, None);
    const UDNode* node66_2 = new UDNode("apple", NN, None);
    mistakeSet.insert(Mistake(QString("квантификатор several некорректно использован. Нельзя использовать с существительным apple в единственном числе")));
    QTest::newRow("Test 66: 'several' with singular noun (mistake)") << node66_1 << node66_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node67_1 = new UDNode("little", DT, None);
    const UDNode* node67_2 = new UDNode("apple", NN, None);
    mistakeSet.insert(Mistake(QString("квантификатор little некорректно использован. Нельзя использовать с существительным apple в единственном числе")));
    QTest::newRow("Test 67: 'little' with singular noun (mistake)") << node67_1 << node67_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node68_1 = new UDNode("much", DT, None);
    const UDNode* node68_2 = new UDNode("apple", NN, None);
    mistakeSet.insert(Mistake(QString("квантификатор much некорректно использован. Нельзя использовать с существительным apple в единственном числе")));
    QTest::newRow("Test 68: 'much' with singular noun (mistake)") << node68_1 << node68_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    // Тесты 69-71: Ошибочный вызов, неверные части речи
    const UDNode* node69_1 = new UDNode("red", JJ, None);
    const UDNode* node69_2 = new UDNode("apple", NN, None);
    QTest::newRow("Test 69: Invalid part of speech (adjective)") << node69_1 << node69_2 << true << emptyMistakes << true << "Invalid part of speech";

    const UDNode* node70_1 = new UDNode("apple", NN, None);
    const UDNode* node70_2 = new UDNode("seen", VBN, None);
    QTest::newRow("Test 70: Invalid part of speech (past participle)") << node70_1 << node70_2 << true << emptyMistakes << true << "Invalid part of speech";

    const UDNode* node71_1 = new UDNode("hello", UH, None);
    const UDNode* node71_2 = new UDNode("there", RB, None);
    QTest::newRow("Test 71: Invalid parts of speech (interjection + adverb)") << node71_1 << node71_2 << true << emptyMistakes << true << "Invalid part of speech";

    // Тест 72: Ошибочный вызов, глагол не was/were
    const UDNode* node72_1 = new UDNode("I", PRP, None);
    const UDNode* node72_2 = new UDNode("went", VBD, None);
    QTest::newRow("Test 72: Invalid verb (not was/were)") << node72_1 << node72_2 << true << emptyMistakes << true << "Verb must be 'was' or 'were'";

    // Тесты 73-74: Ошибочный вызов, нулевой указатель
    const UDNode* node73_1 = nullptr;
    const UDNode* node73_2 = new UDNode("word", NN, None);
    QTest::newRow("Test 73: word1 is nullptr") << node73_1 << node73_2 << true << emptyMistakes << true << "Node pointer is null";
    const UDNode* node74_1 = new UDNode("word", NN, None);
    const UDNode* node74_2 = nullptr;
    QTest::newRow("Test 74: word2 is nullptr") << node74_1 << node74_2 << true << emptyMistakes << true << "Node pointer is null";
};

void Tests::testMainAuxAgreement()
{
    QFETCH(const UDNode*, inputNode1);
    QFETCH(const UDNode*, inputNode2);
    QFETCH(bool, expAgreement);
    QFETCH(QSet<Mistake>, expMistakes);
    QFETCH(bool, expectException);
    QFETCH(QString, exceptionMessage);

    QSet<Mistake> mistakes;
    MainAuxAgreement gRule;
    bool agreement = false;
    bool exceptionThrown = false;
    QString actualExceptionMessage;

    try {
        agreement = gRule.check(inputNode1, inputNode2, mistakes);
    }
    catch (const QString& e) {
        exceptionThrown = true;
        actualExceptionMessage = e;
    }
    catch (...) {
        exceptionThrown = true;
        actualExceptionMessage = "Unexpected exception type";
    }

    // Проверка исключений
    if (expectException) {
        QVERIFY2(exceptionThrown, "Expected an exception, but none was thrown");
        QCOMPARE(actualExceptionMessage, exceptionMessage);
    } else {
        QVERIFY2(!exceptionThrown, QString("Unexpected exception thrown: %1").arg(actualExceptionMessage).toUtf8());

        // Сравнение результата
        QCOMPARE(agreement, expAgreement);

        // Подробное сравнение ошибок
        if (mistakes != expMistakes) {
            qDebug() << "Discrepancy found in mistakes:";

            QSet<Mistake> unexpectedMistakes = mistakes - expMistakes;
            if (!unexpectedMistakes.isEmpty()) {
                qDebug() << "Unexpected mistakes found (" << unexpectedMistakes.size() << "):";
                for (const Mistake& mistake : unexpectedMistakes) {
                    qDebug() << "  -" << mistake.getMessage();
                }
            }

            QSet<Mistake> missingMistakes = expMistakes - mistakes;
            if (!missingMistakes.isEmpty()) {
                qDebug() << "Missing expected mistakes (" << missingMistakes.size() << "):";
                for (const Mistake& mistake : missingMistakes) {
                    qDebug() << "  +" << mistake.getMessage();
                }
            }
        }
        QCOMPARE(mistakes, expMistakes);
    }

}

void Tests::testMainAuxAgreement_data()
{
    // Добавляем столбцы

    QTest::addColumn<const UDNode*>("inputNode1");
    QTest::addColumn<const UDNode*>("inputNode2");
    QTest::addColumn<bool>("expAgreement");
    QTest::addColumn<QSet<Mistake>>("expMistakes");
    QTest::addColumn<bool>("expectException");
    QTest::addColumn<QString>("exceptionMessage");

    QSet<Mistake> emptyMistakes;         // Множество ошибок для тестов, где слова согласованы
    QSet<Mistake> mistakeSet;                // Множество для хранения ошибок

    // Тест 1-3: Нет ошибки, вспомогательный глагол форма do/does/did, основной в начальной форме
    const UDNode* node1_1 = new UDNode("do", VBP, Ind);
    const UDNode* node1_2 = new UDNode("play", VB, None);
    QTest::newRow("Test 1: do + VB") << node1_1 << node1_2 << true << emptyMistakes << false << "";

    const UDNode* node2_1 = new UDNode("does", VBZ, Ind);
    const UDNode* node2_2 = new UDNode("play", VB, None);
    QTest::newRow("Test 2: does + VB") << node2_1 << node2_2 << true << emptyMistakes << false << "";

    const UDNode* node3_1 = new UDNode("did", VBD, Ind);
    const UDNode* node3_2 = new UDNode("play", VB, None);
    QTest::newRow("Test 3: did + VB") << node3_1 << node3_2 << true << emptyMistakes << false << "";

    // Тест 4-8: Есть ошибка, вспомогательный глагол do, основной не в начальной форме
    const UDNode* node4_1 = new UDNode("do", VBP, Ind);
    const UDNode* node4_2 = new UDNode("speaks", VBZ, None);
    mistakeSet.insert(Mistake("Глагол speaks несогласован по времени с вспомогательным глаголом do"));
    QTest::newRow("Test 4: do + VBZ (mistake)") << node4_1 << node4_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node5_1 = new UDNode("does", VBZ, Ind);
    const UDNode* node5_2 = new UDNode("played", VBD, None);
    mistakeSet.insert(Mistake("Глагол played несогласован по времени с вспомогательным глаголом does"));
    QTest::newRow("Test 5: does + VBD (mistake)") << node5_1 << node5_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node6_1 = new UDNode("did", VBD, Ind);
    const UDNode* node6_2 = new UDNode("playing", VBG, None);
    mistakeSet.insert(Mistake("Глагол playing несогласован по времени с вспомогательным глаголом did"));
    QTest::newRow("Test 6: did + VBG (mistake)") << node6_1 << node6_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node7_1 = new UDNode("do", VBP, Ind);
    const UDNode* node7_2 = new UDNode("spoken", VBN, None);
    mistakeSet.insert(Mistake("Глагол spoken несогласован по времени с вспомогательным глаголом do"));
    QTest::newRow("Test 7: do + VBN (mistake)") << node7_1 << node7_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node8_1 = new UDNode("does", VBZ, Ind);
    const UDNode* node8_2 = new UDNode("plays", VBZ, None);
    mistakeSet.insert(Mistake("Глагол plays несогласован по времени с вспомогательным глаголом does"));
    QTest::newRow("Test 8: does + VBZ (mistake)") << node8_1 << node8_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    // Тест 9-17: Нет ошибки, вспомогательный глагол модальный, основной в начальной форме
    const UDNode* node9_1 = new UDNode("can", MD, None);
    const UDNode* node9_2 = new UDNode("go", VB, None);
    QTest::newRow("Test 9: can + VB") << node9_1 << node9_2 << true << emptyMistakes << false << "";

    const UDNode* node10_1 = new UDNode("could", MD, None);
    const UDNode* node10_2 = new UDNode("go", VB, None);
    QTest::newRow("Test 10: could + VB") << node10_1 << node10_2 << true << emptyMistakes << false << "";

    const UDNode* node11_1 = new UDNode("may", MD, None);
    const UDNode* node11_2 = new UDNode("go", VB, None);
    QTest::newRow("Test 11: may + VB") << node11_1 << node11_2 << true << emptyMistakes << false << "";

    const UDNode* node12_1 = new UDNode("might", MD, None);
    const UDNode* node12_2 = new UDNode("go", VB, None);
    QTest::newRow("Test 12: might + VB") << node12_1 << node12_2 << true << emptyMistakes << false << "";

    const UDNode* node13_1 = new UDNode("must", MD, None);
    const UDNode* node13_2 = new UDNode("go", VB, None);
    QTest::newRow("Test 13: must + VB") << node13_1 << node13_2 << true << emptyMistakes << false << "";

    const UDNode* node14_1 = new UDNode("shall", MD, None);
    const UDNode* node14_2 = new UDNode("go", VB, None);
    QTest::newRow("Test 14: shall + VB") << node14_1 << node14_2 << true << emptyMistakes << false << "";

    const UDNode* node15_1 = new UDNode("should", MD, None);
    const UDNode* node15_2 = new UDNode("go", VB, None);
    QTest::newRow("Test 15: should + VB") << node15_1 << node15_2 << true << emptyMistakes << false << "";

    const UDNode* node16_1 = new UDNode("will", MD, None);
    const UDNode* node16_2 = new UDNode("go", VB, None);
    QTest::newRow("Test 16: will + VB") << node16_1 << node16_2 << true << emptyMistakes << false << "";

    const UDNode* node17_1 = new UDNode("would", MD, None);
    const UDNode* node17_2 = new UDNode("go", VB, None);
    QTest::newRow("Test 17: would + VB") << node17_1 << node17_2 << true << emptyMistakes << false << "";

    // Тест 18-22: Есть ошибка, вспомогательный глагол модальный, основной не в начальной форме
    const UDNode* node18_1 = new UDNode("should", MD, None);
    const UDNode* node18_2 = new UDNode("goes", VBZ, None);
    mistakeSet.insert(Mistake("Глагол plays несогласован по времени с вспомогательным глаголом does"));
    QTest::newRow("Test 18: should + VBZ (mistake)") << node18_1 << node18_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node19_1 = new UDNode("can", MD, None);
    const UDNode* node19_2 = new UDNode("gone", VBN, None);
    mistakeSet.insert(Mistake("Глагол plays несогласован по времени с вспомогательным глаголом does"));
    QTest::newRow("Test 19: can + VBN (mistake)") << node19_1 << node19_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node20_1 = new UDNode("must", MD, None);
    const UDNode* node20_2 = new UDNode("played", VBD, None);
    mistakeSet.insert(Mistake("Глагол plays несогласован по времени с вспомогательным глаголом does"));
    QTest::newRow("Test 20: must + VBD (mistake)") << node20_1 << node20_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node21_1 = new UDNode("will", MD, None);
    const UDNode* node21_2 = new UDNode("playing", VBG, None);
    mistakeSet.insert(Mistake("Глагол plays несогласован по времени с вспомогательным глаголом does"));
    QTest::newRow("Test 21: will + VBG (mistake)") << node21_1 << node21_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node22_1 = new UDNode("would", MD, None);
    const UDNode* node22_2 = new UDNode("plays", VBZ, None);
    mistakeSet.insert(Mistake("Глагол plays несогласован по времени с вспомогательным глаголом does"));
    QTest::newRow("Test 22: would + VBZ (mistake)") << node22_1 << node22_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    // Тест 23-26: Нет ошибки, вспомогательный глагол форма be, основной в форме герундия
    const UDNode* node23_1 = new UDNode("was", VBD, Ind);
    const UDNode* node23_2 = new UDNode("going", VBG, None);
    QTest::newRow("Test 23: was + VBG") << node23_1 << node23_2 << true << emptyMistakes << false << "";

    const UDNode* node24_1 = new UDNode("were", VBD, Ind);
    const UDNode* node24_2 = new UDNode("playing", VBG, None);
    QTest::newRow("Test 24: were + VBG") << node24_1 << node24_2 << true << emptyMistakes << false << "";

    const UDNode* node25_1 = new UDNode("be", VB, Ind);
    const UDNode* node25_2 = new UDNode("going", VBG, None);
    QTest::newRow("Test 25: be + VBG") << node25_1 << node25_2 << true << emptyMistakes << false << "";

    const UDNode* node26_1 = new UDNode("been", VBN, Ind);
    const UDNode* node26_2 = new UDNode("going", VBG, None);
    QTest::newRow("Test 26: been + VBG") << node26_1 << node26_2 << true << emptyMistakes << false << "";

    // Тест 27-31: Есть ошибка, вспомогательный глагол be, основной не в форме герундия
    const UDNode* node27_1 = new UDNode("was", VBD, Ind);
    const UDNode* node27_2 = new UDNode("go", VB, None);
    mistakeSet.insert(Mistake("Глагол go несогласован по времени с вспомогательным глаголом was"));
    QTest::newRow("Test 27: was + VB (mistake)") << node27_1 << node27_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node28_1 = new UDNode("were", VBD, Ind);
    const UDNode* node28_2 = new UDNode("goes", VBZ, None);
    mistakeSet.insert(Mistake("Глагол goes несогласован по времени с вспомогательным глаголом were"));
    QTest::newRow("Test 28: were + VBZ (mistake)") << node28_1 << node28_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node29_1 = new UDNode("be", VBD, Ind);
    const UDNode* node29_2 = new UDNode("gone", VBN, None);
    mistakeSet.insert(Mistake("Глагол gone несогласован по времени с вспомогательным глаголом be"));
    QTest::newRow("Test 29: be + VBN (mistake)") << node29_1 << node29_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node30_1 = new UDNode("been", VBD, Ind);
    const UDNode* node30_2 = new UDNode("played", VBD, None);
    mistakeSet.insert(Mistake("Глагол plays несогласован по времени с вспомогательным глаголом does"));
    QTest::newRow("Test 30: been + VBD (mistake)") << node30_1 << node30_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node31_1 = new UDNode("were", VBD, Ind);
    const UDNode* node31_2 = new UDNode("play", VB, None);
    mistakeSet.insert(Mistake("Глагол play несогласован по времени с вспомогательным глаголом were"));
    QTest::newRow("Test 31: were + VB (mistake)") << node31_1 << node31_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    // Тест 32-34: Нет ошибки, вспомогательный глагол форма have, основной в форме причастия прошедшего времени
    const UDNode* node32_1 = new UDNode("have", VBP, Ind);
    const UDNode* node32_2 = new UDNode("seen", VBN, None);
    QTest::newRow("Test 32: have + VBN") << node32_1 << node32_2 << true << emptyMistakes << false << "";

    const UDNode* node33_1 = new UDNode("has", VBZ, Ind);
    const UDNode* node33_2 = new UDNode("seen", VBN, None);
    QTest::newRow("Test 33: has + VBN") << node33_1 << node33_2 << true << emptyMistakes << false << "";

    const UDNode* node34_1 = new UDNode("had", VBD, Ind);
    const UDNode* node34_2 = new UDNode("seen", VBN, None);
    QTest::newRow("Test 34: had + VBN") << node34_1 << node34_2 << true << emptyMistakes << false << "";

    // Тест 35-39: Есть ошибка, вспомогательный глагол форма have, основной не в форме причастия прошедшего времени
    const UDNode* node35_1 = new UDNode("have", VBP, Ind);
    const UDNode* node35_2 = new UDNode("see", VB, None);
    mistakeSet.insert(Mistake("Глагол see несогласован по времени с вспомогательным глаголом have"));
    QTest::newRow("Test 35: have + VB (mistake)") << node35_1 << node35_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node36_1 = new UDNode("has", VBZ, Ind);
    const UDNode* node36_2 = new UDNode("sees", VBZ, None);
    mistakeSet.insert(Mistake("Глагол sees несогласован по времени с вспомогательным глаголом has"));
    QTest::newRow("Test 36: has + VBZ (mistake)") << node36_1 << node36_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node37_1 = new UDNode("had", VBD, Ind);
    const UDNode* node37_2 = new UDNode("saw", VBD, None);
    mistakeSet.insert(Mistake("Глагол saw несогласован по времени с вспомогательным глаголом had"));
    QTest::newRow("Test 37: had + VBD (mistake)") << node37_1 << node37_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node38_1 = new UDNode("have", VBP, Ind);
    const UDNode* node38_2 = new UDNode("seeing", VBG, None);
    mistakeSet.insert(Mistake("Глагол seeing несогласован по времени с вспомогательным глаголом have"));
    QTest::newRow("Test 38: have + VBG (mistake)") << node38_1 << node38_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node39_1 = new UDNode("has", VBZ, Ind);
    const UDNode* node39_2 = new UDNode("sees", VBZ, None);
    mistakeSet.insert(Mistake("Глагол sees несогласован по времени с вспомогательным глаголом has"));
    QTest::newRow("Test 39: has + VBZ (mistake)") << node39_1 << node39_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    // Тест 40-41: Нет ошибки, сокращение 'd (had/would)
    const UDNode* node40_1 = new UDNode("'d", MD, None); // would
    const UDNode* node40_2 = new UDNode("go", VB, None);
    QTest::newRow("Test 40: 'd (would) + VB") << node40_1 << node40_2 << true << emptyMistakes << false << "";

    const UDNode* node41_1 = new UDNode("'d", VBD, None); // had
    const UDNode* node41_2 = new UDNode("gone", VBN, None);
    QTest::newRow("Test 41: 'd (had) + VBN") << node41_1 << node41_2 << true << emptyMistakes << false << "";

    // Тест 42-45: Есть ошибка, сокращение 'd, основной глагол в неверной форме
    const UDNode* node42_1 = new UDNode("'d", MD, None); // would
    const UDNode* node42_2 = new UDNode("goes", VBZ, None);
    mistakeSet.insert(Mistake("Глагол goes несогласован по времени с вспомогательным глаголом 'd"));
    QTest::newRow("Test 42: 'd (would) + VBZ (mistake)") << node42_1 << node42_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node43_1 = new UDNode("'d", VBD, None); // had
    const UDNode* node43_2 = new UDNode("went", VBD, None);
    mistakeSet.insert(Mistake("Глагол went несогласован по времени с вспомогательным глаголом 'd"));
    QTest::newRow("Test 43: 'd (had) + VBD (mistake)") << node43_1 << node43_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node44_1 = new UDNode("'d", MD, None); // would
    const UDNode* node44_2 = new UDNode("going", VBG, None);
    mistakeSet.insert(Mistake("Глагол going несогласован по времени с вспомогательным глаголом 'd"));
    QTest::newRow("Test 44: 'd (would) + VBG (mistake)") << node44_1 << node44_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node45_1 = new UDNode("'d", VBD, None); // had
    const UDNode* node45_2 = new UDNode("go", VB, None);
    mistakeSet.insert(Mistake("Глагол go несогласован по времени с вспомогательным глаголом 'd"));
    QTest::newRow("Test 45: 'd (had) + VB (mistake)") << node45_1 << node45_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    // Тест 46-48: Ошибочный вызов, неверные части речи
    const UDNode* node46_1 = new UDNode("play", VB, None);
    const UDNode* node46_2 = new UDNode("go", VB, None);
    QTest::newRow("Test 46: Invalid aux verb part of speech") << node46_1 << node46_2 << true << emptyMistakes << true << "Invalid auxiliary verb part of speech";

    const UDNode* node47_1 = new UDNode("can", MD, None);
    const UDNode* node47_2 = new UDNode("apple", NN, None);
    QTest::newRow("Test 47: Invalid main verb part of speech") << node47_1 << node47_2 << true << emptyMistakes << true << "Invalid main verb part of speech";

    const UDNode* node48_1 = new UDNode("red", JJ, None);
    const UDNode* node48_2 = new UDNode("blue", JJ, None);
    QTest::newRow("Test 48: Invalid parts of speech") << node48_1 << node48_2 << true << emptyMistakes << true << "Invalid auxiliary verb part of speech";

    // Тест 49: Ошибочный вызов, auxVerb не MD, форма have, форма be, форма do
    const UDNode* node49_1 = new UDNode("play", VB, None);
    const UDNode* node49_2 = new UDNode("go", VB, None);
    QTest::newRow("Test 49: Invalid aux verb type") << node49_1 << node49_2 << true << emptyMistakes << true << "Invalid auxiliary verb type";

    // Тест 50-51: Ошибочный вызов, нулевой указатель
    QTest::newRow("Test 50: auxVerb is nullptr") << static_cast<const UDNode*>(nullptr) << static_cast<const UDNode*>(new UDNode("go", VB, None) )<< true << emptyMistakes << true << "Auxiliary verb node pointer is null";
    QTest::newRow("Test 51: mainVerb is nullptr") << static_cast<const UDNode*>(new UDNode("do", VBP, None)) << static_cast<const UDNode*>(nullptr) << true << emptyMistakes << true << "Main verb node pointer is null";
}

void Tests::testAuxAuxAgreement()
{
    QFETCH(const UDNode*, inputNode1);
    QFETCH(const UDNode*, inputNode2);
    QFETCH(bool, expAgreement);
    QFETCH(QSet<Mistake>, expMistakes);
    QFETCH(bool, expectException);
    QFETCH(QString, exceptionMessage);

    QSet<Mistake> mistakes;
    AuxAuxAgreement gRule;
    bool agreement = false;
    bool exceptionThrown = false;
    QString actualExceptionMessage;

    try {
        agreement = gRule.check(inputNode1, inputNode2, mistakes);
    }
    catch (const QString& e) {
        exceptionThrown = true;
        actualExceptionMessage = e;
    }
    catch (...) {
        exceptionThrown = true;
        actualExceptionMessage = "Unexpected exception type";
    }

    // Проверка исключений
    if (expectException) {
        QVERIFY2(exceptionThrown, "Expected an exception, but none was thrown");
        QCOMPARE(actualExceptionMessage, exceptionMessage);
    } else {
        QVERIFY2(!exceptionThrown, QString("Unexpected exception thrown: %1").arg(actualExceptionMessage).toUtf8());

        // Сравнение результата
        QCOMPARE(agreement, expAgreement);

        // Подробное сравнение ошибок
        if (mistakes != expMistakes) {
            qDebug() << "Discrepancy found in mistakes:";

            QSet<Mistake> unexpectedMistakes = mistakes - expMistakes;
            if (!unexpectedMistakes.isEmpty()) {
                qDebug() << "Unexpected mistakes found (" << unexpectedMistakes.size() << "):";
                for (const Mistake& mistake : unexpectedMistakes) {
                    qDebug() << "  -" << mistake.getMessage();
                }
            }

            QSet<Mistake> missingMistakes = expMistakes - mistakes;
            if (!missingMistakes.isEmpty()) {
                qDebug() << "Missing expected mistakes (" << missingMistakes.size() << "):";
                for (const Mistake& mistake : missingMistakes) {
                    qDebug() << "  +" << mistake.getMessage();
                }
            }
        }
        QCOMPARE(mistakes, expMistakes);
    }
}

void Tests::testAuxAuxAgreement_data()
{
    // Добавляем столбцы

    QTest::addColumn<const UDNode*>("inputNode1");
    QTest::addColumn<const UDNode*>("inputNode2");
    QTest::addColumn<bool>("expAgreement");
    QTest::addColumn<QSet<Mistake>>("expMistakes");
    QTest::addColumn<bool>("expectException");
    QTest::addColumn<QString>("exceptionMessage");

    QSet<Mistake> emptyMistakes;         // Множество ошибок для тестов, где слова согласованы
    QSet<Mistake> mistakeSet;                // Множество для хранения ошибок

    // Тест 1-9: Нет ошибки, модальный глагол + have/be
    const UDNode* node1_1 = new UDNode("will", MD, None);
    const UDNode* node1_2 = new UDNode("have", VB, None);
    QTest::newRow("Test 1: will + have") << node1_1 << node1_2 << true << emptyMistakes << false << "";

    const UDNode* node2_1 = new UDNode("may", MD, None);
    const UDNode* node2_2 = new UDNode("be", VB, None);
    QTest::newRow("Test 2: may + be") << node2_1 << node2_2 << true << emptyMistakes << false << "";

    const UDNode* node3_1 = new UDNode("should", MD, None);
    const UDNode* node3_2 = new UDNode("have", VB, None);
    QTest::newRow("Test 3: should + have") << node3_1 << node3_2 << true << emptyMistakes << false << "";

    const UDNode* node4_1 = new UDNode("can", MD, None);
    const UDNode* node4_2 = new UDNode("have", VB, None);
    QTest::newRow("Test 4: can + have") << node4_1 << node4_2 << true << emptyMistakes << false << "";

    const UDNode* node5_1 = new UDNode("could", MD, None);
    const UDNode* node5_2 = new UDNode("be", VB, None);
    QTest::newRow("Test 5: could + be") << node5_1 << node5_2 << true << emptyMistakes << false << "";

    const UDNode* node6_1 = new UDNode("might", MD, None);
    const UDNode* node6_2 = new UDNode("have", VB, None);
    QTest::newRow("Test 6: might + have") << node6_1 << node6_2 << true << emptyMistakes << false << "";

    const UDNode* node7_1 = new UDNode("must", MD, None);
    const UDNode* node7_2 = new UDNode("be", VB, None);
    QTest::newRow("Test 7: must + be") << node7_1 << node7_2 << true << emptyMistakes << false << "";

    const UDNode* node8_1 = new UDNode("shall", MD, None);
    const UDNode* node8_2 = new UDNode("have", VB, None);
    QTest::newRow("Test 8: shall + have") << node8_1 << node8_2 << true << emptyMistakes << false << "";

    const UDNode* node9_1 = new UDNode("would", MD, None);
    const UDNode* node9_2 = new UDNode("be", VB, None);
    QTest::newRow("Test 9: would + be") << node9_1 << node9_2 << true << emptyMistakes << false << "";

    // Тест 10-15: Есть ошибка, модальный глагол + не have/be
    const UDNode* node10_1 = new UDNode("will", MD, None);
    const UDNode* node10_2 = new UDNode("was", VBD, None);
    mistakeSet.insert(Mistake("Глагол was несогласован по времени с модальным глаголом will. После модального глагола следует в начальной форме"));
    QTest::newRow("Test 10: will + was (mistake)") << node10_1 << node10_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node11_1 = new UDNode("should", MD, None);
    const UDNode* node11_2 = new UDNode("has", VBZ, None);
    mistakeSet.insert(Mistake("Глагол has несогласован по времени с модальным глаголом should. После модального глагола следует в начальной форме"));
    QTest::newRow("Test 11: should + has (mistake)") << node11_1 << node11_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node12_1 = new UDNode("can", MD, None);
    const UDNode* node12_2 = new UDNode("had", VBD, None);
    mistakeSet.insert(Mistake("Глагол had несогласован по времени с модальным глаголом can. После модального глагола следует в начальной форме"));
    QTest::newRow("Test 12: can + had (mistake)") << node12_1 << node12_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node13_1 = new UDNode("may", MD, None);
    const UDNode* node13_2 = new UDNode("am", VBP, None);
    mistakeSet.insert(Mistake("Глагол am несогласован по времени с модальным глаголом may. После модального глагола следует в начальной форме"));
    QTest::newRow("Test 13: may + am (mistake)") << node13_1 << node13_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node14_1 = new UDNode("must", MD, None);
    const UDNode* node14_2 = new UDNode("is", VBZ, None);
    mistakeSet.insert(Mistake("Глагол is несогласован по времени с модальным глаголом must. После модального глагола следует в начальной форме"));
    QTest::newRow("Test 14: must + is (mistake)") << node14_1 << node14_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node15_1 = new UDNode("would", MD, None);
    const UDNode* node15_2 = new UDNode("are", VBP, None);
    mistakeSet.insert(Mistake("Глагол are несогласован по времени с модальным глаголом would. После модального глагола следует в начальной форме"));
    QTest::newRow("Test 15: would + are (mistake)") << node15_1 << node15_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    // Тест 16: Нет ошибки, have + been
    const UDNode* node16_1 = new UDNode("have", VB, None);
    const UDNode* node16_2 = new UDNode("been", VBN, None);
    QTest::newRow("Test 16: have + been") << node16_1 << node16_2 << true << emptyMistakes << false << "";

    // Тест 17-20: Есть ошибка, have/had + не been
    const UDNode* node17_1 = new UDNode("have", VB, None);
    const UDNode* node17_2 = new UDNode("is", VBZ, None);
    mistakeSet.insert(Mistake("Глагол is несогласован по времени свспомогательным глаголом have. Требуется форма Past Participle/V3(been)."));
    QTest::newRow("Test 17: have + is (mistake)") << node17_1 << node17_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node18_1 = new UDNode("had", VBD, None);
    const UDNode* node18_2 = new UDNode("be", VB, None);
    mistakeSet.insert(Mistake("Глагол be несогласован по времени свспомогательным глаголом had. Требуется форма Past Participle/V3(been)."));
    QTest::newRow("Test 18: have + be (mistake)") << node18_1 << node18_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node19_1 = new UDNode("have", VB, None);
    const UDNode* node19_2 = new UDNode("am", VBP, None);
    mistakeSet.insert(Mistake("Глагол am несогласован по времени свспомогательным глаголом have. Требуется форма Past Participle/V3(been)."));
    QTest::newRow("Test 19: have + am (mistake)") << node19_1 << node19_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    const UDNode* node20_1 = new UDNode("had", VBD, None);
    const UDNode* node20_2 = new UDNode("are", VBP, None);
    mistakeSet.insert(Mistake("Глагол are несогласован по времени свспомогательным глаголом had. Требуется форма Past Participle/V3(been)."));
    QTest::newRow("Test 20: have + are (mistake)") << node20_1 << node20_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    // Тест 21: Нет ошибки, 'll + have
    const UDNode* node21_1 = new UDNode("'ll", MD, None);
    const UDNode* node21_2 = new UDNode("have", VB, None);
    QTest::newRow("Test 21: 'll + have") << node21_1 << node21_2 << true << emptyMistakes << false << "";

    // Тест 22: Нет ошибки, 've + been
    const UDNode* node22_1 = new UDNode("'ve", VB, None);
    const UDNode* node22_2 = new UDNode("been", VBN, None);
    QTest::newRow("Test 22: 've + been") << node22_1 << node22_2 << true << emptyMistakes << false << "";

    // Тест 23-25: Нет ошибки, 'd + been/be/have
    const UDNode* node23_1 = new UDNode("'d", MD, None); // had
    const UDNode* node23_2 = new UDNode("been", VBN, None);
    QTest::newRow("Test 23: 'd (had) + been") << node23_1 << node23_2 << true << emptyMistakes << false << "";

    const UDNode* node24_1 = new UDNode("'d", MD, None); // would
    const UDNode* node24_2 = new UDNode("be", VB, None);
    QTest::newRow("Test 24: 'd (would) + be") << node24_1 << node24_2 << true << emptyMistakes << false << "";

    const UDNode* node25_1 = new UDNode("'d", VBD, None); // would
    const UDNode* node25_2 = new UDNode("have", VB, None);
    QTest::newRow("Test 25: 'd (would) + have") << node25_1 << node25_2 << true << emptyMistakes << false << "";

    // Тест 26: Есть ошибка, 'd + не been/be/have
    const UDNode* node26_1 = new UDNode("'d", MD, None);
    const UDNode* node26_2 = new UDNode("was", VBD, None);
    mistakeSet.insert(Mistake("неправильная форма глагола was. После would/have может быть been/be/have"));
    QTest::newRow("Test 26: 'd + was (mistake)") << node26_1 << node26_2 << false << mistakeSet << false << "";
    mistakeSet.clear();

    // Тест 27: Ошибочный вызов, auxVerb не MD/форма have
    const UDNode* node27_1 = new UDNode("go", VB, None);
    const UDNode* node27_2 = new UDNode("have", VB, None);
    QTest::newRow("Test 27: Invalid aux verb type") << node27_1 << node27_2 << true << emptyMistakes << true << "Invalid auxiliary verb type";

    // Тест 28: Ошибочный вызов, mainAuxVerb не форма be/have
    const UDNode* node28_1 = new UDNode("have", VB, None);
    const UDNode* node28_2 = new UDNode("gone", VBN, None);
    QTest::newRow("Test 28: Invalid main aux verb type") << node28_1 << node28_2 << true << emptyMistakes << true << "Invalid main auxiliary verb type";

    // Тест 29-30: Ошибочный вызов, нулевой указатель
    QTest::newRow("Test 29: auxVerb is nullptr") << static_cast<const UDNode*>(nullptr) << static_cast<const UDNode*>(new UDNode("have", VB, None)) << true << emptyMistakes << true << "Auxiliary verb node pointer is null";
    QTest::newRow("Test 30: mainAuxVerb is nullptr") << static_cast<const UDNode*>(new UDNode("have", VB, None))  << static_cast<const UDNode*>(nullptr) << true << emptyMistakes << true << "Main auxiliary verb node pointer is null";

}

void Tests::testPassiveAgreement()
{
    QFETCH(const UDNode*, inputNode1);
    QFETCH(const UDNode*, inputNode2);
    QFETCH(bool, expAgreement);
    QFETCH(QSet<Mistake>, expMistakes);
    QFETCH(bool, expectException);
    QFETCH(QString, exceptionMessage);

    QSet<Mistake> mistakes;
    PassiveAgreement gRule;
    bool agreement = false;
    bool exceptionThrown = false;
    QString actualExceptionMessage;

    try {
        agreement = gRule.check(inputNode1, inputNode2, mistakes);
    }
    catch (const QString& e) {
        exceptionThrown = true;
        actualExceptionMessage = e;
    }
    catch (...) {
        exceptionThrown = true;
        actualExceptionMessage = "Unexpected exception type";
    }

    // Проверка исключений
    if (expectException) {
        QVERIFY2(exceptionThrown, "Expected an exception, but none was thrown");
        QCOMPARE(actualExceptionMessage, exceptionMessage);
    } else {
        QVERIFY2(!exceptionThrown, QString("Unexpected exception thrown: %1").arg(actualExceptionMessage).toUtf8());

        // Сравнение результата
        QCOMPARE(agreement, expAgreement);

        // Подробное сравнение ошибок
        if (mistakes != expMistakes) {
            qDebug() << "Discrepancy found in mistakes:";

            QSet<Mistake> unexpectedMistakes = mistakes - expMistakes;
            if (!unexpectedMistakes.isEmpty()) {
                qDebug() << "Unexpected mistakes found (" << unexpectedMistakes.size() << "):";
                for (const Mistake& mistake : unexpectedMistakes) {
                    qDebug() << "  -" << mistake.getMessage();
                }
            }

            QSet<Mistake> missingMistakes = expMistakes - mistakes;
            if (!missingMistakes.isEmpty()) {
                qDebug() << "Missing expected mistakes (" << missingMistakes.size() << "):";
                for (const Mistake& mistake : missingMistakes) {
                    qDebug() << "  +" << mistake.getMessage();
                }
            }
        }
        QCOMPARE(mistakes, expMistakes);
    }
}

void Tests::testPassiveAgreement_data()
{
    // Добавляем столбцы

    QTest::addColumn<const UDNode*>("inputNode1");
    QTest::addColumn<const UDNode*>("inputNode2");
    QTest::addColumn<bool>("expAgreement");
    QTest::addColumn<QSet<Mistake>>("expMistakes");
    QTest::addColumn<bool>("expectException");
    QTest::addColumn<QString>("exceptionMessage");

    QSet<Mistake> emptyMistakes;         // Множество ошибок для тестов, где слова согласованы
    QSet<Mistake> mistakeSet;                // Множество для хранения ошибок

    // Тест 1-8: Нет ошибки, правильные формы пассивного залога
    // 1. Letters are delivered.
    UDNode* node1_0 = new UDNode(3, "delivered", VBN, 0, Root, Ind);
    const UDNode* node1_1 = new UDNode(1, "Letters", NNS, 3, Nsubj_Pass, None);
    node1_0->addChild(node1_1);
    const UDNode* node1_2 = new UDNode(2, "are", VBP, 3, Aux_Pass, Ind);
    node1_0->addChild(node1_2);
    QTest::newRow("Test 1: are + delivered") << node1_2 << static_cast<const UDNode*>(node1_0) << true << emptyMistakes << false << "";

    // 2.  letters were delivered.
    UDNode* node2_0 = new UDNode(3, "delivered", VBN, 0, Root, Ind);
    const UDNode* node2_2 = new UDNode(1, "letters", NNS, 3, Nsubj_Pass, None);
    node2_0->addChild(node2_2);
    const UDNode* node2_3 = new UDNode(2, "were", VBD, 3, Aux_Pass, Ind);
    node2_0->addChild(node2_3);
    QTest::newRow("Test 2: were + delivered") << node2_3 << static_cast<const UDNode*>(node2_0) << true << emptyMistakes << false << "";

    // 3. letters have been delivered.
    UDNode* node3_0 = new UDNode(4, "delivered", VBN, 0, Root, Ind);
    const UDNode* node3_2 = new UDNode(1, "letters", NNS, 4, Nsubj_Pass, None);
    node3_0->addChild(node3_2);
    const UDNode* node3_3 = new UDNode(2, "have", VBP, 4, Aux, Ind);
    const UDNode* node3_4 = new UDNode(3, "been", VBN, 4, Aux_Pass, Ind);
    node3_0->addChild(node3_3);
    node3_0->addChild(node3_4);
    QTest::newRow("Test 3: have been + delivered") << node3_4 << static_cast<const UDNode*>(node3_0) << true << emptyMistakes << false << "";

    // 4.  letters will be delivered.
    UDNode* node4_0 = new UDNode(4, "delivered", VBN, 0, Root, Ind);
    const UDNode* node4_2 = new UDNode(1, "letters", NNS, 4, Nsubj_Pass, None);
    node4_0->addChild(node4_2);
    const UDNode* node4_3 = new UDNode(2, "will", MD, 4, Aux, Ind);
    const UDNode* node4_4 = new UDNode(3, "be", VB, 4, Aux_Pass, Ind);
    node4_0->addChild(node4_3);
    node4_0->addChild(node4_4);
    QTest::newRow("Test 4: will be + delivered") << node4_4 << static_cast<const UDNode*>(node4_0) << true << emptyMistakes << false << "";

    // 5.  letters had been delivered.
    UDNode* node5_0 = new UDNode(4, "delivered", VBN, 0, Root, Ind);
    const UDNode* node5_2 = new UDNode(1, "letters", NNS, 4, Nsubj_Pass, None);
    node5_0->addChild(node5_2);
    const UDNode* node5_3 = new UDNode(2, "had", VBD, 4, Aux, Ind);
    const UDNode* node5_4 = new UDNode(3, "been", VBN, 4, Aux_Pass, Ind);
    node5_0->addChild(node5_3);
    node5_0->addChild(node5_4);
    QTest::newRow("Test 5: had been + delivered") << node5_4 << static_cast<const UDNode*>(node5_0) << true << emptyMistakes << false << "";

    // 6.  letters are being delivered.
    UDNode* node6_0 = new UDNode(4, "delivered", VBN, 0, Root, Ind);
    const UDNode* node6_2 = new UDNode(1, "letters", NNS, 4, Nsubj_Pass, None);
    node6_0->addChild(node6_2);
    const UDNode* node6_3 = new UDNode(2, "are", VBP, 4, Aux, Ind);
    const UDNode* node6_4 = new UDNode(3, "being", VBG, 4, Aux_Pass, Ind);
    node6_0->addChild(node6_3);
    node6_0->addChild(node6_4);
    QTest::newRow("Test 6: are being + delivered") << node6_4 << static_cast<const UDNode*>(node6_0) << true << emptyMistakes << false << "";

    // 7.  letters were being delivered.
    UDNode* node7_0 = new UDNode(4, "delivered", VBN, 0, Root, Ind);
    const UDNode* node7_2 = new UDNode(1, "letters", NNS, 4, Nsubj_Pass, None);
    node7_0->addChild(node7_2);
    const UDNode* node7_3 = new UDNode(2, "were", VBD, 4, Aux, Ind);
    const UDNode* node7_4 = new UDNode(3, "being", VBG, 4, Aux_Pass, Ind);
    node7_0->addChild(node7_3);
    node7_0->addChild(node7_4);
    QTest::newRow("Test 7: were being + delivered") << node7_4 << static_cast<const UDNode*>(node7_0) << true << emptyMistakes << false << "";

    // 8. letters may be delivered.
    UDNode* node8_0 = new UDNode(4, "delivered", VBN, 0, Root, Ind);
    const UDNode* node8_2 = new UDNode(1, "letters", NNS, 4, Nsubj_Pass, None);
    node8_0->addChild(node8_2);
    const UDNode* node8_3 = new UDNode(2, "may", MD, 4, Aux, Ind);
    const UDNode* node8_4 = new UDNode(3, "be", VB, 4, Aux_Pass, Ind);
    node8_0->addChild(node8_3);
    node8_0->addChild(node8_4);
    QTest::newRow("Test 8: may be + delivered") << node8_4 << static_cast<const UDNode*>(node8_0) << true << emptyMistakes << false << "";

    // Тест 9-13: Есть ошибка, основной глагол не в форме причастия прошедшего времени
    UDNode* node9_0 = new UDNode(3, "deliver", VB, 0, Root, Ind);
    const UDNode* node9_1 = new UDNode(1, "letters", NNS, 3, Nsubj_Pass, None);
    node9_0->addChild(node9_1);
    const UDNode* node9_2 = new UDNode(2, "are", VBP, 3, Aux_Pass, Ind);
    node9_0->addChild(node9_2);
    mistakeSet.insert(Mistake("Глаголы при построении пассивного залога (Passive Voice) несогласованы"));
    QTest::newRow("Test 9: are + deliver (VB)") << node9_2 << static_cast<const UDNode*>(node9_0) << false << mistakeSet << false << "";
    mistakeSet.clear();

    UDNode* node10_0 = new UDNode(3, "delivers", VBZ, 0, Root, Ind);
    const UDNode* node10_1 = new UDNode(1, "letters", NNS, 3, Nsubj_Pass, None);
    node10_0->addChild(node10_1);
    const UDNode* node10_2 = new UDNode(2, "are", VBP, 3, Aux_Pass, Ind);
    node10_0->addChild(node10_2);
    mistakeSet.insert(Mistake("Глаголы при построении пассивного залога (Passive Voice) несогласованы"));
    QTest::newRow("Test 10: are + delivers (VBZ)") << node10_2 << static_cast<const UDNode*>(node10_0) << false << mistakeSet << false << "";
    mistakeSet.clear();

    UDNode* node11_0 = new UDNode(3, "delivered", VBD, 0, Root, Ind);
    const UDNode* node11_1 = new UDNode(1, "letters", NNS, 3, Nsubj_Pass, None);
    node11_0->addChild(node11_1);
    const UDNode* node11_2 = new UDNode(2, "are", VBP, 3, Aux_Pass, Ind);
    node11_0->addChild(node11_2);
    mistakeSet.insert(Mistake("Глаголы при построении пассивного залога (Passive Voice) несогласованы"));
    QTest::newRow("Test 11: are + delivered (VBD)") << node11_2 << static_cast<const UDNode*>(node11_0) << false << mistakeSet << false << "";
    mistakeSet.clear();

    UDNode* node12_0 = new UDNode(3, "delivering", VBG, 0, Root, Ind);
    const UDNode* node12_1 = new UDNode(1, "letters", NNS, 3, Nsubj_Pass, None);
    node12_0->addChild(node12_1);
    const UDNode* node12_2 = new UDNode(2, "are", VBP, 3, Aux_Pass, Ind);
    node12_0->addChild(node12_2);
    mistakeSet.insert(Mistake("Глаголы при построении пассивного залога (Passive Voice) несогласованы"));
    QTest::newRow("Test 12: are + delivering (VBG)") << node12_2 << static_cast<const UDNode*>(node12_0) << false << mistakeSet << false << "";
    mistakeSet.clear();

    UDNode* node13_0 = new UDNode(3, "deliver", VBP, 0, Root, Ind);
    const UDNode* node13_1 = new UDNode(1, "letters", NNS, 3, Nsubj_Pass, None);
    node13_0->addChild(node13_1);
    const UDNode* node13_2 = new UDNode(2, "are", VBP, 3, Aux_Pass, Ind);
    node13_0->addChild(node13_2);
    mistakeSet.insert(Mistake("Глаголы при построении пассивного залога (Passive Voice) несогласованы"));
    QTest::newRow("Test 13: are + deliver (VBP)") << node13_2 << static_cast<const UDNode*>(node13_0) << false << mistakeSet << false << "";
    mistakeSet.clear();

    // Тест 14-21: Есть ошибка, неправильная форма глагола be
    // 14. Letters be delivered. (вместо are)
    UDNode* node14_0 = new UDNode(3, "delivered", VBN, 0, Root, Ind);
    const UDNode* node14_1 = new UDNode(1, "Letters", NNS, 3, Nsubj_Pass, None);
    node14_0->addChild(node14_1);
    const UDNode* node14_2 = new UDNode(2, "be", VB, 3, Aux_Pass, Ind);
    node14_0->addChild(node14_2);
    mistakeSet.insert(Mistake("Глаголы при построении пассивного залога (Passive Voice) несогласованы"));
    QTest::newRow("Test 14: be + delivered (should be are)") << node14_2 << static_cast<const UDNode*>(node14_0) << false << mistakeSet << false << "";
    mistakeSet.clear();

    // 15.  letters have be delivered. (вместо been)
    UDNode* node15_0 = new UDNode(4, "delivered", VBN, 0, Root, Ind);
    const UDNode* node15_2 = new UDNode(1, "letters", NNS, 4, Nsubj_Pass, None);
    node15_0->addChild(node15_2);
    const UDNode* node15_3 = new UDNode(2, "have", VBP, 4, Aux, Ind);
    const UDNode* node15_4 = new UDNode(3, "be", VB, 4, Aux_Pass, Ind);
    node15_0->addChild(node15_3);
    node15_0->addChild(node15_4);
    mistakeSet.insert(Mistake("Глаголы при построении пассивного залога (Passive Voice) несогласованы"));
    QTest::newRow("Test 15: have be + delivered (should have been)") << node15_4 << static_cast<const UDNode*>(node15_0) << false << mistakeSet << false << "";
    mistakeSet.clear();

    // 16. letters are be delivered. (лишний be)
    UDNode* node16_0 = new UDNode(4, "delivered", VBN, 0, Root, Ind);
    const UDNode* node16_2 = new UDNode(1, "letters", NNS, 4, Nsubj_Pass, None);
    node16_0->addChild(node16_2);
    const UDNode* node16_3 = new UDNode(2, "are", VBP, 4, Aux, Ind);
    const UDNode* node16_4 = new UDNode(3, "be", VB, 4, Aux_Pass, Ind);
    node16_0->addChild(node16_3);
    node16_0->addChild(node16_4);
    mistakeSet.insert(Mistake("Глаголы при построении пассивного залога (Passive Voice) несогласованы"));
    QTest::newRow("Test 16: are be + delivered (should are)") << node16_4 << static_cast<const UDNode*>(node16_0) << false << mistakeSet << false << "";
    mistakeSet.clear();

    // 17. letters were be delivered. (вместо были being)
    UDNode* node17_0 = new UDNode(4, "delivered", VBN, 0, Root, Ind);
    const UDNode* node17_2 = new UDNode(1, "letters", NNS, 4, Nsubj_Pass, None);
    node17_0->addChild(node17_2);
    const UDNode* node17_3 = new UDNode(2, "were", VBD, 4, Aux, Ind);
    const UDNode* node17_4 = new UDNode(3, "be", VB, 4, Aux_Pass, Ind);
    node17_0->addChild(node17_3);
    node17_0->addChild(node17_4);
    mistakeSet.insert(Mistake("Глаголы при построении пассивного залога (Passive Voice) несогласованы"));
    QTest::newRow("Test 17: were be + delivered (should were being)") << node17_4 << static_cast<const UDNode*>(node17_0) << false << mistakeSet << false << "";
    mistakeSet.clear();

    // 18. letters may are delivered. (неправильное сочетание)
    UDNode* node18_0 = new UDNode(4, "delivered", VBN, 0, Root, Ind);
    const UDNode* node18_2 = new UDNode(1, "letters", NNS, 4, Nsubj_Pass, None);
    node18_0->addChild(node18_2);
    const UDNode* node18_3 = new UDNode(2, "may", MD, 4, Aux, Ind);
    const UDNode* node18_4 = new UDNode(3, "are", VBP, 4, Aux_Pass, Ind);
    node18_0->addChild(node18_3);
    node18_0->addChild(node18_4);
    mistakeSet.insert(Mistake("Глаголы при построении пассивного залога (Passive Voice) несогласованы"));
    QTest::newRow("Test 18: may are + delivered (should may be)") << node18_4 << static_cast<const UDNode*>(node18_0) << false << mistakeSet << false << "";
    mistakeSet.clear();

    // 19. letters been delivered. (неправильное сочетание)
    UDNode* node19_0 = new UDNode(3, "delivered", VBN, 0, Root, Ind);
    const UDNode* node19_2 = new UDNode(1, "letters", NNS, 3, Nsubj_Pass, None);
    node19_0->addChild(node19_2);
    const UDNode* node19_3 = new UDNode(2, "been", VBP, 3, Aux_Pass, Ind);
    node19_0->addChild(node19_3);
    mistakeSet.insert(Mistake("Глаголы при построении пассивного залога (Passive Voice) несогласованы"));
    QTest::newRow("Test 19: been + delivered (should were)") << node19_3 << static_cast<const UDNode*>(node19_0) << false << mistakeSet << false << "";
    mistakeSet.clear();


    // 20. letters will been delivered. (неправильное сочетание)
    UDNode* node20_0 = new UDNode(4, "delivered", VBN, 0, Root, Ind);
    const UDNode* node20_2 = new UDNode(1, "letters", NNS, 4, Nsubj_Pass, None);
    node20_0->addChild(node20_2);
    const UDNode* node20_3 = new UDNode(2, "will", MD, 4, Aux, Ind);
    const UDNode* node20_4 = new UDNode(3, "been", VBN, 4, Aux_Pass, Ind);
    node20_0->addChild(node20_3);
    node20_0->addChild(node20_4);
    mistakeSet.insert(Mistake("Глаголы при построении пассивного залога (Passive Voice) несогласованы"));
    QTest::newRow("Test 20: will been + delivered (should will be)") << node20_4 << static_cast<const UDNode*>(node20_0) << false << mistakeSet << false << "";
    mistakeSet.clear();

    // 21. letters had be delivered. (неправильное сочетание)
    UDNode* node21_0 = new UDNode(4, "delivered", VBN, 0, Root, Ind);
    const UDNode* node21_2 = new UDNode(1, "letters", NNS, 4, Nsubj_Pass, None);
    node21_0->addChild(node21_2);
    const UDNode* node21_3 = new UDNode(2, "had", VBD, 4, Aux, Ind);
    const UDNode* node21_4 = new UDNode(3, "be", VB, 4, Aux_Pass, Ind);
    node21_0->addChild(node21_3);
    node21_0->addChild(node21_4);
    mistakeSet.insert(Mistake("Глаголы при построении пассивного залога (Passive Voice) несогласованы"));
    QTest::newRow("Test 21: had be + delivered (should had been)") << node21_4 << static_cast<const UDNode*>(node21_0) << false << mistakeSet << false << "";
    mistakeSet.clear();

    // Тест 22: Ошибочный вызов, связь не aux:pass
    UDNode* node22_0 = new UDNode(2, "gone", VBN, 0, Root, Ind);
    const UDNode* node22_1 = new UDNode(1, "have", VBP, 2, Aux, Ind);
    node22_0->addChild(node22_1);
    QTest::newRow("Test 22: Invalid aux relation") << node22_1 << static_cast<const UDNode*>(node22_0) << true << emptyMistakes << true << "Invalid auxiliary verb relation (should be aux:pass)";

    // Тест 23-24: Ошибочный вызов, нулевой указатель
    QTest::newRow("Test 23: auxVerb is nullptr") << static_cast<const UDNode*>(nullptr) << static_cast<const UDNode*>(new UDNode("delivered", VBN, None) )<< true << emptyMistakes << true << "Auxiliary verb node pointer is null";
    QTest::newRow("Test 24: mainVerb is nullptr") << static_cast<const UDNode*>(new UDNode("are", VBP, None) )<<static_cast<const UDNode*>( nullptr) << true << emptyMistakes << true << "Main verb node pointer is null";
}

void Tests::testComplexSentenceAgreement()
{
    QFETCH(const UDNode*, inputNode1);
    QFETCH(const UDNode*, inputNode2);
    QFETCH(bool, expAgreement);
    QFETCH(QSet<Mistake>, expMistakes);
    QFETCH(bool, expectException);
    QFETCH(QString, exceptionMessage);

    QSet<Mistake> mistakes;
    ComplexSentenceAgreement gRule;
    bool agreement = false;
    bool exceptionThrown = false;
    QString actualExceptionMessage;

    try {
        agreement = gRule.check(inputNode1, inputNode2, mistakes);
    }
    catch (const QString& e) {
        exceptionThrown = true;
        actualExceptionMessage = e;
    }
    catch (...) {
        exceptionThrown = true;
        actualExceptionMessage = "Unexpected exception type";
    }

    // Проверка исключений
    if (expectException) {
        QVERIFY2(exceptionThrown, "Expected an exception, but none was thrown");
        QCOMPARE(actualExceptionMessage, exceptionMessage);
    } else {
        QVERIFY2(!exceptionThrown, QString("Unexpected exception thrown: %1").arg(actualExceptionMessage).toUtf8());

        // Сравнение результата
        QCOMPARE(agreement, expAgreement);

        // Подробное сравнение ошибок
        if (mistakes != expMistakes) {
            qDebug() << "Discrepancy found in mistakes:";

            QSet<Mistake> unexpectedMistakes = mistakes - expMistakes;
            if (!unexpectedMistakes.isEmpty()) {
                qDebug() << "Unexpected mistakes found (" << unexpectedMistakes.size() << "):";
                for (const Mistake& mistake : unexpectedMistakes) {
                    qDebug() << "  -" << mistake.getMessage();
                }
            }

            QSet<Mistake> missingMistakes = expMistakes - mistakes;
            if (!missingMistakes.isEmpty()) {
                qDebug() << "Missing expected mistakes (" << missingMistakes.size() << "):";
                for (const Mistake& mistake : missingMistakes) {
                    qDebug() << "  +" << mistake.getMessage();
                }
            }
        }
        QCOMPARE(mistakes, expMistakes);
    }
}

void Tests::testComplexSentenceAgreement_data()
{
    // Добавляем столбцы

    QTest::addColumn<const UDNode*>("inputNode1");
    QTest::addColumn<const UDNode*>("inputNode2");
    QTest::addColumn<bool>("expAgreement");
    QTest::addColumn<QSet<Mistake>>("expMistakes");
    QTest::addColumn<bool>("expectException");
    QTest::addColumn<QString>("exceptionMessage");

    QSet<Mistake> emptyMistakes;         // Множество ошибок для тестов, где слова согласованы
    QSet<Mistake> mistakeSet;                // Множество для хранения ошибок

    Mistake mistake("Несогласование времен в сложноподчиненном предложении");

    // ==============================================
    // Тесты 1-2: Главное предложение в будущем без условных/временных союзов
    // ==============================================

    // Тест 1: He will fail the exam because he never studies.
    UDNode* node1_0 = new UDNode(3, "fail", VB, 0, Root, Ind);
    const UDNode* node1_1 = new UDNode(1, "He", PRP, 3, Nsubj, None);
    const UDNode* node1_2 = new UDNode(2, "will", MD, 3, Aux, Ind);
    UDNode* node1_3 = new UDNode(4, "exam", NN, 3, Obj, None);
    const UDNode* node1_4 = new UDNode(5, "the", DT, 4, Det, None);
    UDNode* node1_5 = new UDNode(9, "studies", VBZ, 3, Advcl, None);
    const UDNode* node1_6 = new UDNode(6, "because", IN, 9, Mark, None);
    const UDNode* node1_7 = new UDNode(7, "he", PRP, 9, Nsubj, None);
    const UDNode* node1_8 = new UDNode(8, "never", RB, 9, Advmod, None);

    node1_0->addChild(node1_1);
    node1_0->addChild(node1_2);
    node1_0->addChild(node1_3);
    node1_3->addChild(node1_4);
    node1_0->addChild(node1_5);
    node1_5->addChild(node1_6);
    node1_5->addChild(node1_7);
    node1_5->addChild(node1_8);

    QTest::newRow("Test 1: Future main + present dep (because)")
        << static_cast<const UDNode*>(node1_5) << static_cast<const UDNode*>(node1_0)
        << true << emptyMistakes << false << "";

    // Тест 2: The doctor will examine the patient who was admitted yesterday
    UDNode* node2_0 = new UDNode(3, "examine", VB, 0, Root, Ind);
    UDNode* node2_1 = new UDNode(1, "doctor", NN, 3, Nsubj, None);
    const UDNode* node2_2 = new UDNode(2, "The", DT, 1, Det, None);
    UDNode* node2_3 = new UDNode(4, "patient", NN, 3, Obj, None);
    const UDNode* node2_4 = new UDNode(5, "the", DT, 4, Det, None);
    const UDNode* node2_5 = new UDNode(6, "will", MD, 3, Aux, Ind);
    UDNode* node2_6 = new UDNode(9, "admitted", VBD, 4, Acl_Relcl, None);
    const UDNode* node2_7 = new UDNode(7, "who", WP, 9, Nsubj_Pass, None);
    const UDNode* node2_8 = new UDNode(8, "was", VBD, 9, Aux_Pass, None);
    const UDNode* node2_9 = new UDNode(10, "yesterday", NN, 9, Nmod, None);

    node2_0->addChild(node2_1);
    node2_1->addChild(node2_2);
    node2_0->addChild(node2_3);
    node2_3->addChild(node2_4);
    node2_0->addChild(node2_5);
    node2_3->addChild(node2_6);
    node2_6->addChild(node2_7);
    node2_6->addChild(node2_8);
    node2_6->addChild(node2_9);

    QTest::newRow("Test 2: Future main + past dep (relative clause)")
        << static_cast<const UDNode*>(node2_6) << static_cast<const UDNode*>(node2_0)
        << true << emptyMistakes << false << "";

    // ==============================================
    // Тесты 3-14: Главное в будущем с временными/условными союзами
    // ==============================================

    // Тест 3: I will call you when I arrive
    UDNode* node3_0 = new UDNode(3, "call", VB, 0, Root, Ind);
    const UDNode* node3_1 = new UDNode(1, "I", PRP, 3, Nsubj, None);
    const UDNode* node3_2 = new UDNode(2, "will", MD, 3, Aux, Ind);
    const UDNode* node3_3 = new UDNode(4, "you", PRP, 3, Obj, None);
    UDNode* node3_4 = new UDNode(7, "arrive", VB, 3, Advcl, None);
    const UDNode* node3_5 = new UDNode(5, "when", WRB, 7, Mark, None);
    const UDNode* node3_6 = new UDNode(6, "I", PRP, 7, Nsubj, None);

    node3_0->addChild(node3_1);
    node3_0->addChild(node3_2);
    node3_0->addChild(node3_3);
    node3_0->addChild(node3_4);
    node3_4->addChild(node3_5);
    node3_4->addChild(node3_6);

    QTest::newRow("Test 3: Future + present with 'when'")
        << static_cast<const UDNode*>(node3_4) << static_cast<const UDNode*>(node3_0)
        << true << emptyMistakes << false << "";

    // Тест 4: She will wait until the rain stops
    UDNode* node4_0 = new UDNode(3, "wait", VB, 0, Root, Ind);
    const UDNode* node4_1 = new UDNode(1, "She", PRP, 3, Nsubj, None);
    const UDNode* node4_2 = new UDNode(2, "will", MD, 3, Aux, Ind);
    UDNode* node4_3 = new UDNode(7, "stops", VBZ, 3, Advcl, None);
    const UDNode* node4_4 = new UDNode(4, "until", IN, 7, Mark, None);
    UDNode* node4_5 = new UDNode(5, "rain", NN, 7, Nsubj, None);
    const UDNode* node4_6 = new UDNode(6, "the", DT, 5, Det, None);

    node4_0->addChild(node4_1);
    node4_0->addChild(node4_2);
    node4_0->addChild(node4_3);
    node4_3->addChild(node4_4);
    node4_3->addChild(node4_5);
    node4_5->addChild(node4_6);

    QTest::newRow("Test 4: Future + present with 'until'")
        << static_cast<const UDNode*>(node4_3) << static_cast<const UDNode*>(node4_0)
        << true << emptyMistakes << false << "";

    // Тест 5: They will leave before the concert begins
    UDNode* node5_0 = new UDNode(3, "leave", VB, 0, Root, Ind);
    const UDNode* node5_1 = new UDNode(1, "They", PRP, 3, Nsubj, None);
    const UDNode* node5_2 = new UDNode(2, "will", MD, 3, Aux, Ind);
    UDNode* node5_3 = new UDNode(7, "begins", VBZ, 3, Advcl, None);
    UDNode* node5_4 = new UDNode(4, "before", IN, 7, Mark, None);
    UDNode* node5_5 = new UDNode(5, "concert", NN, 7, Nsubj, None);
    const UDNode* node5_6 = new UDNode(6, "the", DT, 5, Det, None);

    node5_0->addChild(node5_1);
    node5_0->addChild(node5_2);
    node5_0->addChild(node5_3);
    node5_3->addChild(node5_4);
    node5_3->addChild(node5_5);
    node5_5->addChild(node5_6);

    QTest::newRow("Test 5: Future + present with 'before'")
        << static_cast<const UDNode*>(node5_3) << static_cast<const UDNode*>(node5_0)
        << true << emptyMistakes << false << "";

    // Тест 6: He will forgive you if you apologize
    UDNode* node6_0 = new UDNode(3, "forgive", VB, 0, Root, Ind);
    const UDNode* node6_1 = new UDNode(1, "He", PRP, 3, Nsubj, None);
    const UDNode* node6_2 = new UDNode(2, "will", MD, 3, Aux, Ind);
    const UDNode* node6_3 = new UDNode(4, "you", PRP, 3, Obj, None);
    UDNode* node6_4 = new UDNode(7, "apologize", VB, 3, Advcl, None);
    const UDNode* node6_5 = new UDNode(5, "if", IN, 7, Mark, None);
    const UDNode* node6_6 = new UDNode(6, "you", PRP, 7, Nsubj, None);

    node6_0->addChild(node6_1);
    node6_0->addChild(node6_2);
    node6_0->addChild(node6_3);
    node6_0->addChild(node6_4);
    node6_4->addChild(node6_5);
    node6_4->addChild(node6_6);

    QTest::newRow("Test 6: Future + present with 'if'")
        << static_cast<const UDNode*>(node6_4) << static_cast<const UDNode*>(node6_0)
        << true << emptyMistakes << false << "";

    // Тест 7: We won't start dinner unless everyone is here
    UDNode* node7_0 = new UDNode(4, "start", VB, 0, Root, Ind);
    const UDNode* node7_1 = new UDNode(1, "We", PRP, 4, Nsubj, None);
    UDNode* node7_2 = new UDNode(2, "wo", MD, 4, Aux, Ind);
    UDNode* node7_3 = new UDNode(3, "n't", RB, 2, Other, None);
    const UDNode* node7_4 = new UDNode(5, "dinner", NN, 4, Obj, None);
    UDNode* node7_5 = new UDNode(9, "is", VBZ, 4, Advcl, None);
    const UDNode* node7_6 = new UDNode(6, "unless", IN, 9, Mark, None);
    const UDNode* node7_7 = new UDNode(7, "everyone", NN, 9, Nsubj, None);
    const UDNode* node7_8 = new UDNode(8, "here", RB, 9, Advmod, None);

    node7_0->addChild(node7_1);
    node7_0->addChild(node7_2);
    node7_2->addChild(node7_3);
    node7_0->addChild(node7_4);
    node7_0->addChild(node7_5);
    node7_5->addChild(node7_6);
    node7_5->addChild(node7_7);
    node7_5->addChild(node7_8);

    QTest::newRow("Test 7: Future + present with 'unless'")
        << static_cast<const UDNode*>(node7_5) << static_cast<const UDNode*>(node7_0)
        << true << emptyMistakes << false << "";

    // Тест 8: She will text you as soon as she lands
    UDNode* node8_0 = new UDNode(3, "text", VB, 0, Root, Ind);
    const UDNode* node8_1 = new UDNode(1, "She", PRP, 3, Nsubj, None);
    const UDNode* node8_2 = new UDNode(2, "will", MD, 3, Aux, Ind);
    const UDNode* node8_3 = new UDNode(4, "you", PRP, 3, Obj, None);
    UDNode* node8_4 = new UDNode(8, "lands", VBZ, 3, Advcl, None);
    UDNode* node8_5 = new UDNode(5, "as", IN, 8, Mark, None);
    const UDNode* node8_6 = new UDNode(6, "soon", RB, 5, Advmod, None);
    const UDNode* node8_7 = new UDNode(7, "she", PRP, 8, Nsubj, None);

    node8_0->addChild(node8_1);
    node8_0->addChild(node8_2);
    node8_0->addChild(node8_3);
    node8_0->addChild(node8_4);
    node8_4->addChild(node8_5);
    node8_5->addChild(node8_6);
    node8_4->addChild(node8_7);

    QTest::newRow("Test 8: Future + present with 'as soon as'")
        << static_cast<const UDNode*>(node8_4) << static_cast<const UDNode*>(node8_0)
        << true << emptyMistakes << false << "";

    // Тест 9: We will celebrate after we pass the exam
    UDNode* node9_0 = new UDNode(3, "celebrate", VB, 0, Root, Ind);
    const UDNode* node9_1 = new UDNode(1, "We", PRP, 3, Nsubj, None);
    const UDNode* node9_2 = new UDNode(2, "will", MD, 3, Aux, Ind);
    UDNode* node9_3 = new UDNode(7, "pass", VB, 3, Advcl, None);
    const UDNode* node9_4 = new UDNode(4, "after", IN, 7, Mark, None);
    const UDNode* node9_5 = new UDNode(5, "we", PRP, 7, Nsubj, None);
    UDNode* node9_6 = new UDNode(6, "exam", NN, 7, Obj, None);
    const UDNode* node9_7 = new UDNode(8, "the", DT, 6, Det, None);

    node9_0->addChild(node9_1);
    node9_0->addChild(node9_2);
    node9_0->addChild(node9_3);
    node9_3->addChild(node9_4);
    node9_3->addChild(node9_5);
    node9_3->addChild(node9_6);
    node9_6->addChild(node9_7);

    QTest::newRow("Test 9: Future + present with 'after'")
        << static_cast<const UDNode*>(node9_3) << static_cast<const UDNode*>(node9_0)
        << true << emptyMistakes << false << "";

    // Тест 10: She will listen to music while she cooks dinner
    UDNode* node10_0 = new UDNode(3, "listen", VB, 0, Root, Ind);
    const UDNode* node10_1 = new UDNode(1, "She", PRP, 3, Nsubj, None);
    const UDNode* node10_2 = new UDNode(2, "will", MD, 3, Aux, Ind);
    UDNode* node10_3 = new UDNode(4, "music", NN, 3, Obj, None);
    const UDNode* node10_4 = new UDNode(5, "to", IN, 4, Case, None);
    UDNode* node10_5 = new UDNode(9, "cooks", VBZ, 3, Advcl, None);
    const UDNode* node10_6 = new UDNode(6, "while", IN, 9, Mark, None);
    const UDNode* node10_7 = new UDNode(7, "she", PRP, 9, Nsubj, None);
    const UDNode* node10_8 = new UDNode(8, "dinner", NN, 9, Obj, None);

    node10_0->addChild(node10_1);
    node10_0->addChild(node10_2);
    node10_0->addChild(node10_3);
    node10_3->addChild(node10_4);
    node10_0->addChild(node10_5);
    node10_5->addChild(node10_6);
    node10_5->addChild(node10_7);
    node10_5->addChild(node10_8);

    QTest::newRow("Test 10: Future + present with 'while'")
        << static_cast<const UDNode*>(node10_5) << static_cast<const UDNode*>(node10_0)
        << true << emptyMistakes << false << "";

    // Тест 11: Take an umbrella in case it rains
    UDNode* node11_0 = new UDNode(1, "Take", VB, 0, Root, Imp);
    UDNode* node11_1 = new UDNode(2, "umbrella", NN, 1, Obj, None);
    const UDNode* node11_2 = new UDNode(3, "an", DT, 2, Det, None);
    UDNode* node11_3 = new UDNode(6, "rains", VBZ, 1, Advcl, None);
    UDNode* node11_4 = new UDNode(4, "in", IN, 6, Mark, None);
    const UDNode* node11_5 = new UDNode(5, "case", NN, 4, Fixed, None);
    const UDNode* node11_6 = new UDNode(7, "it", PRP, 6, Nsubj, None);

    node11_0->addChild(node11_1);
    node11_1->addChild(node11_2);
    node11_0->addChild(node11_3);
    node11_3->addChild(node11_4);
    node11_4->addChild(node11_5);
    node11_3->addChild(node11_6);

    QTest::newRow("Test 11: Imperative + present with 'in case'")
        << static_cast<const UDNode*>(node11_3) << static_cast<const UDNode*>(node11_0)
        << true << emptyMistakes << false << "";

    // Тест 12: Once you sign the contract, we'll start
    UDNode* node12_0 = new UDNode(6, "start", VB, 0, Root, Ind);
    const UDNode* node12_1 = new UDNode(4, "we", PRP, 6, Nsubj, None);
    const UDNode* node12_2 = new UDNode(5, "'ll", MD, 6, Aux, Ind);
    UDNode* node12_3 = new UDNode(3, "sign", VB, 6, Advcl, None);
    const UDNode* node12_4 = new UDNode(1, "Once", IN, 3, Mark, None);
    const UDNode* node12_5 = new UDNode(2, "you", PRP, 3, Nsubj, None);
    UDNode* node12_6 = new UDNode(7, "contract", NN, 3, Obj, None);
    const UDNode* node12_7 = new UDNode(8, "the", DT, 7, Det, None);

    node12_0->addChild(node12_1);
    node12_0->addChild(node12_2);
    node12_0->addChild(node12_3);
    node12_3->addChild(node12_4);
    node12_3->addChild(node12_5);
    node12_3->addChild(node12_6);
    node12_6->addChild(node12_7);

    QTest::newRow("Test 12: Future + present with 'once'")
        << static_cast<const UDNode*>(node12_3) << static_cast<const UDNode*>(node12_0)
        << true << emptyMistakes << false << "";

    // Тест 13: By the time you arrive, we will have left
    UDNode* node13_0 = new UDNode(6, "left", VBN, 0, Root, Ind);
    const UDNode* node13_1 = new UDNode(4, "we", PRP, 6, Nsubj, None);
    const UDNode* node13_2 = new UDNode(5, "will", MD, 6, Aux, Ind);
    const UDNode* node13_3 = new UDNode(7, "have", VB, 6, Aux, None);
    UDNode* node13_4 = new UDNode(3, "arrive", VB, 6, Advcl, None);
    UDNode* node13_5 = new UDNode(1, "By", IN, 3, Mark, None);
    UDNode* node13_6 = new UDNode(2, "time", NN, 1, Fixed, None);
    const UDNode* node13_7 = new UDNode(8, "you", PRP, 3, Nsubj, None);
    const UDNode* node13_8 = new UDNode(9, "the", DT, 2, Det, None);

    node13_0->addChild(node13_1);
    node13_0->addChild(node13_2);
    node13_0->addChild(node13_3);
    node13_0->addChild(node13_4);
    node13_4->addChild(node13_5);
    node13_5->addChild(node13_6);
    node13_4->addChild(node13_7);
    node13_6->addChild(node13_8);

    QTest::newRow("Test 13: Future perfect + present with 'by the time'")
        << static_cast<const UDNode*>(node13_4) << static_cast<const UDNode*>(node13_0)
        << true << emptyMistakes << false << "";

    // Тест 14: I'll help you as long as you listen
    UDNode* node14_0 = new UDNode(3, "help", VB, 0, Root, Ind);
    const UDNode* node14_1 = new UDNode(1, "I", PRP, 3, Nsubj, None);
    const UDNode* node14_2 = new UDNode(2, "'ll", MD, 3, Aux, Ind);
    const UDNode* node14_3 = new UDNode(4, "you", PRP, 3, Obj, None);
    UDNode* node14_4 = new UDNode(8, "listen", VB, 3, Advcl, None);
    UDNode* node14_5 = new UDNode(5, "as", IN, 8, Mark, None);
    const UDNode* node14_6 = new UDNode(6, "long", RB, 5, Fixed, None);
    const UDNode* node14_7 = new UDNode(7, "you", PRP, 8, Nsubj, None);

    node14_0->addChild(node14_1);
    node14_0->addChild(node14_2);
    node14_0->addChild(node14_3);
    node14_0->addChild(node14_4);
    node14_4->addChild(node14_5);
    node14_5->addChild(node14_6);
    node14_4->addChild(node14_7);

    QTest::newRow("Test 14: Future + present with 'as long as'")
        << static_cast<const UDNode*>(node14_4) << static_cast<const UDNode*>(node14_0)
        << true << emptyMistakes << false << "";

    // ==============================================
    // Тест 15: Особый случай с before + future
    // ==============================================

    // Тест 15: We will establish the rules before the committee will convene next month
    UDNode* node15_0 = new UDNode(3, "establish", VB, 0, Root, Ind);
    const UDNode* node15_1 = new UDNode(1, "We", PRP, 3, Nsubj, None);
    const UDNode* node15_2 = new UDNode(2, "will", MD, 3, Aux, Ind);
    UDNode* node15_3 = new UDNode(4, "rules", NNS, 3, Obj, None);
    const UDNode* node15_4 = new UDNode(5, "the", DT, 4, Det, None);
    UDNode* node15_5 = new UDNode(10, "convene", VB, 3, Advcl, None);
    const UDNode* node15_6 = new UDNode(6, "before", IN, 10, Mark, None);
    UDNode* node15_7 = new UDNode(7, "committee", NN, 10, Nsubj, None);
    const UDNode* node15_8 = new UDNode(8, "the", DT, 7, Det, None);
    const UDNode* node15_9 = new UDNode(9, "will", MD, 10, Aux, Ind);
    UDNode* node15_10 = new UDNode(11, "month", NN, 10, Nmod, None);
    const UDNode* node15_11 = new UDNode(12, "next", JJ, 11, Amod, None);

    node15_0->addChild(node15_1);
    node15_0->addChild(node15_2);
    node15_0->addChild(node15_3);
    node15_3->addChild(node15_4);
    node15_0->addChild(node15_5);
    node15_5->addChild(node15_6);
    node15_5->addChild(node15_7);
    node15_7->addChild(node15_8);
    node15_5->addChild(node15_9);
    node15_5->addChild(node15_10);
    node15_10->addChild(node15_11);

    QTest::newRow("Test 15: Future + future with 'before' (special case)")
        << static_cast<const UDNode*>(node15_5) << static_cast<const UDNode*>(node15_0)
        << true << emptyMistakes << false << "";

    // ==============================================
    // Тесты 16-17: Ошибки в согласовании времен
    // ==============================================

    // Тест 16: I will call you when I will arrive at the station
    UDNode* node16_0 = new UDNode(3, "call", VB, 0, Root, Ind);
    const UDNode* node16_1 = new UDNode(1, "I", PRP, 3, Nsubj, None);
    const UDNode* node16_2 = new UDNode(2, "will", MD, 3, Aux, Ind);
    const UDNode* node16_3 = new UDNode(4, "you", PRP, 3, Obj, None);
    UDNode* node16_4 = new UDNode(8, "arrive", VB, 3, Advcl, None);
    const UDNode* node16_5 = new UDNode(5, "when", WRB, 8, Mark, None);
    const UDNode* node16_6 = new UDNode(6, "I", PRP, 8, Nsubj, None);
    const UDNode* node16_7 = new UDNode(7, "will", MD, 8, Aux, Ind);
    UDNode* node16_8 = new UDNode(9, "station", NN, 8, Obl, None);
    const UDNode* node16_9 = new UDNode(10, "the", DT, 9, Det, None);
    const UDNode* node16_10 = new UDNode(11, "at", IN, 9, Case, None);

    node16_0->addChild(node16_1);
    node16_0->addChild(node16_2);
    node16_0->addChild(node16_3);
    node16_0->addChild(node16_4);
    node16_4->addChild(node16_5);
    node16_4->addChild(node16_6);
    node16_4->addChild(node16_7);
    node16_4->addChild(node16_8);
    node16_8->addChild(node16_9);
    node16_8->addChild(node16_10);

    mistakeSet.insert(mistake);
    QTest::newRow("Test 16: Future + future with 'when' - error")
        << static_cast<const UDNode*>(node16_4) << static_cast<const UDNode*>(node16_0)
        << false << mistakeSet << false << "";
    mistakeSet.clear();

    // Тест 17: I will be surprised when I heard the news
    UDNode* node17_0 = new UDNode(4, "surprised", JJ, 0, Root, Ind);
    const UDNode* node17_1 = new UDNode(1, "I", PRP, 4, Nsubj, None);
    const UDNode* node17_2 = new UDNode(2, "will", MD, 4, Aux, Ind);
    const UDNode* node17_3 = new UDNode(3, "be", VB, 4, Cop, None);
    UDNode* node17_4 = new UDNode(8, "heard", VBD, 4, Advcl, None);
    const UDNode* node17_5 = new UDNode(5, "when", WRB, 8, Mark, None);
    const UDNode* node17_6 = new UDNode(6, "I", PRP, 8, Nsubj, None);
    UDNode* node17_7 = new UDNode(7, "news", NN, 8, Obj, None);
    const UDNode* node17_8 = new UDNode(9, "the", DT, 7, Det, None);

    node17_0->addChild(node17_1);
    node17_0->addChild(node17_2);
    node17_0->addChild(node17_3);
    node17_0->addChild(node17_4);
    node17_4->addChild(node17_5);
    node17_4->addChild(node17_6);
    node17_4->addChild(node17_7);
    node17_7->addChild(node17_8);

    mistakeSet.insert(mistake);
    QTest::newRow("Test 17: Future + past with 'when' - error")
        << static_cast<const UDNode*>(node17_4) << static_cast<const UDNode*>(node17_0)
        << false << mistakeSet << false << "";
    mistakeSet.clear();

    // ==============================================
    // Тест 18: Главное в прошедшем, придаточное в прошедшем
    // ==============================================

    // Тест 18: While I was cooking dinner, the phone rang
    UDNode* node18_0 = new UDNode(5, "rang", VBD, 0, Root, Ind);
    UDNode* node18_1 = new UDNode(3, "phone", NN, 5, Nsubj, None);
    const UDNode* node18_2 = new UDNode(4, "the", DT, 3, Det, None);
    UDNode* node18_3 = new UDNode(3, "cooking", VBG, 5, Advcl, None);
    const UDNode* node18_4 = new UDNode(1, "While", IN, 3, Mark, None);
    const UDNode* node18_5 = new UDNode(2, "I", PRP, 3, Nsubj, None);
    const UDNode* node18_6 = new UDNode(6, "was", VBD, 3, Aux, None);
    const UDNode* node18_7 = new UDNode(7, "dinner", NN, 3, Obj, None);

    node18_0->addChild(node18_1);
    node18_1->addChild(node18_2);
    node18_0->addChild(node18_3);
    node18_3->addChild(node18_4);
    node18_3->addChild(node18_5);
    node18_3->addChild(node18_6);
    node18_3->addChild(node18_7);

    QTest::newRow("Test 18: Past + past with 'while'")
        << static_cast<const UDNode*>(node18_3) << static_cast<const UDNode*>(node18_0)
        << true << emptyMistakes << false << "";

    // ==============================================
    // Тесты 19-20: Ошибки в прошедшем времени
    // ==============================================

    // Тест 19: She left because he will feel sick
    UDNode* node19_0 = new UDNode(2, "left", VBD, 0, Root, Ind);
    const UDNode* node19_1 = new UDNode(1, "She", PRP, 2, Nsubj, None);
    UDNode* node19_2 = new UDNode(6, "feel", VB, 2, Advcl, None);
    const UDNode* node19_3 = new UDNode(3, "because", IN, 6, Mark, None);
    const UDNode* node19_4 = new UDNode(4, "he", PRP, 6, Nsubj, None);
    const UDNode* node19_5 = new UDNode(5, "will", MD, 6, Aux, Ind);
    const UDNode* node19_6 = new UDNode(7, "sick", JJ, 6, Ccomp, None);

    node19_0->addChild(node19_1);
    node19_0->addChild(node19_2);
    node19_2->addChild(node19_3);
    node19_2->addChild(node19_4);
    node19_2->addChild(node19_5);
    node19_2->addChild(node19_6);

    mistakeSet.insert(mistake);
    QTest::newRow("Test 19: Past + future with 'because' - error")
        << static_cast<const UDNode*>(node19_2) << static_cast<const UDNode*>(node19_0)
        << false << mistakeSet << false << "";
    mistakeSet.clear();

    // Тест 20: She left because he feels sick
    UDNode* node20_0 = new UDNode(2, "left", VBD, 0, Root, Ind);
    const UDNode* node20_1 = new UDNode(1, "She", PRP, 2, Nsubj, None);
    UDNode* node20_2 = new UDNode(5, "feels", VBZ, 2, Advcl, None);
    const UDNode* node20_3 = new UDNode(3, "because", IN, 5, Mark, None);
    const UDNode* node20_4 = new UDNode(4, "he", PRP, 5, Nsubj, None);
    const UDNode* node20_5 = new UDNode(6, "sick", JJ, 5, Ccomp, None);

    node20_0->addChild(node20_1);
    node20_0->addChild(node20_2);
    node20_2->addChild(node20_3);
    node20_2->addChild(node20_4);
    node20_2->addChild(node20_5);

    mistakeSet.insert(mistake);
    QTest::newRow("Test 20: Past + present with 'because' - error")
        << static_cast<const UDNode*>(node20_2) << static_cast<const UDNode*>(node20_0)
        << false << mistakeSet << false << "";
    mistakeSet.clear();

    // ==============================================
    // Тесты 21-23: Ошибочные вызовы
    // ==============================================

    // Тест 21: Неверная связь (have gone)
    UDNode* node21_0 = new UDNode(2, "gone", VBN, 0, Root, Ind);
    const UDNode* node21_1 = new UDNode(1, "have", VBP, 2, Aux, Ind);
    node21_0->addChild(node21_1);
    QTest::newRow("Test 21: Invalid clause relation")
        << node21_1 << static_cast<const UDNode*>(node21_0)
        << true << emptyMistakes << true << "Invalid clause relation";

    // Тест 22: depVerb is nullptr
    QTest::newRow("Test 22: depVerb is nullptr")
        << static_cast<const UDNode*>(nullptr) << static_cast<const UDNode*>(new UDNode("call", VB, None))
        << true << emptyMistakes << true << "Dependent verb node pointer is null";

    // Тест 23: mainVerb is nullptr
    QTest::newRow("Test 23: mainVerb is nullptr")
        << static_cast<const UDNode*>(new UDNode("arrive", VB, None)) << static_cast<const UDNode*>(nullptr)
        << true << emptyMistakes << true << "Main verb node pointer is null";
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
    // Подробное сравнение ошибок
    if (mistakes != expMistakes)
    {
        qDebug() << "Discrepancy found in mistakes:";

        // Находим ошибки, которые есть в mistakes, но нет в expMistakes
        QSet<Mistake> unexpectedMistakes = mistakes - expMistakes;
        if (!unexpectedMistakes.isEmpty())
        {
            qDebug() << "Unexpected mistakes found (" << unexpectedMistakes.size() << "):";
            for (const Mistake& mistake : unexpectedMistakes)
            {
                qDebug() << "  -" << mistake.getMessage();
            }
        }

        // Находим ошибки, которые есть в expMistakes, но нет в mistakes
        QSet<Mistake> missingMistakes = expMistakes - mistakes;
        if (!missingMistakes.isEmpty())
        {
            qDebug() << "Missing expected mistakes (" << missingMistakes.size() << "):";
            for (const Mistake& mistake : missingMistakes)
            {
                qDebug() << "  +" << mistake.getMessage();
            }
        }
    }
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

    // Подробное сравнение ошибок
    if (actualErrors != expErrors)
    {
        qDebug() << "Discrepancy found in error:";

        // Находим ошибки, которые есть в actualErrors, но нет в expErrors
        QSet<Error> unexpectedErrors = actualErrors - expErrors;
        if (!unexpectedErrors.isEmpty())
        {
            qDebug() << "Unexpected error found (" << unexpectedErrors.size() << "):";
            for (const Error& error : unexpectedErrors)
            {
                qDebug() << "  -" << error.getMessage();
            }
        }

        // Находим ошибки, которые есть в expErrors, но нет в actualErrors
        QSet<Error> missingErrors = expErrors - actualErrors;
        if (!missingErrors.isEmpty())
        {
            qDebug() << "Missing expected errors (" << missingErrors.size() << "):";
            for (const Error& error : missingErrors)
            {
                qDebug() << "  +" << error.getMessage();
            }
        }
    }
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

    // Подробное сравнение ошибок
    if (actualErrors != expErrors)
    {
        qDebug() << "Discrepancy found in error:";

        // Находим ошибки, которые есть в actualErrors, но нет в expErrors
        QSet<Error> unexpectedErrors = actualErrors - expErrors;
        if (!unexpectedErrors.isEmpty())
        {
            qDebug() << "Unexpected error found (" << unexpectedErrors.size() << "):";
            for (const Error& error : unexpectedErrors)
            {
                qDebug() << "  -" << error.getMessage();
            }
        }

        // Находим ошибки, которые есть в expErrors, но нет в actualErrors
        QSet<Error> missingErrors = expErrors - actualErrors;
        if (!missingErrors.isEmpty())
        {
            qDebug() << "Missing expected errors (" << missingErrors.size() << "):";
            for (const Error& error : missingErrors)
            {
                qDebug() << "  +" << error.getMessage();
            }
        }
    }
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

    // Подробное сравнение ошибок
    if (actualErrors != expErrors)
    {
        qDebug() << "Discrepancy found in error:";

        // Находим ошибки, которые есть в actualErrors, но нет в expErrors
        QSet<Error> unexpectedErrors = actualErrors - expErrors;
        if (!unexpectedErrors.isEmpty())
        {
            qDebug() << "Unexpected error found (" << unexpectedErrors.size() << "):";
            for (const Error& error : unexpectedErrors)
            {
                qDebug() << "  -" << error.getMessage();
            }
        }

        // Находим ошибки, которые есть в expErrors, но нет в actualErrors
        QSet<Error> missingErrors = expErrors - actualErrors;
        if (!missingErrors.isEmpty())
        {
            qDebug() << "Missing expected errors (" << missingErrors.size() << "):";
            for (const Error& error : missingErrors)
            {
                qDebug() << "  +" << error.getMessage();
            }
        }
    }
    QCOMPARE(actualErrors, expErrors);

    // сравнение детей
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
    rel->getNodes(mainNode,&actNode,parent);

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

void Tests::testMatchesPattern()
{
    qDebug() << "Test data before fail:";
    QFETCH(const UDNode*, inputNode);
    QFETCH(Pattern*, pat);
    QFETCH(Pattern*, expPat);
    QFETCH(bool, expMatch);

    qDebug() << "Test data before fail:" << inputNode << pat << expPat;
    QSet<const UDNode*> actualUsedChildren;
    bool actualMatch = pat->matchesPattern(inputNode, actualUsedChildren);

    QCOMPARE(actualMatch, expMatch);

    qDebug() << "Test data before fail:" << inputNode << pat << expPat;
    QStringList mismatchErrors;
    pat->compareMatches(expPat, mismatchErrors, "root");
    qDebug() << "Test data before fail:" << inputNode << pat << expPat;

    if (!mismatchErrors.isEmpty())
    {
        QString errorReport = "Pattern structure mismatch details:\n";
        errorReport += "================================\n";
        errorReport += mismatchErrors.join("\n--------------------------------\n");
        errorReport += "\n================================\n";
        QFAIL(qPrintable(errorReport));
    }

}

void Tests::testMatchesPattern_data()
{

    QTest::addColumn<const UDNode*>("inputNode");
    QTest::addColumn<Pattern*>("pat");
    QTest::addColumn<Pattern*>("expPat");
    QTest::addColumn<bool>("expMatch");


    //Тест №1. Узел без детей.
    QMap<int, UDNode*> tree1;
    UDNode* node1_0 = new UDNode(1,"Hello",UH,0,Root,None);
    tree1.insert(1,node1_0);
    Pattern* pattern1 = new Pattern({},{});
    Pattern* pattern1s = new Pattern({},{});
    pattern1s->setMatch(node1_0);

    QTest::newRow("Test 1: One word, Any pattern") <<static_cast<const UDNode*>(node1_0) << pattern1 << pattern1s << true;

}

void Tests::testCheckPattern()
{
    QFETCH(const UDNode*, inputNode);
    QFETCH(Pattern*, pat);
    QFETCH(QSet<Mistake>, expMistakes);

    QSet<Mistake> mistakes;

    inputNode->checkPattern(pat,mistakes);

    // Подробное сравнение ошибок
    if (mistakes != expMistakes)
    {
        qDebug() << "Discrepancy found in mistakes:";

        // Находим ошибки, которые есть в mistakes, но нет в expMistakes
        QSet<Mistake> unexpectedMistakes = mistakes - expMistakes;
        if (!unexpectedMistakes.isEmpty())
        {
            qDebug() << "Unexpected mistakes found (" << unexpectedMistakes.size() << "):";
            for (const Mistake& mistake : unexpectedMistakes)
            {
                qDebug() << "  -" << mistake.getMessage();
            }
        }

        // Находим ошибки, которые есть в expMistakes, но нет в mistakes
        QSet<Mistake> missingMistakes = expMistakes - mistakes;
        if (!missingMistakes.isEmpty())
        {
            qDebug() << "Missing expected mistakes (" << missingMistakes.size() << "):";
            for (const Mistake& mistake : missingMistakes)
            {
                qDebug() << "  +" << mistake.getMessage();
            }
        }
    }
    QCOMPARE(mistakes, expMistakes);

}
void Tests::testCheckPattern_data()
{
    QTest::addColumn<const UDNode*>("inputNode");
    QTest::addColumn<Pattern*>("pat");
    QTest::addColumn<QSet<Mistake>>("expMistakes");

    QSet<Mistake> mistakes;

    //Тест 1
    UDNode* node1_0 = new UDNode(1,"Hello",UH,0,Root,None);
    Pattern* pattern1 = new Pattern({},{});
    mistakes.clear();
    QTest::newRow("Test 1: no checks") <<static_cast<const UDNode*>(node1_0) << pattern1 << mistakes;

}


void Tests::testCheckAllPatterns()
{
    typedef QMap<int, UDNode*> NodeMap;
    QFETCH(NodeMap, inputNodes);
    QFETCH(QSet<Pattern*>, pats);
    QFETCH(QSet<Mistake>, expMistakes);

    QSet<Mistake> mistakes;

    checkAllPatterns(inputNodes, pats, mistakes);

    // Подробное сравнение ошибок
    if (mistakes != expMistakes)
    {
        qDebug() << "Discrepancy found in mistakes:";

        // Находим ошибки, которые есть в mistakes, но нет в expMistakes
        QSet<Mistake> unexpectedMistakes = mistakes - expMistakes;
        if (!unexpectedMistakes.isEmpty())
        {
            qDebug() << "Unexpected mistakes found (" << unexpectedMistakes.size() << "):";
            for (const Mistake& mistake : unexpectedMistakes)
            {
                qDebug() << "  -" << mistake.getMessage();
            }
        }

        // Находим ошибки, которые есть в expMistakes, но нет в mistakes
        QSet<Mistake> missingMistakes = expMistakes - mistakes;
        if (!missingMistakes.isEmpty())
        {
            qDebug() << "Missing expected mistakes (" << missingMistakes.size() << "):";
            for (const Mistake& mistake : missingMistakes)
            {
                qDebug() << "  +" << mistake.getMessage();
            }
        }
    }
    QCOMPARE(mistakes, expMistakes);
}

void Tests::testCheckAllPatterns_data()
{
    QTest::addColumn<QMap<int, UDNode*>>("inputNodes");
    QTest::addColumn<QSet<Pattern*>>("pats");
    QTest::addColumn<QSet<Mistake>>("expMistakes");

    QSet<Mistake> mistakes;
    QSet<Pattern*> pats;
    QMap<int,UDNode*> tree;
    //Тест 1
    UDNode* node1_0 = new UDNode(1,"Hello",UH,0,Root,None);
    Pattern* pattern1 = new Pattern({},{});
    mistakes.clear();
    QTest::newRow("Test 1: no checks") << tree <<pats <<mistakes;
}
