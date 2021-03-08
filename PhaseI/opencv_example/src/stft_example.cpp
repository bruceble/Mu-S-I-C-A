#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core/mat.hpp"
#include "../../matplotlib-cpp/matplotlibcpp.h"
#include <iostream>
#include <string>

namespace plt = matplotlibcpp;

int spectoCount = 0;
double pi = 3.1415926;

struct TransformData{
  std::vector<double> time;
  std::vector<double> amplitude;
  std::vector<double> frequency;
  std::vector<double> magnitude;
};

struct Spectrogram{
  int transform_count;
  int max_frequency;
  std::vector<TransformData> data;
  cv::Mat dft_frequency;
};

class STFT{
  private:
    Spectrogram spect;
      /* tranform time domain data from y
      */
    void DFT(std::vector<double> &y, int t_idx){
      cv::Mat fd;
      cv::dft(y,fd); //,cv::DFT_REAL_OUTPUT); // omit optional param for Real #'s
      // std::cout <<fd.size()<<" ";
      fd = cv::abs(fd);
      // std::cout <<fd.t().size()<<std::endl;
      int N = y.size()/2;
      double f_i;
      for(int i = 0; i<N;i++){
        f_i = (double) i/2;
        spect.data[t_idx].magnitude.push_back((fd.at<double>(i*2))/(N/2));
          // if((fd.at<double>(i*2))/(N/2) > 0.1){ // for checking each DFT result
          //   std::cout << "FREQ BINS: " << f_i << ", ";
          //   std::cout << fd.at<double>(i*2)/(N/2) << std::endl;
          // }

        // freq.push_back(f_i); // SKIP UNLESS TAKING FREQ VECTOR INPUT
      }
      int wsz = spect.data[t_idx].magnitude.size();
      for (int i = 0;i<wsz;i++){ // insert magnitudes as column for each transform count
        spect.dft_frequency.at<double>(wsz-i-1,t_idx) = 255 - 250*spect.data[t_idx].magnitude.at(i);
        spectoCount++;
      //   for (int j = 0;j<4; j++){
      //     spect.dft_frequency.at<double>(4*(wsz-i)+0,4*t_idx+j) = 255 - 250*spect.data[t_idx].magnitude.at(i);
      //     spect.dft_frequency.at<double>(4*(wsz-i)+1,4*t_idx+j) = 255 - 250*spect.data[t_idx].magnitude.at(i);
      //     spect.dft_frequency.at<double>(4*(wsz-i)+2,4*t_idx+j) = 255 - 250*spect.data[t_idx].magnitude.at(i);
      //     spect.dft_frequency.at<double>(4*(wsz-i)+3,4*t_idx+j) = 255 - 250*spect.data[t_idx].magnitude.at(i);
      //   }
      }
      // cv::hconcat(spect.dft_frequency, fd, spect.dft_frequency);
      // std::cout << t_idx << std::endl;

    }
    void hanningWindow(std::vector<double> &y){
        // apply hanning window to time domain amplitude data
    }
    void zeroPadding(std::vector<double> &y){
        // apply zero padding to time domain amplitude data
        int Z = y.size();
        for(int i = 0; i<Z; i++){
          y.push_back(0);
        }
    }



