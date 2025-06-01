#ifndef TESTS_H
#define TESTS_H

#include <QObject>
#include <grammarrule.h>
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

    void testCreateNodesFromLines();
    void testCreateNodesFromLines_data();
};

#endif // TESTS_H
