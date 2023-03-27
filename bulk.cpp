#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <chrono>

class Bulk
{
    std::vector<std::string> data;

public:
    size_t size() const
    {
        return data.size();
    }
    void insert(const std::string &cmd)
    {
        data.push_back(cmd);
    }
    void output() const
    {
        if (size())
        {
            std::cout << "bulk: ";
            for (auto &c : data)
            {
                if (&c != &data[0])
                    std::cout << ", ";
                std::cout << c;
            }
            std::cout << '\n';
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
};

int main(int argc, char const *argv[])
{
    int N{3};
    if (argc < 2 || (N = std::stoi(argv[1])) <= 0)
    {
        // return EXIT_FAILURE;
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
