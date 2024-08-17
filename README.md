# CompWolf

CompWolf is a set of libraries that together can be used to create video game and other applications.
The libraries' source code can be found under the directory "CompWolf.Programs".
CompWolf includes a website with tutorials and documentation. The website's source code can be found under the directory "CompWolf.Docs".

## CompWolf.Programs
CompWolf.Programs is compiled using makefiles in Linux. It is set up to work with Visual Studio on Windows.
CompWolf uses Vulkan (for communicating with the GPU), GLFW (for creating windows) and glslc (compiler for shaders); look at the following installation guide on how to set them up to be able to build CompWolf.Programs.

### Installation Guide

Warning: These installation guides have yet to be tested, and so may be wrong.

#### Linux Mint

Get GLFW by running the command "sudo apt install libglfw3".
Get Vulkan by running the command "sudo apt install libvulkan-dev".
Get shaderc, which includes glslc, from: https://github.com/google/shaderc/blob/main/downloads.md

You should now be able to build the programs.
Try going to CompWolf.Programs and run "./build.sh" to build the projects.
You may need to first run the command "chmod +x ./build.sh" to be able to run build.sh.

#### Windows

Install GLFW from: https://www.glfw.org/download.html
Install the Vulkan SDK from: https://vulkan.lunarg.com/

Where you installed the VulkanSDK, you should find two directories "Include" and "Lib"; copy them to "CompWolf.Programs/exern/VulkanSDK"
Where you installed the VulkanSDK, you should find a file "glslc.exe" in the directory "Bin"; copy it to "CompWolf.Programs/exern/VulkanSDK".

You should now be able to build the programs.
Try opening CompWolf.Programs in Visual Studio and build the project.
Alternatively, try another way to use cmake to build CompWolf.Programs.
