# EsCarGo - An autonomous go-kart

We're building an autonomous Go-Kart!

## Simulation

Please read the [README](./simulation/README.md)

## Getting Started

Run `install.sh -o windows|linux|mac`. To build run `install.sh -b -o windows|linux|mac`.

### VSCode support for toolchain

You'll need the cmaketools plugin and then scan for kits. Search the Pallete for "CMake: Edit Local-User CMake Kits and add `"toolchainFile": "${workspaceFolder}/externals/vcpkg/scripts/buildsystems/vcpkg.cmake"` to your kit. To build the toolchain, you must run `install.sh` at least once, you do not need to run it before each build.