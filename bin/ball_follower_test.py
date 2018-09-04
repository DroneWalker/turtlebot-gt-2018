#! /usr/bin/env python

# Introduction to Robotics, Lab #1
# Caleb Harris, Fall 2018

# removing a path folder which messes with opencv import
import sys
sys.path.remove('/opt/ros/kinetic/lib/python2.7/dist-packages')

# importing packages
import cv2
import numpy as np
import argparse

# Define color detection function
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

# Define circle detection
def detect_circles(image, mask):
    # Step 1:  Import the image and copy
    output = image.copy()
    # Step 2:  Grayscale and Blur the image with a kernal size of 5
    if (mask == "none"):
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

    return output


# Start video capture

cap = cv2.VideoCapture(0)

# Record?
record = True

if (record == True):
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    out = cv2.VideoWriter('videos/detection.avi', fourcc, 30, (640, 480))

while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()

    # Our operations on the frame come here
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    color = frame

    #print('Width = ', cap.get(3),' Height = ', cap.get(4),' fps = ', cap.get(5))

    # Do detection and masking and stuff
    # detect_circles(color)
    red_mask = detect_red(color)
    circles_image = detect_circles(color, red_mask)  # Set mask = "none" if none used

    # Display the resulting frame
    cv2.imshow('Masking Red', red_mask)
    cv2.imshow('Circle Detection', circles_image)
    cv2.imshow('Original', color)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything is done, release the capture
cap.release()
if (record == True):
    out.release()
cv2.destroyAllWindows()
