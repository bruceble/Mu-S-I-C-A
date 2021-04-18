cmake_minimum_required(VERSION 3.19)

project(testMLPack)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Define the files we need to compile.
# Anything not in this list will not be compiled into MLPACK.
set(SOURCES
  nca.h
  nca_impl.h
  nca_softmax_error_function.h
  nca_softmax_error_function_impl.h
)

# Add directory name to sources.
set(DIR_SRCS)
foreach(file ${SOURCES})
  set(DIR_SRCS ${DIR_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/${file})
endforeach()
# Append sources (with directory name) to list of all MLPACK sources (used at
# the parent scope).
set(MLPACK_SRCS ${MLPACK_SRCS} ${DIR_SRCS} PARENT_SCOPE)

find_package(Armadillo REQUIRED)
include_directories(${ARMADILLO_INCLUDE_DIRS})

# OpenMP support
find_package(OpenMP REQUIRED)

add_executable(test src/test.cpp)
target_link_libraries(test
  mlpack
  ${ARMADILLO_LIBRARIES}
  ${Boost_LIBRARIES}
  ${COMPILER_SUPPORT_LIBRARIES}
  ${OpenMP_CXX_FLAGS}
  OpenMP::OpenMP_CXX
)

add_executable(CNN src/convolutionalNN.cpp)
target_link_libraries(CNN
  mlpack
  ${ARMADILLO_LIBRARIES}
  ${Boost_LIBRARIES}
  ${COMPILER_SUPPORT_LIBRARIES}
  ${OpenMP_CXX_FLAGS}
  OpenMP::OpenMP_CXX
)
