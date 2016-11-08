/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing#kwsys for details.  */
#ifdef __osf__
#define _OSF_SOURCE
#define _POSIX_C_SOURCE 199506L
#define _XOPEN_SOURCE_EXTENDED
#endif

#include "kwsysPrivate.h"
#include KWSYS_HEADER(Encoding.hxx)
#include KWSYS_HEADER(Encoding.h)

// Work-around CMake dependency scanning limitation.  This must
// duplicate the above list of headers.
#if 0
#include "Encoding.h.in"
#include "Encoding.hxx.in"
#endif

#include <stdlib.h>
#include <string.h>
#include <vector>

#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

// Windows API.
#if defined(_WIN32)
#include <windows.h>

#include <shellapi.h>
#endif

namespace KWSYS_NAMESPACE {

Encoding::CommandLineArguments Encoding::CommandLineArguments::Main(
  int argc, char const* const* argv)
{
#ifdef _WIN32
  (void)argc;
  (void)argv;

  int ac;
  LPWSTR* w_av = CommandLineToArgvW(GetCommandLineW(), &ac);

  std::vector<std::string> av1(ac);
  std::vector<char const*> av2(ac);
  for (int i = 0; i < ac; i++) {
    av1[i] = ToNarrow(w_av[i]);
    av2[i] = av1[i].c_str();
  }
  LocalFree(w_av);
  return CommandLineArguments(ac, &av2[0]);
#else
  return CommandLineArguments(argc, argv);
#endif
}

Encoding::CommandLineArguments::CommandLineArguments(int ac,
                                                     char const* const* av)
{
  this->argv_.resize(ac + 1);
  for (int i = 0; i < ac; i++) {
    this->argv_[i] = strdup(av[i]);
  }
  this->argv_[ac] = 0;
}

Encoding::CommandLineArguments::CommandLineArguments(int ac,
                                                     wchar_t const* const* av)
{
  this->argv_.resize(ac + 1);
  for (int i = 0; i < ac; i++) {
    this->argv_[i] = kwsysEncoding_DupToNarrow(av[i]);
  }
  this->argv_[ac] = 0;
}

Encoding::CommandLineArguments::~CommandLineArguments()
{
  for (size_t i = 0; i < this->argv_.size(); i++) {
    free(argv_[i]);
  }
}

Encoding::CommandLineArguments::CommandLineArguments(
  const CommandLineArguments& other)
{
  this->argv_.resize(other.argv_.size());
  for (size_t i = 0; i < this->argv_.size(); i++) {
    this->argv_[i] = other.argv_[i] ? strdup(other.argv_[i]) : 0;
  }
}

Encoding::CommandLineArguments& Encoding::CommandLineArguments::operator=(
  const CommandLineArguments& other)
{
  if (this != &other) {
    size_t i;
    for (i = 0; i < this->argv_.size(); i++) {
      free(this->argv_[i]);
    }

    this->argv_.resize(other.argv_.size());
    for (i = 0; i < this->argv_.size(); i++) {
      this->argv_[i] = other.argv_[i] ? strdup(other.argv_[i]) : 0;
    }
  }

  return *this;
}

int Encoding::CommandLineArguments::argc() const
{
  return static_cast<int>(this->argv_.size() - 1);
}

char const* const* Encoding::CommandLineArguments::argv() const
{
  return &this->argv_[0];
}

#if KWSYS_STL_HAS_WSTRING

std::wstring Encoding::ToWide(const std::string& str)
{
  return ToWide(str.c_str());
}

std::string Encoding::ToNarrow(const std::wstring& str)
{
  return ToNarrow(str.c_str());
}

std::wstring Encoding::ToWide(const char* cstr)
{
  std::wstring wstr;
  size_t length = kwsysEncoding_mbstowcs(0, cstr, 0) + 1;
  if (length > 0) {
    std::vector<wchar_t> wchars(length);
    if (kwsysEncoding_mbstowcs(&wchars[0], cstr, length) > 0) {
      wstr = &wchars[0];
    }
  }
  return wstr;
}

std::string Encoding::ToNarrow(const wchar_t* wcstr)
{
  std::string str;
  size_t length = kwsysEncoding_wcstombs(0, wcstr, 0) + 1;
  if (length > 0) {
    std::vector<char> chars(length);
    if (kwsysEncoding_wcstombs(&chars[0], wcstr, length) > 0) {
      str = &chars[0];
    }
  }
  return str;
}
#endif // KWSYS_STL_HAS_WSTRING

} // namespace KWSYS_NAMESPACE
