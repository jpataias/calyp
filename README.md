plaYUVer
========

plaYUVer is an open-source raw video player


Usage
----------------------------------------------------------------

Command line interface
./playuver -i [Input File] -w [Width] -h [Height]


Install using cmake
----------------------------------------------------------------

The following are required for compilation:
- an ordinary C++ development environment (g++, make, C++ libraries and headers, ...)
- cmake
- a Qt development environment (libraries, headers, qmake, ...): only QtCore, QtGui and QtWidgets are required, at least in version 4

Dependencies:
- QT
- OpenCv (optional)

Run the following commands: (this will create an executable named playuver)
  mkdir build
  cd build
  cmake ../
  make