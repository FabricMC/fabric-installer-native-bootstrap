# fabric-installer-native-bootstrap

A small native application that uses the Java downloaded by the Minecraft launcher to run the bundled fabric installer.

Use the `fabric-installer-native-bootstrap.sln` Visual Studio project to build on Windows.

Use the cmake project for Linux/MacOS. `mkdir ninja && cd ninja && cmake -G Ninja ../ && ninja`
Linux build deps: `glibc-static libstdc++-static cmake ninja-build`