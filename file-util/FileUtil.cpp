#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

#include "FileUtil.hpp"

#ifndef _WIN32
  #include <wchar.h>
  #include <sys/stat.h>
  #include <regex.h>
  #include <dirent.h>
  #include <unistd.h>
  #include <pwd.h>
  #define GetCurrentDir getcwd
  #define LARGE_STAT_BUFFER struct stat
  #define LARGE_STAT(name,buffer) stat(name,buffer)

  typedef wchar_t WCHAR;
  typedef unsigned char CHAR;
#else
  #include <sys/stat.h>
  #include <shlwapi.h>
  #include <windows.h>
  #include <direct.h>
  #define GetCurrentDir _getcwd
  #define LARGE_STAT_BUFFER struct __stat64
  #define LARGE_STAT(name,buffer) _stat64(name,buffer)
#endif

namespace CPM_FILE_UTIL_NS {

// Forward declarations.
bool getFileStats(const std::string& strFileName, LARGE_STAT_BUFFER& stat_buf);
std::vector<std::string> getSubDirList(const std::string& dir);
std::string findFile(const std::string& file, bool subdirs);

//------------------------------------------------------------------------------
std::string removeExt(const std::string& fileName)
{
  size_t indexDot = fileName.find_last_of(".");
  size_t indexSlash = std::max(fileName.find_last_of("\\"),fileName.find_last_of("/"));
  if (indexDot == std::string::npos ||
      (indexSlash != std::string::npos && indexDot < indexSlash))
    return fileName;
  return fileName.substr(0,indexDot);
}

//------------------------------------------------------------------------------
std::string changeExt(const std::string& fileName, const std::string& newext)
{
  return removeExt(fileName)+ "." + newext;
}

//------------------------------------------------------------------------------
std::string getExt(const std::string& fileName)
{
  size_t indexDot = fileName.find_last_of(".");
  size_t indexSlash = std::max(fileName.find_last_of("\\"),fileName.find_last_of("/"));
  if (indexDot == std::string::npos || (indexSlash != std::string::npos && indexDot < indexSlash)) return "";
  std::string ext = fileName.substr(indexDot+1);
  return ext;
}

//------------------------------------------------------------------------------
std::string findFileInDirs(const std::string& file,
                           const std::vector<std::string>& strDirs,
                           bool subdirs)
{
  if (fileExists(file)) return file;
  for (auto it = strDirs.begin(); it != strDirs.end(); ++it)
  {
    // Skip non-existant directories.
    if (!fileExists(*it)) continue;

    std::string searchFile = *it + "/" + file;
    if (findFile(searchFile, subdirs) != "")
      return searchFile;
  }
  return "";
}

//------------------------------------------------------------------------------
std::string getFilename(const std::string& fileName)
{
  size_t index = std::max(fileName.find_last_of("\\"), fileName.find_last_of("/"));
  std::string name;
  if (index != std::string::npos)
    name = fileName.substr(index,fileName.length()-index);
  else
    name = fileName;

  return name;
}

//------------------------------------------------------------------------------
std::string findFile(const std::string& file, bool subdirs)
{
  if (!fileExists(file))
  {
    // if it doesn't exist but we allow subdir search, try harder
    if (subdirs)
    {
      std::string programPath = ".";
      std::vector<std::string> dirs = getSubDirList(programPath);
      dirs.push_back(programPath);

      std::string raw_fn = getFilename(file);
      for(std::vector<std::string>::const_iterator d = dirs.begin();
          d != dirs.end(); ++d)
      {
        std::string testfn = *d + "/" + raw_fn;
        if(fileExists(testfn)) {
          return testfn;
        }
      }
    }

    std::cerr << "Could not find '" << file << "'" << std::endl;
    return "";
  }

  return file;
}

//------------------------------------------------------------------------------
std::vector<std::string> getSubDirList(const std::string& dir)
{
  std::vector<std::string> subDirs;
  std::string rootdir;

#ifdef _WIN32
  std::stringstream s;
  if (dir == "")
  {
    CHAR path[4096];
    GetCurrentDirectoryA(4096, path);
    s << path << "/";
  }
  else
  {
    s << dir << "/";
  }

  rootdir = s.str();

  WIN32_FIND_DATAA FindFileData;
  HANDLE hFind;

  hFind = FindFirstFileA((rootdir + "*.*").c_str(), &FindFileData);

  if (hFind != INVALID_HANDLE_VALUE)
  {
    do
    {
      std::string strFilename = FindFileData.cFileName;
      if(   (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
         && strFilename != "."
         && strFilename != "..")
      {
        subDirs.push_back(strFilename);
      }
    } while ( FindNextFileA(hFind, &FindFileData) );
  }

  FindClose(hFind);
#else
  if (dir == "")
  {
    rootdir = "./";
  }
  else
  {
    rootdir = dir + "/";
  }

  DIR* dirData = opendir(rootdir.c_str());

  if (dirData != NULL)
  {
    struct dirent *inode;

    while ((inode=readdir(dirData)) != NULL)
    {
      std::string strFilenameLocal = inode->d_name;
      std::string strFilename = rootdir + strFilenameLocal;

      struct ::stat st;
      if (::stat(strFilename.c_str(), &st) != -1)
      {
        if (    S_ISDIR(st.st_mode) 
            &&  strFilenameLocal != "." && strFilenameLocal != "..")
        {
          subDirs.push_back(strFilenameLocal);
        }
      }
    }
    closedir(dirData);
  }
#endif

  std::vector<std::string> completeSubDirs(subDirs.size());
  for (size_t i = 0; i < subDirs.size(); i++)
  {
    completeSubDirs.push_back(rootdir + subDirs[i]);
  }
  return completeSubDirs;
}

//------------------------------------------------------------------------------
bool getFileStats(const std::string& strFileName, LARGE_STAT_BUFFER& stat_buf)
{
  return (LARGE_STAT(strFileName.c_str(), &stat_buf) >= 0);
}

//------------------------------------------------------------------------------
bool fileExists(const std::string& strFileName)
{
  LARGE_STAT_BUFFER stat_buf;
  return getFileStats(strFileName, stat_buf);
}

//------------------------------------------------------------------------------
std::string getCurrentWorkingDir()
{
  char cCurrentPath[FILENAME_MAX];
  if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
  {
    // Should probably throw...
    return "";
  }

  return std::string(cCurrentPath);
} 

//------------------------------------------------------------------------------
std::string getPath(const std::string& fileName)
{
  std::string path = fileName.substr(
      0,std::max(fileName.find_last_of("\\"),fileName.find_last_of("/"))+1);
  if(path.empty()) { path = "./"; }
  return path;
}

} // namespace CPM_FILE_UTIL_NS 

