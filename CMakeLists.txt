cmake_minimum_required(VERSION 3.10.0)
project(teami-net)

# Make sure developers do not run cmake in the main project directory, to keep
# build artifacts from becoming clutter
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

# Turn on debug builds if we are building in a devel shell
if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
    message(STATUS "Setting debug build type by default in devel shell")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Output binaries to a sub directory "bin"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# Enable cmake testing
include(CTest)
enable_testing()

# Enable GoogleTest
include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)

# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS filesystem system log_setup log regex)
message(STATUS "Boost version: ${Boost_VERSION}")

include_directories(include)

# Locate bash program
find_program (BASH_PROGRAM bash)

add_library(server_lib src/server.cc src/session.cc)
add_library(session_lib src/session.cc src/server.cc src/config_parser.cc)
add_library(config_parser_lib src/config_parser.cc)
add_library(server_init_lib src/server_init.cc src/server.cc src/session.cc)
add_library(filesystem_disk_lib src/filesystem_disk.cc)
add_library(filesystem_fake_lib src/filesystem_fake.cc)
add_library(request_handler_crud_lib src/request_handler_crud.cc)
add_library(request_handler_echo_lib src/request_handler_echo.cc)
add_library(request_handler_static_lib src/request_handler_static.cc src/config_parser.cc)
add_library(request_handler_error_lib src/request_handler_error.cc src/config_parser.cc)
add_library(request_handler_sleep_lib src/request_handler_sleep.cc)
add_library(request_handler_health_lib src/request_handler_health.cc)
add_library(request_handler_factory_lib src/filesystem_disk.cc src/request_handler_crud.cc src/request_handler_factory.cc src/request_handler_echo.cc src/request_handler_static.cc src/request_handler_error.cc src/request_handler_dispatcher.cc src/request_handler_health.cc src/request_handler_sleep.cc)
add_library(request_handler_dispatcher_lib src/request_handler_crud.cc src/request_handler_dispatcher.cc src/request_handler_echo.cc src/request_handler_static.cc src/request_handler_error.cc src/request_handler_factory.cc src/utils.cc src/config_parser.cc src/request_handler_health.cc src/request_handler_sleep.cc)
add_library(utils_lib src/utils.cc src/request_handler_factory.cc)
add_executable(server src/server_main.cc)
target_link_libraries(server server_lib session_lib server_init_lib request_handler_echo_lib request_handler_static_lib
                      request_handler_error_lib request_handler_sleep_lib request_handler_factory_lib request_handler_dispatcher_lib config_parser_lib utils_lib
                      Boost::system Boost::log_setup Boost::log)
target_link_libraries(utils_lib request_handler_factory_lib request_handler_dispatcher_lib)
target_link_libraries(filesystem_fake_lib Boost::filesystem)
target_link_libraries(request_handler_dispatcher_lib request_handler_factory_lib)

