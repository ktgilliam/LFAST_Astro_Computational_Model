# Define the project name
# Have to suppress this warning for sanity
if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)
project(instrument_model LANGUAGES C CXX VERSION 0.0.1)
cmake_minimum_required(VERSION 3.10)



# specify the C++ standard
# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

#=================================================================================================#
#========================================= Version Info  =========================================#
#=================================================================================================#
# these will be used to set the version number in config.h and our driver's xml file
set(CDRIVER_VERSION_MAJOR 1)
set(CDRIVER_VERSION_MINOR 2)

#=================================================================================================#
#========================================= Dependencies  =========================================#
#=================================================================================================#
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)


# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)



set(BUILD_TESTS true)

message("BUILD_TESTS=TRUE, unit tests will be built.")
enable_testing()
include(CTest)

add_library(InstrumentModel InstrumentModel.cc InstrumentModel.h)

add_executable(
  InstrumentModel_tests
  InstrumentModel_tests.cc
)

target_link_libraries(
  InstrumentModel_tests
  InstrumentModel
  GTest::gtest_main
)

#### Bringing it all together
include(GoogleTest)
gtest_discover_tests(InstrumentModel_tests)


