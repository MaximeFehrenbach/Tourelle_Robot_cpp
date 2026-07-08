#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
using namespace cv;

int main() {
    cout << "test detection visage" << endl;

    CascadeClassifier haarcascade;
    String model_name = "/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml";
    if (!haarcascade.load(model_name)) {
        cerr << "impossible de charger l'IA" << endl;
        return -1;
    }

    cout << "init cam" << endl;
    VideoCapture cap(0, CAP_V4L2);
    if (!cap.isOpened()) {
        cerr << "pas de caméra" << endl;
        return -1;
    }
    Mat frame;

    // on prend 10 photos pour que le capteur s'habitue à la lumière
    for(int i = 0; i < 20; i++) {
        cap >> frame;
        this_thread::sleep_for(chrono::milliseconds(50));
    }
    cap >> frame;
    if (frame.empty()) {
        cerr << "pas d'image finale" << endl;
        return -1;
    }

    // Détection du visage
    Mat frame_gray;
    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
    equalizeHist(frame_gray, frame_gray);
    vector<Rect> faces;
    haarcascade.detectMultiScale(frame_gray, faces, 1.1, 3, 0, Size(100, 100));

    if (faces.size() == 0) {
        cout << "Aucun visage détecté" << endl;
    }
    else{
        cout << "Un visage détecté" << endl;
        rectangle(frame, faces[0], Scalar(0, 255, 0), 2);
    }

    string filename = "photo.jpg";
    imwrite(filename, frame);

    cout << "Image sauvegardée" << endl;
    cap.release();

    return 0;
}