# Circle Detection Test by Caleb Harris

# removing a path folder which messes with opencv import
import sys
sys.path.remove('/opt/ros/kinetic/lib/python2.7/dist-packages')

# importing packages
import cv2
import numpy as np

# Step 1:  Import the image
#img = cv2.imread('images/opencvlogo.png', cv2.IMREAD_COLOR)
# img = cv2.imread('images/green_ball.jpg', cv2.IMREAD_COLOR)
img = cv2.imread('images/red_balls_multi.jpg', cv2.IMREAD_COLOR)
# img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
cv2.imshow("Initial Picture", img)
key = cv2.waitKey(0)

# Step 2:  Grayscale and Blur the image with a kernal size of 5
img2 = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)
img2 = cv2.medianBlur(img2, 5)
cv2.imshow("Blurred Initial", img2)
key = cv2.waitKey(0)

# Step 3: Find circles
# circles = cv2.HoughCircles(img2, cv2.HOUGH_GRADIENT, 1, 20, param1=50,
#                            param2=30, minRadius=0, maxRadius=0)
circles = cv2.HoughCircles(img2, cv2.HOUGH_GRADIENT, 1, 50, param1=100, param2=80, minRadius=0,
                           maxRadius=0)

print(circles.shape)

for i in circles[0,:]:
    # draw the outer circle
    cv2.circle(img, (i[0], i[1]), i[2], (0,0,255), 2)
    # draw the center of circle
    cv2.circle(img, (i[0], i[1]), 2, (0,0,255), 3)

cv2.imshow("Detected Circles", img)
cv2.waitKey(0)
cv2.destroyAllWindows()




# if key == 27:  # if ESC is pressed, exit loop
#    cv2.destroyAllWindows()

