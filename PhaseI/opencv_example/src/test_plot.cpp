#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core/mat.hpp"

#include <iostream>
using namespace cv;

int main(){
  Mat xData, yData, display;
  plot::Plot2d *plt;
  xData.create(1, 100, CV_64F);//1 Row, 100 columns, Double
  yData.create(1, 100, CV_64F);

  for(int i = 0; i<100; ++i)
  {
    xData.at<double>(i) = i/10.0;
    yData.at<double>(i) = (i/10.0)*(i/10.0);
  }
  plt = plot::createPlot2d(xData, yData);
  plt->setPlotSize(100, 1000);
  plt->setMaxX(10);
  plt->setMinX(0);
  plt->setMaxY(100);
  plt->setMinY(-1);
  plt->render(display);
  imshow("Plot", display);
  waitKey();
  return 0;
}
