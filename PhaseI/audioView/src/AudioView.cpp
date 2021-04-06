#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <SDL.h> // sdl

#include "../../../../matplotlib-cpp/matplotlibcpp.h" // plt

#include "../src/AudioView.hpp"

#include "opencv2/core.hpp" // spect
#include "opencv2/videoio.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core/mat.hpp"

namespace plt = matplotlibcpp;

void MyAudioCallback(void* userdata, Uint8* stream, int streamLength){
    AudioData* audio = (AudioData*)userdata; //reason for casting instead of just using diff input:
    if (audio->length == 0) return;
    Uint32 length = (Uint32)streamLength;
    length = (length > audio->length ? audio->length : length); // if length  > audio->length then assign audio->length to length outher wise don't
    SDL_memcpy(stream, audio->pos, length); //SDL_MixAudioFormat(stream, audio->pos, length, vol);

    audio->pos += length;
    audio->length -= length;
}

AudioView::AudioView(){
    // at some point, assign values to private freq, etc good for sample to time calculations?
}

AudioView::~AudioView(){

}

/* implement ffmpeg to read + store file */
void AudioView::read(const char * filename_in){
    FILE *pipe_in;
    char phrase_in[50];
    int16_t sample;
    int count;
    double amp_i;

    sprintf(phrase_in, "ffmpeg -i %s -f s16le -ac 1 -", filename_in); // using -ac 2 adds reflected data...
    pipe_in  = popen(phrase_in, "r"); // open pipe, "r" signifies read

    while(1){
        count = fread(&sample, 2, 1, pipe_in); // read one 2-byte sample
        audio_amplitude.push_back(sample);
        if (count != 1) break;
        audio_length++;
    }

    /* push back sample number and amplitude to waveform object */
    for (int i=0; i<audio_length; i++) {
        waveform.sample.push_back(i);
        amp_i = (double) audio_amplitude.at(i);
        waveform.amplitude.push_back(amp_i);
    }

    pclose(pipe_in); // close pipe
};

/* implement ffmpeg to write file */
void AudioView::write(const char * filename_in, float damp_ratio){
  FILE *pipe_out;
  int16_t sample;

  pipe_out = popen("ffmpeg -y -f s16le -ar 44100 -ac 1 -i - out.wav", "w"); // -f forces format to sint16 long-endian -ar sets rate to 44100 Hz, -ac (1 channel), -i filename
  for(int i=0;i<audio_length;i++){
      sample = audio_amplitude.at(i)*damp_ratio; // use damp ratio = 1 for not change
      fwrite(&sample, 2, 1, pipe_out);
  }
  pclose(pipe_out);
}

/* implement ffmpeg to write file -- overload for input audio vector*/
void AudioView::write(std::vector<double> input_audio, std::string fname){
  FILE *pipe_out;
  int16_t sample;

  char phrase_out[50];
  sprintf(phrase_out, "ffmpeg -y -f s16le -ar 44100 -ac 1 -i - %s.wav", fname.c_str()); // using -ac 2 adds reflected data...


  pipe_out = popen(phrase_out, "w"); // -f forces format to sint16 long-endian -ar sets rate to 44100 Hz, -ac (1 channel), -i filename
  for(int i=0;i<input_audio.size();i++){
      sample = input_audio.at(i);
      fwrite(&sample, 2, 1, pipe_out);
  }
  pclose(pipe_out);
}

