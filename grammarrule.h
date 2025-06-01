#ifndef GRAMMARRULE_H
#define GRAMMARRULE_H


#include <mistake.h>

class GrammarRule
{
public:
    GrammarRule();
    virtual bool check(const UDNode*,const UDNode*,QSet<Mistake>& mistakes );
};

class PersonNumberAgreement : public GrammarRule
{
public:
    bool check(const UDNode*,const UDNode*,QSet<Mistake>& mistakes ) override;
};

class NumberAgreement : public GrammarRule
{
public:
    bool check(const UDNode*,const UDNode*,QSet<Mistake>& mistakes ) override;
};

class AuxAuxAgreement : public GrammarRule
{
public:
    bool check(const UDNode*,const UDNode*,QSet<Mistake>& mistakes ) override;
};

class MainAuxAgreement : public GrammarRule
{
public:
    bool check(const UDNode*,const UDNode*,QSet<Mistake>& mistakes ) override;
};

class PassiveAgreement : public GrammarRule
{
public:
    bool check(const UDNode*,const UDNode*,QSet<Mistake>& mistakes ) override;
};

class ComplexSentenceAgreement : public GrammarRule
{
public:
    bool check(const UDNode*,const UDNode*,QSet<Mistake>& mistakes ) override;
};

class ConditionalsAgreement : public GrammarRule
{
public:
    bool check(const UDNode*,const UDNode*,QSet<Mistake>& mistakes ) override;
};

#endif // GRAMMARRULE_H
