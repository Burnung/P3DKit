project(P3DPhysics)

include(${ROOT_DIR}/cmake/AddPhyxs.cmake)
message(root dir ${ROOT_DIR})

file(GLOB SOURCEFILES "*.h" "*.cpp")


add_library(${PROJECT_NAME} STATIC ${SOURCEFILES})

set(IN_DIRS ${PROJECT_SOURCE_DIR} ..)

include_directories(${IN_DIRS})

get_physx_libs(physx_libs)
#message(physics ${physx_libs})
target_link_libraries(${PROJECT_NAME} common ${physx_libs})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER products)
