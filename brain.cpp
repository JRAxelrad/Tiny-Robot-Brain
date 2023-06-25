#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <random>
#include <vector>
#include <mutex>
#include <map>
#include <yaml-cpp/yaml.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

class PositionLoop {
public:
    PositionLoop(float position_) : position(position_) {
        YAML::Node config = YAML::LoadFile("../config.yaml");

        position = config["position"].as<float>();}

    void run() {
        while (true) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> distr(-0.1, 0.1);  // Adjust the range as needed
            float noise = distr(gen);
            float newPos = position * (1 + 0.001 * noise);
            setPosition(newPos);
            std::cout << "noise is: " << noise << std::endl;
            std::cout << "Position is: " << getPosition() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(4));
        }
    }

    float getPosition() const {
        return position;
    }

    void setPosition(float newPosition) {
        position = newPosition;
    }

private:
    float position;
    float newPosition;
};

class MotorsLoop {
public:
    MotorsLoop(int numOfMotors_) : numOfMotors(numOfMotors_) {
        YAML::Node config = YAML::LoadFile("../config.yaml");

        numOfMotors = config["num_motors"].as<float>();
        motorsPositions.resize(numOfMotors, 0.1);}

    void setNetFunctionPtr(py::function* netFunctionPtr_);

    int getNumOfMotors() {
        return numOfMotors;
    }

    void setNumOfMotors(int numOfMotors_) {
        numOfMotors = numOfMotors_;
    }

// Net function definitions.

    void setMotorsPosition(float newPos){
        for (int i = 0; i < numOfMotors; i++) {
            motorsPositions[i] = newPos;
    }
    }

    void setMotorsToZero(){
        for (int i = 0; i < numOfMotors; i++) {
            motorsPositions[i] = 0.0;
    }
    }

    void blockExcessivePositions(){
        for (int i = 0; i < numOfMotors; i++){
            if (motorsPositions[i] > 180.0){
                motorsPositions[i] = -180.0;
            };
        } 
    }
    

    void setMotorsToDifferentDirections(){
        for (int i = 0; i < numOfMotors; i++){
            motorsPositions[i] = std::pow((-1),i);
        }
    }

//End of net function definitions.

    void updateMotorsPositions(std::vector<double>& motorsPositions)
    {
        for (int i = 0; i < numOfMotors; i++) {
            motorsPositions[i] *= (1 + 0.1);
            std::cout<<"Motor number "<<i<<" is in position: "<< motorsPositions[i]<<std::endl;
        }
    }

    std::vector<double> getMotorsPositions() {
        return motorsPositions;
    }

