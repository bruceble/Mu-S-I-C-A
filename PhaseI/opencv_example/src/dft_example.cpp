#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core/mat.hpp"
#include "../../matplotlib-cpp/matplotlibcpp.h"
#include <iostream>
#include <string>

namespace plt = matplotlibcpp;

double pi = 3.1415926;

struct dft_data{
  std::vector<double> time;
  std::vector<double> y;
  std::vector<double> amplitude;
  std::vector<double> frequency;

};

void dft_ex(double f, double f2);
dft_data dft_zp(double f, double f2);
dft_data dft_hanning(double f, double f2);
void zero_padding(std::vector<double> &samples, std::vector<double> &time);
void hanning_window(std::vector<double> &samples);
// void plot(cv::Ptr<plot::Plot2d> &plot,cv::Mat &display, cv::Mat &x, std::vector<double> &y);
void dft_plot(plt::Plot &plot, std::vector<double> &x, std::vector<double> &y, std::string xlab, std::string ylab);

int main(){

  std::cout << "DFT - Example" << std::endl;
  double f = 0;
  double f2 = 1;
  std::cout << std::endl;
  std::cout << "----------- Integer Sine Frequency: " << std::endl;
  dft_ex(f,f2);

  f = 0;
  f2 = 1.25;
  std::cout << std::endl;
  std::cout << "----------- Fractional Sine Frequency: " << std::endl;
  dft_ex(f,f2);

  f = 36.71; // D_1HZ
  f2 = 38.89; // E^flat_1
  // f = 0;
  // f2 = 21.5;
  std::vector<double> y;
  cv::Mat x,display;
  std::cout << std::endl;
  std::cout << "----------- Fractional Sine Frequency + Padding: " << std::endl;
  dft_data data = dft_zp(f,f2);
  // x.create(1, y.size(), CV_64F);
  // for (int i = 0; i<y.size(); i++){
  //   x.at<double>(i) = i;
  // }

  std::cout << "----------- HZPlot: " << std::endl;
  int N = data.frequency.size();
    plt::figure_size(600, 390);
    plt::title("Test Signal Frequency Domain");
    plt::xlim(0,N/2);
    // plt::subplot(2,1,1);
    plt::Plot plot;
    dft_plot(plot,data.frequency,data.amplitude,"(HZ)","Normalized Amplitude");
    // plt::subplot(2,1,2);
    plt::figure_size(600, 390);
    plt::title("Test Signal Frequency Domain");
    plt::xlim(0,N);
    dft_plot(plot,data.time,data.y,"(sec)","Volume");

    plt::show();

  // again but with the hanning window -----------------------------------------
  // f = 20.75;
  // f2 = 21.25;
  std::vector<double> yh;
  cv::Mat xh,displayh;
  std::cout << std::endl;
  std::cout << "----------- Fractional Sine Frequency + Window + Padding: " << std::endl;
  dft_data datah = dft_hanning(f,f2);
  // x.create(1, y.size(), CV_64F);
  // for (int i = 0; i<y.size(); i++){
  //   x.at<double>(i) = i;
  // }

  std::cout << "----------- Plot: " << std::endl;
  N = datah.frequency.size();
    plt::figure_size(600, 390);
    plt::title("Test Signal Frequency Domain w Window");
    plt::xlim(0,N/2);
    // plt::subplot(2,1,1);
    plt::Plot ploth;
    dft_plot(ploth,datah.frequency,datah.amplitude,"(HZ)","Normalized Amplitude");
    // plt::subplot(2,1,2);
    plt::figure_size(600, 390);
    plt::title("Test Signal Frequency Domain");
    plt::xlim(0,N);
    dft_plot(ploth,datah.time,datah.y,"(sec)","Volume");

    plt::show();

  return 0;
}

void dft_ex(double f, double f2){
  double pi = 3.1415926;
  int N = 8; // window size
  double amp = 1;
  double w = 2*pi*f;
  double w2 = 2*pi*f2;

  std::vector<double> y;
  double y_i;
  for(int i=0;i<N;i++){
    double t = (double) i;
    y_i = amp*sin(w*(t/N)) + amp*sin(w2*(t/N));
    y.push_back(y_i);
  }
  std::cout << "" << std::endl;
  std::vector<double> n{0, 1, 2, 3, 4, 5, 6, 7};
  cv::Mat fd;
  cv::dft(y,fd);//,cv::DFT_REAL_OUTPUT);
  fd = cv::abs(fd);
  for(int i=0;i<N;i+=2){
    std::cout << i/2 << "th Single Sided Fourier Coef: " << 2*fd.at<double>(i)/N;
    std::cout << " [amplitude]" << std::endl;
  }

  std::cout << "ALL VALUES:" << std::endl;
  for(int i=0;i<N;i++){
    std::cout << i/2.0 << "Hz: " << 2*fd.at<double>(i)/N;
    std::cout << " [amplitude]" << std::endl;
  }
  // plot(n,y); plot(n,fd);
}

