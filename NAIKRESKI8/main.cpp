#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char **argv) {
    using namespace std;
    using namespace cv;

    //Tworzenie dwóch tablic lower i upper o rozmiarze 3, które określają dolną i górną granicę dla koloru.
    array<int, 3> lower = {135, 90, 0};
    array<int, 3> upper = {255, 255, 255};

    VideoCapture vc(0); //otwarcie kamery
    cv::Mat frame, dst, pierwszy;

    while (waitKey(10) != 27) {
        vc >> frame;
        //Przekształcenie koloru z BGR na HSV za pomocą funkcji cvtColor.
        cvtColor(frame, dst, COLOR_BGR2HSV);
        //Odfiltrowanie koloru za pomocą funkcji inRange.
        inRange(dst, lower, upper, pierwszy);

        imshow("pierwszy", pierwszy);
        imshow("frame", frame);

        vector<vector<Size>> contours;
        //Znalezienie konturów za pomocą funkcji findContours.
        findContours(pierwszy, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

        //jezeli 2 kontury to sortuj ich
        if (contours.size() >= 2) {

            //pobierz dwa największe.
            sort(contours.begin(), contours.end(), [](vector<Size> a, vector<Size> b) {
                return contourArea(a) > contourArea(b);
            });

            //Oblicz momenty każdego z konturów.
            auto big_con = moments(contours[0], false);
            auto big_con_sec = moments(contours[1], false);

            //Oblicz środek ciężkości dla każdego konturu i przypisz do zmiennych
            int x1 = (int) (big_con.m10 / big_con.m00);
            int y1 = (int) (big_con.m01 / big_con.m00);

            int x2 = (int) (big_con_sec.m10 / big_con_sec.m00);
            int y2 = (int) (big_con_sec.m01 / big_con_sec.m00);

            imshow("frame", frame);

            double distance = x2 - x1;

            double max_width = 5;
            double min_width = 2;
            double width = min_width + (max_width - min_width) * (1 / (1 + exp(-0.1 * (distance - 150))));

            //Jeśli różnica między środkami ciężkości jest mniejsza niż 4, rysuj trzy linie pomiędzy tymi punktami na oryginalnym obrazie.
            if ((y1 - y2 > -4) && (y1 - y2 < 4) ) {
                line(frame, Point (x1, y1 + 10),
                     Point (x2, y2 + 10), {0,255,0}, width);
                line(frame, Point (x1, y1 - 10),
                     Point (x2, y2 - 10), {0,255,0}, width);
                line(frame, Point (x1, y1 - 0),
                     Point (x2, y2 - 0), {0,255,0}, width);

                imshow("frame", frame);
            }
        }
    }
    return 0;
}
