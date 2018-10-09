//
// Created by charris on 9/16/18.
//

#ifndef _FindBall_SOURCE_
#define _FindBall_SOURCE_


#include "ros/ros.h"
#include "geometry_msgs/Point.h"
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sstream>

#include "find_ball.h"

using namespace cv;
using namespace std;
using namespace ros;

const int KERNEL_LENGTH = 7;
// HSV Ranges
const int low_H = 0, low_S = 220, low_V = 120;
const int high_H = 20, high_S = 255, high_V = 255;
// Working with orange ball
//const int low_H = 0, low_S = 140, low_V = 100;
//const int high_H = 20, high_S = 255, high_V = 255;
//const int low_H = 170, low_S = 100, low_V = 80;
//const int high_H = 180, high_S = 255, high_V = 255;
const int morph_operator = 2;
const int morph_elem = 0;
const int morph_size = 1;


// Convert to string
#define SSTR( x ) static_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()


class FindBallImpl
{
public:
    FindBallImpl(cv::Mat frame);
    ~FindBallImpl();
    geometry_msgs::Point getCenterPoint(std::vector<cv::Vec3f> circles);
    Mat orangeMask(cv::Mat frame);
    vector<Vec3f> detectCircles(Mat mask);
    vector<vector<Point> >getCircles(cv::Mat mask, cv::Mat frame);
    void setFrame(Mat frame);
    Mat getFrame();

private:
    cv_bridge::CvImagePtr _img;
    cv::Mat _frame;
};

FindBall::FindBall(Mat frame)
{
    pimpl = new FindBallImpl(frame);
}

geometry_msgs::Point FindBall::getCenterPoint(std::vector<cv::Vec3f> circles)
{
    return pimpl->getCenterPoint(circles);
}

Mat FindBall::orangeMask(cv::Mat frame)
{
    return pimpl->orangeMask(frame);
}

vector<Vec3f> FindBall::detectCircles(Mat mask)
{
    return pimpl->detectCircles(mask);
}

vector<vector<Point> > FindBall::getCircles(cv::Mat mask, cv::Mat frame)
{
    return pimpl->getCircles(mask, frame);
}

void FindBall::setFrame(Mat frame)
{
    return pimpl->setFrame(frame);
}

Mat FindBall::getFrame()
{
    return pimpl->getFrame();
}
FindBall::~FindBall()
{
    delete pimpl;
}

/**
 * Implementation
 */

FindBallImpl::FindBallImpl(Mat frame) :
        _frame(frame)
{
}

Mat FindBallImpl::orangeMask(cv::Mat frame)
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

vector<Vec3f> FindBallImpl::detectCircles(Mat mask)
{
    cv::GaussianBlur(mask, mask, Size(7,7), 2, 2);

    vector<Vec3f> circles;
    cv::HoughCircles(mask, circles, CV_HOUGH_GRADIENT,1, 100, 20, 30, 10, 0);

    return circles;
}

vector<vector<Point> > FindBallImpl::getCircles(cv::Mat mask, cv::Mat frame)
{
    SimpleBlobDetector::Params params;

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

//    Point circle_cp;
//    double circle_rad;
    int circle_index = 0;


    //detector.detect( frame, keypoints);
    Canny(mask, detected_edges, 0, 100, 3);
    dst = Scalar::all(0);
    frame.copyTo(dst, detected_edges);
    findContours(detected_edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    if (!contours.empty()) {

        /// Approximate contours to polygons
        vector<vector<Point> > contours_poly(contours.size());
        vector<Rect> boundRect(contours.size());
        vector<Point2f> center(contours.size());
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
        imshow("contours", drawing_contours);
        imshow("polygons", drawing_shapes);

        return contours_poly;

    }
}

geometry_msgs::Point FindBallImpl::getCenterPoint(std::vector<cv::Vec3f> circles)
{
    cv::Point centerpoint;
    cv::Rect2d box;
    geometry_msgs::Point circle_cp;
    int largest_radius = 0;
    int largest_circle_index = 0;
    
    if (!circles.empty())
    {
        // Draw the circles detected
        for (size_t i = 0; i < circles.size(); i++)
        {
            int radius = cvRound(circles[i][2]);
            if (radius > largest_radius)
            {
                largest_radius = radius;
                largest_circle_index = i;
                int xp = circles[i][0];
                int yp = circles[i][1];
                circle_cp.x = xp;
                circle_cp.y = yp;
                circle_cp.z = 0;
            }
        }
    }

    return circle_cp;

}

void FindBallImpl::setFrame(Mat frame)
{
    _frame = frame;
}

Mat FindBallImpl::getFrame()
{
    return _frame;
}

FindBallImpl::~FindBallImpl()
{
}




#endif