/* implement SDL to play file */
void AudioView::play(const char * filename_in){
    SDL_Init(SDL_INIT_AUDIO);

    SDL_AudioSpec wavSpec, want;
    Uint8* wavStart;
    Uint32 wavLength;

    if (SDL_LoadWAV(filename_in, &wavSpec, &wavStart, &wavLength) == NULL){ // Load wav file
        std::cerr << "Error: " << filename_in << "not found" << std::endl; // Error handling
        return;
    }

    std::cout << "\n" << "Loading " << filename_in << " ...\n" << std::endl;

    AudioData audio;
    audio.pos = wavStart;
    audio.length = wavLength;

    wavSpec.callback = MyAudioCallback; // SDL will use this M.A.C function to play audio
    wavSpec.userdata = &audio;

    SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0); // changed the last param from SDL_AUDIO_ALLOW_ANY_CHANGE to 0 (due to error of playing without sound

    if (device == 0){ // Error Handling
        std::cerr << "error: " << SDL_GetError() << "could not be loaded as audio file" << std::endl;
        return;
    }

    /* Play audio */
    std::cout << "\n" << "Playing " << filename_in << " ...\n" << std::endl;
    SDL_PauseAudioDevice(device, 0); // zero to pause (pause == play in SDL)
    while (audio.length > 0) SDL_Delay(100); // 100 ms delay's until audio end

    SDL_CloseAudioDevice(device);
    SDL_FreeWAV(wavStart);
    SDL_Quit();

    std::cout << "--------- SDL Quit ---------" << '\n';
}

/* implement matplotlibcpp for ploting time domain waveform */
void AudioView::plotWaveform(){
    // potentiallly add option for sample plot or time plot?

    double limSz = 100000; // added to shorten limits
    std::vector<double> xt(limSz), yt(limSz); // had problems when assigning to Sint16
    int win = (int) limSz;
    double xlow, xupp;

    plt::title("Audio waveform "); // plot full waveform
    plt::xlim(waveform.sample.front()-limSz, limSz + waveform.sample.front());
    plt::ylim(*min_element(waveform.amplitude.begin(),waveform.amplitude.end()), *max_element(waveform.amplitude.begin(),waveform.amplitude.end()));
    plt::axis("equal");
    plt::named_plot("Full Waveform", waveform.sample, waveform.amplitude); // full audio

    plt::Plot plot("Segmented Waveform"); // audio segment of size limSz
    plt::legend();

    for (int i=0; i<audio_length; i++) {
      xt.push_back(waveform.sample.at(i));
      yt.push_back(waveform.amplitude.at(i)/2); // divides by 2 for half amplitude

      if (i % win == 0) { // update limits
          xlow = double (i-2*win);
          xupp = double (i+win);
          plt::xlim(xlow,xupp);
          plot.update(xt, yt);
          plt::pause(0.1); // 0.1 sec delay
          xt.clear();
          yt.clear();
      }

      if (i == audio_length-1){ // handle edge audio data
          xlow = double (i-2*win);
          xupp = double (i+win);
          plt::xlim(xlow,xupp);
          plot.update(xt, yt);
          plt::pause(1);
      }

    }

}

/* Discrete Fourier Transform of audio vector y */
void AudioView::DFT(std::vector<double> &y, int t_idx){
  cv::Mat fd;
  int N = y.size()/2;
  double f_i;

  cv::dft(y,fd);
  fd = cv::abs(fd); // absolut value

  for(int i = 0; i<N;i++){
    f_i = (double) i/2;
    spect.data[t_idx].magnitude.push_back((fd.at<double>(i*2))/(N/2));
  }

  int wsz = spect.data[t_idx].magnitude.size();

  for (int i = 0;i<wsz;i++){ // insert magnitudes as column for each transform count
    for(int j=0;j<pixel_scale_freq;j++){
      for(int k=0;k<pixel_scale_time;k++){
        spect.dft_frequency.at<double>(pixel_scale_freq*(wsz-i-1)+j,pixel_scale_time*t_idx+k) = spect.data[t_idx].magnitude.at(i);
        spect_count++; // track dft frequency data point
        if(spect.data[t_idx].magnitude.at(i) > spect.max_magnitude) spect.max_magnitude = spect.data[t_idx].magnitude.at(i); // track maximum magnitude
      }
    }
  }

}

/* apply hanning window to time domain amplitude data */
void AudioView::hanningWindow(std::vector<double> &y){
  // int N = y.size();
  // double alpha = 0.5;
  // double pi = 3.1415926;
  // std::vector<double> w;
  // double wn;
  //
  //
  // for(int i = 0;i<N;i++){
  //   wn = (double) alpha*(1-cos(2*pi*i/N)); // window function
  //   w.push_back(wn);
  //   y.at(i) = y.at(i)*w.at(i);
  // }

}


