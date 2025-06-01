#ifndef ERROR_H
#define ERROR_H

#include <QSet>

class Error
{
    enum ErrorType
    {
        NoError, InputFile, OutputFile, IncorrectId, duplicateIds, IncorrectWord,
                     IncorrectHeadId, headIdNotFound, IncorrectDepRel, ExtraData, NoRoot,
                     MultipleRoots, IncorrectRootId, IncorrectMood, IncorrectTag, CycleInTree,
                     depNotConnected
    };
    QSet <int> line;
    ErrorType type;
    QString message;
public:
    Error();
    Error(ErrorType t = NoError, const QSet<int>& l = QSet<int>()) : type(t), line(l) {}

    // Перегрузка оператора сравнения
    bool operator==(const Error& other) const
    {
        return type == other.type && line == other.line && message == other.message;
    }

    void printMessage();
    QSet <int> getLine() const
    {
        return line;
    }
    QString getMessage() const
    {
        return message;
    }
    ErrorType getType() const
    {
        return type;
    }
};

// Хеш-функция
inline uint qHash(const Error &error, uint seed = 0) noexcept
{
    uint hash = qHash(static_cast<int>(error.getType()), seed);
    hash ^= qHash(error.getLine(), seed + 1);
    hash ^= qHash(error.getMessage(), seed + 2);
    return hash;
}

#endif // ERROR_H
