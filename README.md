# cos_camera

This is the version of accelerated recorder for XIMEA cameras. The acceleration is done by OpenMP parallel computing.

In order to compile the app, the following dependencies must be resolved:
 - OpenCV >=3.4.4
 - nlohmann_json
 - XIMEA xiapi package.
 
In order to compile the app, choose the makefile suitable for your OS and rename it from Makefile.* to Makefile. Then just type make and Enter.
If you compile the app for OS X, you should have clang++ with extensions installed. The extensions are necessary for OpenMP support.
