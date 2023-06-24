# Tiny-Robot-Brain
Tiny-Robot HW
This program is an implementation of a multi-motor robot that gives a feedback of its position and the position of its motors.
The core code is written in C++ and is wrapped by pybind11 module for a python based interface file.
in order to activate this program a few requirements must be met:
  1. You must download the files given in this repo.
  2. You need the pybind11 mdule which is easily downloadable at: [pybind11](https://github.com/pybind/pybind11)
  3. You also need to download and build on your system the yaml-cpp repo which is available at: [yaml-cpp](https://github.com/jbeder/yaml-cpp)
  4. The number of motors, starting position and nature of the robot are configurable via the config.yaml file.
  5. The project needs to be build using cmake.
