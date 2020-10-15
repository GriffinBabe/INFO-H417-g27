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
        std::string(RESOURCES_DIR) + "company_name.csv";
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
        std::string(OUTPUT_DIR) + "output_1.txt";
};

void TestSimpleOutputStream::SetUp()
{
}

void TestSimpleOutputStream::TearDown()
{
}

//----------------------------------------------
// TEST CLASSES
//----------------------------------------------

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
        std::string(RESOURCES_DIR) + "unexisting_file.csv";
    const std::string malformed_path = "*what_the_f^/hello/file.csv";

    ASSERT_FALSE(stream->open(wrong_path));
    ASSERT_FALSE(stream->open(malformed_path));
}