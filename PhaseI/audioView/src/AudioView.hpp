/****************************************************************/
/*                     Audio Definition                     */
/****************************************************************/
/* LEAVE THIS FILE AS IS! DO NOT MODIFY ANYTHING! =]            */
/****************************************************************/

#pragma once
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

struct AudioData
{
    Uint8* pos;
    Uint32 length;
};

struct WaveformData{
    std::vector<double> sample;
    std::vector<double> time;
    std::vector<double> amplitude;
};

struct TransformData{
    std::vector<double> time;
    std::vector<double> amplitude;
    std::vector<double> frequency;
    std::vector<double> magnitude;
};

struct Spectrogram{
    int transform_count;
    int max_frequency;
    int max_magnitude = 0;
    std::vector<TransformData> data;
    cv::Mat dft_frequency;
};


class AudioView{
  private:
    std::vector<int16_t> audio_amplitude;
    std::string filename_in;
    int audio_length = 0;
    int spect_count = 0;
    int pixel_scale_time = 1; // stretch horizontal
    int pixel_scale_freq = 1;

    void DFT(std::vector<double> &y, int t_idx);
    void hanningWindow(std::vector<double> &y);
    void zeroPadding(std::vector<double> &y, int N);
    void colorize();

    WaveformData waveform;
    Spectrogram spect;

  public:
    /* Constructor for Audioview Class */
    AudioView();

    /* Destructor for Audioview Class */
    ~AudioView();

    /* Method to read audio wav file */
    void read(const char * filename_in);

    /* Method to write audio file to new file */
    void write(const char * filename_in, float damp_ratio);

    /* Method to play audio file */
    void play(const char * filename_in);

    /* Method to display audio in time domain */
    void plotWaveform();


    /* Method to calculate audio to spectrograph */
    void calculateSpectrograph(int windowSize);

    /* Overloaded Method to calculate audio to spectrograph given audio*/
    void calculateSpectrograph(int windowSize, std::vector<double> mixedAmplitude);

    /* Method to display audio as spectrograph */
    void displaySpectrograph(int windowSize);

    /* Method to save audio plot in time domain */
    void saveWaveform();

    /* Method to save audio spectrograph */
    void saveSpectrograph();

    /* Method to reset spectrograph */
    void resetSpectrograph();

    /* Method to reformat and access spectral data */
    std::vector<std::vector<float>> getSpectralData();
};