/* apply zero padding to time domain amplitude data */
void AudioView::zeroPadding(std::vector<double> &y, int N){
    int Z = N - y.size();
    for(int i = 0; i<Z; i++){
      y.push_back(0);
    }
}

/* Add color scalle to Spectrogram */
void AudioView::colorize(){
      cv::Mat img_color;
      cv::Mat img_in = spect.dft_frequency;

      for(int i = 0; i<spect_count; i++){
        spect.dft_frequency.at<double>(i) = 255*spect.dft_frequency.at<double>(i)*spect.dft_frequency.at<double>(i)/spect.max_magnitude;
      }

      img_in.convertTo(img_in, CV_8UC1); // (MATRIX, TARGET DATA TYPE)
      cv::applyColorMap(img_in, img_color, 9); // (ORIGINAL MATRIX, COLORED MATRIX, OPENCV COLOR OPTION)
      cv::namedWindow("Spectrograph", cv::WINDOW_NORMAL);
      cv::resizeWindow("Spectrograph", img_color.size().width, img_color.size().height);
      cv::imshow("Spectrograph", img_color);
      cv::imwrite("spectrograph.png", img_color);
      cv::waitKey(0);
}

void AudioView::calculateSpectrograph(int windowSize){
    int N = 512*2; // sample freq
    int segment_overlap = 192;
    std::vector<double> y = waveform.amplitude;
    std::vector<double> windowSamples;

    spect.transform_count = y.size()/(windowSize - segment_overlap);
    spect.dft_frequency = cv::Mat(pixel_scale_freq*N/2, pixel_scale_time*spect.transform_count, CV_64FC1, 0.0); // scale matrix by pixel_scale for better visibility of freq bins

    // iterators:
    int t_idx = 0;
    int w_idx = 0;

    /* pad end of y samples set so last window is not undersized (possible change: padd windowSize/2 before y and windowSize/2 after y) */
    while(w_idx<windowSize){ // only padding the end
      y.push_back(0);
      w_idx++;
    }

    w_idx = 0; // reset

    while(t_idx<spect.transform_count){ // iterate through data set performing DFT's
      spect.data.push_back(TransformData()); // push new TransformData struct to spect.data

      for(int i = 0; i<windowSize; i++){ // isolate data for transform
        windowSamples.push_back( y.at(w_idx) );
        w_idx++;
      }

      w_idx -= segment_overlap; // moves idx back to start of overlap for next segment

      spect.data[t_idx].amplitude = windowSamples; // store current transform window's samples to spect amplitude data at element transform index [t_idx]
      hanningWindow(windowSamples);
      zeroPadding(windowSamples,N);
      DFT(windowSamples,t_idx); // the DFT function will store resulting DFT magnitudes to corresponding data index for magnitude member

      windowSamples.clear(); // reset vector for next iteration

      t_idx++;
    } // end of transform count loop
}

void AudioView::calculateSpectrograph(int windowSize, std::vector<double> mixedAmplitude){
    int N = 512*2; // sample freq
    int segment_overlap = 192;
    std::vector<double> y = mixedAmplitude;
    std::vector<double> windowSamples;

    spect.transform_count = y.size()/(windowSize - segment_overlap);
    spect.dft_frequency = cv::Mat(pixel_scale_freq*N/2, pixel_scale_time*spect.transform_count, CV_64FC1, 0.0); // scale matrix by pixel_scale for better visibility of freq bins

    // iterators:
    int t_idx = 0;
    int w_idx = 0;

    /* pad end of y samples set so last window is not undersized (possible change: padd windowSize/2 before y and windowSize/2 after y) */
    while(w_idx<windowSize){ // only padding the end
      y.push_back(0);
      w_idx++;
    }

    w_idx = 0; // reset

    while(t_idx<spect.transform_count){ // iterate through data set performing DFT's
      spect.data.push_back(TransformData()); // push new TransformData struct to spect.data

      for(int i = 0; i<windowSize; i++){ // isolate data for transform
        windowSamples.push_back( y.at(w_idx) );
        w_idx++;
      }

      w_idx -= segment_overlap; // moves idx back to start of overlap for next segment

      spect.data[t_idx].amplitude = windowSamples; // store current transform window's samples to spect amplitude data at element transform index [t_idx]
      hanningWindow(windowSamples);
      zeroPadding(windowSamples,N);
      DFT(windowSamples,t_idx); // the DFT function will store resulting DFT magnitudes to corresponding data index for magnitude member

      windowSamples.clear(); // reset vector for next iteration

      t_idx++;
    } // end of transform count loop
}

