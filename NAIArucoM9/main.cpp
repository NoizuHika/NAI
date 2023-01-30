#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>

using namespace cv;
using namespace aruco;
using namespace std;

const float MARKER_MM = 70;

vector<Point2f> fromPixelToMiliMeters(vector<Point2f> original){
    vector<Point2f> result;
    double distance = 0;
    for (int i = 0; i < 4;i++) {
        Point2f v = (original[i] - original[(i+1)%4]);
        distance += sqrt(v.x*v.x + v.y*v.y);
    }
    distance = (distance / 4);
    for (int i = 0; i < 4;i++) {
        result.emplace_back(((MARKER_MM*(original[i].x / distance))),(MARKER_MM*(original[i].y / distance)));
    }

    return result;
}

int main() {

    VideoCapture camera(0); // otwiera domyślną kamerę

    //tworzony jest słownik znaków ArUco (dictionary) i parametry detekcji (detectorParams).
    Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
    Ptr<aruco::DetectorParameters> detectorParams = aruco::DetectorParameters::create();

    Mat frame;
    while (true) {

        camera >> frame;  // frame jest pobierany z kamery jeśli nie jest pusty
        if (frame.empty())
            break;

        vector<int> ids;
        vector<vector<Point2f>> corners;
        //następuje detekcja znaków ArUco w obrazie (detectMarkers).
        aruco::detectMarkers(frame, dictionary, corners, ids, detectorParams);

        //Jeśli w detekcji znaleziono co najmniej 2 znaki ArUco, obliczana jest odległość między nimi i wyświetlana na obrazie.
        if (ids.size() >= 2) {

            // Współrzędne wszystkich rogów 1  markera
            auto firstMarker = corners.at(0);
            // Współrzędne wszystkich rogów 2 markera
            auto secondMarker = corners.at(1);

            auto secondMarkerFirstCorn = fromPixelToMiliMeters(secondMarker)[0];
            auto firstMarkerFirstCorn = fromPixelToMiliMeters(firstMarker)[0];

            double realDistance = sqrt(pow(secondMarkerFirstCorn.x - firstMarkerFirstCorn.x, 2) + pow(secondMarkerFirstCorn.y - firstMarkerFirstCorn.y, 2));
            string distance_str = "Distance: " + to_string(realDistance);
            putText(frame, distance_str, Point(20, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);

        }

        //rysowane są znalezione znaki ArUco na obrazie (drawDetectedMarkers) i wyświetlany jest obraz (imshow).
        aruco::drawDetectedMarkers(frame, corners, ids);
        imshow("Camera", frame);

        int key = waitKey(1);
        if (key == 27) //Esc
            break;
    }
    camera.release();
    destroyAllWindows();
    return 0;
}
