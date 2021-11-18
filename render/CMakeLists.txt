project(vrayRender)

file(GLOB SOURCEFILES "*.h" "*.cpp")

#set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(${PROJECT_NAME} STATIC ${SOURCEFILES})

set(IN_DIRS $ENV{VRAY_SDK}/cpp/include ${PROJECT_SOURCE_DIR} .. ../products/loader ${FREEIMG_INCLUDE} ${ASSIMP_INCLUDE})

include_directories(${IN_DIRS})

target_link_libraries(${PROJECT_NAME} common geo loader)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER products)
