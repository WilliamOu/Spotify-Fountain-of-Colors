# Spotify-Fountain-of-Colors

A small project that renders the Fountain of Colors skin for the Rainmeter app when using the Spotify app. I'm sharing it since I think it's a neat little program, but as it's intended for personal use, the program is quite barebones and will not do things like automatically rescale the Fountain of Colors Frequency Visualizer. I.e, if you would like to use this code, you'll have to manually adjust the bands and set the scale and position. 

# Installation
To turn this script into a proper executable, create a new Visual Studio project and make a new Desktop application using C++ (you will need the Desktop development with C++ workload). From the repo, use the RainmeterController.cpp script as the main program. From Solution Explorer -> Properties -> Linker -> System, set the system to Windows (/SUBSYSTEM:WINDOWS). Then just build the program and you should be good to go. It will open to the system tray, and you can close it by simply right-clicking the icon.

Note that I do not intend to maintain this repo--it's purely a for-fun project.
