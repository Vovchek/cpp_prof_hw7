cmake_minimum_required(VERSION 3.14)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 17)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

project(bulk VERSION ${PROJECT_VESRION})

# ++for google test
include(FetchContent)   # run ..\CMake\share\cmake-X.YY\Modules\FetchContent.cmake
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  #URL G:/softdev/googletest-main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

enable_testing()    # Enables testing for this directory and below
add_executable(bulk_test bulk_test.cpp)
target_link_libraries(bulk_test GTest::gtest_main)
# --for google test

add_executable(bulk bulk.cpp) # target source ...

set_target_properties(bulk PROPERTIES    # target PROPERTIES prop1 value1  ...
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(bulk
    PRIVATE "${CMAKE_BINARY_DIR}"
)

if (MSVC)
    target_compile_options(bulk PRIVATE
        /W4
    )
else ()
    target_compile_options(bulk PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()

# ++for google test
include(GoogleTest)               # run ..\CMake\share\cmake-X.YY\Modules\GoogleTest.cmake
gtest_discover_tests(bulk_test)  # discover tests by asking the compiled test executable to enumerate its tests
# ++for google test

#install(TARGETS bulk bulk_test RUNTIME DESTINATION bin)
install(TARGETS bulk RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT vovchek64@inbox.ru)

include(CPack)
