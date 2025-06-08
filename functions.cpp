#include <Header.h>

/*!
* \brief Функция для считывания данных о слове из строк в объекты класса
* \param [in] lines – строки с данными о слове и его связях
* \param [out] nodes – ассоциативный контейнер связей по идентификатору
* \param [in,out]  errors- множество ошибок
*/
void createNodesFromLines(const QStringList& lines, QMap<int, UDNode*>&nodes, QSet<Error>& errors)
{
    return;
}

/*!
* \brief Функция проверки корректности дерева
* \param [in] root – корень дерева
* \param [in,out] nodes – ассоциативный контейнер связей по идентификатору
* \param [in,out]  errors- множество ошибок
* \return узлы являются связанными в дерево
*/
bool checkTreeConnectivity(UDNode* root, QMap<int, UDNode*>& nodes, QSet<Error>&errors)
{
    return true;
}

/*!
* \brief Функция для построения дерева из связей
* \param [in,out] nodes – ассоциативный контейнер связей по идентификатору
* \param [in,out]  errors- множество ошибок
* \return указатель на корень дерева
*/
UDNode* addChildren(QMap<int, UDNode*>& nodes, QSet<Error>& errors)
{
    UDNode* tree = NULL;
    return tree;
}

/*!
* \brief Функция, которая проверяет все узлы на совпадение шаблонам и вызывает необходимые проверки для них
* \param [in] nodes – ассоциативный контейнер связей по идентификатору
* \param [in] patterns – шаблоны правил
* \param [in,out] mistakes – ошибки согласования
*/
void checkAllPatterns(const QMap<int, UDNode*>& nodes, const QSet<Pattern*> & patterns, QSet<Mistake> &mistakes)
{

};

QString depRelToString(DepRel rel)
{
    switch(rel)
    {
    case Acl: return "acl";
    case Acl_Relcl: return "acl:relcl";
    case Advcl: return "advcl";
    case Advcl_Relcl: return "advcl:relcl";
    case Advmod: return "advmod";
    case Advmod_Emph: return "advmod:emph";
    case Advmod_Lmod: return "advmod:lmod";
    case Amod: return "amod";
    case Appos: return "appos";
    case Aux: return "aux";
    case Aux_Pass: return "aux:pass";
    case Case: return "case";
    case Cc: return "cc";
    case Cc_Preconj: return "cc:preconj";
    case Ccomp: return "ccomp";
    case Clf: return "clf";
    case Compound: return "compound";
    case Compound_Lvc: return "compound:lvc";
    case Compound_Prt: return "compound:prt";
    case Compound_Redup: return "compound:redup";
    case Compound_Svc: return "compound:svc";
    case Conj: return "conj";
    case Cop: return "cop";
    case Csubj: return "csubj";
    case Csubj_Outer: return "csubj:outer";
    case Csubj_Pass: return "csubj:pass";
    case Dep: return "dep";
    case Det: return "det";
    case Det_Numgov: return "det:numgov";
    case Det_Nummod: return "det:nummod";
    case Det_Poss: return "det:poss";
    case Discourse: return "discourse";
    case Dislocated: return "dislocated";
    case Expl: return "expl";
    case Expl_Impers: return "expl:impers";
    case Expl_Pass: return "expl:pass";
    case Expl_Pv: return "expl:pv";
    case Fixed: return "fixed";
    case Flat: return "flat";
    case Flat_Name: return "flat:name";
    case Goeswith: return "goeswith";
    case Iobj: return "iobj";
    case List: return "list";
    case Mark: return "mark";
    case Nmod: return "nmod";
    case Nmod_Poss: return "nmod:poss";
    case Nmod_Tmod: return "nmod:tmod";
    case Nsubj: return "nsubj";
    case Nsubj_Outer: return "nsubj:outer";
    case Nsubj_Pass: return "nsubj:pass";
    case Nummod: return "nummod";
    case Nummod_Gov: return "nummod:gov";
    case Obj: return "obj";
    case Obl: return "obl";
    case Obl_Agent: return "obl:agent";
    case Obl_Arg: return "obl:arg";
    case Obl_Lmod: return "obl:lmod";
    case Obl_Tmod: return "obl:tmod";
    case Obl_Npmod: return "obl:npmod";
    case Orphan: return "orphan";
    case Parataxis: return "parataxis";
    case Punct: return "punct";
    case Reparandum: return "reparandum";
    case Root: return "root";
    case Vocative: return "vocative";
    case Xcomp: return "xcomp";
    case Other: return "other";
    default: return QString("unknown(%1)").arg(static_cast<int>(rel));
    }
}
