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

#include "opencv2/core.hpp"
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

void AudioView::read(const char * filename_in){
    FILE *pipe_in;
    char phrase_in[50];
    sprintf(phrase_in, "ffmpeg -i %s -f s16le -ac 2 -", filename_in);
    pipe_in  = popen(phrase_in, "r");
    int16_t sample;
    int count;
    while(1)
    {
        count = fread(&sample, 2, 1, pipe_in); // read one 2-byte sample
        audio_amplitude.push_back(sample);
        if (count != 1) break;
        audio_length++;
    }

    double amp_i;

    for (int i=0; i<audio_length; i++) {
        waveform.sample.push_back(i);
        amp_i = (double) audio_amplitude.at(i);
        waveform.amplitude.push_back(amp_i);
    }

    pclose(pipe_in);
};

void AudioView::write(const char * filename_in, float damp_ratio){
  FILE *pipe_out;
  pipe_out = popen("ffmpeg -y -f s16le -ar 44100 -ac 2 -i - out.wav", "w");
  int16_t sample;
  for(int i=0;i<audio_length;i++){
      sample = audio_amplitude.at(i)*damp_ratio; // use 1 for not change
      fwrite(&sample, 2, 1, pipe_out);
  }
  pclose(pipe_out);
}

void AudioView::play(const char * filename_in){
    SDL_Init(SDL_INIT_AUDIO);

    SDL_AudioSpec wavSpec, want;
    Uint8* wavStart;
    Uint32 wavLength;

    if (SDL_LoadWAV(filename_in, &wavSpec, &wavStart, &wavLength) == NULL) // Load wav file
    {
        // Error handling
        std::cerr << "Error: " << filename_in << "not found" << std::endl;
        return;
    }

    std::cout << "\n" << "Loading audio: " << filename_in << "\n" << std::endl;

    AudioData audio;
    audio.pos = wavStart;
    audio.length = wavLength;

    // Call the callback with wav spec
    wavSpec.callback = MyAudioCallback; // now SDL will use the MAC function to play audio
    wavSpec.userdata = &audio;

    SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL,
        0);
    // changed the last param from SDL_AUDIO_ALLOW_ANY_CHANGE to 0 bc the audio sound wasn't there
    // for similar reference

    if (device == 0)
    {
        // Error Handling
        std::cerr << "error: " << SDL_GetError()
            << "could not be loaded as audio file" << std::endl;
        return;
    }

    /* Play audio */
    SDL_PauseAudioDevice(device, 0); // zero to pause
    while (audio.length > 0) SDL_Delay(100); // affects how long to delay this line...

    SDL_CloseAudioDevice(device);
    SDL_FreeWAV(wavStart);
    SDL_Quit();

    std::cout << "--------- SDL Quit ---------" << '\n';


}

void AudioView::plotWaveform(){
    // potentiallly add option for sample plot or time plot?

    double limSz = 100000; // added to shorten limits
    std::vector<double> xt(limSz), yt(limSz); // had problems when assigning to Sint16

    plt::title("Audio waveform ");
    plt::xlim(waveform.sample.front()-limSz, limSz + waveform.sample.front());
    plt::ylim(*min_element(waveform.amplitude.begin(),waveform.amplitude.end()), *max_element(waveform.amplitude.begin(),waveform.amplitude.end()));
    plt::axis("equal");

    plt::named_plot("Full Waveform", waveform.sample, waveform.amplitude); // full audio

    plt::Plot plot("Segmented Waveform"); // audio segment of size limSz
    plt::legend();

    int win = (int) limSz;
    double xlow, xupp;

    for (int i=0; i<audio_length; i++) {
      xt.push_back(waveform.sample.at(i));
      yt.push_back(waveform.amplitude.at(i)/2); // divides by 2 for half amplitude
      if (i % win == 0) { // update limits
          xlow = double (i-2*win);
          xupp = double (i+win);
          plt::xlim(xlow,xupp);
          // Just update data for this plot.
          plot.update(xt, yt);

          // Small pause so the viewer has a chance to enjoy the animation.
          plt::pause(0.1);
          xt.clear();
          yt.clear();
      }
      if (i == audio_length-1){
          xlow = double (i-2*win);
          xupp = double (i+win);
          plt::xlim(xlow,xupp);
          // Just update data for this plot.
          plot.update(xt, yt);
          plt::pause(1);
      }
    }
}