  public:
    STFT(){ // default constructor
      std::cout << "STFT - Example" << std::endl;
      std::cout << "Expected constructor: ";
      std::cout << "STFT(std::vector<double> y, int windowSize)" << '\n';
    }
    STFT(std::vector<double> y, int windowSize){
      spect.transform_count = y.size();
      std::vector<double> windowSamples;
      spect.dft_frequency = cv::Mat(windowSize, spect.transform_count, CV_64FC1, 0.0); // C1 not C3
      // int k = 2*spect.transform_count;
      // std::cout << "[" << k << "]" << spect.dft_frequency.at<double>(0,k) << std::endl;

      // iterators:
      int t_idx = 0;
      int w_idx = 0;

      // pad end of y samples set so last window is not undersized
      // at somepoint I need to figure out how to handle edge cases...
        // like frequencies at t_idx > transform_count - windowSize
        // possible change: padd windowSize/2 before y and windowSize/2 after y
      while(w_idx<windowSize){ // only padding the end
        y.push_back(0);
        w_idx++;
      }


      while(t_idx<spect.transform_count){ // iterate through data set performind DFT's
        spect.data.push_back(TransformData()); // push new TransformData struct to spect.data

        w_idx = 0;
        while(w_idx<windowSize){ // isolate data for transform
          windowSamples.push_back( y.at(t_idx+w_idx) );
          w_idx++;
        }
        // std::cout << "------------- Transform Count: " << t_idx << std::endl;
        spect.data[t_idx].amplitude = windowSamples; // store current transform window's samples to spect amplitude data at element transform index (t_idx)
        hanningWindow(windowSamples);
        zeroPadding(windowSamples);
        DFT(windowSamples,t_idx); // the DFT function will store resulting DFT magnitudes to corresponding data index for magnitude member

        windowSamples.clear(); // reset vector for next iteration
        std::cout << t_idx << std::endl;
        t_idx++;
      } // end of transform count loop

    } // end of STFT constructor
    void colorize(){
      // std::cout << "Size of spect.data vector: " << spect.data.size() << std::endl;
      // std::cout << "Size of spect.data[0].magnitude vector: " << spect.data[0].magnitude.size() << std::endl;
      std::cout << "Next Step: colorize to put data in format for opencv COLORMAP: " << std::endl;
      std::cout << spectoCount << std::endl;

      // PUTS BLANK SPACE AT GIVEN X RANGE
      // int k = 1500;
      // while(k<1600){
      //   std::cout << "[" << k << "] " ;
      //   for (int m = 0;m<100;m++){
      //     // spect.dft_frequency.at<double>(m,k) = 1000;
      //      spect.dft_frequency.at<double>(m,k) = 100;
      //   }
      //   std::cout  << std::endl;
      //   k++;
      // }
      std::cout << spect.dft_frequency.size() << std::endl;
      cv::Mat img_color;
      cv::Mat img_in = spect.dft_frequency;
      // BY HAND...
      // cv::Mat img_in(100,1000, CV_64FC3, 0.0);
      // for (int w = 0; w<1000;w++){
      //   for (int h = 0; h<100;h++){
      //     img_in.at<double>(h,w) = spect.dft_frequency.at<double>(h,w);
      //   }
      // }




      // cv::applyColorMap(spect.dft_frequency, img_color, cv::COLORMAP_JET);
      cv::namedWindow("Without color adjustment", cv::WINDOW_NORMAL);
      cv::resizeWindow("Without color adjustment", spect.dft_frequency.size().width, spect.dft_frequency.size().height);
      std::cout << spect.dft_frequency.size() << std::endl;
      cv::imshow("Without color adjustment", spect.dft_frequency);
      std::cout << spect.dft_frequency.size() << std::endl;
      cv::waitKey(0);

      img_in.convertTo(img_in, CV_8UC1); // C1 not C3
      std::cout << img_in.size() << std::endl;
      for(int i = 0; i<13; i++){
        cv::applyColorMap(img_in, img_color, i);
        std::cout << img_color.size() << std::endl;
        cv::namedWindow("Color Map", cv::WINDOW_NORMAL);
        // cv::resizeWindow("Color Map", 1000, 100);
        cv::imshow("Color Map", img_color);
        std::cout << i << std::endl;
        if(i==2) cv::imwrite("image2.jpg",img_color);
        cv::waitKey(0);
      }
      // plt::imshow(spect.dft_frequency);
    }
};

void freqSteps(std::vector<double> &fx){
  // constant amplitude signal of increasing frequency
  int N = 100;
  double w, y_i;
  std::vector<double> tempo;

  for (int f = 4; f<10; f++){

    w = (double) 2*pi*(f*5);
    std::cout << "Freq: " << f << std::endl;

    for(int i=0;i<N;i++){
      double t = (double) i + f*N;
      y_i = sin(w*(t/N)); //  + sin(1.2*w*(t/N));
      fx.push_back(y_i);
      tempo.push_back(t);
    }
  }
}

void timePlot(std::vector<double> &y){
  plt::figure_size(600, 390);
  plt::title("Stepped Frequency signal");
  plt::xlim(0,1001);
  plt::plot(y);
}

int main(){
  std::vector<double> y;
  freqSteps(y);
  timePlot(y);
  plt::show();
  STFT audioData(y,100);
  audioData.colorize();
  return 0;
}
