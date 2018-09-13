# Specter

## What is it?
Specter is a live audio spectrogram. It takes audio from an audio device and 
produces a real-time image of the frequencies contained therein. Specter is 
highly configurable and allows users to choose the range of frequencies and 
volumes to be displayed, the windowing function, and the the audio input device 
(allowing recording from both both physical microphones and sound output 
monitor devices). Planned features include the ability to save the output image 
to disk and the ability to analyze pre-recorded audio files.

## Compiling
First, ensure you have Qt, and fftw isntalled. Then, run qmake, followed by 
make. Currently, Specter only officially supports Linux, but support for other 
platforms shouldn't require much modification to the code.

