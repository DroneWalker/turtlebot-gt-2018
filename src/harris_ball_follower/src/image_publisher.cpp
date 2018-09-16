//
// Created by charris on 9/14/18.
//

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>

using namespace cv;

int main(int argc, char** argv)
{
    ros::init(argc, argv, "image_publisher");
    ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);
    image_transport::Publisher pub = it.advertise("camera/frame", 1);

    VideoCapture cap(0);
    if(!cap.isOpened())
        return -1;
    Mat frame;
    namedWindow("Original Frame");
    sensor_msgs::ImagePtr msg;

    ros::Rate loop_rate(5);
    while (nh.ok()) {
        cap >> frame;  // get  new frame from camera
        if (!frame.empty()) {
            msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame).toImageMsg();
            pub.publish(msg);
        }

        ros::spinOnce();
        loop_rate.sleep();
    }
}


