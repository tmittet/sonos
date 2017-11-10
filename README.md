# SonosUPnP

This library makes interfacing an Arduino with your Sonos system a breeze. The
library supports both controlling and reading the state of your Sonos components.
Playing URIs, files, online radio stations, playlists, and line-in is supported
as well as the most common commands like play, pause, skip, mute, volume and
speaker grouping. What sets this library apart from similar libraries written
for the Arduino platform it its ability to read the state of most of the Sonos
functions: getting source, player state, track number, track position, volume
and more. The library is relatively compact and has a small enough memory
footprint to run on the Arduino Uno and Duemilanove.

**Getting Started:** 
- Download and add this library to your Ardiono libraries folder.
- Download and add the MicroXPath library (github.com/tmittet/microxpath).

**Note:**  
If you download the libraries as ZIP files from GitHub, "-master" will be added
to the end of the file names. You need to rename the files, such that e.g.
"microxpath-master.zip" becomes "microxpath.zip", before adding the libraries
to your Ardiono libraries folder.

**Arduino Example Sketch:**  
In the Arduino example sketch I've implemented a simple serial protocol that
allows executing some of the Sonos commands that are supported by the library.
Here's a list of supported commands:

pl = Play  
pa = Pause  
st = Stop  
pr = Previous track  
nx = Next track  
fi = Play test file (file path must be changed for this to work)  
ht = Play http stream (you need access to the music service WIMP)  
ra = Play radio (works if your speakers are connected to the internet)  
li = Play line in (only works if the device has an AUX input)  
gr = Group speakers (Living Room, Bathroom and Bedroom)  
ug = Ungroups speakers  
re = Toggle repeat  
sh = Toggle shuffle  
lo = Toggle loudness  
mu = Toggle mute  
52 = Set volume level 52 (range is 00 - 99)  
b5 = Set bass level -5 (range is 0 to -9)  
B5 = Set bass level +5 (range is 0 to +9)  
t3 = Set treble level -3  
T0 = Set treble level normal  
