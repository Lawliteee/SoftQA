/**
 * @file UDNode.h
 * @brief Заголовочный файл, содержащий определение класса UDNode для представления узлов дерева зависимостей
 */

#ifndef UDNODE_H
#define UDNODE_H

#include <Header.h>

// Предварительные объявления
class Pattern;
class Mistake;

/**
 * @class UDNode
 * @brief Класс, представляющий узел дерева зависимостей (Universal Dependencies)
 *
 * Класс хранит информацию о слове в предложении, его грамматических характеристиках
 * и связях с другими словами. Предоставляет методы для анализа грамматических свойств.
 */
class UDNode
{
    int id;                         ///< Уникальный идентификатор узла
    QString lemma;                  ///< Лемма (нормальная форма слова)
    PosTag upos;                    ///< Универсальный POS-тег части речи
    int head;                       ///< ID головного (зависимого) слова
    DepRel depRel;                  ///< Тип синтаксической связи с родителем
    VerbMood mood;                  ///< Наклонение глагола (если применимо)
    QSet<UDNode*> children;         ///< Множество дочерних узлов
    bool connectedToRoot;           ///< Флаг достижимости корня дерева

public:
    friend class GrammarRule;  ///< Дружественный класс для доступа к приватным членам

    /**
     * @brief Конструктор по умолчанию
     */
    UDNode();

    /**
     * @brief Основной конструктор с полной инициализацией
     * @param i ID узла
     * @param lem Лемма слова
     * @param up Универсальный POS-тег
     * @param h ID головного слова
     * @param dp Тип зависимости
     * @param m Наклонение глагола
     */
    UDNode(int i, QString lem, PosTag up, int h, DepRel dp, VerbMood m);

    /**
     * @brief Упрощенный конструктор
     * @param lem Лемма слова
     * @param up Универсальный POS-тег
     * @param m Наклонение глагола
     */
    UDNode(QString lem, PosTag up, VerbMood m);

    /**
     * @brief Помечает все связанные дочерние узлы как достижимые до корня
     */
    void markRelatedDescendants();

    /**
     * @brief Возвращает дочерние узлы
     * @return Константная ссылка на множество дочерних узлов
     */
    const QSet<UDNode*>& getChildren() const { return children; }

    /**
     * @brief Проверяет достижимость корня дерева
     * @return true если узел связан с корнем дерева
     */
    bool isConnectedToRoot() const { return connectedToRoot; }

    /**
     * @brief Устанавливает флаг достижимости корня
     * @param connected Значение флага
     */
    void setConnectedToRoot(bool connected) { connectedToRoot = connected; }

    /**
     * @brief Проверяет узел на соответствие шаблону
     * @param pattern Указатель на шаблон для проверки
     * @param mistakes Ссылка на множество для записи ошибок
     */
    void checkPattern(const Pattern* pattern, QSet<Mistake>& mistakes) const;

    // Методы проверки грамматических форм
    bool isBeForm() const;       ///< Проверяет, является ли узел формой глагола "to be"
    bool isModalVerb() const;    ///< Проверяет, является ли узел модальным глаголом
    bool isHaveForm() const;     ///< Проверяет, является ли узел формой глагола "to have"
    bool isDoForm() const;       ///< Проверяет, является ли узел формой глагола "to do"

    /**
     * @brief Проверяет наличие дочернего узла с указанным типом связи
     * @param rel Тип зависимости для поиска
     * @return true если найден соответствующий дочерний узел
     */
    bool hasChildWithRel(DepRel rel) const;

    /**
     * @brief Проверяет, является ли узел условным или временным союзом
     * @return true если узел является условным/временным союзом
     */
    bool hasTemporalConditionalConjunction() const;

