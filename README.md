[![Build Binary](https://github.com/jasonyang-ee/G431KB/actions/workflows/build-all.yml/badge.svg)](https://github.com/jasonyang-ee/G431KB/actions/workflows/build-all.yml)

# STM32G431 Playground

## Hardware Basics

Project Using STM32G431KB as Example. Test hardware is NUCLEO-G431KB.

STM32CubeMX File Included.

Hardware using jumper pin only to test out all functionalities.





## Build This Project

- Docker Build
```
docker run -v "{Your_Local_Full_Path}":"/home" jasonyangee/stm32_ubuntu:latest https://github.com/jasonyang-ee/STM32-CLI
```
> Visit Here for More Details: https://github.com/jasonyang-ee/STM32-Dockerfile
![Run](doc/img/run.gif)

- Manual Build
```
cmake -B build -G Ninja
cmake --build build
```
