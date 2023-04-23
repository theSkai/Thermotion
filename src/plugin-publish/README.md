# Thermotion
![Thermotion](/images/teaser.jpg)

This is the supplementary repository for 'Thermotion: Design and Fabrication of Thermofluidic Composites for Animation Effects on Object Surfaces' paper (ACM CHI2023). 
The work studies the mechanism, design space, and the design and fabrication workflow of Themotion composites, which utilize thermofluidic flows in fluidic channels embedded under the object surfaces, dynamically controlling the surface temperature as an actuator for thermochromic animation effects. 

Here we provide the instructions on installation and usage of Thermotion Design Tool, source code for secondary development, and 3D-printing models of most examples we used in the research for replication. We would thank Tianyu Yu and Weiye Xu for the contribution to the repository.

#### Contents

- [Installation and Usage of Thermotion Design Tool](#installation-and-usage-of-thermotion-design-tool)
- [Secondary Development](#secondary-development)
- [3D Models for Replication](#3d-models-for-replication)
- [Citation and License](#citation-and-license)

## Installation and Usage of Thermotion Design Tool

The program package of the design tool is in [./bin](/bin) folder, including one '.gh' file, one '.3dm', a pre-compiled program 'SIMULATOR', and a 'Tool' folder which consists of four '.gha' files. You can download and use the tool through a few steps of installation.

### Configuration Before Using

Our design tool runs on Rhino and Windows Subsystem for Linux (wsl) on Windows, so you may install [Rhino](https://www.rhino3d.com/) and [wsl](https://learn.microsoft.com/en-us/windows/wsl/install) (We recommend [Ubuntu](https://ubuntu.com/)) in advance to launch the design tool software. Also, several plugins for Rhino Grasshopper need to be installed, which are: 

- Pufferfish
- Weaverbird
- MetaHopper
- Human
- Human UI
- Thermotion Plugin

Except for Thermotion Plugin, you can get the latest version of other plugins at [Food4Rhino](https://www.food4rhino.com/), where most of the plugins can be installed following the instructions on their download page or [FAQ | Food4Rhino](https://www.food4rhino.com/en/faq#users-install-grasshopper-plugin). To install Thermotion Plugin, you simply copy the four '.gha' files in [./bin/Tools](/bin/Tools) into the component folder of grasshopper just like normal plugins.

The design tool can run correctly on Rhino 7.21, and wsl (Ubuntu 20.04). You can run 'wsl' on your command line (terminal) to check whether the wsl is installed and configured correctly, and you can open the 'design tool.gh' file to check whether plugins missing. 

### Usage and Tips

#### Launch the design tool
1. Always double-click a .3dm file to open it with Rhino first. Next, open GrassHopper in Rhino, then opens the .gh file to pop out the GUI console. (This will guarantee the correct directory path configuration.)
1. We provide a template 'design tool template.3dm' file (A Mobius example) with configured render settings and parameters, you can use this template to test whether the 'design tool.gh' runs correctly and also create your own project based on the template.
1. To use the design tool, you may follow the steps presented in our paper for further instructions, as well as the paragraphs below for tips and cautions.

#### Fluidic structure design tips
1. All the *surface mesh* or *solid mesh* as inputs for modeling are supposed to be obtained from the same 3D model, i.e., they can be slightly different digitally but should fit together geometrically. You may prepare these geometries before using the tool.
1. All the manual guidelines as inputs for fluidic channel generation, that are supposed to fit the 3D model surface, should be *polyline* near the 3D model surface. You may use `PolylineOnMesh` in Rhino to create these guidelines in advance, and then pick them into the design tool.
1. The current version doesn't support mesh-like fluidic channel structure generation (while dose support for simulation, see below).

#### Simulation tips
1. Always click the *Update and Save Configuration* button before starting the simulation when applying any changes to the configuration.
1. The automatically generated 'Temper' folder saves the heat distribution every 0.5 seconds, and to see the record of the simulation process, you may not delete files in 'Temper' folder.
1. If you want to get a more accurate temperature on points, you can use *check points* tool in *Advance* panel, and read the result from the 'Checkpoints.txt' file after simulation. Each line in 'Checkpoints.txt' represents the temperature of 
the preset points sequentially, and every 0.5 seconds the program will write the current temperature of checkpoints in a line (e.g., the 5th line is the temperature at 2 sec).
1. To simulate mesh-like fluidic channels, in the design stage, firstly import the multiple poly-channel lines by *Customized lines* in Step1, and choose *Intersection Method (fixed diameter)* in Step2 and ignore the error of structure generation. Next, directly open the simulation panel and choose *mesh-like* for *water type*, then finish the other operations.

## Secondary Development

Source codes are in [./src](/src) folder, including 'code-publish' (source code of Simulator) and 'plugin-publish' (source code of Thermotion plugins in grasshopper). The source file for fluidic structure design is the './bin/design tool.gh' file.

### Fluidic Structure Design

We build components to create the channel lines and generate the 3D fluidic channel structure according to the channel lines. The components and the algorithms can be found, referred to, and utilized for secondary development directly in the 'design tool.gh' file. 

### Simulator

We adopt Finite Volume Method (FVM) as the fundamental structure. As illustrated in the paper, simulation goes through three layers and several simplifications are applied to accelerate calculation. Specifically, layer meshes are organized as a 3-D tree, and the central line of the liquid conduit is segmented and discretized into points (or polyline). We adopt forward deduction to solve heat distribution, and instructions (changes on water) can be executed alongside simulation. Details can be referred to from the source code.

To compile and link (get an executable program), you may run the command line below in the folder that contains 'run_all.sh'.

```
bash ./run_all.sh
```

Our source code structure doesn't rely on any external dependencies or plugins, which means that you can use any compiler with CMake suite to compile the program. After running the command line above, you can get the executable program in the 'bin' folder. It is important to point out that the currently used SIMULATOR was compiled using the attached CMake suite of Ubuntu 20.04, which may not executable in some systems.

### Grasshopper plugins 

Grasshopper and simulator exchange information by constantly writing and reading the same temporary files. It is a primitive way and can be optimized if needed. Those four plugins correspond to four folders of source code respectively as follows:

- SimulatorLinker <=> ThermotionDesignTool1
- SimulatorSetter <=> ThermotionDesignTool2
- SimulatorStarter <=> ThermotionDesignTool3
- SimulatorClassifier <=> ThermotionDesignTool4

grasshopper plugins use Microsoft Visual Studio Community 2022 (64), Version 17.3.4 to compile, and the .NET Framework version is 4.8.04084.

## 3D Models for Replication
We provide 3D printed models for most examples in the paper in [./3D model](/3D%20model) folder, including design primitives and application prototypes. You may download and try to 3D-print these models for replication of the examples in the paper. 

## Citation and License 

When using or building upon this device/work in an academic publication, please consider citing as follows:
...

This project follows CC BY-NC-SA 4.0 License, see [Creative Commons — Attribution-NonCommercial-ShareAlike 4.0 International — CC BY-NC-SA 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode). This project uses the vector computing module (vecmath) from the source code of Basic Computer Graphics course in Tsinghua on 2022/3/18. Document (on 2022/3/18) from the source code of that course claim to follow the CC BY-NC-SA 4.0 License and partially borrow code from MIT Open Courseware, see [OCW Course Index | MIT OpenCourseWare | Free Online Course Materials](https://ocw-origin.odl.mit.edu/courses/) 

## Contact
For questions or further information, contact <yty21@mails.tsinghua.edu.cn>.
