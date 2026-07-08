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
    haarcascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0, Size(70, 70));

    int centre_image_x = frame.cols / 2;
    line(frame, Point(centre_image_x, 0), Point(centre_image_x, frame.rows), Scalar(255, 0, 0), 2);


    if (faces.size() == 0) {
        cout << "Aucun visage détecté" << endl;
    }
    else{
        // affichage
        int centre_visage_x = faces[0].x + faces[0].width / 2;
        int centre_visage_y = faces[0].y + faces[0].height / 2;
        rectangle(frame, faces[0], Scalar(0, 255, 0), 2);
        drawMarker(frame, Point(centre_visage_x, centre_visage_y), Scalar(0, 0, 255), MARKER_CROSS, 20, 2);

        cout << "Un visage détecté" << endl;

        // différence entre le centre du visage et le centre de l'image
        int erreur = centre_visage_x - centre_image_x;
        if (erreur > 40) {
            cout << "Tourne à droite" << endl;
        } else if (erreur < -40) {
            cout << "Tourner à gauche" << endl;
        } else {
            cout << "ne tourne pas" << endl;
        }
    }
    

    string filename = "photo.jpg";
    imwrite(filename, frame);

    cout << "Image sauvegardée" << endl;
    cap.release();

    return 0;
}