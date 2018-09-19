//
// Created by charris on 9/16/18.
//

#include "ros/ros.h"
#include "geometry_msgs/Point.h"

#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>

#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>

#include <sstream>

#include "ball_follower.h"

using namespace cv;
using namespace std;
using namespace ros;

Mat frame;


cv::Mat imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
    try {
        frame = cv_bridge::toCvShare(msg, "bgr8")->image;
        //cv::imshow("find_ball_debugger", cv_bridge::toCvShare(msg, "bgr8")->image);
        cv::waitKey(10);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }
    return frame;
}


int main(int argc, char **argv)
{
    // ROS Setup
    init(argc, argv, "find_ball");
    NodeHandle n;
    image_transport::ImageTransport it(n);
    image_transport::Subscriber image_sub = it.subscribe("/raspicam_node/image/raw",
            1,imageCallback);
    Publisher trackpoint_pub = n.advertise<geometry_msgs::Point>("trackpoint",1000);
    image_transport::Publisher img_debug = it.advertise("camera/image",1);
    Rate loop_rate(10);

    // open CV
    namedWindow("Tracking", CV_WINDOW_AUTOSIZE);
    Mat track;
    Mat detect;
    geometry_msgs::Point circle_cp;

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
    video.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    video.set(CV_CAP_PROP_FRAME_HEIGHT, 480);



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

    while(video.read(frame) && ros::ok()) {
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
                    cv::Point center(cvRound(xp), cvRound(yp));
                    detect = frame;
                    circle_cp.x = xp;
                    circle_cp.y = yp;
                    circle_cp.z = 0;
                    circle(detect, center, 3, Scalar(0, 255, 0), -1, 8, 0);// circle center
                    circle(detect, center, radius, Scalar(0, 0, 255), 3, 8, 0);// circle outline
                    bbox = Rect2d(xp - radius, yp - radius, radius * 2, radius * 2);
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
            Point center = Point(bbox.x+bbox.height/2, bbox.y+bbox.width/2);
            cv::circle(frame, center, 3, Scalar(0, 255, 0), -1, 8, 0);// circle center
            circle_cp.x = bbox.x+bbox.height/2;
            circle_cp.y = bbox.y+bbox.width/2;
            circle_cp.z = 0;
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

        // Ros publisher output
        trackpoint_pub.publish(circle_cp);
        ros::spinOnce();
        loop_rate.sleep();



        // Exit if ESC pressed.
        int k = waitKey(1);
        if (k == 27) {
            break;
        }
    }
    ros::spin();
    return 0;
}
