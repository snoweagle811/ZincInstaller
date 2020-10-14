cmake_minimum_required(VERSION 3.3.2)
project(ZincInstaller C CXX)

# set everything up for c++ 17 features
set(CMAKE_CXX_STANDARD 17)
# Don't add this line if you will try_compile with boost.
set(CMAKE_CXX_STANDARD_REQUIRED 1)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/std:c++17)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-std=C++17)
endif()

include(cmake/App.cmake)
add_subdirectory(7zip-cpp)
include_directories(7zip-cpp)

set(SOURCES "src/MyApp.h"
            "src/MyApp.cpp"
            "src/main.cpp")
			
			
# define the standard link libraries
if (FIPS_UWP)
    set(CMAKE_CXX_STANDARD_LIBRARIES "WindowsApp.lib")
    set(CMAKE_C_STANDARD_LIBRARIES "WindowsApp.lib")
else()
    set(CMAKE_CXX_STANDARD_LIBRARIES "kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib dbghelp.lib wsock32.lib ws2_32.lib rpcrt4.lib wininet.lib urlmon.lib comctl32.lib")
    set(CMAKE_C_STANDARD_LIBRARIES "kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib dbghelp.lib wsock32.lib ws2_32.lib rpcrt4.lib wininet.lib urlmon.lib comctl32.lib")
endif()

add_app("${SOURCES}")

target_link_libraries(${PROJECT_NAME} 7zpp)