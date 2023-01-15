# VOIR

This project is a program that performs video analysis of a delimited trajectory for a car replica.
The algorithm runs on a Raspberry Pi, and communicates with an Arduino card that handles the robotic logic.

The trajectory is made of pairs of plastic cones: red cones are located on the left, and yellow cones on the right 
side of the road.

The algorithm relies heavily on the OpenCV library.

## Video recognition

Using OpenCV, the following algorithm is run on snapshot of the camera input taken on a given interval.
Processing each frame would bring poor performances and would also not be useful since the result end result
on the robotic side would be the same.

### Step 1: identification of the cones

First of all, the image is converted to *Hue Saturation Value* (HSV).
This allows for better isolation of cones.
Arbitrary values are used to detect yellow and red pixels in the camera snapshot:
- Yellow: between HSV(30, 100, 100) and HSV(50, 255, 255)
- Red (appears white in HSV): between HSV(160, 100, 100) and HSV(180, 255, 255)
Only areas which pixels are in those intervals are kept in the filtering result.

After this step, yellow and red cones are isolated for better processing.
Having them in different matrices eases the cost of deciding which recognized zones of interest belongs to which side of the road.

### Step 2: noise removal

Color filtering using the HSV domain can sometimes include undesired areas in the resulting matrix if their color is close to the one filtered.

To compensate this behavior, opening and closing algorithms are applied.
Their goal is to remove small and isolated areas in the filtered picture, with the supposition that those are either not cones, or cones that are far from the car.

The opening process consists of performing a morphological dilation following a morphological erosion, and closing is the opposite.
In order to keep only wide areas of the filtered picture, a bigger kernel size have been chosen for closing than for opening.

A downside of having split right and left cones from the first step is that opening and closing have to be called twice.
If this is performed on a merged matrix, there is a non-negligible chance to not be able to retrieve the cone color after the third step.

### Step 3: detection of connected components

After the previous steps, two matrices are obtained: one with red cones and an other one with the yellow ones.
Only the biggest areas have been kept, those are cones that are supposed to be the closest to the car.

First those two matrix needs to be merged in order to run the following algorithm only once.
Once this is done, we run a connected component detection algorithm to retrieve the blobs. For each one of them, 
