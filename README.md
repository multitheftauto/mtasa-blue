## Multi Theft Auto: San Andreas

[![Build Status](https://github.com/multitheftauto/mtasa-blue/workflows/Build/badge.svg?event=push&branch=master)](https://github.com/multitheftauto/mtasa-blue/actions?query=branch%3Amaster+event%3Apush) [![Unique servers online](https://img.shields.io/endpoint?url=https%3A%2F%2Fmultitheftauto.com%2Fapi%2Fservers-shields.io.json)](https://community.multitheftauto.com/index.php?p=servers) [![Unique players online](https://img.shields.io/endpoint?url=https%3A%2F%2Fmultitheftauto.com%2Fapi%2Fplayers-shields.io.json)](https://multitheftauto.com) [![Unique players last 24 hours](https://img.shields.io/endpoint?url=https%3A%2F%2Fmultitheftauto.com%2Fapi%2Funique-players-shields.io.json)](https://multitheftauto.com) [![Discord](https://img.shields.io/discord/278474088903606273?label=discord&logo=discord)](https://discord.com/invite/mtasa) [![Crowdin](https://badges.crowdin.net/e/f5dba7b9aa6594139af737c85d81d3aa/localized.svg)](https://multitheftauto.crowdin.com/multitheftauto)

[Multi Theft Auto](https://www.multitheftauto.com/) (MTA) is a software project that adds network play functionality to Rockstar North's Grand Theft Auto game series, in which this functionality is not originally found. It is a unique modification that incorporates an extendable network play element into a proprietary commercial single-player PC game.

> **Note**
> If you're a fork developer, please read this note carefully. We have changed the default build type back to *CUSTOM* in `Shared/sdk/version.h`. If you're developing without the anti-cheat in mind, say in the Debug configuration, this doesn't affect you at all. Now, if you plan to test your custom client with anti-cheat enabled, you should change your build type to `UNTESTED`. If you want to publish a release of your custom client, you must switch to a *fork support* hardened release of `netc.dll`.
> Please read our [Forks_Full_AC](https://wiki.multitheftauto.com/wiki/Forks_Full_AC) wiki page for more information.

## Introduction

Multi Theft Auto is based on code injection and hooking techniques whereby the game is manipulated without altering any original files supplied with the game. The software functions as a game engine that installs itself as an extension of the original game, adding core functionality such as networking and GUI rendering while exposing the original game's engine functionality through a scripting language.

Originally founded back in early 2003 as an experimental piece of C/C++ software, Multi Theft Auto has since grown into an advanced multiplayer platform for gamers and third-party developers. Our software provides a minimal sandbox style gameplay that can be extended through the Lua scripting language in many ways, allowing servers to run custom created game modes with custom content for up to hundreds of online players.

Formerly a closed-source project, we have migrated to open-source to encourage other developers to contribute as well as showing insight into our project's source code and design for educational reasons.

Multi Theft Auto is built upon the "Blue" concept that implements a game engine framework. Since the class design of our game framework is based upon Grand Theft Auto's design, we are able to insert our code into the original game. The game is then heavily extended by providing new game functionality (including tweaks and crash fixes) as well as a completely new graphical interface, networking and scripting component.

## Gameplay content

By default, Multi Theft Auto provides the minimal sandbox style gameplay of Grand Theft Auto. The gameplay can be heavily extended through the use of the Lua scripting language that has been embedded in the client and server software. Both the server hosting the game, as well as the client playing the game are capable of running and synchronizing Lua scripts. These scripts are layered on top of Multi Theft Auto's game framework that consists of many classes and functions so that the game can be adjusted in virtually any possible way.

All gameplay content such as Lua scripts, images, sounds, custom models or textures is grouped into a "resource". This resource is nothing more than an archive (containing the content) and a metadata file describing the content and any extra information (such as dependencies on other resources).

Using a framework based on resources has a number of advantages. It allows content to be easily transferred to clients and servers. Another advantage is that we can provide a way to import and export scripting functionality in a resource. For example, different resources can import (often basic) functionality from one or more common resources. These will then be automatically downloaded and started. Another feature worth mentioning is that server administrators can control the access to specific resources by assigning a number of different user rights to them.

## Development

Our project's code repository can be found on the [multitheftauto/mtasa-blue](https://github.com/multitheftauto/mtasa-blue/) Git repository at [GitHub](https://github.com/). We are always looking for new developers, so if you're interested, here are some useful links:

* [Contributors Guide and Coding Guidelines](https://github.com/multitheftauto/mtasa-docs/blob/main/mtasa-blue/CONTRIBUTING.md)
* [Nightly Builds](https://nightly.multitheftauto.com/)
* [Milestones](https://github.com/multitheftauto/mtasa-blue/milestones)

### Build Instructions

#### Windows

Prerequisites
- [Visual Studio 2022](https://visualstudio.microsoft.com/vs/) with:
  - Desktop development with C++
  - Optional component *C++ MFC for latest v143 build tools (x86 & x64)*
- [Microsoft DirectX SDK](https://wiki.multitheftauto.com/wiki/Compiling_MTASA#Microsoft_DirectX_SDK)
- [Git for Windows](https://git-scm.com/download/win) (Optional)

1. Execute `win-create-projects.bat`
2. Open `MTASA.sln` in the `Build` directory
3. Compile
4. Execute: `win-install-data.bat`

Visit the wiki article ["Compiling MTASA"](https://wiki.multitheftauto.com/wiki/Compiling_MTASA) for additional information and error troubleshooting.

#### GNU/Linux

You can build the MTA:SA server on GNU/Linux distributions only for x86, x86_64, armhf and arm64 CPU architectures. ARM architectures are currently in **experimental phase**, which means they're unstable, untested and may crash randomly. Beware that we only officially support building from x86_64 and that includes cross-compiling for x86, arm and arm64.

**Build dependencies**

*Please always read the Dockerfiles for up-to-date build dependencies.*
*Note: ncftp is not required for building the MTA:SA server.*

- git
- make
- GNU GCC compiler (version 10 or newer)
- libncursesw6
- libncurses-dev
- libmysqlclient-dev

**Build instructions: Script**

**Note:** This script always deletes `Build/` and `Bin/` directories and does a clean build.

```sh
$ ./linux-build.sh [--arch=x86|x64|arm|arm64] [--config=debug|release]
$ ./linux-install-data.sh  # optional step
```

If build architecture `--arch` is not provided, then it's taken from the environment variable `BUILD_ARCHITECTURE` (defaults to: x64).

If build configuration `--config` is not provided, then it's taken from the environment variable `BUILD_CONFIG` (defaults to: release).

If you are trying to **cross-compile** to another architecture, then set `AR`, `CC`, `CXX`, `GCC_PREFIX` environment variables accordingly (see Dockerfile.arm64 for an example).

**Build instructions: Manual**

```sh
$ ./utils/premake5 gmake
$ make -C Build/ config=release_x64 all
$ ./linux-install-data.sh  # optional step
```

If you don't want to build the release configuration for the x86_64 architecture, you can instead pick another build configuration from: `{debug|release}_{x86|x64|arm|arm64}`.

#### GNU/Linux: Docker Build Environment

If you have problems resolving the required dependencies or want maximum compatibility, you can use our dockerized build environment that ships all needed dependencies. We also use this environment to build the official binaries.

**Pulling the Docker image**

```sh
$ docker pull ghcr.io/multitheftauto/mtasa-blue-build:latest
```

| Architecture | Docker image tag |
| ------------ | ---------------- |
| x86_64       | latest           |
| x86          | i386             |
| arm          | armhf            |
| arm64        | arm64            |

**Building with Docker**

These examples assume that your current directory is the mtasa-blue checkout directory. You should also know that `/build` is the code directory required by our Docker images inside the container. If the current directory is not a valid git repository, it instead create a (shallow) clone of the mtasa-blue repository. After compiling, you will find the resulting binaries in `./Bin`. To build the unoptimised debug build, add `-e BUILD_CONFIG=debug` to the docker run arguments.

| Architecture | Build command                                                                          |
| ------------ | -------------------------------------------------------------------------------------- |
| x86_64       | ``` docker run --rm -v `pwd`:/build ghcr.io/multitheftauto/mtasa-blue-build:latest ``` |
| x86          | ``` docker run --rm -v `pwd`:/build ghcr.io/multitheftauto/mtasa-blue-build:i386 ```   |
| arm          | ``` docker run --rm -v `pwd`:/build ghcr.io/multitheftauto/mtasa-blue-build:armhf ```  |
| arm64        | ``` docker run --rm -v `pwd`:/build ghcr.io/multitheftauto/mtasa-blue-build:arm64 ```  |

### Premake FAQ

#### How to add new C++ source files?

Execute `win-create-projects.bat`

## License

Unless otherwise specified, all source code hosted on this repository is licensed under the GPLv3 license. See the [LICENSE](./LICENSE) file for more details.

Grand Theft Auto and all related trademarks are © Rockstar North 1997–2024.
