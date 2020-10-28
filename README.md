## Overview

This project is heavily reference from the [obs-headless](https://github.com/a-rose/obs-headless). 
And wrap the [obs-studio](https://github.com/obsproject/obs-studio) with [node-addon-api](https://github.com/nodejs/node-addon-api)
to used by NodeJs application.

This project will **prebuild** the obs-studio with DISABLE_UI mode, and as well as the node wrapper. Prebuild libs will
upload to the github packages.

While the package is installed from github, prebuild libs will also be downloaded from github packages. without the need 
to build from user machine. Currently windows(64bit), macos, linux (ubuntu20.04) is supported.

## Windows build requirements
[7z](https://www.7-zip.org/a/7z1900-x64.exe)    
[Visual Studio 2019](https://visualstudio.microsoft.com/vs/)    
[Windows 10 SDK (minimum 10.0.19041.0)](https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk/)

## Scripts
1. Build prebuild libs for macos/linux
    ```shell script
    bash scripts/build.sh <all/obs-studio/obs-node>
    ```
2. Build prebuild libs for windows
    ```cmd
    scripts/build-windows.cmd <all/obs-studio/obs-node>
    ```
3. Upload to github packages
    ```shell script
    npm run upload
    ```
   
## Docker env
Sometimes, there is a need to build/test linux prebuilds in the local machine (MacOS), a docker env is provided in the
project. Run
```shell script
bash docker/build-and-run.sh
```
to run and attach the docker env. 
