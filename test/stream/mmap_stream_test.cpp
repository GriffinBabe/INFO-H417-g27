#include "resources_config.hpp"
#include "stream/mmap_input_stream.hpp"
#include "stream/mmap_output_stream.hpp"
#include <gtest/gtest.h>

//----------------------------------------------
// INPUT TEST CLASS
//----------------------------------------------

class TestMMapInputStream : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

    const std::string company_name_path =
        std::string(RESOURCES_DIR) + "/company_name.csv";
};

void TestMMapInputStream::SetUp()
{
}

void TestMMapInputStream::TearDown()
{
}

//----------------------------------------------
// OUTPUT TEST CLASS
//----------------------------------------------

class TestMMapOutputStream : public testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

    const std::string output_file_path =
        std::string(OUTPUT_DIR) + "/output_1.txt";
};

void TestMMapOutputStream::SetUp()
{
}

void TestMMapOutputStream::TearDown()
{
}

//----------------------------------------------
// INPUT TEST
//----------------------------------------------

TEST_F(TestMMapInputStream, test_no_open)
{
    std::unique_ptr<io::InputStream> stream =
        std::make_unique<io::MMapInputStream>(10);

    // the stream closes in it's destructor but no files has been opened yet
}

TEST_F(TestMMapInputStream, test_wrong_file)
{
    // here we use a pointer instead of a local object
    // because we want to test our MMapInputStream/MMapOutputStream
    // through their InputStream/OutputStream interfaces.

    // unique_ptr, cannot have more than one pointer to
    // the held object, but the object is automatically
    // freed when the std::unique_ptr object goes out of scope
    std::unique_ptr<io::InputStream> stream =
        std::make_unique<io::MMapInputStream>(10);

    const std::string wrong_path =
        std::string(RESOURCES_DIR) + "/unexisting_file.csv";
    const std::string malformed_path = "*what_the_f^/hello/file.csv";

    ASSERT_FALSE(stream->open(wrong_path));
    ASSERT_FALSE(stream->open(malformed_path));
}

TEST_F(TestMMapInputStream, test_good_file)
{
    std::unique_ptr<io::InputStream> stream =
        std::make_unique<io::MMapInputStream>(10);

    ASSERT_TRUE(stream->open(company_name_path));

    std::string line = stream->readln();
    ASSERT_NE(line.size(), 0);
}

TEST_F(TestMMapInputStream, test_read_on_closed)
{
    std::unique_ptr<io::InputStream> stream =
        std::make_unique<io::MMapInputStream>(10);

    try {
        stream->readln();
        FAIL();
    }
    catch (std::runtime_error const& err) {
        SUCCEED();
    }
}

TEST_F(TestMMapInputStream, test_read_full)
{
    std::vector<std::string> lines;

    std::unique_ptr<io::InputStream> stream =
        std::make_unique<io::MMapInputStream>(100);

    ASSERT_TRUE(stream->open(company_name_path));

    while (!stream->end_of_stream()) {
        std::string line = stream->readln();
        if (!line.empty())
            lines.push_back(line);
    }

    std::cout << "Read: " << lines.size() << " lines." << std::endl;
    ASSERT_EQ(lines.size(), 234997);
}

//----------------------------------------------
// OUTPUT TEST
//----------------------------------------------

TEST_F(TestMMapOutputStream, test_no_open)
{
    std::unique_ptr<io::OutputStream> stream =
        std::make_unique<io::MMapOutputStream>(100);
}

TEST_F(TestMMapOutputStream, test_good_file)
{
    std::unique_ptr<io::OutputStream> stream =
        std::make_unique<io::MMapOutputStream>(100);

    ASSERT_TRUE(stream->create(output_file_path));

    // BEWARE: End of line character is added in the function
    std::string example_line = "Hello Brussels!";
    ASSERT_TRUE(stream->writeln(example_line));

    ASSERT_TRUE(stream->close());
    ASSERT_TRUE(stream->close()); // should emit a warning
}

TEST_F(TestMMapOutputStream, test_write_on_closed)
{
    std::unique_ptr<io::OutputStream> stream =
        std::make_unique<io::MMapOutputStream>(100);

    std::string example_line = "Hello Brussels!";

    ASSERT_FALSE(stream->writeln(example_line));
    ASSERT_TRUE(stream->close()); // emits warning

    ASSERT_TRUE(stream->create(output_file_path));
    ASSERT_TRUE(stream->writeln(example_line));
    ASSERT_TRUE(stream->writeln(example_line));

    ASSERT_TRUE(stream->close());
}