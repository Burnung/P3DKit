project(geo)

file(GLOB SOURCEFILES "*.h" "*.cpp")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

message(sourcfile is  ${SOURCEFILES})
add_library(${PROJECT_NAME} STATIC ${SOURCEFILES})

set(IN_DIRS ${PROJECT_SOURCE_DIR} ..)

include_directories(${IN_DIRS})

target_link_libraries(${PROJECT_NAME}  common)