    // Методы проверки временных форм
    bool isPresentSimple() const;                ///< Проверяет Present Simple
    bool isPresentContinuous() const;            ///< Проверяет Present Continuous
    bool isPresentPerfect() const;               ///< Проверяет Present Perfect
    bool isPresentPerfectContinuous() const;     ///< Проверяет Present Perfect Continuous
    bool isNominalPredicate() const;             ///< Проверяет именное сказуемое
    bool isPresentBe() const;                    ///< Проверяет форму "to be" в настоящем времени
    bool isPresentModal() const;                 ///< Проверяет модальный глагол в настоящем времени
    bool isPresentHave() const;                  ///< Проверяет форму "to have" в настоящем времени

    /**
     * @brief Проверяет, относится ли узел к настоящему времени
     * @return true если узел в любой форме настоящего времени
     */
    bool isPresentTense() const {
        return isPresentSimple() ||
               isPresentContinuous() ||
               isPresentPerfect() ||
               isPresentPerfectContinuous() ||
               isNominalPredicate() ||
               isPresentModal();
    }

    bool isPastSimple() const;                  ///< Проверяет Past Simple
    bool isPastContinuous() const;              ///< Проверяет Past Continuous
    bool isPastPerfect() const;                 ///< Проверяет Past Perfect
    bool isPastPerfectContinuous() const;       ///< Проверяет Past Perfect Continuous
    bool isPastNominalPredicate() const;        ///< Проверяет именное сказуемое в прошлом
    bool isPastModal() const;                   ///< Проверяет модальный глагол в прошлом
    bool isPastBe() const;                      ///< Проверяет форму "to be" в прошлом времени
    bool isPastHave() const;                    ///< Проверяет форму "to have" в прошлом времени

    /**
     * @brief Проверяет, относится ли узел к прошедшему времени
     * @return true если узел в любой форме прошедшего времени
     */
    bool isPastTense() const {
        return isPastSimple() ||
               isPastContinuous() ||
               isPastPerfect() ||
               isPastPerfectContinuous() ||
               isPastNominalPredicate() ||
               isPastModal();
    }

    bool isCountable() const;                   ///< Проверяет, является ли существительное исчисляемым

    /**
     * @brief Возвращает лицо глагола (1, 2, 3)
     * @return Число, обозначающее лицо (0 если не применимо)
     */
    int getPerson() const;

    /**
     * @brief Возвращает число (единственное/множественное)
     * @return 1 для единственного, 2 для множественного числа
     */
    int getNumber() const;

    /**
     * @brief Добавляет дочерний узел
     * @param child Указатель на дочерний узел
     */
    void addChild(const UDNode* child);

    /**
     * @brief Оператор сравнения узлов
     * @param other Другой узел для сравнения
     * @return true если узлы идентичны по всем параметрам
     */
    bool operator==(const UDNode& other) const {
        return id == other.id && lemma == other.lemma && upos == other.upos &&
               head == other.head && depRel == other.depRel && mood == other.mood;
    }

    // Методы доступа к полям
    int getId() const;              ///< Возвращает ID узла
    QString getlemma() const;       ///< Возвращает лемму
    PosTag getUpos() const;         ///< Возвращает POS-тег
    int getHead() const;            ///< Возвращает ID головного слова
    DepRel getDepRel() const;       ///< Возвращает тип зависимости
    VerbMood getMood() const;       ///< Возвращает наклонение глагола

    /**
     * @brief Записывает дочерние узлы в переданное множество
     * @param ch Ссылка на множество для записи дочерних узлов
     */
    void writeChildren(QSet<UDNode*>& ch) {
        ch = children;
    }

private:
    /**
     * @brief Проверяет наличие дочернего узла в инфинитиве
     * @return true если найден дочерний узел с инфинитивом
     */
    bool hasInfinitiveChild() const {
        for (UDNode* child : children) {
            if ((child->depRel == Xcomp || child->depRel == Ccomp) &&
                child->upos == VB) {
                return true;
            }
        }
        return false;
    }
};

#endif // UDNODE_H
