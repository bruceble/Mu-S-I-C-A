#include <iostream>
#include <string>

#include "../src/AudioView.hpp"

const char * FILEPATH = "../src/res/audio/trombone.wav";

void displayOptions(bool wrongUsage){
  if(wrongUsage) std::cout << "Incorrect usage...\n";
  std::cout << "Usage Instructions here \n"
  "./AudioView -rpwts FILEPATH \n"
  "options:\n"
  "-r   read file\n"
  "-p   play file\n"
  "-w   write file\n"
  "-t   time plot of waveform\n"
  "-s   spectrogram" << std::endl;
}

int main(int argc, const char * argv[]){
  const char * filename;
  if(argc<2)
	{
        displayOptions(false);
        std::cout << "Default audio used  ..." << std::endl;
        filename = FILEPATH;

        std::cout << filename << std::endl;
        AudioView AView;
        AView.read(filename);
        // AView.play(filename);
        // AView.write(filename,0.5); // half amplitude
        // AView.plotWaveform();
        AView.displaySpectrograph(812);
	}
  else
  {
        if(argc==3){
          filename = argv[2];
        }
        else{
          filename = FILEPATH;
          std::cout << "Default audio used  ..." << std::endl;
        }

        std::cout << filename << std::endl;

        AudioView AView;

        std::string opt = argv[1];
        bool optTrigger = false;
        bool isRead = false;
        if (opt.find("r") != std::string::npos) {
          AView.read(filename);
          optTrigger = true;
          isRead = true;
        }
        if (opt.find("p") != std::string::npos) {
          AView.play(filename);
          optTrigger = true;
        }
        if (opt.find("w") != std::string::npos) {
          if(!isRead) AView.read(filename);
          AView.write(filename,0.5); // half amplitude
          optTrigger = true;
        }
        if (opt.find("t") != std::string::npos) {
          if(!isRead) AView.read(filename);
          AView.plotWaveform();
          optTrigger = true;
        }
        if (opt.find("s") != std::string::npos) {
          if(!isRead) AView.read(filename);
          AView.displaySpectrograph(812/2);
          optTrigger = true;
        }
        if(!optTrigger) displayOptions(true);

  }

  return 0;
};
