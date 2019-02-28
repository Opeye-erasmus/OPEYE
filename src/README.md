# Opeye

Opeye aims to use a person's gaze as media input source. A screen coordinate is computed based on a user's gaze. In a showcase application, which is part of this source code, the screen contents are magnified with respect to a user's gaze in order to help a person with viewing deficiencies.

## Principle
Opeye's gaze estimation extracts the user's head & eyes portions of a webcam picture and feeds them to a set of neural networks. The neural networks do form a pyramid. At the first level the whole screen is partitioned into 4 parts. Depending on the user's gaze, at this level, a single neural network determines the screen quadrant. The selected quadrant represents a next neural network that is specialized on that region and can be used to choose a sub quadrant again. This process can be repeated to achieve the desired precision.

## Dependencies
- DLIB  (and for training, CUDA is nessecary, or a lot of time)
- OpenCV

## Build
See [building](BUILD.md) instructions.

## Components

### Tracker
The main library that can be used to teach and apply the neural networks as well as to serialize them.

### Tools
A tool set that is used to record labeled samples and to teach the neural networks.

### Record Samples:
Use `./tools -r` to start recording samples.

The subject must look at a given square and press the space bar to save a picture. Press escape to close the application.

### Training
Use  `./tools -t -l x -e y`  to train a neural netowrk. Where x is the number of levels and y the number of epoch to train for each nueral net.

I.e. `./tools -t -l 3 -e 1000` Trains 16+4+1 neural nets for 1000 epochs. (The complete network will be able to distinguish between 64 screen regions.

### Visualize
Use  `./tools -v -l x` to test the neural network. Where `x` corresponds to the level that should be evaluated at most.
Examples:
 - `./tools -v -l 1`
    - 4 regions will be distinguished
 - `./tools -v -l 2`
    - 16 regions will be distinguished
 - `./tools -v -l 3`
    - 64 regions will be distinguished

### Smoothened results with calibration
Use `./tools -m -l x` to visualize smoothend results. Where `x` corresponds to the level that should be evaluated at most. 

To calibrate, glance at a point and press the right mouse button. After four points, these will be used to calibrate. (Calibration only works if many levels are evaluated)


### Magnifier
A simple application showcasing the use of the tracker in a real life application. It captures the screen and magnifies the portion that is gazed at.

## Notes
We have proven that we can interpret a person's gaze and compute real time screen coordinates based on a webcam image.

The resulting success is limited by the computing power / resources available. In theory more memory, more GPU power and more labeled sample should yield better results.


Opeye was realized within the context of an Erasmus+ project.