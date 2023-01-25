#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

int main() {
    VideoCapture cap(0);

    Mat frame, hsv_frame, mask;

    while (true) {
        cap >> frame;

        cvtColor(frame, hsv_frame, COLOR_BGR2HSV);

        inRange(hsv_frame, Scalar(0, 100, 100), Scalar(10, 255, 255), mask);

        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        findContours(mask, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

        double max_area1 = 0;
        double max_area2 = 0;
        int max_index1 = 0;
        int max_index2 = 0;

        for (int i = 0; i < contours.size(); i++) {
            double area = contourArea(contours[i]);
            if (area > max_area1) {
                max_area2 = max_area1;
                max_area1 = area;
                max_index2 = max_index1;
                max_index1 = i;
            } else if (area > max_area2) {
                max_area2 = area;
                max_index2 = i;
            }
        }

        int margin = 10;
        int x1 = min(contours[max_index1][0].x, contours[max_index1][contours[max_index1].size() - 1].x);
        int x2 = min(contours[max_index2][0].x, contours[max_index2][contours[max_index2].size() - 1].x);
        int y1 = min(contours[max_index1][0].y, contours[max_index1][contours[max_index1].size() - 1].y);
        int y2 = min(contours[max_index2][0].y, contours[max_index2][contours[max_index2].size() - 1].y);
        if (abs(y1 - y2) < margin && x1 < x2) {

            drawContours(frame, contours, max_index1, Scalar(0, 0, 255), 2);
            drawContours(frame, contours, max_index2, Scalar(0, 0, 255), 2);

            double distance = x2 - x1;

            double max_width = 20;
            double min_width = 2;

            double width = min_width + (max_width - min_width) * (1 / (1 + exp(-0.1 * (distance - 150))));

            line(frame, Point(x1, y1), Point(x2, y1), Scalar(0, 255, 0), width);
            line(frame, Point(x1, y1 + margin), Point(x2, y1 + margin), Scalar(0, 255, 0), width);
            line(frame, Point(x1, y1 + (2 * margin)), Point(x2, y1 + (2 * margin)), Scalar(0, 255, 0), width);
        }

        imshow("frame", frame);

        if (waitKey(30) >= 0) break;
    }

    return 0;
}