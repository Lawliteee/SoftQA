/**
 * @file mistake.h
 * @brief Заголовочный файл, содержащий определение класса Mistake для хранения информации об ошибках
 */

#ifndef MISTAKE_H
#define MISTAKE_H

#include <udnode.h>

/**
 * @class Mistake
 * @brief Класс для хранения информации о грамматической ошибке
 *
 * Класс хранит сообщение об ошибке и идентификаторы узлов, связанных с ошибкой.
 * Поддерживает операции сравнения для использования в QSet.
 */
class Mistake
{
    QString message;      ///< Текст сообщения об ошибке
    QPair<int, int> nodeIds; ///< Пара идентификаторов узлов, связанных с ошибкой

public:
    /**
     * @brief Конструктор по умолчанию
     *
     * Создает пустую ошибку без сообщения и идентификаторов узлов
     */
    Mistake();

    /**
     * @brief Конструктор с сообщением об ошибке
     * @param message Текст сообщения об ошибке
     */
    Mistake(QString message);

    /**
     * @brief Конструктор с полной инициализацией
     * @param str Текст сообщения об ошибке
     * @param node1 ID первого узла, связанного с ошибкой
     * @param node2 ID второго узла, связанного с ошибкой
     */
    Mistake(QString str, int node1, int node2);

    /**
     * @brief Создает сообщение об ошибке по шаблону
     * @param temp Шаблон сообщения, в который будут подставлены дополнительные данные
     *
     * Метод заменяет специальные маркеры в шаблоне на конкретные значения
     */
    void createMessage(QString temp);

    /**
     * @brief Добавляет текст к существующему сообщению
     * @param str Текст для добавления
     */
    void addToMessage(QString str);

    /**
     * @brief Устанавливает идентификаторы узлов, связанных с ошибкой
     * @param firstId ID первого узла
     * @param secondId ID второго узла
     */
    void setNodeIds(int firstId, int secondId)
    {
        nodeIds.first = firstId;
        nodeIds.second = secondId;
    }

    /**
     * @brief Возвращает идентификаторы узлов, связанных с ошибкой
     * @return Пара идентификаторов узлов
     */
    QPair<int, int> getNodeIds() const
    {
        return nodeIds;
    }

    /**
     * @brief Возвращает сообщение об ошибке
     * @return Текст сообщения об ошибке
     */
    QString getMessage() const
    {
        return message;
    }

    /**
     * @brief Оператор сравнения для работы с QSet
     * @param other Другая ошибка для сравнения
     * @return true если сообщения об ошибках идентичны
     */
    bool operator==(const Mistake& other) const {
        return message == other.message;
    }

    /**
     * @brief Функция хеширования для работы с QSet
     * @param key Ошибка для хеширования
     * @param seed Начальное значение хеша
     * @return Хеш-значение ошибки
     *
     * @note Объявлена как friend для доступа к приватным членам
     */
    friend uint qHash(const Mistake& key, uint seed = 0) {
        return qHash(key.message, seed);
    }
};

#endif // MISTAKE_H
