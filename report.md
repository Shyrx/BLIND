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

### Step 2: noise removal

Color filtering using the HSV domain can sometimes include undesired areas in the resulting matrix if their color is close to the one filtered.

To compensate this behavior, opening and closing algorithms are applied.
Their goal is to remove smalls and isolated areas in the filtered picture, with the supposition that those are either not cones, or cones that are far from the car.

The opening process consists in performing an erosion and a dilation, and closing is the opposite.
For opening, we use a kernel of size 16 pixels ; for closing, a kernel of size 30 pixels.
Those kernel values have the advantage of keeping only wide areas of the filtered picture: the closing is more aggressive than the opening.

A downside of having split right and left cones in the first step is that opening and closing has to be done twice.
If this is performed on a merged matrix, their is a chance to not be able to retrieve the cone color after the third step.
