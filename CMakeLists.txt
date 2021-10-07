cmake_minimum_required(VERSION 3.21)

project(tgafunc C)

add_library(${PROJECT_NAME} STATIC tgafunc.c)
target_compile_features(${PROJECT_NAME} PUBLIC c_std_99)

# Set strict warning level for different compilers.
if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_include_directories(${PROJECT_NAME}
    INTERFACE ${PROJECT_SOURCE_DIR}
)

if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory(test)
endif()
