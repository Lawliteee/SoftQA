/*!
* \file main.cpp
* \brief Данный файл содержит главную функцию программы FindMistakesINWordAdreement.
*
* \mainpage Документация для программы "Find Mistakes In Word Agreement Universal Dependecies"
Программа предназначена для поиска и вывода ошибок согласования слов в предложении на английском языке на основе дерева связей - Universal Dependencies.
Для функционирования программы необходима операционная система Windows 7 или выше.
Программа разработана на языке С++ с использованием стандартных библиотек C++, библиотеки Qt и фреймворка Qt Creator.
Программа должна получать два аргумента командной строки: имя входного файла и имя выходного файла в формате 'txt', или один аргумент командной строки: --test.

Пример команды запуска программы:
* \code
./FindMsitakesInWordAgreementUD.exe ./input.txt ./output.txt
* \endcode

* \author Mikhailov Ilya
* \date June 2025
* \version 1.0
*/

#include <QCoreApplication>
#include "tests.h"

/*!
 * \brief Главная функция программы
 * \param argc Количество аргументов командной строки
 * \param argv Массив аргументов командной строки
 * \return Код возврата:
 *         - 0 при успешном выполнении
 *         - 1 при ошибках ввода/вывода или анализе
 *
 * Аргументы командной строки:
 * - argv[0] - имя исполняемого файла
 * - argv[1] - путь к входному файлу или "--test" для запуска тестов
 * - argv[2] - путь к выходному файлу (требуется при анализе файла)
 *
 */
int main(int argc, char *argv[])
{
    // Включить поддержку русских символов в консоли
    setlocale(LC_ALL, "Russian");

    if (argc == 2 && QString(argv[1]) == "--test") // Если передали в качестве второго аргумента запрос на запуск тестов
    {
        // Запуск тестов
        runTests();
    }
    else if (argc == 3) // ИначеЕсли передано 2 или 3 аргумента
    {
        // Запуск программы
        QString inputFilePath = argv[1];
        QString outFilePath = argv[2];

        // Считать строки из входного файла
        QSet<Error> errors;
        QStringList input;
        readFile(inputFilePath, input, errors);

        // Проверить наличие ошибок при чтении файлов
        if (!errors.isEmpty())
        {
            for (const Error& error : errors) {
                qDebug() << error.getMessageForUser();
            }
            return 1;
        }

        // Считать связи из строк в объекты класса
        QMap<int, UDNode*> nodes;
        createNodesFromLines(input, nodes, errors);

        // Проверить наличие ошибок при создании узлов
        if (!errors.isEmpty())
        {
            for (const Error& error : errors) {
                qDebug() << error.getMessageForUser();
            }
            return 1;
        }

        // Построить из считанных связей дерево
        UDNode* treeRoot = addChildren(nodes, errors);

        // Проверить наличие ошибок при построении дерева
        if (!errors.isEmpty())
        {
            for (const Error& error : errors) {
                qDebug() << error.getMessageForUser();
            }
            return 1;
        }

        // Проверить связность элементов дерева
        checkTreeConnectivity(treeRoot, nodes, errors);

        // Проверить наличие ошибок связности
        if (!errors.isEmpty())
        {
            for (const Error& error : errors) {
                qDebug() << error.getMessageForUser();
            }
            return 1;
        }

        // Найти ошибки согласования
        QSet<Pattern*> patterns;
        getPatternsNew(patterns, nodes);
        QSet<Mistake> mistakes;
        checkAllPatterns(nodes, patterns, mistakes);


        // Сформировать выходные строки с ошибками
        QStringList output;
        formOutput(mistakes, output, nodes);

        // Записать сформированное предложение в выходной файл
        writeFile(outFilePath, output, errors);

        // Проверить наличие ошибок при записи
        if (!errors.isEmpty())
        {
            for (const Error& error : errors) {
                qDebug() << error.getMessageForUser();
            }
            return 1;
        }
    }
    else // Иначе
    {
        // Сообщить о неверном количестве переданных аргументов
        qDebug() << "Программа принимает три аргумента: '<путь к программе> <путь к входному файлу> <путь к выходному файлу>', или два аргумента: '<путь к программе> --test'";
    }

    return 1;
}
