#include <gtest/gtest.h>
#include "stream/simple_input_stream.hpp"
#include "stream/simple_output_stream.hpp"

class TestSimpleInputStream : public testing::Test
{
protected:
    void SetUp() override;
    void TearDown() override;

    // here we use a pointer instead of a local object
    // because we want to test our SimpleInputStream/SimpleOutputStream
    // through their InputStream/OutputStream interfaces.

    // unique_ptr, cannot have more than one pointer to
    // the held object, but the object is automatically
    // freed when the std::unique_ptr object goes out of scope
    std::unique_ptr<io::InputStream> _input_stream;
    std::unique_ptr<io::OutputStream> _output_stream;
};

void TestSimpleInputStream::SetUp()
{
    _input_stream = std::make_unique<io::SimpleInputStream>();
    _output_stream = std::make_unique<io::SimpleOutputStream>();
}

void TestSimpleInputStream::TearDown()
{
}

//----------------------------------------------
// TEST CLASSES
//----------------------------------------------

TEST_F(TestSimpleInputStream, test_wrong_file)
{
}