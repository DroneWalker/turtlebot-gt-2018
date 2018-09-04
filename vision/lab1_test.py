#! /usr/bin/env python

# Introduction to Robotics, Lab #1
# Caleb Harris, Fall 2018

# removing a path folder which messes with opencv import
import sys
sys.path.remove('/opt/ros/kinetic/lib/python2.7/dist-packages')
#sys.path.append('~/introrobotics/turtlebot-gt-2018/vision')

# importing packages
from imutils.video import VideoStream, FPS
import cv2
import numpy as np
import argparse
import time
from vision.tracker.centroidtracker import CentroidTracker

(major_ver, minor_ver, subminor_ver) = (cv2.__version__).split('.')


## Function 1: Define color detection function
def detect_red(image):
    img = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

    # Use picture in gimp to find BGR values, then use converter.py to change to HSV
    # This is for red
    red_lower_range = np.array([169, 100, 100], dtype=np.uint8)
    red_upper_range = np.array([189, 255, 255], dtype=np.uint8)

    mask = cv2.inRange(img, red_lower_range, red_upper_range)

    imgout = mask

    clean = False

    if (clean == True):
        kernel = np.ones((5, 5), np.uint8)
        opening = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)
        closing = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
        openandclose = cv2.morphologyEx(opening, cv2.MORPH_CLOSE, kernel)
        closeandopen = cv2.morphologyEx(closing, cv2.MORPH_OPEN, kernel)
        imgout = closeandopen

    return imgout

## Function 2: Define circle detection
def detect_circles(image, mask):
    # Step 1:  Import the image and copy
    output = image.copy()
    # Step 2:  Grayscale and Blur the image with a kernal size of 5
    if (mask == 0):
        img = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)
    elif (mask is None):
        img = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)
        print("A valid option value was not used!  Use either 'raw' or 'mask'.  Default 'raw' used. \n")
    else:
        img = mask

    img = cv2.GaussianBlur(img, (5,5), 0)
    img = cv2.medianBlur(img, 5)

    img = cv2.adaptiveThreshold(img, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 11, 3.5)
    # gray = cv2.erode(gray, kernel, iterations=1)
    # gray = cv2.dilate(gray, kernel, iterations=1)



    # Step 3: Find circles
    # circles = cv2.HoughCircles(img2, cv2.HOUGH_GRADIENT, 1, 20, param1=50,
    #                            param2=30, minRadius=0, maxRadius=0)
    circles = cv2.HoughCircles(img, cv2.HOUGH_GRADIENT, 1, 200, param1=30, param2=45, minRadius=0,
                               maxRadius=0)

    # initialize
    circle_detections = []

    if (circles is not None):
        # for i in circles[0, :]:
        #     # draw the outer circle
        #     cv2.circle(img, (i[0], i[1]), i[2], (0, 0, 255), 2)
        #     # draw the center of circle
        #     cv2.circle(img, (i[0], i[1]), 2, (0, 0, 255), 3)

        # convert the (x, y) coordinates and radius of the circles to integers
        circles = np.round(circles[0, :]).astype("int")


        # loop over the (x, y) coordinates and radius of the circles
        for (x, y, r) in circles:
            # draw the circle in the output image, then draw a rectangle in the image
            # corresponding to the center of the circle
            cv2.circle(output, (x, y), r, (0, 255, 0), 4)
            cv2.rectangle(output, (x - 5, y - 5), (x + 5, y + 5), (0, 128, 255), -1)
            # time.sleep(0.5)
            print
            "Column Number: "
            print
            x
            print
            "Row Number: "
            print
            y
            print
            "Radius is: "
            print
            r

            # Define bounding box (startX, startY, endX, endY)
            circle_detections.append(np.array([x-r, y-r, x+r, y+r]))

    return output, circle_detections


