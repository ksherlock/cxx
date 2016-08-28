cmake_minimum_required(VERSION 3.1)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

include(CheckCXXSourceCompiles)
include(CheckIncludeFileCXX)

CHECK_INCLUDE_FILE_CXX(filesystem CXX_HAVE_FILESYSTEM)
CHECK_INCLUDE_FILE_CXX(experimental/filesystem CXX_HAVE_EXPERIMENTAL_FILESYSTEM)

if (CXX_HAVE_FILESYSTEM)
	CHECK_CXX_SOURCE_COMPILES("
		#include <filesystem>
		namespace fs = std::experimental::filesystem;
		int main() { return 0; }
		"
		CXX_STD_EXPERIMENTAL_FILESYSTEM_NAMESPACE)

	CHECK_CXX_SOURCE_COMPILES("
		#include <filesystem>
		namespace fs = std::filesystem;
		int main() { return 0; }
		"
		CXX_STD_FILESYSTEM_NAMESPACE)
elseif(CXX_HAVE_EXPERIMENTAL_FILESYSTEM)
	CHECK_CXX_SOURCE_COMPILES("
		#include <experimental/filesystem>
		namespace fs = std::experimental::filesystem;
		int main() { return 0; }
		"
		CXX_STD_EXPERIMENTAL_FILESYSTEM_NAMESPACE)

	CHECK_CXX_SOURCE_COMPILES("
		#include <experimental/filesystem>
		namespace fs = std::filesystem;
		int main() { return 0; }
		"
		CXX_STD_FILESYSTEM_NAMESPACE)
endif()

if(CXX_HAVE_FILESYSTEM)
	set(FS_HEADER "<filesystem>")
elseif(CXX_HAVE_EXPERIMENTAL_FILESYSTEM)
	set(FS_HEADER "<experimental/filesystem>")
else()
	set(FS_HEADER "\"cxx_filesystem.h\"")
endif()

if(CXX_STD_FILESYSTEM_NAMESPACE)
	set(FS_NAMESPACE "std::filesystem")
	set(IMPORT_FS_NAMESPACE "namespace filesystem = std::filesystem;")
elseif(CXX_HAVE_EXPERIMENTAL_FILESYSTEM)
	set(FS_NAMESPACE "std::experimental::filesystem")
	set(IMPORT_FS_NAMESPACE "namespace filesystem = std::experimental::filesystem;")
else()
	set(FS_NAMESPACE "filesystem")
	set(IMPORT_FS_NAMESPACE " ")
endif()

configure_file(filesystem.h.in ${CMAKE_CURRENT_SOURCE_DIR}/filesystem.h)

if (CXX_HAVE_FILESYSTEM OR CXX_HAVE_EXPERIMENTAL_FILESYSTEM)
	set(CXX_SRC mapped_file.cpp)
else()
	set(CXX_SRC mapped_file.cpp filesystem.cpp path.cpp directory_iterator.cpp)
endif()



add_library(CXX_LIB ${CXX_SRC})