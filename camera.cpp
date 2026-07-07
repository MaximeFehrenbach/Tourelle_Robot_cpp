#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
using namespace cv;

int main() {
    cout << "init cam" << endl;
    VideoCapture cap(0, CAP_V4L2);

    if (!cap.isOpened()) {
        cerr << "pas de caméra" << endl;
        return -1;
    }

    cout << "Caméra détectée" << endl; 
    Mat frame;

    // on prend 10 photos pour que le capteur s'habitue à la lumière
    for(int i = 0; i < 10; i++) {
        cap >> frame;
        this_thread::sleep_for(chrono::milliseconds(50));
    }
    cap >> frame;

    if (frame.empty()) {
        cerr << "pas d'image finale" << endl;
        return -1;
    }

    string filename = "photo.jpg";
    imwrite(filename, frame);

    cout << "Image sauvegardée" << endl;
    cap.release();

    return 0;
}