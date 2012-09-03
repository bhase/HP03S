#include "CppUTest/TestHarness.h"
#include "HP03SBuildTime.h"

TEST_GROUP(HP03SBuildTime)
{
  HP03SBuildTime* projectBuildTime;

  void setup()
  {
    projectBuildTime = new HP03SBuildTime();
  }
  void teardown()
  {
    delete projectBuildTime;
  }
};

TEST(HP03SBuildTime, Create)
{
  CHECK(0 != projectBuildTime->GetDateTime());
}

