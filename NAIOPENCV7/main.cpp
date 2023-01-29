#include <opencv2/opencv.hpp>
using namespace cv;

int main()
{
    VideoCapture cam(0); // Open the default camera
    namedWindow("Mirror Image", WINDOW_NORMAL);

    while(true)
    {
        Mat frame;
        cam >> frame; // Capture a new frame from the camera
        flip(frame, frame, 1); // Flip the image horizontally to create a mirror image
        imshow("Mirror Image", frame); // Display the mirror image

        if(waitKey(1) == 27) // Exit the loop if the 'ESC' key is pressed
        {
            break;
        }
    }

    return 0;
}
