// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of libreallive, a dependency of RLVM.
//
// -----------------------------------------------------------------------
//
// Copyright (c) 2006 Peter Jolly
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// -----------------------------------------------------------------------

#include "libreallive/filemap.h"

#include <fcntl.h>
#ifndef O_BINARY
const int O_BINARY = 0;
#endif
#if 0
const HANDLE___ INVALID_HANDLE_VALUE = -1;
#else
#ifndef _MSC_VER
#define INVALID_HANDLE_VALUE (-1)
#endif
#endif

#if !defined(__MINGW32__) && !defined(_MSC_VER)
#include <sys/mman.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#if !defined(_MSC_VER)
#include <unistd.h>
#endif

#include <algorithm>
#include <cerrno>
#include <string>

namespace libreallive {

#ifdef WIN32
#if 0
const int O_BINARY = 0;
const int O_RDONLY = GENERIC_READ;
const int O_RDWR = GENERIC_READ | GENERIC_WRITE;
#else
#ifndef _MSC_VER
#define O_BINARY 0
#define O_RDONLY GENERIC_READ
#define O_RDWR (GENERIC_READ | GENERIC_WRITE)
#endif
#endif
inline HANDLE open(const char* filename, int mode, int ignored) {
	/*
	HANDLE hFile = CreateFile(filename,
                    mode,
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL,
                    OPEN_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);*/
	/*HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);*/
	//O_BINARY | (mode_ == Read ? O_RDONLY : O_RDWR
	HANDLE hFile = CreateFile(filename, 
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
  if (!hFile || hFile == INVALID_HANDLE_VALUE)
  {
	  LPVOID lpMsgBuf;
	  FormatMessage(
		  FORMAT_MESSAGE_ALLOCATE_BUFFER |
		  FORMAT_MESSAGE_FROM_SYSTEM |
		  FORMAT_MESSAGE_IGNORE_INSERTS,
		  NULL,
		  GetLastError(),
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR)&lpMsgBuf,
		  0,
		  NULL);
	  MessageBox(NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION);
	  LocalFree(lpMsgBuf);
  }
  return hFile;
}
inline void close(HANDLE fp) { ::CloseHandle(fp); }
#endif

void Mapping::mclose() {
  if (mem && mapped) {
#if !defined(__MINGW32__) && !defined(_MSC_VER)
    munmap(mem, len);
#endif
  } else if (mem) {
    delete[](char*)mem;
  }
  if (fp != INVALID_HANDLE_VALUE)
    close(fp);
}

void Mapping::mopen() {
  struct stat st;
  if (stat(fn_.c_str(), &st) != 0) {
    if (errno != ENOENT)
      throw Error("Could not open file");
    if (mode_ == Read)
      throw Error("File not found");
    if (msz_ == 0)
      throw Error("Cannot create empty file");
    len = msz_;
  } else {
#ifndef _MSC_VER
    len = std::max(msz_, st.st_size);
#else
    len = max(msz_, st.st_size);
#endif
  }
  fp = open(fn_.c_str(), O_BINARY | (mode_ == Read ? O_RDONLY : O_RDWR), 0644);
  if (fp == INVALID_HANDLE_VALUE)
    throw Error("Could not open file");
#if !defined(__MINGW32__) && !defined(_MSC_VER)
  mem = mmap(0,
             len,
             mode_ == Read ? PROT_READ : PROT_READ | PROT_WRITE,
             MAP_SHARED,
             fp,
             0);
  if (mem == MAP_FAILED) {
    close(fp);
    fp = INVALID_HANDLE_VALUE;
    if (mode_ != Read) {
      throw Error("Could not map memory");
    } else {
      mapped = false;
      mem = (void*)(new char[len]);
      FILE* fh = fopen(fn_.c_str(), "rb");
      fread(mem, 1, len, fh);
      fclose(fh);
    }
  } else {
    mapped = true;
  }
#else
  close(fp);
  fp = INVALID_HANDLE_VALUE;
  mapped = false;
  mem = (void*)(new char[len]);
  FILE* fh = fopen(fn_.c_str(), "rb");
  fread(mem, 1, len, fh);
  fclose(fh);
#endif
}

Mapping::Mapping(string filename, Mode mode, off_t min_size)
    : fp(INVALID_HANDLE_VALUE),
      mem(NULL),
      fn_(filename),
      mode_(mode),
      msz_(min_size) {
  mopen();
}

Mapping::~Mapping() { mclose(); }

}  // namespace libreallive