void AudioView::displaySpectrograph(int windowSize){
    calculateSpectrograph(windowSize);
    colorize();
}

void AudioView::saveWaveform(){}

void AudioView::saveSpectrograph(std::string filename){
  std::string fmt = ".png";
  std::string outName = filename+fmt;
  cv::Mat img_color;
  cv::Mat img_in = spect.dft_frequency;

  img_in.convertTo(img_in, CV_8UC1); // (MATRIX, TARGET DATA TYPE)
  cv::applyColorMap(img_in, img_color, 9); // (ORIGINAL MATRIX, COLORED MATRIX, OPENCV COLOR OPTION)
  cv::imwrite(outName, img_color);
}

void AudioView::animateSpectrograph(std::string filename){
  std::string fmt = ".avi";
  std::string outName = filename+fmt;
  cv::Mat img_color;
  cv::Mat img_in = spect.dft_frequency;
  cv::Mat frame;

  int frameHeight = 512;
  int stride = frameHeight/16*9; // USE THIS FOR SLOWER VIEW
  stride = frameHeight*1280/720; // THIS FRAME SIZE WORKS WELL WITH SEEING BIG PICTURE AT REAL SPEED

  frame = cv::Mat(frameHeight, stride, CV_8UC3, 0.0);

  img_in.convertTo(img_in, CV_8UC3); // (MATRIX, TARGET DATA TYPE)
  cv::applyColorMap(img_in, img_color, 9); // (ORIGINAL MATRIX, COLORED MATRIX, OPENCV COLOR OPTION)

  int rows = spect.dft_frequency.size().height;
  int cols = spect.dft_frequency.size().width;

  cv::VideoWriter video;
  int codec = cv::VideoWriter::fourcc('M','J','P','G');
  double fps = 30/5;

  std::cout << "Opening video writer" << std::endl;

  video.open(outName, codec, fps, frame.size(), true);

  int p = 0;
  int p2 = stride;
  int pct;
  cv::Mat cropped;

  std::cout << "Started writing: " << outName << std::endl;
  // cv::Rect = cropRegion(0,0,stride,frameHeight);
  while(p2 < cols){
    // pHalf = (p2-p)/2;
    // std::cout << p << ": " <<  animated.size() << " " << rows << " " << (p2-1-p) << std::endl;
    // for(int r=0;r<rows;r++){
    //   for(int c=p;c<p2;c++){
    //     animated.at<double>(r,c-p) = img_color.at<double>(r,c);
    //     // std::cout << r << " " << c-p << std::endl;
    //     // if(c<(p+50) && c>=(p)) animated.at<double>(r,c) = 0;
    //     // std::cout << c << ", " << p << std::endl;
    //   }
    //   // std::cout << r << std::endl;
    // }

    // pct = 100*p2/cols;
    // if(pct%10 == 0)
    // std::cout << outName <<  << " % complete..." << std::endl;

    cropped = img_color(cv::Rect(p,0,stride,frameHeight));
    // std::cout << 0 << " " << p2-p << " | " << p << " " << p2 << " " << cols << std::endl;
    video.write(cropped);
    p+=20;
    p2+=20;
  }
  // video.release();
  return;

}

void AudioView::resetSpectrograph(){
  cv::Mat resetMat;

  spect.dft_frequency = resetMat;
}

std::vector<std::vector<float>> AudioView::getSpectralData(){
  std::vector<std::vector<float>> data;
  int rows = spect.dft_frequency.size().height;
  int cols = spect.dft_frequency.size().width;
  data.resize(rows, std::vector<float>(cols,0));

  for(int r=0;r<rows;r++){
    for(int c=0;c<cols;c++){
      data[r][c] = spect.dft_frequency.at<double>(r,c);
    }
  }
  return data;
}
