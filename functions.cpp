#include <pattern.h>
#include <QRegularExpression>
/*!
* \brief Функция для считывания данных о слове из строк в объекты класса
* \param [in] lines – строки с данными о слове и его связях
* \param [out] nodes – ассоциативный контейнер связей по идентификатору
* \param [in,out]  errors- множество ошибок
*/
void createNodesFromLines(const QStringList& lines, QMap<int, UDNode*>&nodes, QSet<Error>& errors)
{
    QSet<int> existingIds;

    for (int i = 0; i < lines.size(); i++) {
        const QString& line = lines[i];
        if (line.trimmed().isEmpty()) continue;

        UDNode* node = nullptr;
        bool success = createUdNodeFromLine(line, i, node, errors);

        if (success && node) {
            int id = node->getId();

            // Проверяем на дублирующиеся ID
            if (existingIds.contains(id)) {
                QSet<int> errLines; errLines << i;
                errors.insert(Error(errLines, duplicateIds, "Дублирующийся идентификатор слова: " + QString::number(id)));
                delete node;
            } else {
                nodes.insert(id, node);
                existingIds.insert(id);
            }
        }
    }
}

bool createUdNodeFromLine(const QString& line, int lineNumber, UDNode*& node, QSet<Error>& errors) {
    QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    if (parts.size() < 6) {
        QSet<int> errorLines;
        errorLines.insert(lineNumber);
        errors.insert(Error(errorLines, IncorrectMood, "Недостаточно данных для создания узла"));
        return false;
    }


    bool hasErrors = false;
    QSet<int> errLines{lineNumber}; // Создаем QSet с текущим номером строки

    // Проверка ID
    bool idOk;
    int id = parts[0].toInt(&idOk);
    if (!idOk || id <= 0) {
        errors.insert(Error(errLines, IncorrectId, "Неверный идентификатор слова: " + parts[0]));
        hasErrors = true;
    }

    // Проверка слова
    QRegularExpression wordRegex("^[\\p{L}\\p{P}]+$");
    if (!wordRegex.match(parts[1]).hasMatch()) {
        errors.insert(Error(errLines, IncorrectWord, "Неверное написание слова: " + parts[1]));
        hasErrors = true;
    }

    // Проверка POS-тега
    PosTag upos = stringToPosTag(parts[2]);
    if (upos == ANY && parts[2] != "ANY") {
        errors.insert(Error(errLines, IncorrectTag, "Неверный тег части речи: " + parts[2]));
        hasErrors = true;
    }

    // Проверка head ID
    bool headOk;
    int head = parts[3].toInt(&headOk);
    if (!headOk || head < 0) {
        errors.insert(Error(errLines, IncorrectHeadId, "Неверный идентификатор определяющего слова: " + parts[3]));
        hasErrors = true;
    } else if (head == id) {
        errors.insert(Error(errLines, CycleInTree, "Идентификатор определяющего слова совпадает с идентификатором текущего слова: " + QString::number(id)));
        hasErrors = true;
    }

    // Проверка типа зависимости
    DepRel depRel = stringToDepRel(parts[4]);
    if (depRel == Other && parts[4].toLower() != "other") {
        errors.insert(Error(errLines, IncorrectDepRel, "Неверный тип связи: " + parts[4]));
        hasErrors = true;
    }

    // Проверка наклонения
    VerbMood mood = stringToVerbMood(parts[5]);
    if (mood == None && parts[5] != "-") {
        errors.insert(Error(errLines, IncorrectMood, "Неверное наклонение: " + parts[5]));
        hasErrors = true;
    }

    // Проверка на лишние данные
    if (parts.size() > 6) {
        errors.insert(Error(errLines, ExtraData, "Посторонние символы после данных: " + parts.mid(6).join(" ")));
        hasErrors = true;
    }

    if (hasErrors) {
        return false;
    }

    node = new UDNode(id, parts[1], upos, head, depRel, mood);
    return true;
}

