//
// Created by charris on 10/4/18.
//

#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <sstream>

#include "../src/find_ball.cpp"




using namespace cv;
using namespace std;

int main(int argc, char **argv) {
    // Class setup

    VideoCapture video(0);
    video.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    video.set(CV_CAP_PROP_FRAME_HEIGHT, 480);



// Exit if video is not opened
    if (!video.isOpened()) {
        cout << "Video open error!" << endl;
        return 1;
    }

// Read first frame
    Mat frame;
    //SimpleBlobDetector detector;
    // Setup SimpleBlobDetector parameters.
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

//    Point circle_cp;
//    double circle_rad;
    int circle_index = 0;

while(video.read(frame)) {
    //detector.detect( frame, keypoints);
    FindBall ballTrack = FindBall(frame);
    Mat mask = ballTrack.orangeMask(frame);
//    cvtColor(mask_bgr, mask, CV_HSV2BGR);
//    cvtColor(mask_gray, mask, CV_BGR2GRAY);
    Canny(mask, detected_edges, 0, 100, 3);
//    cvtColor(frame, frame_gray, CV_BGR2GRAY);
//    Canny(frame_gray, detected_edges, 0, 100, 3);
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

//    for (int i = 0; i< contours.size(); i++)
//    {
//        Scalar color = Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
//        drawContours(drawing_contours, contours, i, color, 2, 8, hierarchy, 0, Point());
//        drawContours(drawing_shapes,contours_poly, i, color, 1, 8,  vector<Vec4i>(), 0, Point());
//        rectangle( drawing_shapes, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
//        circle( drawing_shapes, center[i], (int)radius[i], color, 2, 8, 0 );
//        imshow("contours", drawing_contours);
//        imshow("polygons", drawing_shapes);
//    }

        putText(frame, "Pixel Coordinates:  x = " + SSTR(int(center[circle_index].x))
        + " y = " + SSTR(int(center[circle_index].y)),
                Point(20, 20), FONT_HERSHEY_SIMPLEX, 0.50, Scalar(50, 170, 50), 2);


        Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
            drawContours(drawing_contours, contours, circle_index, color, 2, 8, hierarchy, 0, Point());
            drawContours(drawing_shapes, contours_poly, circle_index, color, 1, 8, vector<Vec4i>(), 0, Point());
//            rectangle(frame, boundRect[circle_index].tl(), boundRect[circle_index].br(), color, 2, 8, 0);
            circle(frame, center[circle_index], (int) radius[circle_index], 255, 2, 8, 0);
            circle(frame, center[circle_index], 10, 255, 2, 8, 0);
            imshow("contours", drawing_contours);
            imshow("polygons", drawing_shapes);
    }

//    imshow("mask",mask);
//    imshow("canny", dst);


    imshow("mask",mask);
    imshow("detection", frame);


    // Exit if ESC pressed.
    int k = waitKey(1);
    if (k == 27) {
        break;
    }

}

    return 0;
}