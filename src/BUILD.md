# Build OPEYE

## Linux

Download dlib (i.e. version 19.13)
Link it to the opeye/dlib folder:
ln -s  /path/to/dlib-19.13/ dlib

Install opencv, then cuda & cudnn.

- mkdir build
- cd build
- cmake ..

### CUDA & CUDNN
Download & install cuda:
- https://developer.nvidia.com/cuda-downloads

`sudo dpkg -i cuda-repo-ubuntu1710-9-2-local_9.2.88-1_amd64.deb`
`sudo apt-key add /var/cuda-repo-<version>/7fa2af80.pub`
`sudo apt-get update`
`sudo apt-get install cuda`
Make sure the right NVIDIA driver is installed (it will complain about unmet dependencies otherwise)


Download & install libcudnn runtime & development version (you need an nvidia account for this):
- https://developer.nvidia.com/rdp/cudnn-download

(on ubuntu make sure to use the deb packages => easiest)


## Windows
- Started using the [Window Development Env image](https://developer.microsoft.com/en-us/windows/downloads/virtual-machines).
 - But it might be enough to install the "Visual Studio Build Tools" on your computer.
- Install git (git-scm.com)
- Install cmake (use MSI version, chose the "ass cmake to the system path for all (or current) users  option")
- git clone <repo>
- cd to the repo
- Load json lib via: cd lib && mv json json.old && git clone https://github.com/nlohmann/json.git json (make sure there is a src folder => maybe rename include to src)
- download dlib / extract. DLib will be build together with the project. But if you want to test DLib, build as described [here](http://dlib.net/compile.html). (To use CUDA, install it before building, it is a must to train the neural networks)
- Download the lastest opencv source release. Extract to c:\opencv Build it (mkdir build && cd build && cmake .. && cmake --build . --config Release)
- Link the latest dlib version in th src folder. (Or extract it there)
- Download / install freeglut: (https://www.transmissionzero.co.uk/software/freeglut-devel/) extract it to c:
- Download / Install qt (use the online installer, install fhe latest prebuild for msvc 2017.
- mkdir build
- cd build
- cmake ../src/ -DCMAKE_PREFIX_PATH="C;\\opencv\\build;C:\\freeglut;C:\Qt\5.11.0;C:\\Qt\\5.11.0\\msvc2017_x64 -A x64"
- cmake --build .  --config Release


