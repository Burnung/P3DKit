# set (TARGET_CXX_COMPILE_OPTIONS
# 	-O3
# 	-DARM_NEON_ENABLE
# 	-ffast-math
# )

# set (TARGET_C_COMPILE_OPTIONS
# 	-O3
# 	-ffast-math
# 	-mno-thumb
# )

add_compile_options (
	#-DEIGEN_USE_BLAS
	-O3
	-DARM_NEON_ENABLE
	-DGL_SILENCE_DEPRECATION
	#-fembed-bitcode
	-ffast-math
	-Wno-deprecated-declarations
	-fvisibility=hidden
)
#set(USE_OPENCV_LITE 1)

#predefined macros
add_definitions(-DIS_ANDROID=1)
