#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
using namespace cv;
using namespace cv::dnn;

int main() {
    cout << "test detection visage" << endl;

    string model_name = "yolo11n.onnx";
    Net net = readNetFromONNX(model_name);
    if (net.empty()) {
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

    // préparation de l'image pour YOLO
    Mat blob = blobFromImage(frame, 1.0 / 255.0, Size(640, 640), Scalar(), true, false);
    net.setInput(blob);

    Mat output = net.forward(); // Utilisation de l'IA

    // format de sortie [1, 84, 8400] (8400 boîtes, 80 classes)
    Mat detectionMat(output.size[1], output.size[2], CV_32F, output.ptr<float>());
    detectionMat = detectionMat.t();

    // Facteurs pour remettre les coordonnées à l'échelle
    float x_factor = frame.cols / 640.0;
    float y_factor = frame.rows / 640.0;

    vector<int> classIds;
    vector<float> confidences;
    vector<Rect> boxes;

    for (int i = 0; i < detectionMat.rows; ++i) {
        float* data = (float*)detectionMat.row(i).data;
        Mat scores(1, 80, CV_32F, data + 4); // Les scores commencent après X,Y,W,H
        
        Point classIdPoint;
        double confidence;
        minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);

        // confiance sup à 50% ET une personne est détéctée
        if (confidence > 0.50 && classIdPoint.x == 0) {
            float cx = data[0];
            float cy = data[1];
            float w = data[2];
            float h = data[3];

            // Calcul du rectangle
            int left = int((cx - 0.5 * w) * x_factor);
            int top = int((cy - 0.5 * h) * y_factor);
            int width = int(w * x_factor);
            int height = int(h * y_factor);

            classIds.push_back(classIdPoint.x);
            confidences.push_back((float)confidence);
            boxes.push_back(Rect(left, top, width, height));
        }
    }

    // éliminer les boîtes en double
    vector<int> indices;
    NMSBoxes(boxes, confidences, 0.5, 0.4, indices);

    // centre de la photo
    int centre_image_x = frame.cols / 2;
    line(frame, Point(centre_image_x, 0), Point(centre_image_x, frame.rows), Scalar(255, 0, 0), 2);

    if (indices.empty()) {
        cout << "Aucune personne détectée" << endl;
    } else {
        //première personne détectée
        int idx = indices[0];
        Rect box = boxes[idx];
        int centre_personne_x = box.x + box.width / 2;
        int centre_personne_y = box.y + box.height / 2;

        // Affichage
        rectangle(frame, box, Scalar(0, 255, 0), 2);
        drawMarker(frame, Point(centre_personne_x, centre_personne_y), Scalar(0, 0, 255), MARKER_CROSS, 20, 2);

        // différence entre le centre de la personne et le centre de l'image
        int erreur = centre_personne_x - centre_image_x;
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