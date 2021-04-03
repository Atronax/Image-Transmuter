# Image-Transmuter
Demo project, used to remember qt concepts, source tree and git.

Allows to encode text messages into input images. 

Besides input image, user can choose other parameters of conversion through controls. 
Among them are:
- Font: family, point size, italic and bold variables;
- Text: from txt file, may be changed later through textbox;
- Scale: how much the pixel of output image is greater than pixel of input image, should be at least 1 point higher than font point size to get nice results;
- Threshold: if user don't want to change pixels with red\green\blue component less than corresponding threshold value, he can set this variable for that purpose; hsv colorspace could be more interesting, but that is task for some other epoch.

Status label uses qt signal-slot connection to gather current process message into a list and shows the current message. It also has the ability to move across message by wheel scroll.
Could be used as logging system, if we output the gathered data into some sort of file.

When convert button is clicked, image is divided between several threads, which do their task separately and return the value when done. Main loop stiches the images into a single output image
and show the result on right preview widget. User can look the result by double clicking on it and save if he pleased.

User interface class and logic class instance are communicating using qt pigeon mail system. Manager makes connections for these.
