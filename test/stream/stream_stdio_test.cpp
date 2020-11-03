#include "stream/input_stdio_stream.hpp"
#include "stream/ouput_stdio_stream.hpp"
#include <gtest/gtest.h>
#include <resources_config.hpp>

class StdioTest : public testing::Test{

protected:
    const std::string name =   std::string(RESOURCES_DIR) + "/movie_info.csv";
};

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
        SUCCEED();
    }
    catch(std::runtime_error const& err){
        FAIL();
    }
    std::cout << " n lines ";
    std::cout << lines.size();
}

// output tests

class TestStdioOutputStream : public testing::Test {
protected:

    const std::string output_file_path =
        std::string(OUTPUT_DIR) + "/output_1.txt";
};

TEST_F(TestStdioOutputStream, create_close){
    std::unique_ptr<io::OutputStream> stream = std::make_unique<io::StdioOutputStream>();
    ASSERT_TRUE(stream->create(output_file_path));
    ASSERT_TRUE(stream->close());
}

TEST_F(TestStdioOutputStream, fill){
    std::unique_ptr<io::OutputStream> stream = std::make_unique<io::StdioOutputStream>();
    ASSERT_TRUE(stream->create(output_file_path));
    std::string to_write;
    for(int i=0; i<100 ; i++){
        to_write = std::to_string(i);
        ASSERT_TRUE(stream->writeln(to_write));
    }
    ASSERT_TRUE(stream->close());
}


