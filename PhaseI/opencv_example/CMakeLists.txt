cmake_minimum_required(VERSION 3.19)
project( DisplayIMG )
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

# include Python2 for matplotlib wrapper
find_package(Python2 COMPONENTS Development NumPy)

add_executable( DisplayIMG src/displayIMG.cpp )
target_link_libraries( DisplayIMG ${OpenCV_LIBS} )

add_executable( dftBasic src/dft_example.cpp )
target_link_libraries( dftBasic ${OpenCV_LIBS} )
target_include_directories(dftBasic PRIVATE ${Python2_INCLUDE_DIRS} ${Python2_NumPy_INCLUDE_DIRS})
target_link_libraries(dftBasic Python2::Python Python2::NumPy)


#add_executable( testPlot src/test_plot.cpp )
#target_link_libraries( testPlot ${OpenCV_LIBS} )

add_executable( stftBasic src/stft_example.cpp )
target_link_libraries( stftBasic ${OpenCV_LIBS} )
target_include_directories(stftBasic PRIVATE ${Python2_INCLUDE_DIRS} ${Python2_NumPy_INCLUDE_DIRS})
target_link_libraries(stftBasic Python2::Python Python2::NumPy)
