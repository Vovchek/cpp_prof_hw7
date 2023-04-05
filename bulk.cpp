/*! \mainpage Homework #7 Syllabus.

Разработать программу для пакетной обработки команд.\n
Команды считываются построчно из стандартного ввода и обрабатываются блоками по N команд.\n
Одна команда - одна строка, конкретное значение роли не играет. Если данные закончились - блок\n
завершается принудительно. Параметр N передается как единственный параметр командной\n
строки в виде целого числа.\n
Логика работы для статических блоков (в примере N == 3):\n\n
./bulk 3
| Ввод | Вывод | Комментарий |
|:-----|:------|:------------|
| cmd1 | | |
| cmd2 | | |
| cmd3 | | |
|      | bulk: cmd1, cmd2, cmd3 | Блок завершён – выводим блок.|
| cmd4 | | |
| cmd5 | | |
| EOF  | | |
|      | bulk: cmd4, cmd5 | Конец ввода – \nпринудительно завершаем блок.|

Размер блока можно изменить динамически, если перед началом блока и сразу после дать\n
команды `{` и `}` соответственно. Предыдущий пакет при этом принудительно завершается. Такие\n
блоки могут быть включены друг в друга при этом вложенные команды `{` и `}` игнорируются (но не\n
сами блоки). Если данные закончились внутри динамического блока, весь динамический блок\n
игнорируется.

Логика работы для динамического размера блоков (в примере N == 3):\n\n
./bulk 3
| Ввод | Вывод | Комментарий |
|:-----|:------|:------------|
| cmd1 | | |
| cmd2 | | |
| {    | | |
|      | bulk: cmd1, cmd2 | Начало динамического блока – \nвыводим предыдущий статический досрочно. |
| cmd3 | | |
| cmd4 | | |
| }    | | |
|      | bulk: cmd3, cmd4 | Конец динамического блока – выводим. |
| {    | | |
| cmd5 | | |
| cmd6 | | |
| {    | | Игнорируем вложенные команды. |
| cmd7 | | |
| cmd8 | | |
| }    | | Игнорируем вложенные команды. |
| cmd9 | | |
| }    | | |
|      | bulk: cmd5, cmd6, cmd7, cmd8, cmd9 | Конец динамического блока – выводим. |
| {    | | |
| cmd10 | | |
| cmd11 | | |
| EOF   | | |
|       | | Конец ввода – динамический блок \nигнорируется, не выводим. |

Вместе с выводом в консоль блоки должны сохранятся в отдельные файлы с именами\n
bulk1517223860.log , где 1517223860 - это время получения первой команды из блока. По одному\n
файлу на блок.

Требования к реализации
-----------------------
Бинарный файл должен называться bulk.\n
Проверка
--------
Задание считается выполненным успешно, если после установки пакета и запуска с тестовыми\n
данными вывод соответствует описанию. Данные подаются на стандартный вход построчно с\n
паузой в 1 секунду для визуального контроля.\n
Будет отмечена низкая связанность обработки данных, накопления пачек команд, вывода в\n
консоль и сохранения в файлы.

*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <chrono>
#include <sstream>

/// @brief
class Bulk
{
    std::vector<std::string> data;
    std::string fn;

public:
    size_t size() const
    {
        return data.size();
    }
    void insert(const std::string &cmd)
    {
        if(!size())
            fn = time_to_filename(std::chrono::steady_clock::now());
        data.push_back(cmd);
    }
    void output() const
    {
        write_to(std::cout);
        std::ofstream f{fn};
        write_to(f);
    }
    void write_to(std::ostream &os) const {
        if (size())
        {
            os << "bulk: ";
            for (auto &c : data)
            {
                if (&c != &data[0])
                    os << ", ";
                os << c;
            }
            os << '\n';
        }
    }
    void clear()
    {
        data.clear();
    }
    void flush()
    {
        output();
        clear();
    }
    std::string time_to_filename(const std::chrono::time_point<std::chrono::steady_clock> &t) const {
        std::string fn {"bulk" +
            std::to_string(
                std::chrono::duration_cast<std::chrono::microseconds>(t.time_since_epoch()).count()
                ) + ".log"};
        return fn;
    }
};

int main(int argc, char const *argv[])
{

    size_t N{3};
    if (argc < 2 || (N = std::stoi(argv[1])) <= 0)
    {
        std::cerr << "Should specify positive bulk size as a program parameter (e.g. bulk.exe 3)\n";
        return EXIT_FAILURE;
    }

    while (!std::cin.eof())
    {
        int block_depth{0};
        Bulk blk;
        while (blk.size() < N || block_depth > 0)
        {
            std::string cmd;
            if (!std::getline(std::cin, cmd))
            {
                break;
            }
            if (cmd.find('{') != std::string::npos)
            {
                if (!block_depth)
                {
                    blk.flush();
                }
                ++block_depth;
            }
            else if (block_depth && cmd.find('}') != std::string::npos)
            {
                if (--block_depth == 0)
                {
                    break;
                }
            }
            else
            {
                blk.insert(cmd);
            }
        }
        if(!block_depth)
            blk.flush();
    }

    return EXIT_SUCCESS;
}
