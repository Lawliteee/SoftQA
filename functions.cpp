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
void CheckAllPatterns(const QMap<int, UDNode*>& nodes, const QSet<Pattern> & patterns, QSet<Mistake> &mistakes)
{
    return;
};
