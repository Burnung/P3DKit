project(testPhy)

file(GLOB SOURCEFILES "*.h" "*.cpp")
find_package(OpenCV REQUIRED)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(${ROOT_DIR}/cmake/AddPhyxs.cmake)

add_executable(${PROJECT_NAME} ${SOURCEFILES})

set(IN_DIRS ${PROJECT_SOURCE_DIR} ../.. ${OPENCV_INCLUDE_DIRS})

include_directories(${IN_DIRS})

#linkGL(${PROJECT_NAME}) 

getGL_libs(gl_libs)

get_physx_libs(phy_libs)

target_link_libraries(${PROJECT_NAME} common geo P3DEngine P3DPhysics ${OpenCV_LIBS} ${gl_libs} ${phy_libs})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER testCode)


