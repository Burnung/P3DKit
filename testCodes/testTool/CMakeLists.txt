project(testTool)
file(GLOB SOURCEFILES "*.h" "*.cpp")

find_package(OpenCV REQUIRED)
find_package(OpenMP)

if(OpenMP_CXX_FOUND)
	message(OpenMP found!)
endif()
#set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(embree 3.6.1 REQUIRED)
#set(SOURCEFILES main.cpp cartoon.h cartoon.cpp)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
add_executable(${PROJECT_NAME} ${SOURCEFILES})

set(IN_DIRS $ENV{VRAY_SDK}/cpp/include ${PROJECT_SOURCE_DIR} ../..${OPENCV_INCLUDE_DIRS} ${ROOT_DIR}/products/hairCloud ${ROOT_DIR}/products/hairCloud/Math)

include_directories(${IN_DIRS} ${EMBREE_INCLUDE_DIRS})

getGL_libs(gl_libs)

target_link_libraries(${PROJECT_NAME} common vrayRender common geo P3DEngine ${OpenCV_LIBS} ${gl_libs} hairCloud deformTools ${EMBREE_LIBRARY})


set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER testCode)