dft_data dft_zp(double f, double f2){
  int N = 128; // window size
  double amp = 1;
  double w = 2*pi*f;
  double w2 = 2*pi*f2;

  std::vector<double> y, tempo;
  double y_i;
  for(int i=0;i<N;i++){
    double t = (double) i;
    y_i = amp*sin(w*(t/N)) + amp*sin(w2*(t/N));
    // std::cout << y_i << std::endl;
    y.push_back(y_i);
    tempo.push_back(t);
  }
  zero_padding(y,tempo);
  std::cout << "" << std::endl;
  // std::vector<double> n{0, 1, 2, 3, 4, 5, 6, 7};
  cv::Mat fd;
  cv::dft(y,fd);//,cv::DFT_REAL_OUTPUT);
  fd = cv::abs(fd);
  N = y.size();
  // for(int i=0;i<N;i++){
  //   std::cout << y[N] << ", ";
  // }
  // std::cout << std::endl;
  for(int i=0;i<N/4;i+=2){
    std::cout << i/2 << "th Single Sided Fourier Coef: " << 2*fd.at<double>(i)/N;
    std::cout << " [amplitude]" << std::endl;
  }

  std::cout << "ALL VALUES:" << std::endl;
  for(int i=0;i<N;i++){
    std::cout << i/2.0 << "Hz: " << 2*fd.at<double>(i)/N;
    std::cout << " [amplitude]" << std::endl;
  }
  // plot(n,y); plot(n,fd);
  std::vector<double> X,freq;

  double idx;
  N = y.size();
  std::cout << "WINSZ: " << N/4 << std::endl;
  for(int i = 0; i<N;i+=2){
    idx = (double) i/4;
    X.push_back((fd.at<double>(i))/(N/4));
      if((fd.at<double>(i))/(N/4) > 0.1){
        std::cout << "FREQ BINS: " << idx << ", ";
        std::cout << fd.at<double>(i)/(N/4) << std::endl;
      }

    freq.push_back(idx);
  }

  cv::Mat fd_clean;
  dft_data data;
  data.y = y;
  data.amplitude = X;
  data.frequency = freq;
  data.time = tempo;
  return data;
}

dft_data dft_hanning(double f, double f2){
  double pi = 3.1415926;
  int N = 128; // window size
  double amp = 1;
  double w = 2*pi*f;
  double w2 = 2*pi*f2;

  std::vector<double> y, tempo;
  double y_i;
  for(int i=0;i<N;i++){
    double t = (double) i;
    y_i = amp*sin(w*(t/N)) + amp*sin(w2*(t/N));
    // std::cout << y_i << std::endl;
    y.push_back(y_i);
    tempo.push_back(t);
  }

  // window before Padding
  hanning_window(y);
  zero_padding(y,tempo);
  std::cout << "" << std::endl;
  // std::vector<double> n{0, 1, 2, 3, 4, 5, 6, 7};
  cv::Mat fd;
  cv::dft(y,fd);//,cv::DFT_REAL_OUTPUT);
  fd = cv::abs(fd);
  N = y.size();
  // for(int i=0;i<N;i++){
  //   std::cout << y[N] << ", ";
  // }
  // std::cout << std::endl;
  for(int i=0;i<N/4;i+=2){
    std::cout << i/2 << "th Single Sided Fourier Coef: " << 2*fd.at<double>(i)/N;
    std::cout << " [amplitude]" << std::endl;
  }

  std::cout << "ALL VALUES:" << std::endl;
  for(int i=0;i<N;i++){
    std::cout << i/2.0 << "Hz: " << 2*fd.at<double>(i)/N;
    std::cout << " [amplitude]" << std::endl;
  }
  // plot(n,y); plot(n,fd);
  std::vector<double> X,freq;

  double idx;
  N = y.size();
  std::cout << "WINSZ: " << N/4 << std::endl;
  for(int i = 0; i<N;i+=2){
    idx = (double) i/4;
    X.push_back((fd.at<double>(i))/(N/4));
      if((fd.at<double>(i))/(N/4) > 0.1){
        std::cout << "FREQ BINS: " << idx << ", ";
        std::cout << fd.at<double>(i)/(N/4) << std::endl;
      }

    freq.push_back(idx);
  }

  cv::Mat fd_clean;
  dft_data data;
  data.y = y;
  data.amplitude = X;
  data.frequency = freq;
  data.time = tempo;
  return data;
}

void hanning_window(std::vector<double> &samples){
  int L = samples.size();
  int N = L;
  double alpha = 0.5;
  std::vector<double> w;
  double wn;
  for(int i = 0;i<N;i++){
    wn = (double) alpha*(1-cos(2*pi*i/N));
    w.push_back(wn);
    samples.at(i) = samples.at(i)*w.at(i);
  }
  plt::figure_size(600,390);
  plt::named_plot("W(n)",w);
  plt::show();

}

void zero_padding(std::vector<double> &samples, std::vector<double> &time){
  int Z = samples.size();
  for(int i = 0; i<Z; i++){
    samples.push_back(0);
    time.push_back(time.size()+i);
  }
}

void dft_plot(plt::Plot &plot, std::vector<double> &x, std::vector<double> &y, std::string xlab, std::string ylab){
  std::cout << "----------- DFT Plot: " << std::endl;

  plt::plot(x,y);
  plt::xlabel(xlab);
  plt::ylabel(ylab);
  // plt::pause(5);

}

// void freq_interp(std::vector<double> &bins, double freq){}// ???
