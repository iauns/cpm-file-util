#ifndef IAUNS_CPM_FILEUTIL_H
#define IAUNS_CPM_FILEUTIL_H

#include <string>
#include <vector>

namespace CPM_FILE_UTIL_NS {

/// Finds a particular file in a list of possible directories.
/// \param file     Name (no path) of the file to find.
/// \param strDirs  Directories to search.
/// \param subdirs  If true, then the search will be recursive.
std::string findFileInDirs(const std::string& file,
                           const std::vector<std::string>& strDirs,
                           bool subdirs);

/// Returns true if the file 'strFileName' exists.
bool fileExists(const std::string& strFileName);

/// Returns the current working directory.
std::string getCurrentWorkingDir();

/// Returns the entire contents of the file in an std::string.
/// This is mostly just useful for quick testing.
std::string readFile(const std::string& filename);

} // namespace CPM_FILE_UTIL_NS 

#endif 
