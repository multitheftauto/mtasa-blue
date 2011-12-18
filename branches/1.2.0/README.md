Multi Theft Auto: San Andreas
-----------------------------

[Multi Theft Auto](http://www.multitheftauto.com/) (MTA) is a software project that adds network play functionality to Rockstar North's Grand Theft Auto game series, in which this functionality is not originally found. It is a unique modification that incorporates an extendable network play element into a proprietary commercial single-player PC game.

### Introduction

Multi Theft Auto is based on code injection and hooking techniques whereby the game is manipulated without altering any original files supplied with the game. The software functions as a game engine that installs itself as an extension of the original game, adding core functionality such as networking and GUI rendering while exposing the original game's engine functionality through a scripting language.

Originally founded back in early 2003 as an experimental piece of C/C++ software, Multi Theft Auto has since grown into an advanced multiplayer platform for gamers and third-party developers. Our software provides a minimal sandbox style gameplay that can be extended through the Lua scripting language in many ways, allowing servers to run custom created game modes with custom content for up to hundreds of online players.

Formerly a closed-source project, we have migrated to open-source to encourage other developers to contribute as well as showing insight into our project's source code and design for educational reasons.

Multi Theft Auto is built upon the "Blue" concept that implements a game engine framework. Since the class design of our game framework is based upon Grand Theft Auto's design, we are able to insert our code into the original game. The game is then heavily extended by providing new game functionality (including tweaks and crash fixes) as well as a completely new graphical interface, networking and scripting component.

### Gameplay content

By default, Multi Theft Auto provides the minimal sandbox style gameplay of Grand Theft Auto. The gameplay can be heavily extended through the use of the Lua scripting language that has been embedded in the client and server software. Both the server hosting the game, as well as the client playing the game are capable of running and synchronizing Lua scripts. These scripts are layered on top of Multi Theft Auto's game framework that consists of many classes and functions so that the game can be adjusted in virtually any possible way.

All gameplay content such as Lua scripts, images, sounds, custom models or textures is grouped into a "resource". This resource is nothing more than an archive (containing the content) and a metadata file describing the content and any extra information (such as dependencies on other resources).

Using a framework based on resources has a number of advantages. It allows content to be easily transferred to clients and servers. Another advantage is that we can provide a way to import and export scripting functionality in a resource. For example, different resources can import (often basic) functionality from one or more common resources. These will then be automatically downloaded and started. Another feature worth mentioning is that server administrators can control the access to specific resources by assigning a number of different user rights to them.

### Development

Our project's code repository can be found on the [multitheftauto/multitheftauto](http://github.com/multitheftauto/multitheftauto/) git repository at [GitHub](http://github.com/). We are always looking for new developers, so if you're interested, here are some useful links:

*   [Git Coding Guidelines](http://wiki.mtasa.com/index.php?title=Git_Coding_Guidelines)
*   [Nightly Builds](http://nightly.mtasa.com/)
*   [Bugtracker Roadmap](http://bugs.mtasa.com/roadmap_page.php)
*   [Wiki Roadmap](http://wiki.mtasa.com/index.php?title=Roadmap)

### Screenshots

![screenshot 1](http://www.mtasa.com/images/c_wipeout.png "Screenshot 1")
![screenshot 2](http://www.mtasa.com/images/c_bigpeds.png "Screenshot 2")
![screenshot 3](http://www.mtasa.com/images/c_yougofirst.png "Screenshot 3")

For in-game videos, head over to [MTAQA on YouTube](http://www.youtube.com/user/MTAQA).

### License

Unless otherwise specified, all source code hosted on this repository is licensed under the GPLv3 license. See the LICENSE file for more details.

Grand Theft Auto and all related trademarks are © Rockstar North 1997-2009.