DepRel stringToDepRel(const QString& relStr) {
    static QMap<QString, DepRel> depRelMap = {
        {"acl", Acl}, {"acl:relcl", Acl_Relcl}, {"advcl", Advcl}, {"advcl:relcl", Advcl_Relcl},
        {"advmod", Advmod}, {"advmod:emph", Advmod_Emph}, {"advmod:lmod", Advmod_Lmod},
        {"amod", Amod}, {"appos", Appos}, {"aux", Aux}, {"aux:pass", Aux_Pass},
        {"case", Case}, {"cc", Cc}, {"cc:preconj", Cc_Preconj}, {"ccomp", Ccomp},
        {"clf", Clf}, {"compound", Compound}, {"compound:lvc", Compound_Lvc},
        {"compound:prt", Compound_Prt}, {"compound:redup", Compound_Redup},
        {"compound:svc", Compound_Svc}, {"conj", Conj}, {"cop", Cop}, {"csubj", Csubj},
        {"csubj:outer", Csubj_Outer}, {"csubj:pass", Csubj_Pass}, {"dep", Dep},
        {"det", Det}, {"det:numgov", Det_Numgov}, {"det:nummod", Det_Nummod},
        {"det:poss", Det_Poss}, {"discourse", Discourse}, {"dislocated", Dislocated},
        {"expl", Expl}, {"expl:impers", Expl_Impers}, {"expl:pass", Expl_Pass},
        {"expl:pv", Expl_Pv}, {"fixed", Fixed}, {"flat", Flat}, {"flat:name", Flat_Name},
        {"goeswith", Goeswith}, {"iobj", Iobj}, {"list", List}, {"mark", Mark},
        {"nmod", Nmod}, {"nmod:poss", Nmod_Poss}, {"nmod:tmod", Nmod_Tmod},
        {"nsubj", Nsubj}, {"nsubj:outer", Nsubj_Outer}, {"nsubj:pass", Nsubj_Pass},
        {"nummod", Nummod}, {"nummod:gov", Nummod_Gov}, {"obj", Obj}, {"obl", Obl},
        {"obl:agent", Obl_Agent}, {"obl:arg", Obl_Arg}, {"obl:lmod", Obl_Lmod},
        {"obl:tmod", Obl_Tmod}, {"obl:npmod", Obl_Npmod}, {"orphan", Orphan},
        {"parataxis", Parataxis}, {"punct", Punct}, {"reparandum", Reparandum},
        {"root", Root}, {"vocative", Vocative}, {"xcomp", Xcomp}
    };

    return depRelMap.value(relStr.toLower(), Other);
}

PosTag stringToPosTag(const QString& tagStr) {
    static QMap<QString, PosTag> posTagMap = {
        {"CC", CC}, {"CD", CD}, {"DT", DT}, {"EX", EX}, {"FW", FW}, {"IN", IN},
        {"JJ", JJ}, {"JJR", JJR}, {"JJS", JJS}, {"LS", LS}, {"MD", MD},
        {"NN", NN}, {"NNS", NNS}, {"NNP", NNP}, {"NNPS", NNPS}, {"VB", VB},
        {"VBD", VBD}, {"VBG", VBG}, {"VBN", VBN}, {"VBP", VBP}, {"VBZ", VBZ},
        {"PRP", PRP}, {"PRP$", PRP_}, {"WP", WP}, {"WP$", WP_}, {"WRB", WRB},
        {"RB", RB}, {"RBR", RBR}, {"RBS", RBS}, {"RP", RP}, {"SYM", SYM},
        {"TO", TO}, {"UH", UH}, {"WDT", WDT}, {"PDT", PDT}, {"ANY", ANY}
    };

    return posTagMap.value(tagStr, ANY);
}

