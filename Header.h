#ifndef HEADER_H
#define HEADER_H

#endif // HEADER_H

#include <QObject>
#include <QMultiMap>
#include <QSet>

class UDNode;
class Error;
class Pattern;
class Mistake;

enum DepRel {
    Acl, Acl_Relcl, Advcl, Advcl_Relcl, Advmod, Advmod_Emph, Advmod_Lmod, Amod, Appos, Aux, Aux_Pass, Case, Cc, Cc_Preconj, Ccomp, Clf,
    Compound, Compound_Lvc, Compound_Prt, Compound_Redup, Compound_Svc, Conj, Cop, Csubj, Csubj_Outer, Csubj_Pass, Dep, Det, Det_Numgov,
    Det_Nummod, Det_Poss, Discourse, Dislocated, Expl, Expl_Impers, Expl_Pass, Expl_Pv, Fixed, Flat, Flat_Name, Goeswith, Iobj, List, Mark, Nmod,
    Nmod_Poss, Nmod_Tmod, Nsubj, Nsubj_Outer, Nsubj_Pass, Nummod, Nummod_Gov, Obj, Obl, Obl_Agent, Obl_Arg, Obl_Lmod, Obl_Tmod, Obl_Npmod,
    Orphan, Parataxis, Punct, Reparandum, Root, Vocative, Xcomp
};

enum VerbMood {
    None, Indicative, Imperative, Subjunctive
};

enum PosTag {
    CC, CD, DT, EX, FW, IN, JJ, JJR, JJS, LS, MD, NN, NNS,
    NNP, NNPS, VB, VBD, VBG, VBN, VBP, VBZ, PRP, PRP_,WP,WP_ ,
    WRB, RB, RBR, RBS, RP, SYM, TO, UH, WDT, PDT, ANY
};


/*!
* \brief Функция для считывания данных о слове из строк в объекты класса
* \param [in] lines – строки с данными о слове и его связях
* \param [out] nodes – ассоциативный контейнер связей по идентификатору
* \param [in,out]  errors- множество ошибок
*/
void createNodesFromLines(const QStringList& lines, QMap<int, UDNode*>&nodes, QSet<Error>& errors);

/*!
* \brief Функция проверки корректности дерева
* \param [in] root – корень дерева
* \param [in,out] nodes – ассоциативный контейнер связей по идентификатору
* \param [in,out]  errors- множество ошибок
* \return узлы являются связанными в дерево
*/
bool checkTreeConnectivity(UDNode* root, const QMap<int, UDNode*>& nodes, QSet<Error>&errors);

/*!
* \brief Функция для построения дерева из связей
* \param [in,out] nodes – ассоциативный контейнер связей по идентификатору
* \param [in,out]  errors- множество ошибок
* \return указатель на корень дерева
*/
UDNode* addChildren(QMap<int, UDNode*>& nodes, QSet<Error>& errors);

/*!
* \brief Функция, которая проверяет все узлы на совпадение шаблонам и вызывает необходимые проверки для них
* \param [in] nodes – ассоциативный контейнер связей по идентификатору
* \param [in] patterns – шаблоны правил
* \param [in,out] mistakes – ошибки согласования
*/
void CheckAllPatterns(const QMap<int, UDNode*>& nodes, const QSet<Pattern> & patterns, QSet<Mistake> &mistakes);


