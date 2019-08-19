cmake_minimum_required(VERSION 3.1)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

include(CheckCXXSourceCompiles)
include(CheckIncludeFileCXX)
include(CheckStructHasMember)
include(CheckFunctionExists)

CHECK_INCLUDE_FILE_CXX(filesystem CXX_HAVE_FILESYSTEM)
CHECK_INCLUDE_FILE_CXX(experimental/filesystem CXX_HAVE_EXPERIMENTAL_FILESYSTEM)

CHECK_STRUCT_HAS_MEMBER("struct stat" st_mtimespec sys/stat.h HAVE_STAT_ST_MTIMESPEC)
CHECK_STRUCT_HAS_MEMBER("struct stat" st_atimespec sys/stat.h HAVE_STAT_ST_ATIMESPEC)

CHECK_STRUCT_HAS_MEMBER("struct stat" st_mtim sys/stat.h HAVE_STAT_ST_MTIM)
CHECK_STRUCT_HAS_MEMBER("struct stat" st_atim sys/stat.h HAVE_STAT_ST_ATIM)


SET(CMAKE_EXTRA_INCLUDE_FILES sys/stat.h)
CHECK_FUNCTION_EXISTS(utimensat HAVE_UTIMENSAT)
SET(CMAKE_EXTRA_INCLUDE_FILES)

# gcc 8.3.0 has experimental/filesystem (c++14)
# and filesystem (c++17 ONLY).
# therefore, we need to check everything.

if (CXX_HAVE_FILESYSTEM)
	CHECK_CXX_SOURCE_COMPILES("
		#include <filesystem>
		namespace fs = std::experimental::filesystem;
		int main() { return 0; }
		"
		CXX_STD_EXPERIMENTAL_FILESYSTEM_NAMESPACE_1)

	CHECK_CXX_SOURCE_COMPILES("
		#include <filesystem>
		namespace fs = std::filesystem;
		int main() { return 0; }
		"
		CXX_STD_FILESYSTEM_NAMESPACE_1)

		if (NOT CXX_STD_EXPERIMENTAL_FILESYSTEM_NAMESPACE_1 AND NOT CXX_STD_FILESYSTEM_NAMESPACE_1)
			unset(CXX_HAVE_FILESYSTEM)
		endif()
endif()
if(CXX_HAVE_EXPERIMENTAL_FILESYSTEM)
	CHECK_CXX_SOURCE_COMPILES("
		#include <experimental/filesystem>
		namespace fs = std::experimental::filesystem;
		int main() { return 0; }
		"
		CXX_STD_EXPERIMENTAL_FILESYSTEM_NAMESPACE_2)

	CHECK_CXX_SOURCE_COMPILES("
		#include <experimental/filesystem>
		namespace fs = std::filesystem;
		int main() { return 0; }
		"
		CXX_STD_FILESYSTEM_NAMESPACE_2)

		if (NOT CXX_STD_EXPERIMENTAL_FILESYSTEM_NAMESPACE_2 AND NOT CXX_STD_FILESYSTEM_NAMESPACE_2)
			unset(CXX_HAVE_EXPERIMENTAL_FILESYSTEM)
		endif()
endif()

if(CXX_HAVE_FILESYSTEM)
	set(FS_HEADER "<filesystem>")
elseif(CXX_HAVE_EXPERIMENTAL_FILESYSTEM)
	set(FS_HEADER "<experimental/filesystem>")
else()
	set(FS_HEADER "<cxx/cxx_filesystem.h>")
endif()

if(CXX_STD_FILESYSTEM_NAMESPACE_1 OR CXX_STD_FILESYSTEM_NAMESPACE_2)
	set(FS_NAMESPACE "std::filesystem")
	set(IMPORT_FS_NAMESPACE "namespace filesystem = std::filesystem;")
elseif(CXX_STD_EXPERIMENTAL_FILESYSTEM_NAMESPACE_1 OR CXX_STD_EXPERIMENTAL_FILESYSTEM_NAMESPACE_2)
	set(FS_NAMESPACE "std::experimental::filesystem")
	set(IMPORT_FS_NAMESPACE "namespace filesystem = std::experimental::filesystem;")
else()
	set(FS_NAMESPACE "filesystem")
	set(IMPORT_FS_NAMESPACE " ")
endif()

configure_file(filesystem.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmake_filesystem.h)
CONFIGURE_FILE(cxx_config.h.in cxx_config.h)


if (CXX_HAVE_FILESYSTEM OR CXX_HAVE_EXPERIMENTAL_FILESYSTEM)
	set(CXX_SRC src/mapped_file.cpp)
else()
	set(CXX_SRC src/mapped_file.cpp src/filesystem.cpp src/path.cpp src/directory_iterator.cpp)
endif()



add_library(CXX_LIB ${CXX_SRC})

target_include_directories(CXX_LIB PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_compile_options(CXX_LIB PUBLIC -Dcmake_filesystem_h)

if (CXX_HAVE_EXPERIMENTAL_FILESYSTEM AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        # "Linking to libstdc++fs is required for use of the 
        # Filesystem library extensions in <experimental/filesystem>."
        target_link_libraries(CXX_LIB -lstdc++fs)
endif()

add_executable(path_test src/path_test.cpp)
target_link_libraries(path_test CXX_LIB)

enable_testing()
add_test(NAME path_test COMMAND path_test)

