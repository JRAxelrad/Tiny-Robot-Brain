import build.brain_cpp as brain_cpp
from build.brain_cpp import *
from net_functions import *
import yaml

def main():
    # Load YAML configuration
    with open('config.yaml', 'r') as file:
        config = yaml.safe_load(file)

    # Create brain object and set up its components
    brain = brain_cpp.PyBrain()

    position_loop = brain_cpp.PyPosLoop(config['position'])
    motors_loop = brain_cpp.PyMotorsLoop(config['num_motors'])
    factory = brain_cpp.PyFactory()

    for name, net_function in config['net_functions'].items():
        module = net_function['module']
        function = net_function['function']
        func_ptr = getattr(__import__(module), function)
        factory.register_net_function(name, func_ptr)

    brain.setPositionLoop(position_loop)
    brain.setMotorsLoop(motors_loop)
    brain.setFactory(factory)
    brain.setNetFunction(config['desired_net_function'])

    # Run the brain
    brain.run()

    

if __name__ == '__main__':
    main()

    