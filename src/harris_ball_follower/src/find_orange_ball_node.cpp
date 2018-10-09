//
// Created by charris on 10/8/18.
//

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
cv::Mat frame;
FindBall ballTrack = FindBall(frame);
vector<vector<Point> > circles;

// DEBUG?
bool debug = true;


void imageTracking()
{
    ROS_INFO("In Ros loop");

    // Mask, detect, centerpoint
    Mat frame = ballTrack.getFrame();
    Mat mask = ballTrack.orangeMask(frame);

    SimpleBlobDetector::Params params;

// Change thresholds
    //params.minThreshold = 10;
    //params.maxThreshold = 256;

// Filter by Area.
    params.filterByArea = true;
    params.minArea =4000;
    params.maxArea=10000;

// filter my min distance
//params.minDistBetweenBlobs=100;

// Filter by Circularity
    params.filterByCircularity = true;
    params.minCircularity = 0.7;

// Filter by Convexity
    params.filterByConvexity = true;
    params.minConvexity = 0.6;

// Filter by Inertia
    params.filterByInertia = false;
    params.minInertiaRatio = 0.01;

//filter by colour
    params.filterByColor=true;
    params.blobColor=255;

// Storage for blobs
    vector<KeyPoint> keypoints;



// Set up detector with params
    Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);


// Draw detected blobs as red circles.
// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
    Mat im_with_keypoints;
    Mat detected_edges;
    Mat dst;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    RNG rng(12345);
    Mat dst_mono;
    Mat mask_gray;
    Mat mask_bgr;
    Mat frame_gray;

    int circle_index = 0;

    //detector.detect( frame, keypoints);
    FindBall ballTrack = FindBall(frame);
    mask = ballTrack.orangeMask(frame);
    Canny(mask, detected_edges, 0, 100, 3);
    dst = Scalar::all(0);
    frame.copyTo(dst, detected_edges);
    findContours(detected_edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    if (!contours.empty()) {

        /// Approximate contours to polygons
        vector <vector<Point> > contours_poly(contours.size());
        vector <Rect> boundRect(contours.size());
        vector <Point2f> center(contours.size());
        vector<float> radius(contours.size());
        double largest_radius = 0;
        for (int i = 0; i < contours.size(); i++) {
            approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
            boundRect[i] = boundingRect(Mat(contours_poly[i]));
            minEnclosingCircle((Mat) contours_poly[i], center[i], radius[i]);
            if (radius[i] > largest_radius) {
                circle_index = i;
            }
        }

        /// Drawing polygonal contours + boxes + circles
        Mat drawing_contours = Mat::zeros(detected_edges.size(), CV_8UC3);
        Mat drawing_shapes = Mat::zeros(detected_edges.size(), CV_8UC3);


        Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        drawContours(drawing_contours, contours, circle_index, color, 2, 8, hierarchy, 0, Point());
        drawContours(drawing_shapes, contours_poly, circle_index, color, 1, 8, vector<Vec4i>(), 0, Point());
//            rectangle(frame, boundRect[circle_index].tl(), boundRect[circle_index].br(), color, 2, 8, 0);
        circle(frame, center[circle_index], (int) radius[circle_index], 255, 2, 8, 0);
        circle(frame, center[circle_index], 5, 255, 2, 8, 0);

        // center of Image
        circle_cp.x = center[circle_index].x;
        circle_cp.y = center[circle_index].y;

        if (debug == true)
        {
            imshow("contours", drawing_contours);
            imshow("polygons", drawing_shapes);
        }

    }


    // Display center pixel point on frame
    putText(frame, "Pixel Coordinates:  x = " + SSTR(int(circle_cp.x)) + " y = " + SSTR(int(circle_cp.y)),
            Point(20, 20), FONT_HERSHEY_SIMPLEX, 0.50, Scalar(50, 170, 50), 2);

    // Display frame.
    if (debug == true)
    {
        imshow("main loop", frame);
        imshow("mask", mask);
    }
    imshow("Tracking", frame);
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

    image_transport::Subscriber image_sub = it.subscribe("/find_orange_ball/image",
                                                         1, imageCallback);
    Publisher trackpoint_pub = n.advertise<geometry_msgs::Point>("trackpoint", 10);
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

