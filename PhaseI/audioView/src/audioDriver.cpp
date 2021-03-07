#include <iostream>

#include "../src/AudioView.hpp"

int main(int argc, const char * argv[]){
  const char * filename;
  if(argc==1)
	{
        std::cout << "Usage Instructions here:" << std::endl;
        std::cout << "Default audio used..." << std::endl;
        filename = "../src/res/audio/trombone.wav";
	}
  else
  {
        filename = argv[1];
  }

  AudioView AView;
  AView.read(filename);
  // AView.play(filename);
  // AView.write(filename,0.5); // half amplitude
  // AView.plotWaveform();
  AView.displaySpectrograph(812);

  return 0;
};
