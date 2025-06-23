/**
 * @file  Header.h
 * @brief Заголовочный файл с объявлениями классов и функций для обработки деревьев зависимостей
 */

#ifndef HEADER_H
#define HEADER_H

#include <QObject>
#include <QMultiMap>
#include <error.h>
#include <QDebug>
#include <QIODevice>
#include <QFile>

// Предварительные объявления классов
class UDNode;
class Pattern;
class Mistake;

/**
 * @brief Основная функция запуска программы
 * @param inputFilePath Путь к входному файлу
 * @param outputFilePath Путь к выходному файлу
 */
void runProgramm(const QString& inputFilePath, const QString& outputFilePath);

/**
 * @brief Получение набора шаблонов для анализа
 * @param s Ссылка на набор шаблонов
 */
void getPatterns(QSet<Pattern*>& s);

/**
 * @brief Формирование выходных данных на основе найденных ошибок
 * @param mistakes Набор найденных ошибок
 * @param out Список выходных строк
 * @param nodes Карта узлов дерева
 */
void formOutput(QSet<Mistake>& mistakes, QStringList& out, QMap<int, UDNode*>& nodes);

/**
 * @brief Чтение данных из файла
 * @param inputFilePath Путь к входному файлу
 * @param lines Список прочитанных строк
 * @param errors Набор ошибок чтения
 */
void readFile(const QString& inputFilePath, QStringList& lines, QSet<Error>& errors);

/**
 * @brief Запись данных в файл
 * @param outputFilePath Путь к выходному файлу
 * @param lines Список строк для записи
 * @param errors Набор ошибок записи
 */
void writeFile(const QString& outputFilePath, const QStringList& lines, QSet<Error>& errors);

/**
 * @enum DepRel
 * @brief Перечисление типов зависимостей в дереве
 */
enum DepRel
{
    Acl, Acl_Relcl, Advcl, Advcl_Relcl, Advmod, Advmod_Emph, Advmod_Lmod, Amod, Appos, Aux, Aux_Pass, Case, Cc, Cc_Preconj, Ccomp, Clf,
    Compound, Compound_Lvc, Compound_Prt, Compound_Redup, Compound_Svc, Conj, Cop, Csubj, Csubj_Outer, Csubj_Pass, Dep, Det, Det_Numgov,
    Det_Nummod, Det_Poss, Discourse, Dislocated, Expl, Expl_Impers, Expl_Pass, Expl_Pv, Fixed, Flat, Flat_Name, Goeswith, Iobj, List, Mark, Nmod,
    Nmod_Poss, Nmod_Tmod, Nsubj, Nsubj_Outer, Nsubj_Pass, Nummod, Nummod_Gov, Obj, Obl, Obl_Agent, Obl_Arg, Obl_Lmod, Obl_Tmod, Obl_Npmod,
    Orphan, Parataxis, Punct, Reparandum, Root, Vocative, Xcomp, Other
};

/**
 * @brief Преобразование типа зависимости в строку
 * @param rel Тип зависимости
 * @return Строковое представление зависимости
 */
QString depRelToString(DepRel rel);

/**
 * @enum VerbMood
 * @brief Перечисление наклонений глагола
 */
enum VerbMood
{
    None,   ///< Нет наклонения
    Ind,    ///< Изъявительное
    Imp,    ///< Повелительное
    Subj    ///< Сослагательное
};

/**
 * @enum PosTag
 * @brief Перечисление частей речи (POS-тегов)
 */
enum PosTag
{
    CC, CD, DT, EX, FW, IN, JJ, JJR, JJS, LS, MD, NN, NNS,
    NNP, NNPS, VB, VBD, VBG, VBN, VBP, VBZ, PRP, PRP_, WP, WP_,
    WRB, RB, RBR, RBS, RP, SYM, TO, UH, WDT, PDT, ANY
};

/**
 * @brief Преобразование строки в тип зависимости
 * @param relStr Строка с типом зависимости
 * @return Тип зависимости
 */
DepRel stringToDepRel(const QString& relStr);

/**
 * @brief Преобразование строки в POS-тег
 * @param tagStr Строка с тегом
 * @return POS-тег
 */
PosTag stringToPosTag(const QString& tagStr);

/**
 * @brief Преобразование строки в наклонение глагола
 * @param moodStr Строка с наклонением
 * @return Наклонение глагола
 */
VerbMood stringToVerbMood(const QString& moodStr);

/**
 * @brief Создание узлов дерева из строк входных данных
 * @param lines Список строк с данными
 * @param nodes Карта для хранения узлов
 * @param errors Набор ошибок
 */
void createNodesFromLines(const QStringList& lines, QMap<int, UDNode*>& nodes, QSet<Error>& errors);

/**
 * @brief Создание одного узла UDNode из строки данных
 * @param line Строка с данными
 * @param lineNumber Номер строки
 * @param node Ссылка на указатель создаваемого узла
 * @param errors Набор ошибок
 * @return true, если узел успешно создан, иначе false
 */
bool createUdNodeFromLine(const QString& line, int lineNumber, UDNode*& node, QSet<Error>& errors);

/**
 * @brief Проверка связности дерева
 * @param root Корень дерева
 * @param nodes Карта узлов
 * @param errors Набор ошибок
 * @return true, если дерево связное, иначе false
 */
bool checkTreeConnectivity(UDNode* root, QMap<int, UDNode*>& nodes, QSet<Error>& errors);

/**
 * @brief Построение дерева зависимостей
 * @param nodes Карта узлов
 * @param errors Набор ошибок
 * @return Указатель на корень дерева
 */
UDNode* addChildren(QMap<int, UDNode*>& nodes, QSet<Error>& errors);

/**
 * @brief Проверка всех узлов на соответствие шаблонам
 * @param nodes Карта узлов
 * @param patterns Набор шаблонов
 * @param mistakes Набор найденных ошибок
 */
void checkAllPatterns(const QMap<int, UDNode*>& nodes, const QSet<Pattern*>& patterns, QSet<Mistake>& mistakes);

#endif // HEADER_H
