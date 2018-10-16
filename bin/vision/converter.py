# Green Color Detection Test by Caleb Harris

# removing a path folder which messes with opencv import
import sys
sys.path.remove('/opt/ros/kinetic/lib/python2.7/dist-packages')
import numpy as np
import cv2

blue = sys.argv[1]
green = sys.argv[2]
red = sys.argv[3]

color = np.uint8([[[blue, green, red]]])
hsv_color = cv2.cvtColor(color, cv2.COLOR_BGR2HSV)

hue = hsv_color[0][0][0]

print("Lower bound is :"),
print("[" + str(hue - 10) + ", 100, 100]\n")

print("Upper bound is :"),
print("[" + str(hue + 10) + ", 255, 255]")
