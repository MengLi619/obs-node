## overview

This project is heavily reference to the [obs-headless](https://github.com/a-rose/obs-headless). 
And wrap the [obs-studio](https://github.com/obsproject/obs-studio) with [node-addon-api](https://github.com/nodejs/node-addon-api)
to used by NodeJs application.

This project will **prebuild** the obs-studio with DISABLE_UI mode, and as well as the node wrapper. Prebuild libs will
upload to the github packages.

While the package is installed from github, prebuild libs will also be downloaded from github packages. without the need 
to build from user machine. Currently macos, linux (ubuntu20.04) is supported.

## scripts
1. Build prebuild libs
    ```shell script
    bash scripts/build.sh <all/obs-studio/obs-node>
    ```
2. Upload to github packages
    ```shell script
    npm run upload
    ```
   
## Docker env
Sometimes, there is a need to build/test linux prebuilds in the local machine (MacOS), a docker env is provided in the
project. Run
```shell script
bash docker/build-and-run.sh
```
to run and attache the docker env. 
