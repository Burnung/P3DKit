project(faceApp)
file(GLOB SOURCEFILES "*.h" "*.cpp")
find_package(OpenCV REQUIRED)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${PROJECT_NAME} ${SOURCEFILES})

set(IN_DIRS ${PROJECT_SOURCE_DIR} ../.. ${OPENCV_INCLUDE_DIRS} ${ROOT_DIR} ${ROOT_DIR}/products ${ASSIMP_INCLUDE} ${FREEIMG_INCLUDE})

#find_package(Assimp REQUIRED)
include_directories(${IN_DIRS} ${ASSIMP_INCLUDE_DIRS})

include_directories(${ROOT_DIR}/libs/eigen/include/eigen3)

#linkGL(${PROJECT_NAME}) 

getGL_libs(gl_libs)

message(testGL ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} common geo ${OpenCV_LIBS} ${gl_libs} imgui baseApp faceDetector face3D)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER apps)


