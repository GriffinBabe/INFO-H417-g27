#include "stream/input_stream.hpp"
#include <gtest/gtest.h>

// -----------------------------------------------------------------------------
// EXAMPLE CLASS FOR GTEST USAGE
// -----------------------------------------------------------------------------

class TestExample : public testing::Test
{
protected:
    void SetUp() override ;
    void TearDown() override ;

private:
    const std::string file_path = "/to/be/determined";

};

void TestExample::SetUp()
{
    // initialize stuff
    // for example: objects that are commonly used in many different tests
}

void TestExample::TearDown()
{
    // cleans stuff
}

TEST_F(TestExample, test_1)
{
    bool some_value = true;
    bool another_value = true;

    ASSERT_TRUE(some_value);
    ASSERT_EQ(some_value, another_value);
}

TEST_F(TestExample, test_2)
{
    SUCCEED(); // forces test success
    // or FAIL() if you want to force test failure.
}
