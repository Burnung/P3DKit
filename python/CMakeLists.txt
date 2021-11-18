Project(pyP3D)

file(GLOB SOURCEFILES "*.h" "*.cpp" "*.hpp")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

#find_package(embree  REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${EMBREE_INCLUDE_DIRS} ${ROOT_DIR} ${ROOT_DIR}/products/hairCloud ${ROOT_DIR}/products/hairCloud/Math)
include_directories(${ROOT_DIR}/libs/eigen/include/eigen3)

set(IN_DIRS $ENV{VRAY_SDK}/cpp/include ${PROJECT_SOURCE_DIR} .. ../libs ${FREEIMG_INCLUDE} ${ASSIMP_INCLUDE})

find_package(OpenCV REQUIRED)
include_directories(${IN_DIRS} ${OPENCV_INCLUDE_DIRS})

pybind11_add_module(${PROJECT_NAME} ${SOURCEFILES})

getGL_libs(gl_libs)
target_link_libraries(${PROJECT_NAME}  PRIVATE common geo P3DEngine  ${OpenCV_LIBS} ${ASSIMP_STATIC_LIB} ${gl_libs} renderTech ${EMBREE_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OPNECV_LIBS} faceDetector face3D baseApp imgui)
#target_link_libraries(${PROJECT_NAME} common geo ${OpenCV_LIBS} ${gl_libs} ${assimp} ${freeimage} imgui baseApp faceDetector face3D)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER products)
