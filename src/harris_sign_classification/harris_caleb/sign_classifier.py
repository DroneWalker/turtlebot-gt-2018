"""

Sign Classifier Testing - Main File

by Caleb Harris (caleb.harris94@gatech.edu)
for Fall 2018 Introduction to Robotics Class - Georgia Tech

-------------------------------------------------------------------------------

Functions:
===========
Main -


Before Starting:
================
Please make sure that the test data is in the appropriate folders as detailed
in the README

"""

import torch
from torch.autograd import Variable
import torch.nn.functional as F
from torchvision import datasets, models, transforms
import numpy as np
import matplotlib.pyplot as plt
import sys
import os
if sys.path.__contains__('/opt/ros/kinetic/lib/python2.7/dist-packages'):
    sys.path.remove('/opt/ros/kinetic/lib/python2.7/dist-packages')
import cv2
from src.convnet_c4f4 import ConvNet_C4F4


def setup():
    global name
    global lower_red1
    global lower_red2
    global upper_red1
    global upper_red2
    global lower_blue
    global upper_blue
    global lower_green
    global upper_green

    name = "Sign!"

    # Thresholds for 3 colors of signs:
    lower_red1 = np.array([164, 50, 10], np.uint8)  # Array (H,S,V) for the lower threshold bound of the HSV image
    upper_red1 = np.array([190, 230, 200], np.uint8)  # Array (H,S,V) for the upper threshold bound of the HSV image

    lower_red2 = np.array([0, 50, 10], np.uint8)  # Array (H,S,V) for the lower threshold bound of the HSV image
    upper_red2 = np.array([10, 230, 200], np.uint8)  # Array (H,S,V) for the upper threshold bound of the HSV image

    lower_blue = np.array([90, 50, 10], np.uint8)  # Array (H,S,V) for the lower threshold bound of the HSV image
    upper_blue = np.array([125, 230, 200], np.uint8)  # Array (H,S,V) for the upper threshold bound of the HSV image

    lower_green = np.array([50, 50, 10], np.uint8)  # Array (H,S,V) for the lower threshold bound of the HSV image
    upper_green = np.array([95, 230, 200], np.uint8)  # Array (H,S,V) for the upper threshold bound of the HSV image


    global titleTracker
    global titleOriginal
    global titleMask
    global debug
    global save
    global width
    global blurSize
    global morphOpSize
    global maxObjects
    global minObjectArea

    # Tool Setup
    titleTracker = "Color Tracker"  # Debugging Image Title
    titleOriginal = "Original Image"  # Debugging Image Title
    titleMask = "Mask Image"  # Debugging Image Title
    debug = True  # True - shows the images. False - Does not show the images.
    save = True

    width = 320  # Width of the image, this is sent in our point message as the z-component to know the zero point in the frame.
    blurSize = 9  # Blur Kernel Size
    morphOpSize = 5  # Closing and Opening Kernel Size

    maxObjects = 1  # Max number of object to detect.
    minObjectArea = 50  # Min number of pixels for an object to be recognized.


def morphOps(binaryMatrix, kernelSize):
    # Morphological operations (open and close) used to reduce noise in the acquired image.
    kernel = np.ones((kernelSize, kernelSize), np.uint8)
    tempFix = cv2.morphologyEx(binaryMatrix, cv2.MORPH_CLOSE, kernel)  # Fill in holes
    fix = cv2.morphologyEx(tempFix, cv2.MORPH_OPEN, kernel)  # Get rid of noise
    return fix


def drawCOM(frame, x, y, name):
    cv2.circle(frame, (x, y), 5, (0, 255, 0))
    cv2.putText(frame, name, (x - 30, y - 25), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 0), 2)


def findObjects(binaryMatrix):
    global imgCrop

    global x1
    global x2
    global y1
    global y2

    # Finds the location of the desired object in the image.
    output = []
    trash, contours, hierarchy = cv2.findContours(binaryMatrix, cv2.RETR_EXTERNAL,
                                                  cv2.CHAIN_APPROX_SIMPLE)  # Contours the image to find blobs of the same color
    cont = sorted(contours, key=cv2.contourArea, reverse=True)[
           :maxObjects]  # Sorts the blobs by size (Largest to smallest)

    imgCrop = imgBGR

    # Find the center of mass of the blob if there are any
    if hierarchy is not None:
        for i in range(0, len(cont)):
            M = cv2.moments(cont[i])
            if M['m00'] > minObjectArea:  # Check if the total area of the contour is large enough to care about!
                rect = cv2.minAreaRect(cont[0])
                w = int(rect[1][0])
                x = int(M['m10'] / M['m00'])
                y = int(M['m01'] / M['m00'])
                if (debug):
                    cv2.drawContours(imgTrack, cont[i], -1, (255, 0, 0), 3)  # Draws the contour.
                    drawCOM(imgTrack, x, y, name)
                    # print(rect[0][0])
                    # print(rect[0][1])
                    # print(rect)
                    x1 = int(rect[0][0]) - int(rect[1][0]/2)
                    y1 = int(rect[0][1]) + int(rect[1][1]/2)
                    x2 = int(rect[0][0]) + int(rect[1][0]/2)
                    y2 = int(rect[0][1]) - int(rect[1][1]/2)

                    cv2.rectangle(imgTrack,(x1,y1), (x2,y2),(0,255,0), 5)

                if output == []:
                    output = [[x, w]]
                else:
                    output.append[[x, w]]

                imgCrop = imgBGR[y2:y1, x1:x2]

    return output,imgCrop



