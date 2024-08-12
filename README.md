# C++SDK_Framework

This project is a C++ framework for controlling jaka Corobot. It uses a C++ backend for performance and efficiency, and a React frontend for a dynamic and interactive user interface. The implementation and declaration of functionalities are divided into separate source files, providing several benefits:

- **Readability**: It makes the code easier to read and understand. The declaration (header file) provides an interface to the functionality implemented in the source file. This allows someone to understand what the code does without having to understand how it does it.

- **Reusability**: Header files can be included in multiple source files, allowing the functions and classes they declare to be used in different parts of your program.

- **Compiling**: Changes in a source file require only that file to be recompiled, not all files that include it. This can significantly reduce build times for large projects.

- **Encapsulation**: It hides the implementation details from the user. The user doesn't need to know how the function is implemented, only its inputs and output.

## Key Features

- **Performance**: C++ often used for low-level programming. Ideal for controlling a collaborative robot as it can handle the complex computations and large data involved in robot control. 

- **Real-time Interaction**: The Crow library used in the C++ backend enables real-time web server interfaces for robot control. 

- **Modularity**: Having a separate frontend (React/next/Vue) and backend (C++) increases modularity. This makes the code easier to maintain and extend, as changes in one part of the codebase are less likely to affect others. 

- **Scalability**: As the framework grows, both in terms of the complexity of robot control and the complexity of the user interface, the current setup will be able to handle it.

## Running the Project

To run the project, use Visual Studio. You can also use the provided batch file `run_program.bat` to automate the build and run process every time the PC starts.