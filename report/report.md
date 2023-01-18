---
title: "Handout VOIR"
author:
    - antoine.sole
    - david.horozian
    - thomas.crambert
    - tristan.floch
colortheme: "beaver"
fonttheme: "professionalfonts"
theme: "Frankfurt"
fontsize: 12pt
linkstyle: bold
aspectratio: 169
urlcolor: blue
language: en-US
indent: true
colorlinks: true
geometry: margin=2cm
geometry: a4paper
section-titles: true
titlecolor: red
header-includes:
 - \usepackage{fvextra}
 - \DefineVerbatimEnvironment{Highlighting}{Verbatim}{breaklines,commandchars=\\\{\}}
 - \setlength{\parindent}{0pt}
 - \usepackage[pipeTables=true]{markdown}
 - \usepackage{titlesec}
 - \titleformat{\section}{\large\bfseries}{\thesection}{1em}{\hrule}
---

This project is a program that performs video analysis of a delimited trajectory for a car replica.
The algorithm runs on a Raspberry Pi, and communicates with an Arduino card that handles the robotic logic.

The trajectory is made of pairs of plastic cones: red cones are located on the left, and yellow cones on the right
side of the road.

The algorithm relies heavily on the OpenCV library.

## Video recognition

Using OpenCV, the program captures snapshots of the camera input at a fixed interval, and feeds them
to the algorithm described below. Snapshots are not taken for every frame of the video input, as that would
hinder performances and be useless for the robotic side, which would not be able to process these inputs in
time.

### Step 1: identification of the cones

First of all, the image is converted to the *Hue Saturation Value (HSV)* model.
This allows for better isolation of cones.
Arbitrary values are used to detect yellow and red pixels in the camera snapshot:
- Yellow: between HSV(30, 100, 100) and HSV(50, 255, 255)
- Red (appears white in HSV): between HSV(160, 100, 100) and HSV(180, 255, 255)
Only areas whose pixels are in these intervals are kept in the filtering result.

After this step, yellow and red cones are isolated for better processing.
Having them in different matrices eases the cost of deciding which side of the road a recognized zone of interest belongs to.

### Step 2: noise removal

Color filtering using the HSV domain can sometimes include undesired areas in the resulting matrix if their color is close to the one filtered.

To compensate this behavior, opening and closing algorithms are applied.
Their goal is to remove small and isolated areas in the filtered picture, with the supposition that they are either not cones, or cones that are too far away from the car.

The opening process consists of performing a morphological dilation following a morphological erosion, and closing is the opposite.
In order to keep only wide areas of the filtered picture, a bigger kernel size have been chosen for closing than for opening.

A downside of having split right and left cones from the first step is that opening and closing have to be called twice.
If this is performed on a merged matrix, there is a non-negligible chance to not be able to retrieve the cone color after the third step.

### Step 3: detection of connected components

After the previous steps, two matrices are obtained: one with red cones and an other one with the yellow ones.
Only the biggest areas have been kept, those are cones that are supposed to be the closest to the car.

First those two matrix needs to be merged in order to run the following algorithm only once.
Once this is done, we run a connected component detection algorithm to retrieve the blobs.
For each one of them, from their center pixel is retrieved their color.
This iteration aims at computing the lowest component for red and for yellow cones on the Y axis.
After those points are found, their center is returned. This should be the direction of the car.

## Serial communication

The Raspberry Pi is supposed to communicate its decisions with the Arduino card.
It achieves the former through its GPIO ports. In the code, the `libpigpio` has been used.

A simple protocol was put in place: the Raspberry Pi program writes each digit of the angle, including the optional minus sign, followed by a newline character.
When the Arduino card receives the newline character, it parses back the angle.

## Testing

The algorithm has been tested on given samples.
The program provides a visualization of the results consisting of a black and white view of retained components, together with an arrow indicating the selected direction.