VerbMood stringToVerbMood(const QString& moodStr) {
    if (moodStr == "Ind") return Ind;
    if (moodStr == "Imp") return Imp;
    if (moodStr == "Subj") return Subj;
    return None;
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
    if (!root || root->getHead() != 0 || root->getDepRel()!= Root) {
        throw(QString("Invalid root"));
    }

    // Сброс флагов связанности
    for (UDNode* node : nodes) {
        node->setConnectedToRoot(false);
    }

    root->markRelatedDescendants();

    bool isConnected = true;
    QSet<int> disconnectedNodes;

    // Проверка связности и циклов
    for (UDNode* node : nodes) {
        if (!node->isConnectedToRoot()) {
            disconnectedNodes.insert(node->getId());
            isConnected = false;
        }
    }

    // Формируем сообщения об ошибках
    if (!disconnectedNodes.isEmpty()) {
        QStringList nodeNames;
        QList<int> sortedIds = disconnectedNodes.values();
        std::sort(sortedIds.begin(), sortedIds.end());
        for (int id : sortedIds) {
            if (nodes.contains(id)) {
                nodeNames << nodes[id]->getlemma();
            }
        }
        errors.insert(Error(disconnectedNodes, CycleInTree,
                            "Ошибка об отсутствии связи с корнем (" + nodeNames.join(", ") + "), образуют цикл"));
    }

    return isConnected;
}

/*!
* \brief Функция для построения дерева из связей
* \param [in,out] nodes – ассоциативный контейнер связей по идентификатору
* \param [in,out]  errors- множество ошибок
* \return указатель на корень дерева
*/
UDNode* addChildren(QMap<int, UDNode*>& nodes, QSet<Error>& errors)
{
    UDNode* root = nullptr;
    QSet<int> rootIds;
    QSet<int> cycleNodes;

    // 1. Поиск корня и проверка условий
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        UDNode* node = it.value();
        int id = node->getId();
        int headId = node->getHead();
        DepRel depRel = node->getDepRel();

        // 1.1. Проверка на корень
        if (headId == 0) {
            if (depRel == Root) {
                if (root == nullptr) {
                    root = node;
                }
                rootIds.insert(id);
            } else {
                // 1.2. Несоответствие head=0 и типа связи
                QSet<int> errorLines;
                errorLines.insert(id);
                errors.insert(Error(errorLines, IncorrectRootId,
                    QString("Несовпадение между head=0 и типом связи '%1' у слова с ID=%2")
                    .arg(depRelToString(depRel))
                    .arg(id)));
            }
        }
    }

    // 1.3. Обработка нескольких корней
    if (rootIds.size() > 1) {
        QList<int> sortedRootIds = rootIds.values();
        std::sort(sortedRootIds.begin(), sortedRootIds.end());

        QStringList idStrs;
        for (int id : sortedRootIds) {
            idStrs << QString::number(id);
        }

        errors.insert(Error(rootIds, MultipleRoots,
            QString("Обнаружено несколько корней (ID: %1)").arg(idStrs.join(", "))));

        // Возвращаем первый корень (с минимальным ID)
        if (root == nullptr) {
            root = nodes.value(*rootIds.begin());
        }
    }

    // 2. Построение дерева зависимостей
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        UDNode* node = it.value();
        int headId = node->getHead();

        // Пропускаем корень
        if (headId == 0) continue;

        // 2.1. Проверка существования head
        if (nodes.contains(headId)) {
            UDNode* headNode = nodes[headId];

            // Проверка на циклы
            if (node->getId() == headNode->getHead()) {
                cycleNodes.insert(node->getId());
                cycleNodes.insert(headNode->getId());
            }
            headNode->addChild(node);
        } else {
            // 2.2. Head не найден
            QSet<int> errorLines;
            errorLines.insert(node->getId());
            errors.insert(Error(errorLines, headIdNotFound,
                QString("Не найден head с ID=%1 для слова с ID=%2")
                .arg(headId)
                .arg(node->getId())));
        }
    }

    // 3. Обработка циклов (с сортировкой ID для стабильного порядка)
    if (!cycleNodes.isEmpty()) {
        QList<int> sortedCycleIds = cycleNodes.values();
        std::sort(sortedCycleIds.begin(), sortedCycleIds.end());

        QStringList idStrs;
        for (int id : sortedCycleIds) {
            idStrs << QString::number(id);
        }

        errors.insert(Error(cycleNodes, CycleInTree,
            QString("Обнаружен цикл между узлами %1").arg(idStrs.join(", "))));
    }

    // 4. Проверка наличия корня
    if (root == nullptr && !nodes.isEmpty()) {
        QSet<int> emptyLines;
        errors.insert(Error(emptyLines, NoRoot, "В дереве отсутствует корневой узел (head=0)"));
    }

    return root;
}

