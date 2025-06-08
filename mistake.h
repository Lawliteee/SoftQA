#ifndef MISTAKE_H
#define MISTAKE_H
#include <udnode.h>
class Mistake
{
    QString message ;// сообщение
    QList<UDNode*> nodes ;// узлы c ошибкой
public:
    Mistake();
    Mistake(QString message);
    void createMessage(QString temp);   // метод, вставляет слово или id при необходимости в сообщение, на вход получает шаблон сообщения
    void addToMessage(QString str);      // добавляет строку к сообщению

    QString getMessage() const
    {
        return message;
    }
    // Для работы с QSet
    bool operator==(const Mistake& other) const
    {
        if (message != other.message) return false;
        if (nodes.size() != other.nodes.size()) return false;

        for (int i = 0; i < nodes.size(); ++i)
        {
            if (nodes[i] != other.nodes[i]) return false;
        }

        return true;
    }

    friend uint qHash(const Mistake& key, uint seed = 0)
    {
        uint hash = qHash(key.message, seed);

        for (const auto node : key.nodes)
        {
            hash ^= qHash(reinterpret_cast<quintptr>(node));
        }

        return hash;
    }
};

#endif // MISTAKE_H
