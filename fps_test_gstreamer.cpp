/*
  Example code for displaying (and finding FPS of) gstreamer video in OpenCV.
  Created by Peter Moran on 7/29/17.
  https://gist.github.com/peter-moran/742998d893cd013edf6d0c86cc86ff7f

  Changed to build with cmake and added fixed time boundary. 

  Usage
  -------
  After compiling, run this program with the following arguments. All are optional, but must be used cumulatively.
  `./fps_test <width> <height> <fps> <time_elapsed> <window_size> <display_video>`
  For example, to display 1080p video at 30 fps and calculate the true fps over a 15 sample running window for 10 total seconds, run:
  `./fps_test 640 480 30 10 15 0`
  Note
  -------
  FPS measurements are not fully accurate when displying the video.
*/

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <chrono>

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::duration<float> fsec;

std::string get_tegra_pipeline(int width, int height, int fps) {
    return "nvcamerasrc ! video/x-raw(memory:NVMM), width=(int)" + std::to_string(width) + ", height=(int)" +
           std::to_string(height) + ", format=(string)I420, framerate=(fraction)" + std::to_string(fps) +
           "/1 ! nvvidconv flip-method=2 ! video/x-raw, format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}

int main(int argc, char *argv[]) {
    // Options
    int WIDTH, HEIGHT, FPS, WINDOW_SIZE, DISPLAY_VIDEO;
    double TIME; 
    if (argc > 1) WIDTH = std::atoi(argv[1]); else WIDTH = 1920;
    if (argc > 2) HEIGHT = std::atoi(argv[2]); else HEIGHT = 1080;
    if (argc > 3) FPS = std::atoi(argv[3]); else FPS = 30;
    if (argc > 4) TIME = std::atof(argv[4]); else TIME = 5.0;
    if (argc > 5) WINDOW_SIZE = std::atoi(argv[4]); else WINDOW_SIZE = 100;
    if (argc > 6) DISPLAY_VIDEO = std::atoi(argv[5]); else DISPLAY_VIDEO = 0;
    std::cout << "Using parameters:\n\tWIDTH = " << WIDTH << "\n\tHEIGHT = " << HEIGHT
              << "\n\tFPS = " << FPS << "\n\tELAPSED TIME = " << std::atoi(argv[4]) <<
		"\n\tWINDOW_SIZE = " << WINDOW_SIZE << std::endl;

    // Sanity check version
    std::cout << "Running with OpenCV Version: " << CV_VERSION << "\n";

    // Define the gstream pipeline
    std::string pipeline = get_tegra_pipeline(WIDTH, HEIGHT, FPS);
    std::cout << "Using pipeline: \n\t" << pipeline << "\n";

    // Create OpenCV capture object, ensure it works.
    cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);
    if (!cap.isOpened()) {
        std::cout << "Connection failed";
        return -1;
    }

    // Time reading speed
    std::deque<double> frame_delas;
    cv::Mat frame;
    if (DISPLAY_VIDEO) cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);

    double totalTime = 0.0;

    while (totalTime < TIME) {
        auto start = Time::now();

        cap >> frame;  // Get a new frame from camera

        auto stop = Time::now();
        fsec duration = stop - start;
        double sec = duration.count();
	totalTime += sec;
        double fps = (1.0 / sec);
        if (frame_delas.size() >= WINDOW_SIZE) frame_delas.pop_front();
        frame_delas.push_back(fps);
        double avg_fps = accumulate(frame_delas.begin(), frame_delas.end(), 0.0) / frame_delas.size();
        std::cout << "Curr FPS: " << fps << "\tAvg FPS: " << avg_fps << std::endl;

        // Display frame
        if (DISPLAY_VIDEO) {
            imshow("Display window", frame);
            cv::waitKey(1); //needed to show frame
        }
    }
}
