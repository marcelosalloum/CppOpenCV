CppOpenCV
=========

PYTHON that runs C++ !!!

There are three projects within this git, separetad in 3 different folders:
- LibCppOpenCV: This is an Eclipse built C++ shared library. In this specific case, the library uses OpenCV libraries. To Run this project one will need to install the OpenCV.
- CppOpenCV: It is a C++ project with the AntiSHake dyfocus algorithm. It can also be used to test the library LibCppOpenCV and also relies on OpenCV libraries.
- python_cpp_interface: It is THE wrapping project. It contains two things:
  1. The compiler project used to interface the python with the C++ *.so dynamic library.
  2. An example of how to use the already built wrapping interface.


<b>1st Step: Set up development environment.<b/>
  1. If you dont have macports, install it (from http://www.macports.org/install.php). If you do have, update and upgrade it through command line as follows:
     - $ sudo port selfupdate
     - $ sudo port upgrade outdated
  2. Close and reopen the terminal.
  3. Install the OpenCV: $ sudo port install opencv
  4. Install Cython (case you wanna build the python_cpp_interface project): $ sudo easy_install cython
  5. Make your $DYLD_LIBRARY_PATH envirnomental variable point to your dynamic lib LibCppOpenCV:
     - To check its value type: $ echo $DYLD_LIBRARY_PATH
     - To add your LibCppOpenCV's path to the variable, type: $ export DYLD_LIBRARY_PATH=~/Projects/CppOpenCV/LibCppOpenCV/Debug/:$DYLD_LIBRARY_PATH
     - P.S.: if, when running your application, you get the following error: 
<i>R2D2:python_cpp_interface mac$ python userect.py aux/foot0.jpeg aux/foot1.jpeg
Traceback (most recent call last):
  File "userect.py", line 3, in <module>
    from rectangle import Rectangle
ImportError: dlopen(/Users/mac/CppOpenCV/python_cpp_interface/rectangle.so, 2): Library not loaded: libLibCppOpenCV.so
  Referenced from: /Users/mac/CppOpenCV/python_cpp_interface/rectangle.so
  Reason: image not found</i>, you must re-check the DYLD_LIBRARY_PATH variable.

<b>2rd Step: Run our antiShake script to get a piece of action<b/>
- In your terminal, go to the python_cpp_interface folder and run the userect.py file: $ python userect.py path_to_img1 path_to_img2
- Awesome! You shall now be running the antiShake algorithm !!!

==========
<b>Learning Material: OpenCV<b/>

OpenCV FAQs, learning material, last versions and some examples:
http://opencv.willowgarage.com/wiki/

OpenCV C++ cheatsheet:
http://opencv.willowgarage.com/wiki/Welcome?action=AttachFile&do=view&target=opencv_cheatsheet.pdf

OpenCV Full Documentation:
http://docs.opencv.org/master/index.html

==========
<b>Learning Material: Cython<b/>

Cython Tutorial:
http://docs.cython.org/src/userguide/wrapping_CPlusPlus.html?highlight=cpp#

Cython implemented example:
http://wiki.cython.org/WrappingCPlusPlus?action=AttachFile&do=get&target=cythoncpp.tgz
