/**
 * @file error.h
 * @brief Заголовочный файл, содержащий определение классов и перечислений для обработки ошибок
 */

#ifndef ERROR_H
#define ERROR_H

#include <QSet>

/**
 * @enum ErrorType
 * @brief Перечисление типов возможных ошибок
 */
enum ErrorType
{
    NoError,          ///< Нет ошибки
    InputFile,        ///< Ошибка входного файла
    OutputFile,       ///< Ошибка выходного файла
    IncorrectId,      ///< Некорректный ID
    duplicateIds,     ///< Дублирующиеся ID
    IncorrectWord,    ///< Некорректное слово
    IncorrectHeadId,  ///< Некорректный ID головного элемента
    headIdNotFound,   ///< ID головного элемента не найден
    IncorrectDepRel,  ///< Некорректное отношение зависимости
    ExtraData,        ///< Лишние данные
    NoRoot,           ///< Отсутствует корень
    MultipleRoots,    ///< Множественные корни
    IncorrectRootId,  ///< Некорректный ID корня
    IncorrectMood,    ///< Некорректное наклонение
    IncorrectTag,     ///< Некорректный тег
    CycleInTree,      ///< Цикл в дереве
    depNotConnected   ///< Зависимость не соединена
};

/**
 * @class Error
 * @brief Класс для хранения и обработки информации об ошибке
 */
class Error
{
    QSet<int> line;   ///< Номера строк, связанных с ошибкой
    ErrorType type;   ///< Тип ошибки
    QString message;  ///< Сообщение об ошибке

public:
    /**
     * @brief Конструктор по умолчанию
     */
    Error();

    /**
     * @brief Конструктор с параметрами
     * @param l Множество номеров строк
     * @param t Тип ошибки
     * @param s Сообщение об ошибке
     */
    Error(QSet<int>& l, ErrorType t, QString s)
    {
        line = l;
        type = t;
        message = s;
    };

    /**
     * @brief Конструктор с параметрами (без указания строк)
     * @param t Тип ошибки
     * @param s Сообщение об ошибке
     */
    Error(ErrorType t, QString s)
    {
        type = t;
        message = s;
    };

    /**
     * @brief Оператор сравнения
     * @param other Другая ошибка для сравнения
     * @return true если ошибки идентичны, иначе false
     */
    bool operator==(const Error& other) const
    {
        return type == other.type && line == other.line && message == other.message;
    }

    /**
     * @brief Вывод сообщения об ошибке
     */
    void printMessage();

    /**
     * @brief Получить номера строк с ошибкой
     * @return Множество номеров строк
     */
    QSet<int> getLine() const
    {
        return line;
    }

    /**
     * @brief Получить сообщение об ошибке
     * @return Сообщение об ошибке
     */
    QString getMessage() const
    {
        return message;
    }

    /**
     * @brief Получить форматированное сообщение для пользователя
     * @return Форматированное сообщение
     */
    QString getMessageForUser() const
    {
        if (type == InputFile || type == OutputFile)
        {
            return message;
        }
        else if(type == CycleInTree || type == MultipleRoots || type == depNotConnected)
        {
            return QString("Ошибка во входных данных. ") + message;
        }
        else
        {
            int l;
            if (line.isEmpty())
            {
                l = -1;
            }
            l = *line.begin()+1;
            return QString("Ошибка во входных данных в строке номер %1. %2").arg(l).arg(message);
        }
    }

    /**
     * @brief Получить тип ошибки
     * @return Тип ошибки
     */
    ErrorType getType() const
    {
        return type;
    }
};

/**
 * @brief Хеш-функция для класса Error
 * @param error Ошибка для хеширования
 * @param seed Начальное значение хеша
 * @return Хеш-значение ошибки
 */
inline uint qHash(const Error &error, uint seed = 0) noexcept
{
    uint hash = qHash(static_cast<int>(error.getType()), seed);
    hash ^= qHash(error.getLine(), seed + 1);
    hash ^= qHash(error.getMessage(), seed + 2);
    return hash;
}

#endif // ERROR_H
