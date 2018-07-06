# fps_test
C++ script to view CSI Camera Video at Specified FPS from Nvidia Jetson Tx2

From https://gist.github.com/peter-moran/742998d893cd013edf6d0c86cc86ff7f

Changed to build with CMake and added fixed time boundary. 

```
git clone https://github.com/connorsoohoo/fps_test.git
cd fps_test
cmake .
make 
./fps_test <width> <height> <fps> <time_elapsed> <window_size> <display_video>
```
