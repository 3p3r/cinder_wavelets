# Cinder Wavelets

This is the repository of a real-time [Wavelet](https://en.wikipedia.org/wiki/Wavelet) decomposition plotter, implemented using [Cinder](https://libcinder.org/) and C++11. It is a personal project I have developed for my senior year at Portland State University, studying Signal Processing.

![Cinder Wavelets UI](assets/screenshot.png?raw=true)

## Build instructions

This project depends on [*wavelib*](https://github.com/rafat/wavelib) which is checked in as a git submodule. *Wavelib* will be built along-side the project itself. To build the project, download [**Cinder 0.9.0**](https://libcinder.org/static/releases/cinder_0.9.0_vc2013.zip)  and define `CINDER_090` environment variable to point to its root directory. Open up `CinderWavelets.sln` and build.
