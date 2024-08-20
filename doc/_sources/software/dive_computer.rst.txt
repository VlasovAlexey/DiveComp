Dive Computer
========================================
These are the sources for flashing the DIVE COMPUTER board micro:bit as well.

Dependencies
------------------------
- `GNU Arm Embedded Toolchain <https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads>`_
- `Git <https://git-scm.com>`_
- `CMake <https://cmake.org/download/>`_
- `Python 3 <https://www.python.org/downloads/>`_
- `Ninja Build <https://ninja-build.org/>`_ (only needed on Windows)

For example, to install these dependencies on Ubuntu 18.x:

.. code:: console

    sudo apt install gcc
    sudo apt install git
    sudo apt install cmake
    sudo apt install gcc-arm-none-eabi binutils-arm-none-eabi


Building the project
------------------------

- Install the dependencies
- Clone this repository

.. code:: console
    
    git clone https://github.com/VlasovAlexey/DiveComputer.git

- In the root of this repository run the ``build.py`` script
  
.. code:: console

    cd DiveComputer/dive_computer/src
    python3 build.py

- The ``MICROBIT.hex`` hex file will be built and placed in the root folder.
- Copy the ``MICROBIT.hex`` file into the ``MICORBIT`` USB drive to flash the micro:bit

.. Tip::
    
    The DIVE COMPUTER sources run under CODAL. If you want to customize for Windows or MacOS you can read more about it `here <https://github.com/lancaster-university/codal>`_.