/*!
* \brief Функция, которая проверяет все узлы на совпадение шаблонам и вызывает необходимые проверки для них
* \param [in] nodes – ассоциативный контейнер связей по идентификатору
* \param [in] patterns – шаблоны правил
* \param [in,out] mistakes – ошибки согласования
*/
void checkAllPatterns(const QMap<int, UDNode*>& nodes, const QSet<Pattern*> & patterns, QSet<Mistake> &mistakes)
{
    // 1. Для каждого узла в дереве
    for (UDNode* node : nodes) {
        // 2. Для каждого шаблона
        for (Pattern* pattern : patterns) {
            // 3. Проверяем соответствие узла шаблону
            QSet<const UDNode*> usedNodes;
            if (pattern->matchesPattern(node, usedNodes)) {
                // 4. Если соответствует - выполняем проверки шаблона
                try {
                    node->checkPattern(pattern, mistakes);
                } catch (const QString& error) {
                    throw;
                }
            }
        }
    }
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

void formOutput( QSet<Mistake> & mistakes, QStringList & out, QMap<int,UDNode*> & nodes)
{
    for (const Mistake &mistake : mistakes)
    {
        QPair<int, int> ids = mistake.getNodeIds();
        int id1 = ids.first;
        int id2 = ids.second;

        // Получаем леммы для обоих узлов
        QString lemma1 = nodes.contains(id1) ? nodes.value(id1)->getlemma() : "UNKNOWN";
        QString lemma2 = nodes.contains(id2) ? nodes.value(id2)->getlemma() : "UNKNOWN";

        // Формируем строку вывода
        QString outputLine = QString("Ошибка для слов : %1 %2 и %3 %4. Сообщение: %5")
                                 .arg(id1)
                                 .arg(lemma1)
                                 .arg(id2)
                                 .arg(lemma2)
                                 .arg(mistake.getMessage());

        out.append(outputLine);
    }
}

void readFile(const QString & fileName, QStringList & lines, QSet <Error> &errors)
{
    QFile file(fileName);

    // если нельзя считывать из файла
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        errors.insert(Error(InputFile,"Неверно указан файл с входными данными. Возможно, файл не существует."));
        return;
    }

    QTextStream in(&file);

    // Пока не конец файла
    while (!in.atEnd())
    {
        // Считать одну строку из файла
        QString line = in.readLine();

        // Добавить строку в массив строк
        lines.append(line);
    }

    // если файл пуст
    if (lines.isEmpty())
    {
        errors.insert(Error(InputFile,"Пустой входной файл"));
        return;
    }

    // Закрыть файл
    file.close();
}

void writeFile(const QString & fileName, const QStringList &lines, QSet <Error> &errors)
{
    QFile file(fileName);

    // если нельзя записывать в файл
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        errors.insert(Error(OutputFile,"Неверно указан файл для выходных данных. Указанного расположения не существует или нет прав на запись."));
        return;
    }

    QTextStream out(&file);

    // Записать строки в файл
    for (const QString & line : lines)
    {
        out <<line<<"\n";
    }

    // Закрыть файл
    file.close();
}
