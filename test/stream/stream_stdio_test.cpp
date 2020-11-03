#include "stream/input_stdio_stream.hpp"
#include "stream/ouput_stdio_stream.hpp"
#include <gtest/gtest.h>
#include <resources_config.hpp>

class StdioTest : public testing::Test{
    void SetUp() override ;

protected:
    const std::string name =   std::string(RESOURCES_DIR) + "/movie_info.csv";
};
void StdioTest::SetUp()
{printf("STDIO TESTS");}

// input tests

TEST_F(StdioTest, test_OPEN)
{
    std::unique_ptr<io::InputStream> stream = std::make_unique<io::StdioInputStream>();
    ASSERT_TRUE(stream->open(name));
}

TEST_F(StdioTest, test_OPEN_wr)
{
    std::unique_ptr<io::InputStream> stream = std::make_unique<io::StdioInputStream>();
    ASSERT_FALSE(stream->open("aorddÙde%de7z!jdda7"));
}


TEST_F(StdioTest, read_seek)
{
    std::unique_ptr<io::InputStream> stream = std::make_unique<io::StdioInputStream>();
    stream->open(name);
    std::string input1 = stream->readln();
    std::string inputNext = stream->readln();
    ASSERT_NE(input1,inputNext);
    stream->seek(0);
    std::string inputBack = stream->readln();
    ASSERT_EQ(input1,inputBack);
}

TEST_F(StdioTest, read_all)
{
    std::vector<std::string> lines;
    std::unique_ptr<io::InputStream> stream =
        std::make_unique<io::StdioInputStream>();
    stream->open(name);
    try {
        while (!stream->end_of_stream()) {
            std::string input = stream->readln();
            if (!input.empty())
                lines.push_back(input);
        }
        std::cout << " n lines ";
        std::cout << lines.size();
        SUCCEED();
    }
    catch(std::runtime_error const& err){
        std::cout << " n lines ";
        std::cout << lines.size();
        FAIL();
    }
}
