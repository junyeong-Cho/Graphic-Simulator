# Environment Setup

## Windows OS Setup

Make sure you have installed 

1. [Visual Studio](https://visualstudio.microsoft.com/downloads/)
	- Make sure C++ related packages are selected during the installation
2. [CMake](https://cmake.org/download/)
	- Make sure it is available on the command line / PATH if given the option during installation
3. [Git SCM](https://git-scm.com/downloads)
	- Make sure it is available on the command line / PATH if given the option during installation

## Web / Emscripten Setup

Note it is highly recommended that you should **not use the windows version of emscripten**. Because of line ending differences, the generated output files will most likely not work once they are uploaded to a web server. To avoid this issue, use Emscripten on a Linux based OS like Ubuntu or MacOS.

1. Read through their [Platform-specific notes](https://emscripten.org/docs/getting_started/downloads.html#platform-notes-installation-instructions-sdk) to see if you need to do anything for your platforms, like install python3.

2. Go to a directory where you would like to put the Emscripten tools and run the following:

```sh
# Get the emsdk repo
git clone https://github.com/emscripten-core/emsdk.git

# Enter that directory
cd emsdk

# Fetch the latest version of the emsdk (not needed the first time you clone)
git pull

# Download and install the SDK tools.
./emsdk install 3.1.53

# Make this version active" for the current user. (writes .emscripten file)
./emsdk activate 3.1.53

# Activate PATH and other environment variables in the current terminal
source ./emsdk_env.sh
```

## Ubuntu / Debian OS Setup

#### Install required tools

You will need **GNU** and related development tools, like **make** and **git**. 

```sh
sudo apt install build-essential git cmake libsdl2-dev libglew-dev
```

## Mac OS Setup

### Install Xcode:

Xcode is the integrated development environment (IDE) for macOS. It includes the necessary tools and compilers for C++ development.

1. Open the App Store on your Mac.
2. Search for "Xcode" and install it.
3. Launch Xcode once the installation is complete to complete the setup process.
4. Open Terminal (you can find it in Applications > Utilities or use Spotlight).
5. Run the following command to install Command Line Tools:
   
   ```bash
   xcode-select --install
   ```
6. Follow the on-screen instructions to complete the installation.

### Install Dev Tools & Libraries

1. **Install Homebrew** (if not already installed):

   Homebrew is a popular package manager for macOS. It simplifies the process of installing various software packages, including CMake.

   Checkout how to install it here [https://brew.sh](https://brew.sh)

2. **Install CMake, SDL2, and GLEW using Homebrew**:

   With Homebrew installed, you can easily install CMake from the terminal:

   ```bash
   brew install cmake sdl2 glew
   ```

   Homebrew will download and install the latest versions for you.




