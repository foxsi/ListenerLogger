cmake_minimum_required(VERSION 3.18)

project(
    etherlogger VERSION 0.1
    DESCRIPTION "FOXSI-4 telemetry data logger"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_REQUIRED_LINK_OPTIONS "-arch;arm64")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_CXX_EXTENSIONS NO)

# use Boost
find_package(Boost 1.55 REQUIRED COMPONENTS program_options date_time)
include_directories( ${Boost_INCLUDE_DIRS} )
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# Add other sources and headers here:
set(
    SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/apps/etherlogger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ListenerLogger.cpp)

# Build the target "listenerlogger" using source files
add_executable(listenerlogger ${SOURCES})
target_include_directories(
    listenerlogger 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link Boost:
target_link_libraries(listenerlogger PRIVATE ${Boost_LIBRARIES})
