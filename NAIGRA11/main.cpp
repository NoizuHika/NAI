#include <chrono>
#include <list>
#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <random>
#include <tuple>

std::random_device rd;
std::mt19937 generator(rd());


// wyklad:
// Poza: https://docs.opencv.org/4.x/d5/d1f/calib3d_solvePnP.html
// O detektorze https://docs.opencv.org/3.4/d5/dae/tutorial_aruco_detection.html


// g++ `pkg-config --cflags opencv4` detector.cpp -o detector `pkg-config --libs opencv4`
// https://docs.opencv.org/4.x/d5/dae/tutorial_aruco_detection.html

std::pair<cv::Mat, cv::Mat> get_camera_properties_ps3eye()
{
    // coefficients for approximated PS3 Eye camera
    //! Camera matrix
    cv::Mat camera_matrix = (cv::Mat_<double>(3, 3) << 600, 0, 320,
            0, 600, 240,
            0, 0, 1);
    //! Distortion coefficients
    cv::Mat dist_coeffs = (cv::Mat_<double>(5, 1) << 0, 0, 0, 0, 0);

    return {camera_matrix, dist_coeffs};
}

cv::Mat get_object_points()
{
    const double MARKER_MM = 30;
    cv::Mat obj_points(4, 1, CV_32FC3);
    obj_points.ptr<cv::Vec3f>(0)[0] = cv::Vec3f(-MARKER_MM / 2.f, MARKER_MM / 2.f, 0);
    obj_points.ptr<cv::Vec3f>(0)[1] = cv::Vec3f(MARKER_MM / 2.f, MARKER_MM / 2.f, 0);
    obj_points.ptr<cv::Vec3f>(0)[2] = cv::Vec3f(MARKER_MM / 2.f, -MARKER_MM / 2.f, 0);
    obj_points.ptr<cv::Vec3f>(0)[3] = cv::Vec3f(-MARKER_MM / 2.f, -MARKER_MM / 2.f, 0);
    return obj_points;
}

