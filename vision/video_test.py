# coding: utf-8

import sys
sys.path.remove('/opt/ros/kinetic/lib/python2.7/dist-packages')


import numpy as np
import cv2

cap = cv2.VideoCapture(0)

# Record?
record = False

if (record == True):
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    out = cv2.VideoWriter('videos/out_Test.avi', fourcc, 30, (640, 480))

while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()

    # Our operations on the frame come here
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    color = frame

    #print('Width = ', cap.get(3),' Height = ', cap.get(4),' fps = ', cap.get(5))

    # Display the resulting frame
    cv2.imshow('grayscale', gray)
    cv2.imshow('color', color)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything is done, release the capture
cap.release()
out.release()
cv2.destroyAllWindows()
