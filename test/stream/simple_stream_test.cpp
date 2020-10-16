// does give paths to our database and output folders
#include "resources_config.hpp"

#include "stream/simple_input_stream.hpp"
#include "stream/simple_output_stream.hpp"
#include <gtest/gtest.h>

//----------------------------------------------
// INPUT TEST CLASS
//----------------------------------------------

class TestSimpleInputStream : public testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

    const std::string company_name_path =
        std::string(RESOURCES_DIR) + "/company_name.csv";
};

void TestSimpleInputStream::SetUp()
{
}

void TestSimpleInputStream::TearDown()
{
}

//----------------------------------------------
// OUTPUT TEST CLASS
//----------------------------------------------

class TestSimpleOutputStream : public testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

    const std::string output_file_path =
        std::string(OUTPUT_DIR) + "/output_1.txt";
};

void TestSimpleOutputStream::SetUp()
{
}

void TestSimpleOutputStream::TearDown()
{
}

//----------------------------------------------
// INPUT TEST CLASSES
//----------------------------------------------

TEST_F(TestSimpleInputStream, test_no_open)
{
    std::unique_ptr<io::InputStream> stream =
        std::make_unique<io::SimpleInputStream>();

    // the stream closes in it's destructor but no files has been opened yet
}

TEST_F(TestSimpleInputStream, test_wrong_file)
{
    // here we use a pointer instead of a local object
    // because we want to test our SimpleInputStream/SimpleOutputStream
    // through their InputStream/OutputStream interfaces.

    // unique_ptr, cannot have more than one pointer to
    // the held object, but the object is automatically
    // freed when the std::unique_ptr object goes out of scope
    std::unique_ptr<io::InputStream> stream =
        std::make_unique<io::SimpleInputStream>();

    const std::string wrong_path =
        std::string(RESOURCES_DIR) + "/unexisting_file.csv";
    const std::string malformed_path = "*what_the_f^/hello/file.csv";

    ASSERT_FALSE(stream->open(wrong_path));
    ASSERT_FALSE(stream->open(malformed_path));
}

TEST_F(TestSimpleInputStream, test_good_file)
{
    std::unique_ptr<io::InputStream> stream =
        std::make_unique<io::SimpleInputStream>();

    ASSERT_TRUE(stream->open(company_name_path));

    std::string line = stream->readln();
    ASSERT_NE(line.size(), 0);
}

TEST_F(TestSimpleInputStream, test_read_on_closed)
{
    std::unique_ptr<io::InputStream> stream =
        std::make_unique<io::SimpleInputStream>();

    try {
        stream->readln();
        FAIL();
    }
    catch (std::runtime_error const& err) {
        SUCCEED();
    }
}

TEST_F(TestSimpleInputStream, test_read_full)
{
    std::vector<std::string> lines;

    std::unique_ptr<io::InputStream> stream =
        std::make_unique<io::SimpleInputStream>();

    ASSERT_TRUE(stream->open(company_name_path));

    while (!stream->end_of_stream()) {
        std::string line = stream->readln();
        if (line.size() != 0)
            lines.push_back(line);
    }

    std::cout << "Read: " << lines.size() << " lines." << std::endl;
    ASSERT_EQ(lines.size(), 234997);
}

//----------------------------------------------
// OUTPUT TEST
//----------------------------------------------

TEST_F(TestSimpleOutputStream, test_no_open)
{
    std::unique_ptr<io::OutputStream> stream =
        std::make_unique<io::SimpleOutputStream>();
}

TEST_F(TestSimpleOutputStream, test_good_file)
{
    std::unique_ptr<io::OutputStream> stream =
        std::make_unique<io::SimpleOutputStream>();

    ASSERT_TRUE(stream->create(output_file_path));

    // BEWARE: End of line character is added in the function
    std::string example_line = "Hello Brussels!";
    ASSERT_TRUE(stream->writeln(example_line));

    ASSERT_TRUE(stream->close());
    ASSERT_TRUE(stream->close()); // should emit a warning
}

TEST_F(TestSimpleOutputStream, test_write_on_closed)
{
    std::unique_ptr<io::OutputStream> stream =
        std::make_unique<io::SimpleOutputStream>();

    std::string example_line = "Hello Brussels!";

    ASSERT_FALSE(stream->writeln(example_line));
    ASSERT_TRUE(stream->close()); // emits warning

    ASSERT_TRUE(stream->create(output_file_path));
    ASSERT_TRUE(stream->writeln(example_line));
    ASSERT_TRUE(stream->writeln(example_line));

    ASSERT_TRUE(stream->close());
}