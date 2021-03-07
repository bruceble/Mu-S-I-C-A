cmake_minimum_required(VERSION 3.19)
project(AudioView)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# include SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# include Python2 for matplotlib wrapper
find_package(Python2 COMPONENTS Development NumPy)

# include OpenCV
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )



add_executable(AudioView src/audioDriver.cpp src/AudioView.cpp)
target_link_libraries(AudioView ${SDL2_LIBRARIES})
target_link_libraries(AudioView ${OpenCV_LIBS} )
target_include_directories(AudioView PRIVATE ${Python2_INCLUDE_DIRS} ${Python2_NumPy_INCLUDE_DIRS})
target_link_libraries(AudioView Python2::Python Python2::NumPy)
