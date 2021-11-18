project(renderTech)
file(GLOB SOURCEFILES "*.h" "*.cpp" "*.hpp")
find_package(OpenCV REQUIRED)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(${PROJECT_NAME} STATIC ${SOURCEFILES})

set(IN_DIRS ${PROJECT_SOURCE_DIR} .. ${OPENCV_INCLUDE_DIRS} ${ROOT_DIR}/products)
SET(IN_DIRS ${IN_DIRS} ${FREEIMG_INCLUDE} ${ASSIMP_INCLUDE})

include_directories(${IN_DIRS} ${ASSIMP_INCLUDE_DIRS})
include_directories(${IN_DIRS})


#linkGL(${PROJECT_NAME}) 

getGL_libs(gl_libs)

message(testGL ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} common geo P3DEngine ${OpenCV_LIBS} ${gl_libs})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER products)


