#! /usr/bin/env python

# Introduction to Robotics, Lab #1
# Caleb Harris, Fall 2018

# removing a path folder which messes with opencv import
import sys
sys.path.remove('/opt/ros/kinetic/lib/python2.7/dist-packages')

# importing packages
from imutils.video import VideoStream, FPS
import cv2
#import
import numpy as np
import argparse
import time
from tracker.centroidtracker import CentroidTracker

(major_ver, minor_ver, subminor_ver) = (cv2.__version__).split('.')


## Function 1: Define color detection function
def detect_red(image):
    img = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
    img = cv2.GaussianBlur(img, (7, 7), 0)

    # Use picture in gimp to find BGR values, then use converter.py to change to HSV
    # This is for pink ball
    mag_lower_range = np.array([165, 60, 60], dtype=np.uint8)
    mag_upper_range = np.array([180, 220, 220], dtype=np.uint8)

    #org_lower_range = np.array([0, 120, 80], dtype=np.uint8)
    #org_upper_range = np.array([20, 220, 220], dtype=np.uint8) # At Lab

    org_lower_range = np.array([0, 220, 120], dtype=np.uint8)
    org_upper_range = np.array([20, 255, 255], dtype=np.uint8)  # At Home

    mask = cv2.inRange(img, org_lower_range, org_upper_range)
    #mask = cv2.inRange(img, mag_lower_range, mag_upper_range)

    imgout = mask

    clean = True

    if (clean == True):
        kernel = np.ones((7, 7), np.uint8)
        # openandclose = cv2.morphologyEx(opening, cv2.MORPH_CLOSE, kernel)
        closing = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
        closeandopen = cv2.morphologyEx(closing, cv2.MORPH_OPEN, kernel)
        imgout = closeandopen

    return imgout

## Function 2: Define circle detection
def detect_circles(image, mask):
    # Step 1:  Import the image and copy
    output = image.copy()
    # Step 2:  Grayscale and Blur the image with a kernal size of 5
    if (mask is None):
        img = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)
        print("A valid option value was not used!  Use either 'raw' or 'mask'.  Default 'raw' used. \n")
    else:
        img = mask


    img = cv2.GaussianBlur(img, (7,7), 0)
    img = cv2.medianBlur(img, 5)

    img = cv2.adaptiveThreshold(img, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 11, 3.5)



    # Step 3: Find circles
    #circles = cv2.HoughCircles(img, cv2.HOUGH_GRADIENT, 1, minDist=80, param1=20, param2=30, minRadius=50,
                               #maxRadius=0)
    circles = cv2.HoughCircles(img, cv2.HOUGH_GRADIENT, 1, minDist=100, param1=20, param2=30, minRadius=10,maxRadius=0)
    # Hough circles:  minDist (min dist between detected circles), param1 (edge detection param)
        # param2 (smaller - more circles),

    if (circles is not None):
        # convert the (x, y) coordinates and radius of the circles to integers
        circles = np.round(circles[0, :]).astype("int")


        # loop over the (x, y) coordinates and radius of the circles
        for (x, y, r) in circles:
            # draw the circle in the output image, then draw a rectangle in the image
            # corresponding to the center of the circle
            cv2.circle(output, (x, y), r, (0, 255, 0), 4)
            cv2.rectangle(output, (x - 5, y - 5), (x + 5, y + 5), (0, 128, 255), -1)
            # time.sleep(0.5)
            print("Column Number: %f, Row Number: %f, Radius is %f \n" % (x,y,r))
    return output, circles


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

    # intitate tracker
    tracker = cv2.TrackerKCF_create()


    # initialize the bounding box coordinates of the object we are going
    # to track
    circles = None
    initBB = None
    objects = None
    # if no tracker supplied, use detect_circles
    # if not args.get("tracker", False):
    #     print("No tracker supplied, using built in circle detection!")
    #     ct = CentroidTracker()
    #     (H,W) = (None, None)

    ct = CentroidTracker()
    (H,W) = (None, None)
    framecounter = 0


    # if a video path was not supplied, grab the reference to the web cam
    if not args.get("video", False):
        print("[INFO] starting video stream...")
        cap = cv2.VideoCapture(0)
        time.sleep(1.0)

    # otherwise, grab a reference to the video file
    else:
        #cap = cv2.VideoCapture(args["video"])
        cap = cv2.VideoCapture('~/introrobotics/turtlebot-gt-2018/vision/videos/red_ball3.mp4')

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
        kcf = frame

        #print('Width = ', cap.get(3),' Height = ', cap.get(4),' fps = ', cap.get(5))

        # Do detection and masking and stuff
        # detect_circles(color)
        red_mask = detect_red(color)

        status = "Waiting"
        rects = []

        circles_image, circles = detect_circles(color, red_mask)  # Set mask = 0 if none used

        # For showing off KCF tracker
        if circles is not None:
            tracker.init(kcf, circles[0])
            fps = FPS().start()
            # grab the new bounding box coordinates of the object
            (success, box) = tracker.update(kcf)

            # check to see if the tracking was a success
            if success:
                (x, y, w, h) = [int(v) for v in box]
                cv2.rectangle(kcf, (x, y), (x + w, y + h),
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
                cv2.putText(kcf, text, (10, H - ((i * 20) + 20)),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 0, 255), 2)


        if (framecounter % 2 == 0):
            skip = True
        else:
            skip = False

        if circles is not None:
            # grab the new bounding box coordinates of the object
            for (x, y, r) in circles:
                box = np.array([x - r, y - r, x + r, y + r])
                rects.append(box.astype("int"))
                # draw bounding box
                (startX, startY, endX, endY) = box.astype("int")
                #cv2.rectangle(frame, (startX, startY), (endX, endY), (0, 255, 0), 2)
                #cv2.circle(frame, (x, y), r, (0, 255, 0), 2)
               # update our centroid tracker using the computed set of bounding
                if skip == False:
                    objects = ct.update(rects)
        if objects is not None:
            # loop over the tracked objects
            for (objectID, centroid) in objects.items():
                # draw both the ID of the object and the centroid of the
                # object on the output frame
                text = "ID {}".format(objectID)
                xt = str(centroid[0])
                yt = str(centroid[1])
                r = str(centroid[2])
                cv2.putText(frame, text, (centroid[0] - 10, centroid[1] - 10),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
                cv2.putText(frame, "Centroid: x = " + xt + ", y = " + yt, (20, 20),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
                cv2.circle(frame, (centroid[0], centroid[1]), centroid[2], (0, 255, 0), 2)

        framecounter = framecounter + 1
        # Display the resulting frame
        cv2.imshow("Tracking", frame)
        cv2.imshow('Masking Red', red_mask)
        cv2.imshow('Circle Detection', circles_image)
        cv2.imshow('KCF', kcf)
        # cv2.imshow('Original', color)
        key = cv2.waitKey(1) & 0xFF

        # If you want to compare to a selected object (to show how tracker works)
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