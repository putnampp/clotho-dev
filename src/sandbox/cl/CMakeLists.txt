INCLUDE_DIRECTORIES( ${POPSIM_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${CUDA_INCLUDE_DIRS} )
cuda_add_executable( platform_test platform_test.cpp )

TARGET_LINK_LIBRARIES( platform_test ${Boost_LIBRARIES} ${CUDA_LIBRARIES} -lOpenCL )