    void run() {
        while (true) {
            updateMotorsPositions(motorsPositions);
            // std::cout<<"Motor number "<<i<<" is in position: "<< motorsPositions[i]<<std::endl;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    

private:
    int numOfMotors;
    std::vector<double> motorsPositions;
};

class Factory {
public:
    Factory() : netFunctions() {}

    void registerNetFunction(std::string name, py::function* netFunctionPtr) {
        netFunctions[name] = netFunctionPtr;
    }

    py::function* getNetFunction(std::string name) {
        std::cout << "This is the name of the netFunction: " << name << std::endl;
        return netFunctions[name];
    }

private:
    std::map<std::string, py::function*> netFunctions;
};


class MainLoop {
public:
    MainLoop() : positionLoop(nullptr), motorsLoop(nullptr), factory(nullptr), netFunction(), netFunctionName("") {
        std::cout << "Constructing MainLoop" << std::endl;
    }

    void run() {
        while (true) {
            // Access position from PositionLoop
            float position = positionLoop->getPosition();
            std::cout << "Position from PositionLoop: " << position << std::endl;

            // Access motor positions from MotorsLoop
            std::vector<double> motorPositions = motorsLoop->getMotorsPositions();
            for (int i = 0; i < motorPositions.size(); ++i) {
                std::cout << "Position of Motor " << i << ": " << motorPositions[i] << std::endl;
            }

            if (!netFunctionName.empty()) {
                // Access the Factory object and perform desired operations
                std::cout << "Factory object is available: " << netFunctionName << std::endl;

                // Use switch-case based on netFunctionName
                if (netFunctionName == "set_motors_to_zero") {
                    motorsLoop->setMotorsToZero();
                    positionLoop->setPosition(0.0);
                } 
                else if (netFunctionName == "set_motors_to_different_directions") {
                    motorsLoop->setMotorsToDifferentDirections();
                } 
                else if (netFunctionName == "move_motors_to_position_5"){
                    motorsLoop->setMotorsPosition(5.0);
                    positionLoop->setPosition(0.0);
                }
                else if (netFunctionName == "block_excessive_positions"){
                    motorsLoop->blockExcessivePositions();
                }
                else if (netFunctionName == "advance_regularly"){
                    continue;
                }

                
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void setFactory(Factory* factoryPtr_) {
        factory = factoryPtr_;
    }

    void setPositionLoop(PositionLoop* positionLoop_) {
        positionLoop = positionLoop_;
    }

    void setMotorsLoop(MotorsLoop* motorsLoop_) {
        motorsLoop = motorsLoop_;
    }

    void setNetFunction(py::function* netFunctionPtr_, const std::string& netFunctionName_) {
        netFunction = *netFunctionPtr_;
        netFunctionName = netFunctionName_;
    }

private:
    PositionLoop* positionLoop;
    MotorsLoop* motorsLoop;
    Factory* factory;
    py::function netFunction;
    std::string netFunctionName;
};



class Brain {
public:
    Brain() : mainLoop(), positionLoop(0.0), motorsLoop(2), factory() {}

    void setMainLoop(MainLoop mainLoop_) {
        mainLoop = mainLoop_;
    }

    void setPositionLoop(PositionLoop positionLoop_) {
        positionLoop = positionLoop_;
        mainLoop.setPositionLoop(&positionLoop);  // Pass the address of the PositionLoop object to MainLoop
    }

    void setMotorsLoop(MotorsLoop motorsLoop_) {
        motorsLoop = motorsLoop_;
        mainLoop.setMotorsLoop(&motorsLoop);  // Pass the address of the MotorsLoop object to MainLoop
    }

    void setFactory(Factory factory_) {
        factory = factory_;
    }

    void setNetFunction(std::string name) {
        mainLoop.setNetFunction(factory.getNetFunction(name), name);
    }

    void run() {
        std::thread mainLoopThread(&MainLoop::run, &mainLoop);
        std::thread positionLoopThread(&PositionLoop::run, &positionLoop);
        std::thread motorsLoopThread(&MotorsLoop::run, &motorsLoop);
        mainLoopThread.join();
        positionLoopThread.join();
        motorsLoopThread.join();
    }

private:
    MainLoop mainLoop;
    PositionLoop positionLoop;
    MotorsLoop motorsLoop;
    Factory factory;
};



PYBIND11_MODULE(brain_cpp, m) {

    py::class_<PositionLoop>(m, "PyPosLoop")
        .def(py::init<float>())
        .def("get_pos", &PositionLoop::getPosition)
        .def("set_pos", &PositionLoop::setPosition)
        .def("run", &PositionLoop::run);

    py::class_<MotorsLoop>(m, "PyMotorsLoop")
        .def(py::init<int>())
        // .def("update_motors_pos", &MotorsLoop::updateMotorsPositions)
        .def("get_num_of_motors", &MotorsLoop::getNumOfMotors)
        .def("run", &MotorsLoop::run)
        .def("get_motors_pos", &MotorsLoop::getMotorsPositions)
        .def("set_num_of_motors", &MotorsLoop::setNumOfMotors);

    py::class_<Factory>(m, "PyFactory")
    .def(py::init<>())
    .def("register_net_function", [](Factory& self, const std::string& name, py::function* netFunctionPtr) {
        self.registerNetFunction(name, netFunctionPtr);
    })
    .def("get_net_function", [](Factory& self, const std::string& name) {
        return self.getNetFunction(name);
    });


    py::class_<MainLoop>(m, "PyMainLoop")
        .def(py::init<>())
        .def("run", &MainLoop::run)
        .def("setFactory", &MainLoop::setFactory)
        .def("setPositionLoop", &MainLoop::setPositionLoop)
        .def("setMotorsLoop", &MainLoop::setMotorsLoop);

    py::class_<Brain>(m, "PyBrain")
        .def(py::init<>())
        .def("setMainLoop", &Brain::setMainLoop)
        .def("setPositionLoop", &Brain::setPositionLoop)
        .def("setMotorsLoop", &Brain::setMotorsLoop)
        .def("setFactory", &Brain::setFactory)
        .def("setNetFunction", &Brain::setNetFunction)
        .def("run", &Brain::run);
}