def get_image(inputs_dir, output_dir, image_dir,filename):
    global imgBGR
    global imgHSV
    global imgBLUR
    global mask
    global imgMorphOps
    global imgTrack

    # Needed parameters from outside this function (lazy and globaling them).
    global p
    global update
    global start
    global morphOpSize
    global blurSize
    global width



    # The "CompressedImage" is transformed to a color image in BGR space and is store in "imgBGR"
    im = cv2.imread(inputs_dir + image_dir + filename, cv2.IMREAD_COLOR)
    # im = cv2.imread("test_data/2/3.png", cv2.IMREAD_COLOR)
    imgBGR = cv2.resize(im, dsize=(320, 240))

    # height and width of the image to pass along to the PID controller as the reference point.
    height, width = imgBGR.shape[:2]

    # Image used to draw things on!
    imgTrack = imgBGR.copy()

    # Blur the image to reduce edges caused by noise or that are useless to us.
    imgBlur = cv2.GaussianBlur(imgBGR, (blurSize, blurSize), 0)

    # Transform BGR to HSV to avoid lighting issues.
    imgHSV = cv2.cvtColor(imgBlur, cv2.COLOR_BGR2HSV)

    # Threshold the image using the selected lower and upper bounds of the color of the object.
    mask_red1 = cv2.inRange(imgHSV, lower_red1, upper_red1)
    mask_red2 = cv2.inRange(imgHSV, lower_red2, upper_red2)

    mask_red = cv2.addWeighted(mask_red1, 1.0, mask_red2, 1.0, 0.0)

    mask_green = cv2.inRange(imgHSV, lower_green, upper_green)
    mask_blue = cv2.inRange(imgHSV, lower_blue, upper_blue)

    mask_bg = cv2.add(mask_green,mask_blue)

    mask = cv2.add(mask_red, mask_bg)

    # To get rid of noise and fill in gaps in our object use open and close.
    imgMorphOps = morphOps(mask, morphOpSize)

    centers, imgCrop = findObjects(imgMorphOps)

    # Not always, the houghCircles function finds circle, so a None inspection is made
    if centers is not []:
        for i in centers:
            # The x position of the center of the object, the width of the object, and the width of the image.
            # p = Point(i[0], i[1], width)

            # Bool to indicate the need to publish new information
            update = True
            if save is True:
                # savefolder = "cropped_test_data"
                cv2.imwrite(output_dir + image_dir + "cropped_" + filename, imgCrop)
                print("Cropped Image Saved: " + output_dir + image_dir + "cropped_" + filename)
            else:
                print("Save File is turned off")
    else:
        # If no object was found, sends bogus numbers.
        # p = Point(99999, 99999, width)
        update = True

    # Once the first image has been processed set start to True to display.
    start = True


def crop_images(image_folder, cropped_image_folder):
    setup()

    for i in range(0,10,1):
        for filename in os.listdir(image_folder + str(i) + "/"):
            get_image(image_folder, cropped_image_folder, "/" + str(i) + "/", str(filename))

    # Create Debugging Windows
    if (debug):
        cv2.namedWindow(titleTracker, cv2.WINDOW_AUTOSIZE)
        cv2.moveWindow(titleTracker, 620, 50)
        cv2.namedWindow(titleMask, cv2.WINDOW_AUTOSIZE)
        cv2.moveWindow(titleMask, 1240, 50)
        cv2.namedWindow(titleOriginal, cv2.WINDOW_AUTOSIZE)
        cv2.moveWindow(titleOriginal, 50, 50)
        cv2.namedWindow("Cropped Image", cv2.WINDOW_AUTOSIZE)

        # Display the image
        if debug:
            cv2.imshow(titleOriginal, imgBGR)
            cv2.imshow(titleMask, mask)
            cv2.imshow(titleTracker, imgTrack)
            cv2.imshow("Cropped Image", imgCrop)


def main():

    # Initialize Pytorch CNN Model
    model = ConvNet_C4F4()

    # Load CNN Weights from trained model (indicate CPU process)
    model.load_state_dict(torch.load('Learning_models/cnn_trained_best_94.pt',map_location='cpu'))

    # Setup Transformations for input image
    transformations = transforms.Compose([
        transforms.Resize((32, 32)),
        transforms.ToTensor(),
        transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])])

    # Get Working Directory
    cwd = os.getcwd()

    # Get directory of images
    image_folder = cwd + '/sign_classification/'
    cropped_image_folder = cwd + '/sign_classification_cropped/'

    # Crop Image data to the sign
    if len(os.listdir(image_folder)) == 0:
        print("No test images were provided, please place images in:  " + image_folder)
        quit()
    # if len(os.listdir(cropped_image_folder)) == 0:
    #     print("Cropping images...")
    #     crop_images(image_folder, cropped_image_folder)
    # else:
    #     print("Images already cropped, if new dataset then clean images in:  " + cropped_image_folder)
    crop_images(image_folder, cropped_image_folder)


    # Prepare data in Dataloader
    test_set = datasets.ImageFolder(root=cropped_image_folder, transform=transformations)
    test_loader = torch.utils.data.DataLoader(test_set, batch_size=32, shuffle=False, num_workers=1)

    # Setup testing parameters
    model.eval()
    test_loss = 0
    correct = 0

    # Run classification for test dataset
    for batch_idx, (data, target) in enumerate(test_loader):
        data, target = Variable(data), Variable(target)
        output = model(data)
        test_loss += F.nll_loss(output, target, size_average=False).data[0]
        # get the index of the max log-probability
        pred = output.data.max(1, keepdim=True)[1]
        correct += pred.eq(target.data.view_as(pred)).cpu().sum()

        test_loss /= len(test_loader.dataset)
    print('Test set: Average loss: {:.4f}, Accuracy: {}/{} ({:.0f}%)'.format(
        test_loss, correct, len(test_loader.dataset),
        100. * correct / len(test_loader.dataset)))


if __name__ == "__main__":
    main()







