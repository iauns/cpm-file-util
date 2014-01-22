/// \author James Hughes
/// \date   November 2013

#include <fstream>
#include <gtest/gtest.h>

#include <file-util/FileUtil.hpp>

namespace futil = CPM_FILE_UTIL_NS;

TEST(FileUtilTests, FileExistsTest)
{
  std::string tmpFile = "./tmpTestFile";

  // Check to see if file we are going to create exists...
  ASSERT_FALSE(futil::fileExists(tmpFile));

  // Create a new file, 
  std::ofstream fs;
  fs.open(tmpFile);
  fs << "Test\n";
  fs.close();

  ASSERT_TRUE(futil::fileExists(tmpFile));
}

