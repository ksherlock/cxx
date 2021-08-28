#ifndef __filesystem_stub_h__
#define __filesystem_stub_h__

//#if __has_include("cmake_filesystem.h")
//#include "cmake_filesystem.h"
#ifdef cmake_filesystem_h
#include "cmake_filesystem.h"
#else

/* default settings ... */

#define FS_HEADER "cxx_filesystem.h"
#define FS_NAMESPACE filesystem
#define IMPORT_FS_NAMESPACE

#include FS_HEADER

IMPORT_FS_NAMESPACE

#endif

#endif