# tests
add_executable(config_parser_test tests/config_parser_test.cc)
add_executable(server_init_test tests/server_init_test.cc)
add_executable(server_test tests/server_test.cc)
add_executable(session_test tests/session_test.cc)
add_executable(request_handler_crud_test tests/request_handler_crud_test.cc)
add_executable(request_handler_static_test tests/request_handler_static_test.cc)
add_executable(request_handler_echo_test tests/request_handler_echo_test.cc)
add_executable(request_handler_sleep_test tests/request_handler_sleep_test.cc)
add_executable(request_handler_health_test tests/request_handler_health_test.cc)
add_executable(request_handler_error_test tests/request_handler_error_test.cc)
add_executable(request_handler_factory_test tests/request_handler_factory_test.cc)
add_executable(request_handler_dispatcher_test tests/request_handler_dispatcher_test.cc)
add_executable(filesystem_fake_test tests/filesystem_fake_test.cc)
add_executable(filesystem_disk_test tests/filesystem_disk_test.cc)
add_test(NAME integration_test COMMAND ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_test.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME multi_integration_test COMMAND ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/tests/multi_thread_integration_test.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(config_parser_test config_parser_lib gtest_main)
target_link_libraries(server_init_test server_init_lib config_parser_lib server_lib session_lib request_handler_echo_lib request_handler_static_lib request_handler_dispatcher_lib Boost::system Boost::log_setup Boost::log gtest_main)
target_link_libraries(server_test server_init_lib config_parser_lib server_lib session_lib request_handler_echo_lib request_handler_static_lib request_handler_dispatcher_lib Boost::system Boost::log_setup Boost::log gtest_main)
target_link_libraries(session_test server_init_lib config_parser_lib server_lib session_lib request_handler_echo_lib request_handler_static_lib request_handler_dispatcher_lib Boost::system Boost::log_setup Boost::log gtest_main)
target_link_libraries(request_handler_crud_test request_handler_crud_lib filesystem_fake_lib gtest_main)
target_link_libraries(request_handler_static_test config_parser_lib utils_lib request_handler_echo_lib request_handler_static_lib Boost::system Boost::log_setup Boost::log gtest_main)
target_link_libraries(request_handler_echo_test config_parser_lib utils_lib request_handler_echo_lib request_handler_static_lib Boost::system Boost::log_setup Boost::log gtest_main)
target_link_libraries(request_handler_error_test config_parser_lib utils_lib request_handler_error_lib  Boost::system Boost::log_setup Boost::log gtest_main)
target_link_libraries(request_handler_sleep_test config_parser_lib utils_lib request_handler_sleep_lib Boost::system Boost::log_setup Boost::log gtest_main)
target_link_libraries(request_handler_health_test config_parser_lib request_handler_health_lib Boost::system Boost::log_setup Boost::log gtest_main)
target_link_libraries(request_handler_factory_test config_parser_lib request_handler_factory_lib request_handler_echo_lib request_handler_static_lib request_handler_error_lib request_handler_health_lib utils_lib Boost::system Boost::log_setup Boost::log gtest_main)
target_link_libraries(request_handler_dispatcher_test request_handler_dispatcher_lib config_parser_lib request_handler_echo_lib request_handler_static_lib request_handler_error_lib request_handler_health_lib utils_lib request_handler_factory_lib Boost::system Boost::log_setup Boost::log gtest_main)
target_link_libraries(filesystem_fake_test filesystem_fake_lib Boost::filesystem gtest_main)
target_link_libraries(filesystem_disk_test filesystem_disk_lib Boost::filesystem gtest_main)

gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test_configs)
gtest_discover_tests(server_init_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test_configs)
gtest_discover_tests(server_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test_configs)
gtest_discover_tests(session_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test_configs)
gtest_discover_tests(request_handler_crud_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
gtest_discover_tests(request_handler_static_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
gtest_discover_tests(request_handler_echo_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test_configs)
gtest_discover_tests(request_handler_error_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test_configs)
gtest_discover_tests(request_handler_sleep_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
gtest_discover_tests(request_handler_health_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
gtest_discover_tests(request_handler_factory_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test_configs)
gtest_discover_tests(request_handler_dispatcher_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test_configs)
gtest_discover_tests(filesystem_fake_test WORKING_DIRECTORY)
gtest_discover_tests(filesystem_disk_test WORKING_DIRECTORY)

# Test Coverage
include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(TARGETS server server_lib session_lib config_parser_lib server_init_lib 
                                 request_handler_crud_lib request_handler_static_lib
                                 request_handler_echo_lib request_handler_error_lib
                                 request_handler_dispatcher_lib utils_lib request_handler_sleep_lib
                                 filesystem_fake_lib filesystem_disk_lib request_handler_health_lib
                         TESTS config_parser_test server_init_test server_test session_test
                               request_handler_crud_test request_handler_static_test
                               request_handler_echo_test request_handler_error_test 
                               request_handler_factory_test request_handler_dispatcher_test request_handler_sleep_test
                               filesystem_fake_test filesystem_disk_test request_handler_health_test)
