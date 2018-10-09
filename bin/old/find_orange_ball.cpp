//
// Created by charris on 9/26/18.
//


#include "ros/ros.h"
#include "geometry_msgs/Point.h"
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <image_transport/image_transport.h>
#include <compressed_image_transport/compression_common.h>
#include <sensor_msgs/image_encodings.h>
#include <cv_bridge/cv_bridge.h>
#include <sstream>

#include "find_ball.cpp"

using namespace cv;
using namespace std;
using namespace ros;
geometry_msgs::Point circle_cp;
Rect2d bbox;
Ptr<Tracker> tracker;

//cv_bridge::CvImagePtr img;
cv::Mat frame;
FindBall ballTrack = FindBall(frame);


/*void compressedimageCallback(const sensor_msgs::CompressedImageConstPtr& msg)
{
    cv_bridge::CvImagePtr cv_ptr(new cv_bridge::CvImagePtr);
    //cv_bridge::CvImagePtr cv_ptr;
    // Copy message header
    //cv_ptr->header = msg->header;

    //Decode image
    try
    {
        //img = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
        cv_ptr->image = cv::imdecode(cv::Mat(msg->data), CV_LOAD_IMAGE_UNCHANGED);
        //cv_ptr->encoding = sensor_msgs::image_encodings::BGR8;
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("Could not convert compressed image");
    }
    // Publish message to user callback

    //cv::imshow("subscribing", cv_ptr->image);
}*/



void imageTracking()
{
    bool ok;
    bool init = false;
    ROS_INFO("In Ros loop");

    // Mask, detect, centerpoint
    Mat mask = ballTrack.orangeMask();
    Mat frame = ballTrack.getFrame();

    imshow("main loop", frame);
    imshow("mask", mask);

    vector<Vec3f> circles = ballTrack.detectCircles(mask);

    if (circles.empty())
    {
        // Nothing to do
        ROS_INFO("No circles detected!");
    } else
    {
        //geometry_msgs::Point centerpoint = ballTrack.getCenterPoint(circles);
        cv::Point centerpoint;
        cv::Rect2d box;
        geometry_msgs::Point circle_cp;
        int largest_radius = 0;
        int largest_circle_index = 0;
        int xp;
        int yp;
        int radius;

        // Draw the circles detected
        for (size_t i = 0; i < circles.size(); i++)
        {
            radius = cvRound(circles[i][2]);
            if (radius > largest_radius)
            {
                largest_radius = radius;
                largest_circle_index = i;
                xp = circles[i][0];
                yp = circles[i][1];
                circle_cp.x = xp;
                circle_cp.y = yp;
                circle_cp.z = 0;
            }
        }

        cv::Point center(cvRound(xp), cvRound(xp));

        circle(frame, center, 3, Scalar(0, 255, 0), -1, 8, 0);// circle center
        circle(frame, center, radius, Scalar(0, 0, 255), 3, 8, 0);// circle outline
        bbox = Rect2d(xp - radius, yp - radius, radius * 2, radius * 2);
        if (init == false)
        {
            tracker->init(mask, bbox); // was frame
            init == true;
            cout << "center : " << center << "\nradius : " << radius << endl;
            cout << "bounding box :" << bbox << endl;
        }


        imshow("circles", frame);
	ok = tracker->update(mask, bbox); // was frame
    }

    


    // Start timer
        double timer = (double) getTickCount();

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
            init == true;
        } else {
            // Tracking failure detected.
            putText(frame, "Tracking failure detected", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75,
                    Scalar(0, 0, 255), 2);
            init = false;
        }

        // Display tracker type on frame
        putText(frame, "KCF Tracker", Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50),
                2);

        // Display FPS on frame
        putText(frame, "FPS : " + SSTR(int(fps)), Point(100, 50), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50),
                2);

        // Display center pixel point on frame
        putText(frame, "Pixel Coordinates:  x = " + SSTR(int(circle_cp.x)) + " y = " + SSTR(int(circle_cp.y)),
                Point(20, 100), FONT_HERSHEY_SIMPLEX, 0.50, Scalar(50, 170, 50), 2);


        // Display frame.
        imshow("Tracking", frame);




   // }
}

void imageCallback(const sensor_msgs::ImageConstPtr msg)
{
    cv_bridge::CvImagePtr cv_ptr;
    try
    {
        //cv::imshow("subscribing", cv_bridge::toCvShare(msg, "bgr8")->image);
        //cv::waitKey(10);
        cv_ptr = cv_bridge::toCvCopy(msg, "bgr8");
        ROS_INFO("Converting Image Loop");
        ballTrack.setFrame(cv_ptr->image);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }
    //Update GUI
    cv::imshow("subscribing", cv_ptr->image);
    cv::waitKey(10);
    imageTracking();

}

int main(int argc, char **argv)
{
    // Class setup

    // ROS Setup
    ros::init(argc, argv, "find_ball");
    NodeHandle n;
    image_transport::ImageTransport it(n);
    //namedWindow("Tracking", CV_WINDOW_AUTOSIZE);
    namedWindow("subscribing", CV_WINDOW_AUTOSIZE);
    startWindowThread();
    // "/raspicam_node/image"

    // open CV
    Mat track;
    Mat detect;

    // List of tracker types in OpenCV 3.4.1
    string trackerTypes[8] = {"BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN", "MOSSE", "CSRT"};
    // vector <string> trackerTypes(types, std::end(types));

    // Create a tracker
    string trackerType = trackerTypes[2];

    //Ptr<Tracker> tracker;

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

    Rect2d bbox;
    bool init = false;
    bool ok = false;

    image_transport::Subscriber image_sub = it.subscribe("/image",
                                                         1, imageCallback);
    Publisher trackpoint_pub = n.advertise<geometry_msgs::Point>("trackpoint", 1000);
    Rate loop_rate(10);

    while(ros::ok()) {
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
