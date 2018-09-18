//
// Created by charris on 9/16/18.
//

#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>

using namespace cv;
using namespace std;

/// Global Variables
const int KERNEL_LENGTH = 7;
// HSV Ranges
const int low_H = 0, low_S = 220, low_V = 120;
const int high_H = 20, high_S = 255, high_V = 255;
const int morph_operator = 2;
const int morph_elem = 0;
const int morph_size = 1;



// Convert to string
#define SSTR( x ) static_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()

Mat orangeMask(Mat frame)
{
    Mat mask;
    cv::cvtColor(frame, mask, COLOR_BGR2HSV);

    cv::GaussianBlur(mask, mask, Size(KERNEL_LENGTH, KERNEL_LENGTH), 0, 0);
    inRange(mask, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), mask);


    Mat element = cv::getStructuringElement( morph_elem, Size( 7, 7 ), Point( morph_size, morph_size ) );

    cv::morphologyEx(mask, mask, 2, element);
    cv::morphologyEx(mask, mask, 1, element);


    return mask;

}

vector<Vec3f> detect_circles(Mat frame)
{
    cv::GaussianBlur(frame, frame, Size(7,7), 2, 2);

    vector<Vec3f> circles;
    cv::HoughCircles(frame, circles, CV_HOUGH_GRADIENT,1, 100, 20, 30, 10, 0);

    return circles;
}


 vector<Vec3f> detectBall(Mat frame)
{
    Mat circle_image;
}


int main(int argc, char **argv)
{
    namedWindow("Tracking", CV_WINDOW_AUTOSIZE);
    Mat track;
    Mat detect;
    Point circle_cp = Point(0,0);

    // List of tracker types in OpenCV 3.4.1
    string trackerTypes[8] = {"BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN", "MOSSE", "CSRT"};
    // vector <string> trackerTypes(types, std::end(types));

    // Create a tracker
    string trackerType = trackerTypes[2];

    Ptr<Tracker> tracker;

#if (CV_MINOR_VERSION < 3)
    {
        tracker = Tracker::create(trackerType);
    }
#else
    {
        if (trackerType == "BOOSTING")
            tracker = TrackerBoosting::create();
        if (trackerType == "MIL")
            tracker = TrackerMIL::create();
        if (trackerType == "KCF")
            tracker = TrackerKCF::create();
        if (trackerType == "TLD")
            tracker = TrackerTLD::create();
        if (trackerType == "MEDIANFLOW")
            tracker = TrackerMedianFlow::create();
        if (trackerType == "GOTURN")
            tracker = TrackerGOTURN::create();
        //if (trackerType == "MOSSE")
            //tracker = TrackerMOSSE::create();
        //if (trackerType == "CSRT")
            //tracker = TrackerCSRT::create();
    }
#endif
    // Read video
    VideoCapture video(0);



    // Exit if video is not opened
    if(!video.isOpened())
    {
        cout << "Video open error!" << endl;
        return 1;
    }

    // Read first frame
    Mat frame;
    bool ok = video.read(frame);
    bool init = false;

    Rect2d bbox;

    // Uncomment the line below to select a different bounding box
    // bbox = selectROI(frame, false);
    // Display bounding box.
    //cv::rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 );



    //imshow("Tracking", frame);
    //tracker->init(frame, bbox);

    while(video.read(frame)) {
        // Mask
        Mat mask = orangeMask(frame);
        vector<Vec3f> circles = detect_circles(mask);

        int largest_radius = 0;
        int largest_circle_index = 0;

        if (!circles.empty()) {
            // Draw the circles detected
            for (size_t i = 0; i < circles.size(); i++) {
                int radius = cvRound(circles[i][2]);
                if (radius > largest_radius) {
                    largest_radius = radius;
                    largest_circle_index = i;
                    int xp = circles[i][0];
                    int yp = circles[i][1];
                    Point center(cvRound(xp), cvRound(yp));
                    detect = frame;
                    circle_cp = Point(cvRound(xp), cvRound(yp));
                    circle(detect, center, 3, Scalar(0, 255, 0), -1, 8, 0);// circle center
                    circle(detect, center, radius, Scalar(0, 0, 255), 3, 8, 0);// circle outline
                    bbox = Rect2d(xp-radius, yp-radius, radius*2, radius*2);
                    if (init == false) {
                        tracker->init(frame, bbox);
                        init == true;
                    }
                    cout << "center : " << center << "\nradius : " << radius << endl;
                    cout << "bounding box :" << bbox << endl;
                }
            }
        }

        // Start timer
        double timer = (double) getTickCount();

        // Update the tracking result
        bool ok = tracker->update(frame, bbox);

        // Calculate Frames per second (FPS)
        float fps = getTickFrequency() / ((double) getTickCount() - timer);

        if (ok) {
            // Tracking success : Draw the tracked object
            cv::rectangle(frame, bbox, Scalar(255, 0, 0), 2, 1);
            Point center = Point(bbox.x, bbox.y);
            cv::circle(frame, center, 3, Scalar(0, 255, 0), -1, 8, 0);// circle center
            circle_cp.x = bbox.x;
            circle_cp.y = bbox.y;
        } else {
            // Tracking failure detected.
            putText(frame, "Tracking failure detected", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75,
                    Scalar(0, 0, 255), 2);
        }

        // Display tracker type on frame
        putText(frame, trackerType + " Tracker", Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50),
                2);

        // Display FPS on frame
        putText(frame, "FPS : " + SSTR(int(fps)), Point(100, 50), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50),
                2);

        // Display center pixel point on frame
        putText(frame, "Pixel Coordinates:  x = " + SSTR(int(circle_cp.x)) + " y = " + SSTR(int(circle_cp.y)),
                Point(20, 100), FONT_HERSHEY_SIMPLEX, 0.50, Scalar(50, 170, 50), 2);


        // Display frame.
        imshow("Tracking", frame);
        //imshow("Detecting", detect);
        imshow("Masking", mask);



        // Exit if ESC pressed.
        int k = waitKey(1);
        if(k == 27)
        {
            break;
        }

    }
}