int main()
{
    using namespace cv;
    using namespace std;


    list<pair<std::chrono::steady_clock::time_point, vector<Point2f>>> detectedPositions;

    VideoCapture cam(0);
    Ptr<aruco::DetectorParameters> parameters = aruco::DetectorParameters::create();
    Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
    namedWindow("markers", WINDOW_NORMAL);

    auto [camera_matrix, dist_coeffs] = get_camera_properties_ps3eye();
    auto obj_points = get_object_points();
    auto obj_points1 = get_object_points();

    Mat car_image = imread("car0.png");
    Mat car_image2 = imread("car1.png");
    Mat goal_image = imread("obstacle.png");
    Vec2f car_position(200, 200);
    Vec2f car_position1(300, 300);

    vector<Vec2f> goals;
    int counter = 0;
    int points = 0;
    int points1 = 0;
    while (waitKey(10) != 27) {
        Mat inputImage;
        Mat detected;
        cam >> inputImage;
        //resize(inputImage, inputImage, Size(320, 240), 0, 0, INTER_CUBIC);
        detected = inputImage.clone();

        if ((counter++ % 20 == 0) && (goals.size() < 10)) {
            std::uniform_int_distribution<int> distr_x(0 + goal_image.cols, detected.cols - goal_image.cols);
            std::uniform_int_distribution<int> distr_y(0 + goal_image.rows, detected.rows - goal_image.rows);
            Vec2f p;
            p.val[0] = distr_x(generator);
            p.val[1] = distr_y(generator);
            goals.push_back(p);
        }

        vector<int> markerIds;
        vector<vector<Point2f>> markerCorners, rejectedCandidates;
        aruco::detectMarkers(inputImage, dictionary, markerCorners,
                             markerIds, parameters, rejectedCandidates);
        Mat outputImage = inputImage.clone();
        aruco::drawDetectedMarkers(detected, markerCorners, markerIds);

        auto found = find(markerIds.begin(), markerIds.end(), 19);
        if (found != markerIds.end()) {
            int foundIdx = distance(markerIds.begin(), found);
            auto found = markerCorners.at(foundIdx);

            cv::Vec3d rvecs, tvecs;
            cv::solvePnP(obj_points, found, camera_matrix, dist_coeffs, rvecs, tvecs);
            cv::drawFrameAxes(detected, camera_matrix, dist_coeffs, rvecs, tvecs, 10.1);

            Mat rot_mat;
            Rodrigues(rvecs, rot_mat);
            double angle_z = atan2(rot_mat.at<double>(1, 0), rot_mat.at<double>(0, 0));
            double angle_x = atan2(rot_mat.at<double>(2, 1), rot_mat.at<double>(2, 2));
            angle_x = -angle_x + ((angle_x < 0) ? (-M_PI) : M_PI);
            double angle_y = -asin(rot_mat.at<double>(2, 0));

            //std::cout << angle_x << " " << angle_y << " " << angle_z << std::endl;
            auto new_pos = car_position;
            new_pos.val[0] += 10.0 * angle_y;
            new_pos.val[1] -= 10.0 * angle_x;
            auto new_pos_scr = new_pos;
            new_pos_scr.val[0] -= car_image.cols / 2;
            new_pos_scr.val[1] -= car_image.rows / 2;
            if ((new_pos_scr.val[0] > 0) && (new_pos_scr.val[0] < (detected.cols - car_image.cols - 1)) && (new_pos_scr.val[1] > 0) && (new_pos_scr.val[1] < (detected.rows - car_image.rows - 1))) {
                car_position = new_pos;
            }
        }
        auto found1 = find(markerIds.begin(), markerIds.end(), 1); //2
        if (found1 != markerIds.end()) {
            int foundIdx1 = distance(markerIds.begin(), found1);
            auto found1 = markerCorners.at(foundIdx1);

            cv::Vec3d rvecs, tvecs;
            cv::solvePnP(obj_points1, found1, camera_matrix, dist_coeffs, rvecs, tvecs);
            cv::drawFrameAxes(detected, camera_matrix, dist_coeffs, rvecs, tvecs, 10.1);

            Mat rot_mat;
            Rodrigues(rvecs, rot_mat);
            double angle_z = atan2(rot_mat.at<double>(1, 0), rot_mat.at<double>(0, 0));
            double angle_x = atan2(rot_mat.at<double>(2, 1), rot_mat.at<double>(2, 2));
            angle_x = -angle_x + ((angle_x < 0) ? (-M_PI) : M_PI);
            double angle_y = -asin(rot_mat.at<double>(2, 0));

            //std::cout << angle_x << " " << angle_y << " " << angle_z << std::endl;
            auto new_pos = car_position1;
            new_pos.val[0] += 10.0 * angle_y;
            new_pos.val[1] -= 10.0 * angle_x;
            auto new_pos_scr = new_pos;
            new_pos_scr.val[0] -= car_image2.cols / 2;
            new_pos_scr.val[1] -= car_image2.rows / 2;
            if ((new_pos_scr.val[0] > 0) && (new_pos_scr.val[0] < (detected.cols - car_image2.cols - 1)) && (new_pos_scr.val[1] > 0) && (new_pos_scr.val[1] < (detected.rows - car_image2.rows - 1))) {
                car_position1 = new_pos;
            }
        }
        Mat insetImage(detected,
                       Rect(
                               car_position.val[0] - (car_image.cols / 2),
                               car_position.val[1] - (car_image.rows / 2),
                               car_image.rows, car_image.cols));
        car_image.copyTo(insetImage);
        Mat insetImage1(detected,
                       Rect(
                               car_position1.val[0] - (car_image2.cols / 2),
                               car_position1.val[1] - (car_image2.rows / 2),
                               car_image2.rows, car_image2.cols));
        car_image2.copyTo(insetImage1);
        for (auto goal : goals) {
            Mat insetImage(detected,
                           Rect(
                                   goal.val[0] - (goal_image.cols / 2),
                                   goal.val[1] - (goal_image.rows / 2),
                                   goal_image.rows, goal_image.cols));
            goal_image.copyTo(insetImage);
        }
        for (auto goal : goals) {
            Mat insetImage1(detected,
                           Rect(
                                   goal.val[0] - (goal_image.cols / 2),
                                   goal.val[1] - (goal_image.rows / 2),
                                   goal_image.rows, goal_image.cols));
            goal_image.copyTo(insetImage1);
        }
        for (int i = 0; i < goals.size(); i++) {
            if (cv::norm(goals[i] - car_position) < (goal_image.cols / 2)) {
                points++;
                std::cout << points << std::endl;
                goals.erase(goals.begin() + i);
                i--;
            }
        }
        for (int i = 0; i < goals.size(); i++) {
            if (cv::norm(goals[i] - car_position1) < (goal_image.cols / 2)) {
                points1++;
                std::cout << points1 << std::endl;
                goals.erase(goals.begin() + i);
                i--;
            }
        }
        putText(detected, "Score: " + to_string(points), Point(10, 50), FONT_HERSHEY_SIMPLEX, 2, Scalar(255, 0, 0), 2);
        putText(detected, "Score: " + to_string(points1), Point(10, 475), FONT_HERSHEY_SIMPLEX, 2, Scalar(255, 0, 0), 2);
        imshow("markers", detected);
    }

    if (points > points1) {
        std::cout << "Player 1 wins!" << std::endl;
    } else if (points1 > points) {
        std::cout << "Player 2 wins!" << std::endl;
    } else {
        std::cout << "It's a tie!" << std::endl;
    }

    return 0;
}



