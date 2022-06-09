# Induction Motors Fault Detection Firmware Prototype

###	Getting started
TODO: how to install, what to install (versions), load submodules, LabVIEW soft
required software and hardware

###	System arcitecture
TODO: describe threads and what is going on
What used: FreeRTOS, etc...

###	Algorithm description
TODO: add documentation...

###	How to debug system
TODO: describe system possible debugs

###	Repo structure

 
```C
├── docs
├── projects
	├─application
	├─bootloader
	├─samples
	├─tests
├── sdk
└── sources
	├─application
	├─bootloader
	├─common
```
          
- **docs** - documentation: datasheets, app notes, project's documentation
- **projects** - IDE's projects
    + **application** - IDE's projects for application
    + **bootloader** - IDE's projects for bootloader
    + **samples** - samples and examples
    + **tests** - projects for tests
- **sdk** - external components, libraries and tools
- **sources** - source code
    + **application** - source files and configurations of the application
    + **bootloader** - source files and configurations of the bootloader
    + **common** - shared modules between applications

#### Addition information
...
