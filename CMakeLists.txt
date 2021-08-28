cmake_minimum_required(VERSION 3.1)


if (NOT CMAKE_CXX_STANDARD)
# inherit values from parent project.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)
endif()

include(CheckCXXSourceCompiles)
include(CheckIncludeFileCXX)
include(CheckStructHasMember)
include(CheckFunctionExists)

# CHECK_INCLUDE_FILE_CXX(filesystem CXX_HAVE_FILESYSTEM)
# CHECK_INCLUDE_FILE_CXX(experimental/filesystem CXX_HAVE_EXPERIMENTAL_FILESYSTEM)

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

#msvc w/ c++17 has filesystem.
# if < c++17, has experimental/filesystem but now need to
# #define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
# to use it.


#
# MacOS - filesystem::path requires macos 10.15+
#

if (APPLE)
	# if (CMAKE_OSX_DEPLOYMENT_TARGET)
endif()

CHECK_CXX_SOURCE_COMPILES("
	#include <filesystem>
	namespace fs = std::filesystem;
	int main() { fs::path p; return 0; }
	"
	CXX_FS_STD_STD)

CHECK_CXX_SOURCE_COMPILES("
	#include <filesystem>
	namespace fs = std::experimental::filesystem;
	int main() { fs::path p; return 0; }
	"
	CXX_FS_STD_EXP)

CHECK_CXX_SOURCE_COMPILES("
	#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
	#include <experimental/filesystem>
	namespace fs = std::filesystem;
	int main() { fs::path p; return 0; }
	"
	CXX_FS_EXP_STD)

CHECK_CXX_SOURCE_COMPILES("
	#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
	#include <experimental/filesystem>
	namespace fs = std::experimental::filesystem;
	int main() { fs::path p; return 0; }
	"
	CXX_FS_EXP_EXP)





if(CXX_FS_STD_STD OR CXX_FS_STD_EXP)
	set(FS_HEADER "<filesystem>")
elseif(CXX_FS_EXP_STD OR CXX_FS_EXP_EXP)
	set(FS_HEADER "<experimental/filesystem>")
else()
	set(FS_HEADER "<cxx/cxx_filesystem.h>")
endif()

if(CXX_FS_STD_STD OR CXX_FS_EXP_STD)
	set(FS_NAMESPACE "std::filesystem")
	set(IMPORT_FS_NAMESPACE "namespace filesystem = std::filesystem;")
	set(CXX_HAVE_FILESYSTEM 1)
elseif(CXX_FS_STD_EXP OR CXX_FS_EXP_EXP)
	set(FS_NAMESPACE "std::experimental::filesystem")
	set(IMPORT_FS_NAMESPACE "namespace filesystem = std::experimental::filesystem;")
	set(CXX_HAVE_EXPERIMENTAL_FILESYSTEM 1)
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

if (CXX_HAVE_EXPERIMENTAL_FILESYSTEM AND MSVC)
        target_compile_options(CXX_LIB PUBLIC -D_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING)
endif()



add_executable(path_test src/path_test.cpp)
target_link_libraries(path_test CXX_LIB)

add_executable(string_view_test src/string_view_test.cpp)
target_link_libraries(string_view_test CXX_LIB)


enable_testing()
add_test(NAME path_test COMMAND path_test)
add_test(NAME string_view_test COMMAND string_view_test)

