### Project Overview

The Musical Speech Isolation and Conversion Automation (Mu.S.I.C.A) Project spanned the winter and spring semesters of 2021. The Mu.S.I.C.A Project was a self-driven project to gain experience with C++ programming, explore interests in machine learning, and develop applied engineering skills. The objective of this project is to design a program that analyses musical audio and extracts frequency data in order to develop a dataset for vocal isolation. 

This project completes three primary tasks: <br/>
1. Read (write, and play) music .wav files
2. Extract music frequency data and construct spectrograms
3. Create musical dataset for vocal and non vocal audio frequency data

Project Primary Dependencies: <br/>
- FFmpeg
- SDL
- MatplotLib (C++)
- OpenCV

### Project Motivation

This project was inspired by my interest in the process of learning spoken language, the topic of translation technology, and my desire to improve my engineering and programming skills. Furthermore, this project expands upon work done for a Complex Leadership Challenges course where I researched and proposed a solution for a robust translation technology program. The goal of this project is to explore the broad topic of automatic speech recognition and get exposure to topics such as computational linguistics, feature extraction, ant machine learning. 

In addition to multiple learning goals for technical skills, my goals for this experience also included embracing active learning, pushing my comfort zone, and developing strong reflection habits.

### Project By-The-Numbers
**Phase 0: Winter C++ Bootcamp:** 25+ hours <br/>
**Phase I: Testing and Visualization**:88 hours <br/>
**Phase II: Vocal-Isolation Dataset**:60 hours <br/>
**Concurrent Data Structures Course**:5-6 hours a week x 15 weeks <br/>


### Phase I
## Read, Write, Play Audio
![](audf.png) <br/>
[Figure 1: Sampled Audio Data](https://www.izotope.com/en/learn/digital-audio-basics-sample-rate-and-bit-depth.html)<br/>
In order to store audio (and ultimately read data), several parameters must be defined: 
<br/>
**Typical Sampling Rates**
- 11 kHz
- 22 kHz
- 44 kHz

**Typical Sample Data Types**
- uint8/16/32
- sint8/16/32

For this project, .wav file sampling rate was 44kHz and data type was uint16.

## Visualize Musical Data (Time Domain)
# Methodology
# Results

## Visualize Musical Data (Frequency Domain)
# Methodology
# Results
<img src="noHanns.png" height="150">
<img src="Hanns.png" height="150">


```c
/* Hanning Window Pseudocode */
double alpha = 0.5;
double pi = 3.1415926;
for(i<N){
  w.at(i) = alpha*(1-cos(2*pi*i/N)); // window function
  y.at(i) = y.at(i)*w.at(i);
}
```

### Phase II
## Creating the Dataset
