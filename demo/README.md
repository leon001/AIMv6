Demo directory
==========

Put hardware demo sources (e.g. an example of reading disk sectors,
an example of writing characters to a serial console) here as
quick references of **how to use those devices end-to-end**, to allow
users interested in driver framework design focus on framework itself
rather than hardware specifications (as they are very boring).

### Requirements

The code here has to be correct, of course.

The code should be end-to-end.

Since the code here is for demonstrations, comments should be as
detailed as possible.  Every magic number should be explained.

One source file/directory per device or device-machine combination.

Future devices to be supported (e.g. Loongson 3A VGA driver, USB
devices, etc.) can have their demos placed here.

