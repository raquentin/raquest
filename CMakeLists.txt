cmake_minimum_required(VERSION 3.24)
project(raquest)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMAND ON)

include_directories(include)
add_definitions(-DSOME_DEFINITION)
add_subdirectory(src)

enable_testing()
add_subdirectory(tests)

include(FetchContent)

FetchContent_Declare(
  CLI11 URL https://github.com/CLIUtils/CLI11/archive/refs/tags/v2.3.2.zip)

FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)

FetchContent_Declare(
  curl
  URL https://github.com/curl/curl/releases/download/curl-7_81_0/curl-7.81.0.tar.gz
)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1
FetchContent_MakeAvailable(fmt)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest CLI11 json curl fmt)
