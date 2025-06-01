#include "grammarrule.h"

GrammarRule::GrammarRule() {}


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

bool PersonNumberAgreement::check(const UDNode* node1,const UDNode* node2,QSet<Mistake>& mistakes )
{
    return true;
};

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
