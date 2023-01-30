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
find_package(Boost 1.55 REQUIRED COMPONENTS program_options)
include_directories( ${Boost_INCLUDE_DIRS} )
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# Add other sources and headers here:
set(
    SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/apps/etherlogger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ListenerLogger.cpp)

# Build the target "etherlogger" using source files
add_executable(etherlogger ${SOURCES})
target_include_directories(
    etherlogger 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Will need to add more libs:
target_link_libraries(etherlogger PRIVATE ${Boost_LIBRARIES})


# Force use pthreads (for Boost/asio)
# set(THREADS_PREFER_PTHREAD_FLAG ON)
# find_package(Threads REQUIRED)

# message(STATUS "CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

# # Boost library

# find_package(Boost 1.50 REQUIRED COMPONENTS filesystem)
# if(Boost_FOUND)
#     message(STATUS "Boost version: ${Boost_VERSION}")
    
#     # Check if Boost version newer than CMake, or if CMake is too old:
#     if(NOT TARGET Boost::filesystem)
#         add_library(Boost::filesystem IMPORTED INTERFACE)
#         set_property(TARGET Boost::filesystem PROPERTY
#             INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
#         set_property(TARGET Boost::filesystem PROPERTY
#             INTERFACE_LINK_LIBRARIES ${Boost_LIBRARIES})
#     endif()

#     target_link_libraries(etherlogger PUBLIC Boost::filesystem)
# elseif(NOT Boost_FOUND)
#     error("Boost not found.")
# endif()
