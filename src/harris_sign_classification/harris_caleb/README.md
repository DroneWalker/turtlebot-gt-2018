CNN Sign Detection
By Caleb Harris
Fall 2018 Introduction to Robotics Class
===============================================================================

! Packages !
---------------------------------------
Python Version:  3.5
This Python package primarily uses Pytorch, Torchvision, OpenCV, and Numpy
If Pytorch or Torchvision is not installed please see:  https://pytorch.org/
Suggested Install:  $ pip3 install http://download.pytorch.org/whl/cpu/torch-0.4.1-cp36-cp36m-linux_x86_64.whl
$ pip3 install torchvision
Conda Install:  $ conda install pytorch-cpu torchvision-cpu -c pytorch
---------------------------------------

! Background !
---------------------------------------
Information on Convolutional Neural Nets in Pytorch can be found here:  https://blog.algorithmia.com/convolutional-neural-nets-in-pytorch/
and here:  http://cs231n.github.io/convolutional-networks/

A major portion of the blob detection code used for cropping the sign out of the full picture was taken from code by Sean Wilson (the TA for the class)

! How the Code Works !
---------------------------------------
Simply add your test data in the folder:  /sign_classification/
The matching number for each picture can be seen as examples in the folder:
/Numbers/

Once the data is placed in the correct place, run the script sign_classification.py
$ python35 sign_classification.py

