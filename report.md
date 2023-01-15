# VOIR

This project is a program that performs video analysis of a delimited trajectory for a car replica.
The algorithm runs on a Raspberry Pi, and communicates with an Arduino card that handles the robotic side.

The trajectory is composed of plastic cones.
Red cones are located on the left, and yellow cones on the right of the road.

The algorithm relies heavily on the OpenCV library.

## Video recognition

Using OpenCV, the following algorithm is run on each frame of the camera input.

### Step 1: identification of the cones

First, the image is converted to *Hue Saturation Value* (HSV).
This allows for better isolation of cones.
Arbitrary values are the used to detect yellow and red values of the picture:
- Yellow: between HSV(30, 100, 100) and HSV(50, 255, 255)
- Red (appears white in HSV): between HSV(160, 100, 100) and HSV(180, 255, 255)
Only areas which pixels are in those intervals are kept in the filtering result.

After this step, yellow and red cones are isolated in separate matrix.
Having the left and right cones in different matrix eases the process of deciding which recognized zone of interest belongs to which side of the road.

