# Green Color Detection Test by Caleb Harris

# removing a path folder which messes with opencv import
import sys
sys.path.remove('/opt/ros/kinetic/lib/python2.7/dist-packages')

# importing packages
import cv2
import numpy as np
import argparse

# construct the argument parse and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-i", "--image", help = "path to the image")
args = vars(ap.parse_args())

# load the image in color (BGR)
image = cv2.imread(args["image"], 1)

# change to HSV image
img = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

# Use picture in gimp to find BGR values, then use converter.py to change to HSV
# This is for red
red_lower_range = np.array([169, 100, 100], dtype = np.uint8)
red_upper_range = np.array([189, 255, 255], dtype = np.uint8)


mask = cv2.inRange(img, red_lower_range, red_upper_range)

cv2.imshow('mask', mask)
cv2.imshow('image', image)

clean = True

if (clean == True):
    kernel = np.ones((5,5), np.uint8)
    opening = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)
    closing = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
    openandclose = cv2.morphologyEx(opening, cv2.MORPH_CLOSE, kernel)
    closeandopen = cv2.morphologyEx(closing, cv2.MORPH_OPEN, kernel)

    #cv2.imshow('opening', opening)
    #cv2.imshow('closing', closing)
    #cv2.imshow('open then close', openandclose)
    cv2.imshow('close then open', closeandopen)

while (1):
    k = cv2.waitKey(0)
    if (k == 27):
        break

cv2.destroyAllWindows()
