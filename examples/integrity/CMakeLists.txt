cmake_minimum_required(VERSION 3.18)

project(
    integrity_test VERSION 0.1
    DESCRIPTION "ListerLogger performance integrity tester"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_REQUIRED_LINK_OPTIONS "-arch;arm64")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_CXX_EXTENSIONS NO)

# include pthread (Linux)
set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# use Boost
find_package(Boost 1.56 REQUIRED COMPONENTS program_options date_time)
include_directories( ${Boost_INCLUDE_DIRS} )
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# Add other sources and headers here:
set(
    SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# Build the target "integrity" using source files
add_executable(integrity ${SOURCES})
# target_include_directories(
#     integrity)

# Link Boost:
target_link_libraries(integrity PRIVATE Threads::Threads PRIVATE ${Boost_LIBRARIES})
