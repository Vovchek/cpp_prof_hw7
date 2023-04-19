/**
 * @file bulk.cpp
 * @brief command batches processor. Detects bulks and writes them both to cout and files.
 * @author Vladimir Chekal
 * @date April 2023

 * @mainpage Homework #7 Syllabus.
 * @anchor business_logic
 *
\nРазработать программу для пакетной обработки команд.\n
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
#include <list>
#include <chrono>
#include <sstream>
#include <memory>

class Observer
{
public:
    virtual void startBlock() = 0;
    virtual void setNextCommand(const std::string &) = 0;
    virtual void finalizeBlock() = 0;
};

class Observable
{
public:
    virtual ~Observable() = default;
    virtual void subscribe(const std::shared_ptr<Observer> &obs) = 0;
};

class CommandProcessor : public Observable
{
    std::list<std::weak_ptr<Observer>> m_subs;
    int bulk_depth{0};
    int bulk_size{0};
    int max_bulk{3};

public:
    CommandProcessor() = default;
    CommandProcessor(int N) : max_bulk{N} {}

    enum Events
    {
        StartBlock,
        EndBlock,
        NewCommand
    };

    void subscribe(const std::shared_ptr<Observer> &obs) override
    {
        m_subs.emplace_back(obs);
    }

    void notify(Events e, const std::string &cmd)
    {
        auto iter = m_subs.begin();
        while (iter != m_subs.end())
        {
            auto ptr = iter->lock();
            if (ptr)
            { // notify subscriber if it still survived
                switch (e)
                {
                case StartBlock:
                    ptr->startBlock();
                    break;
                case EndBlock:
                    ptr->finalizeBlock();
                    break;
                case NewCommand:
                    ptr->setNextCommand(cmd);
                    break;
                }
                ++iter;
            }
            else
            { // subscriber is dead
                m_subs.erase(iter++);
            }
        }
    }

    void addCommand(const std::string &cmd)
    {
        if (!bulk_size)
            notify(StartBlock, "");
        bulk_size++;
        notify(NewCommand, cmd);
    }

    void endBlock()
    {
        notify(EndBlock, "");
        bulk_size = 0;
    }

    void onInput(const std::string &cmd)
    {
        if (bulk_size < max_bulk || bulk_depth > 0)
        {

            if (cmd.find('{') != std::string::npos)
            {
                if (!bulk_depth && bulk_size)
                {
                    endBlock();
                }
                ++bulk_depth;
            }
            else if (bulk_depth && cmd.find('}') != std::string::npos)
            {
                if (--bulk_depth == 0)
                {
                    endBlock();
                }
            }
            else
            {
                addCommand(cmd);
            }
        }
        if (bulk_size >= max_bulk && !bulk_depth)
        {
            endBlock();
        }
    }

    void terminate()
    {
        if (bulk_size && !bulk_depth)
            endBlock();
    }
};

class OstreamLogger : public Observer, public std::enable_shared_from_this<OstreamLogger>
{
public:
    static std::shared_ptr<OstreamLogger> create(CommandProcessor *cp)
    {
        auto ptr = std::shared_ptr<OstreamLogger>{new OstreamLogger{}};
        ptr->subscribe(cp);
        return ptr;
    }
    void subscribe(CommandProcessor *cp)
    {
        cp->subscribe(shared_from_this());
    }

    void startBlock() override
    {
    }

    void setNextCommand(const std::string &cmd) override
    {
        data.push_back(cmd);
    }

    void finalizeBlock() override
    {
        std::cout << "bulk: ";
        for (auto &c : data)
        {
            if (&c != &(*data.begin()))
                std::cout << ", ";
            std::cout << c;
        }
        std::cout << '\n';

        data.clear();
    }

private:
    OstreamLogger() = default;
    std::list<std::string> data;
};

class FileLogger : public Observer, public std::enable_shared_from_this<FileLogger>
{
public:
    static std::shared_ptr<FileLogger> create(CommandProcessor *cp)
    {
        auto ptr = std::shared_ptr<FileLogger>{new FileLogger{}};
        ptr->subscribe(cp);
        return ptr;
    }
    void subscribe(CommandProcessor *cp)
    {
        cp->subscribe(shared_from_this());
    }

    void startBlock() override
    {
        log_name = time_to_filename(std::chrono::steady_clock::now());
    }

    void setNextCommand(const std::string &cmd) override
    {
        data.push_back(cmd);
    }

    void finalizeBlock() override
    {
        std::ofstream log(log_name);

        log << "bulk: ";
        for (auto &c : data)
        {
            if (&c != &(*data.begin()))
                log << ", ";
            log << c;
        }
        log << '\n';

        data.clear();
    }

    /**
     * @brief Create file name string based on time point.
     * @param [in] t Reference to time_point structure.
     * @details Converts time point to a readable string of a microseconds since epoch start,
     *           then combines file name starting with "bulk" and ending on ".log" extension.
     */
    std::string time_to_filename(const std::chrono::time_point<std::chrono::steady_clock> &t) const
    {
        std::string fn{"bulk" +
                       std::to_string(
                           std::chrono::duration_cast<std::chrono::microseconds>(t.time_since_epoch()).count()) +
                       ".log"};
        return fn;
    }

private:
    FileLogger() = default;
    std::list<std::string> data;
    std::string log_name;
};

/**
 * @brief main function.
 * @param argc [in] Number of a program parameters strings.
 * @param argv [in] Array of pointers to c-style char arrays, storing program parameters
 * @returns Exit status - 0 (EXIT_SUCCESS) on success, non-zero (EXIT_FAILURE) on failure.
 * @details Business logic implementation has been moved to CommandProcessor class methods.
 * See \ref business_logic "mainpage" for detailed description.
 */
int main(int argc, char const *argv[])
{
    size_t N{3};
    if (argc < 2 || (N = std::stoi(argv[1])) <= 0)
    {
        std::cerr << "Should specify positive bulk size as a program parameter (e.g. `>bulk.exe 3`)\n";
        return EXIT_FAILURE;
    }

    CommandProcessor commands(N);

    std::shared_ptr<OstreamLogger> coutPtr = OstreamLogger::create(&commands);
    std::shared_ptr<FileLogger> filePtr = FileLogger::create(&commands);

    std::string cmd;
    while (std::getline(std::cin, cmd))
    {
        commands.onInput(cmd);
    }
    commands.terminate();

    return EXIT_SUCCESS;
}
