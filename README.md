# Amazon_Bootcamp
A gas camera project

The project is actually a model of a gas camera, which detects temperature and creates an image accordingly, so that a high temperature will have a red color, and a low temperature will have a blue color.
The camera has a recording and a picture.
The recording consists of five stages that work in the pipeline:
1. Capture-creates a matrix of numbers that make up the temperature.
2. Conversion to RGB - each number in the matrix is converted to a color in the form of RGB.
3. Conversion to YUV - the colors are converted to YUV form.
4. Decode.
5. Write.
The image creates a JPEG file that represents the temperatures by the colors, as above.