## Main Function
if __name__ == '__main__':
    # construct the argument parser and parse the arguments
    ap = argparse.ArgumentParser()
    ap.add_argument("-v", "--video", type=str,
                    help="path to input video file")
    ap.add_argument("-t", "--tracker", type=str, default="kcf",
                    help="OpenCV object tracker type")
    args = vars(ap.parse_args())

    # extract the OpenCV version info
    (major, minor) = cv2.__version__.split(".")[:2]

    # if we are using OpenCV 3.2 OR BEFORE, we can use a special factory
    # function to create our object tracker
    if int(major) == 3 and int(minor) < 3:
        tracker = cv2.Tracker_create(args["tracker"].upper())

    # otherwise, for OpenCV 3.3 OR NEWER, we need to explicity call the
    # approrpiate object tracker constructor:
    else:
        # initialize a dictionary that maps strings to their corresponding
        # OpenCV object tracker implementations
        OPENCV_OBJECT_TRACKERS = {
            #"csrt": cv2.TrackerCSRT_create,
            "kcf": cv2.TrackerKCF_create,
            "boosting": cv2.TrackerBoosting_create,
            "mil": cv2.TrackerMIL_create,
            "tld": cv2.TrackerTLD_create,
            "medianflow": cv2.TrackerMedianFlow_create,
            #"mosse": cv2.TrackerMOSSE_create
        }

        # grab the appropriate object tracker using our dictionary of
        # OpenCV object tracker objects
        tracker = OPENCV_OBJECT_TRACKERS[args["tracker"]]()

    # initialize the bounding box coordinates of the object we are going
    # to track
    initBB = None
    # if no tracker supplied, use detect_circles
    if not args.get("tracker", False):
        print("No tracker supplied, using built in circle detection!")
        ct = CentroidTracker()
        (H,W) = (None, None)

    # if a video path was not supplied, grab the reference to the web cam
    if not args.get("video", False):
        print("[INFO] starting video stream...")
        cap = cv2.VideoCapture(0)
        time.sleep(1.0)

    # otherwise, grab a reference to the video file
    else:
        cap = cv2.VideoCapture(args["video"])

    # initialize the FPS throughput estimator
    fps = None

    # Start video capture (if a file, then VideoCapture("url")
    #cap = cv2.VideoCapture(0)

    # Exit if video not opened.
    if not cap.isOpened():
        print ("Could not open video")
        sys.exit()

    # Record?
    record = False

    if (record == True):
        fourcc = cv2.VideoWriter_fourcc(*'XVID')
        out = cv2.VideoWriter('videos/detection.avi', fourcc, 30, (640, 480))

    while True:
        # Capture frame-by-frame
        ret, frame = cap.read()
        frame = frame[1] if args.get("video", False) else frame

        # Checks for exiting
        if frame is None:
            break


        if not ret:
            print('Cannot read video file')
            sys.exit()

        (H, W) = frame.shape[:2]

        # Our operations on the frame come here
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        color = frame

        #print('Width = ', cap.get(3),' Height = ', cap.get(4),' fps = ', cap.get(5))

        # Do detection and masking and stuff
        # detect_circles(color)
        red_mask = detect_red(color)
        circles_image, circles = detect_circles(color, 0)  # Set mask = 0 if none used

        # check to see if we are currently tracking an object
        if initBB is not None:
            # grab the new bounding box coordinates of the object
            (success, box) = tracker.update(frame)

            # check to see if the tracking was a success
            if success:
                (x, y, w, h) = [int(v) for v in box]
                cv2.rectangle(frame, (x, y), (x + w, y + h),
                              (0, 255, 0), 2)

            # update the FPS counter
            fps.update()
            fps.stop()

            # initialize the set of information we'll be displaying on
            # the frame
            info = [
                ("Tracker", args["tracker"]),
                ("Success", "Yes" if success else "No"),
                ("FPS", "{:.2f}".format(fps.fps())),
            ]

            # loop over the info tuples and draw them on our frame
            for (i, (k, v)) in enumerate(info):
                text = "{}: {}".format(k, v)
                cv2.putText(frame, text, (10, H - ((i * 20) + 20)),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 0, 255), 2)




        # Display the resulting frame
        cv2.imshow("Tracking", frame)
        cv2.imshow('Masking Red', red_mask)
        cv2.imshow('Circle Detection', circles_image)
        cv2.imshow('Original', color)
        key = cv2.waitKey(1) & 0xFF

        # if the 's' key is selected, we are going to "select" a bounding
        # box to track
        if key == ord("s"):
            # select the bounding box of the object we want to track (make
            # sure you press ENTER or SPACE after selecting the ROI)
            initBB = cv2.selectROI("Frame", frame, fromCenter=False,
                                   showCrosshair=True)

            # start OpenCV object tracker using the supplied bounding box
            # coordinates, then start the FPS throughput estimator as well
            tracker.init(frame, initBB)
            fps = FPS().start()
        elif key == ord("q"):
            break
    # When everything is done, release the capture
    cap.release()
    if (record == True):
        out.release()
    cv2.destroyAllWindows()