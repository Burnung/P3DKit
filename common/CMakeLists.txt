project(common)
file(GLOB SOURCEFILES "*.h" "*.cpp" ".hpp")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(${PROJECT_NAME} STATIC  ${SOURCEFILES})