void AudioView::DFT(std::vector<double> &y, int t_idx){
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
      // if((fd.at<double>(i*2))/(N/2) > 2){ // for checking each DFT result
      //   std::cout << "FREQ BINS: " << f_i << ", ";
      //   std::cout << fd.at<double>(i*2)/(N/2) << std::endl;
      // }

    // freq.push_back(f_i); // SKIP UNLESS TAKING FREQ VECTOR INPUT
  }
  int wsz = spect.data[t_idx].magnitude.size();
  for (int i = 0;i<wsz;i++){ // insert magnitudes as column for each transform count
    spect.dft_frequency.at<double>(wsz-i-1,t_idx) = spect.data[t_idx].magnitude.at(i);
    spect_count++;
    if(spect.data[t_idx].magnitude.at(i) > spect.max_magnitude) spect.max_magnitude = spect.data[t_idx].magnitude.at(i); // track maximum magnitude

  //   for (int j = 0;j<4; j++){
  //     spect.dft_frequency.at<double>(4*(wsz-i)+0,4*t_idx+j) = 255 - 250*spect.data[t_idx].magnitude.at(i);
  //     spect.dft_frequency.at<double>(4*(wsz-i)+1,4*t_idx+j) = 255 - 250*spect.data[t_idx].magnitude.at(i);
  //     spect.dft_frequency.at<double>(4*(wsz-i)+2,4*t_idx+j) = 255 - 250*spect.data[t_idx].magnitude.at(i);
  //     spect.dft_frequency.at<double>(4*(wsz-i)+3,4*t_idx+j) = 255 - 250*spect.data[t_idx].magnitude.at(i);
  //   }
  }

  // plt::figure_size(600, 390);
  // plt::title("Test Signal Frequency Domain");
  // plt::xlim(0,wsz);
  // plt::plot(spect.data[t_idx].magnitude);
  // plt::xlabel("Frequency in Hz");
  // plt::ylabel("Magnitude");
  // plt::show();
  // plt::pause(1);
  // plt::close();

  // cv::hconcat(spect.dft_frequency, fd, spect.dft_frequency);
  // std::cout << t_idx << std::endl;

}

void AudioView::hanningWindow(std::vector<double> &y){
    // apply hanning window to time domain amplitude data
}

void AudioView::zeroPadding(std::vector<double> &y, int N){
    // apply zero padding to time domain amplitude data
    int Z = N - y.size();
    for(int i = 0; i<Z; i++){
      y.push_back(0);
    }
}

void AudioView::colorize(){
      std::cout << "Next Step: colorize to put data in format for opencv COLORMAP: " << std::endl;
      std::cout << spect_count << std::endl;

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
      std::cout << spect.max_magnitude << std::endl;

      for(int i = 0; i<spect_count; i++){
        spect.dft_frequency.at<double>(i) = 255*spect.dft_frequency.at<double>(i)*spect.dft_frequency.at<double>(i)/spect.max_magnitude;
      }

      cv::Mat img_color;
      cv::Mat img_in = spect.dft_frequency;


      img_in.convertTo(img_in, CV_8UC1);// replaced from CV_8UC3
      cv::applyColorMap(img_in, img_color, 9);
      cv::namedWindow("Spectrograph", cv::WINDOW_NORMAL);
      cv::resizeWindow("Spectrograph", img_color.size().width, img_color.size().height);
      cv::imshow("Spectrograph", img_color);
      cv::waitKey(0);
}

void AudioView::displaySpectrograph(int windowSize){
    //
    int N = 8192/4; // sample freq
    int segment_overlap = 192;
    std::vector<double> y = waveform.amplitude;
    std::cout << windowSize << "|" << y.size() << std::endl;
    spect.transform_count = y.size()/(windowSize - segment_overlap);
    std::vector<double> windowSamples;
    spect.dft_frequency = cv::Mat(N/2, spect.transform_count, CV_64FC1, 0.0); // replaced from CV_64FC3

    std::cout << "RIP" << std::endl;

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

    w_idx = 0;

    while(t_idx<spect.transform_count){ // iterate through data set performind DFT's
      spect.data.push_back(TransformData()); // push new TransformData struct to spect.data

      // std::cout << w_idx;
      for(int i = 0; i<windowSize; i++){ // isolate data for transform
        windowSamples.push_back( y.at(w_idx) );
        w_idx++;
      }
      // std::cout << ", " << w_idx << std::endl;
      w_idx -= segment_overlap; // moves idx back to start of overlap for next segment
      // std::cout << "------------- Transform Count: " << t_idx << std::endl;
      spect.data[t_idx].amplitude = windowSamples; // store current transform window's samples to spect amplitude data at element transform index (t_idx)
      hanningWindow(windowSamples);
      // std::cout << windowSamples.size() << " - ";
      zeroPadding(windowSamples,N);
      // std::cout << windowSamples.size() << std::endl;
      DFT(windowSamples,t_idx); // the DFT function will store resulting DFT magnitudes to corresponding data index for magnitude member

      windowSamples.clear(); // reset vector for next iteration
      // std::cout << t_idx << std::endl;
      t_idx++;
    } // end of transform count loop

    colorize();
}

void AudioView::saveWaveform(){}
void AudioView::saveSpectrograph(){}
