project(testApp)
file(GLOB SOURCEFILES "*.h" "*.cpp")
find_package(OpenCV REQUIRED)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${PROJECT_NAME} ${SOURCEFILES})

set(IN_DIRS ${PROJECT_SOURCE_DIR} ../.. ${OPENCV_INCLUDE_DIRS} ${ROOT_DIR} ${ROOT_DIR}/products ${ASSIMP_INCLUDE} ${FREEIMG_INCLUDE})

include_directories(${IN_DIRS})


#linkGL(${PROJECT_NAME}) 

getGL_libs(gl_libs)

message(testGL ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} common geo ${OpenCV_LIBS} ${gl_libs} ${assimp} ${freeimage} imgui baseApp cv_common)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER apps)


