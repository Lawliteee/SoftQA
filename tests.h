#ifndef TESTS_H
#define TESTS_H

#include <QObject>
#include <reltypecheck.h>
#include <QtTest/QtTest>

class Tests : public QObject
{
    Q_OBJECT

private slots:
    void testPersonNumberCheck();
    void testPersonNumberCheck_data();

    void testNumberCheck();
    void testNumberCheck_data();

    void testMainAuxAgreement();
    void testMainAuxAgreement_data();

    void testAuxAuxAgreement();
    void testAuxAuxAgreement_data();

    void testPassiveAgreement();
    void testPassiveAgreement_data();

    void testComplexSentenceAgreement();
    void testComplexSentenceAgreement_data();

    void testConditionalsAgreement();
    void testConditionalsAgreement_data();

    void testCreateNodesFromLines();
    void testCreateNodesFromLines_data();

    void testCheckTreeConnectivity();
    void testCheckTreeConnectivity_data();

    void testAddChildren();
    void testAddChildren_data();

    void testGetNodesChild();
    void testGetNodesChild_data();

    // void testGetNodesParent();
    // void testGetNodesParent_data();

    //void testMatchesPattern();
    //void testMatchesPattern_data();

    // void testCheckPattern();
    // void testCheckPattern_data();

    // void testCheckAllPatterns();
    // void testCheckAllPatterns_data();
};

#endif // TESTS_H
