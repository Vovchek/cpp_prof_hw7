#include <gtest/gtest.h>
#include <sstream>

#define main test_main
#include "bulk.cpp"
#undef main

auto run_bulk(int argc, const char *argv[], std::string&& input)
{
    std::stringstream ss_cin{input}, ss_cout {};

    auto cinbuf = std::cin.rdbuf(ss_cin.rdbuf()); // save & redirect
    auto coutbuf = std::cout.rdbuf(ss_cout.rdbuf()); // save & redirect
    
    test_main(argc, argv);
    
    auto output {ss_cout.str()};
    
    std::cin.rdbuf(cinbuf);
    std::cout.rdbuf(coutbuf);
    
    return output;
}

TEST(BulkTest, Test1)
{

  Bulk blk;

  ASSERT_EQ(blk.size(), 0);
}

TEST(MainTest, Test1)
{
    const char *argv[] {"", "3"};
    const auto output {run_bulk(2, argv, "cmd1\ncmd2\ncmd3\ncmd4\ncmd5\n")};
    const char output_expected[] {"bulk: cmd1, cmd2, cmd3\nbulk: cmd4, cmd5\n"};
    
    ASSERT_EQ(output.size()+1, sizeof(output_expected));
    EXPECT_STREQ(output.c_str(), output_expected);

}

TEST(MainTest, Test2)
{
    const char *argv[] {"", "3"};
    const auto output {run_bulk(2, argv, "cmd1\ncmd2\n{\ncmd3\ncmd4\n}\n{\ncmd5\ncmd6\n{\ncmd7\ncmd8\n}\ncmd9\n}\n{\ncmd10\ncmd11")};
    const char output_expected[] {"bulk: cmd1, cmd2\nbulk: cmd3, cmd4\nbulk: cmd5, cmd6, cmd7, cmd8, cmd9\n"};
    
    ASSERT_EQ(output.size()+1, sizeof(output_expected));
    EXPECT_STREQ(output.c_str(), output_expected);

}
