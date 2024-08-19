<div align="center"><img src="docpages/DPP-markdown-logo.png" alt="DPP"/>
<h3>An incredibly lightweight C++ Discord library</h3>
<br />

[![Discord](https://img.shields.io/discord/825407338755653642?style=flat)](https://discord.gg/dpp)
![Downloads](https://dl.dpp.dev/dlcount.php)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/39b054c38bba411d9b25b39524016c9e)](https://www.codacy.com/gh/brainboxdotcc/DPP/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=brainboxdotcc/DPP&amp;utm_campaign=Badge_Grade) [![OpenSSF Best Practices](https://bestpractices.coreinfrastructure.org/projects/7726/badge)](https://bestpractices.coreinfrastructure.org/projects/7726)
[![D++ CI](https://github.com/brainboxdotcc/DPP/actions/workflows/ci.yml/badge.svg)](https://github.com/brainboxdotcc/DPP/actions/workflows/ci.yml)
[![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/brainboxdotcc/DPP/badge)](https://securityscorecards.dev/viewer/?uri=github.com/brainboxdotcc/DPP)
[![AUR version](https://img.shields.io/aur/version/dpp)](https://aur.archlinux.org/packages/dpp)
![vcpkg version](https://img.shields.io/vcpkg/v/dpp)
[![Homebrew version](https://img.shields.io/homebrew/v/libdpp)](https://formulae.brew.sh/formula/libdpp#default)
[![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.1-4baaaa.svg)](CODE_OF_CONDUCT.md)

</div>

<hr />

D++ is a lightweight and efficient library for **Discord** written in **modern C++**, covering as much of the API specification as possible with an **incredibly small memory footprint** even when caching large amounts of data.

### Library Features

* Support for Discord API v10
* Really small memory footprint
* Efficient caching system for guilds, channels, guild members, roles, users
* Sharding and clustering (Many shards, one process: specify the number of shards, or let the library decide)
* Highly optimised ETF (Erlang Term Format) support for very fast websocket throughput
* [Slash Commands/Interactions support](https://dpp.dev/slashcommands.html)
* [Voice support](https://dpp.dev/soundboard.html) (sending **and** receiving audio)
* The entire Discord API is available for use in the library
* Stable [Windows support](https://dpp.dev/buildwindows.html)
* Ready-made compiled packages for Windows, Raspberry Pi (ARM64/ARM7/ARMv6), Debian x86/x64, and RPM based distributions
* Highly scalable for large amounts of guilds and users

Want to help? Drop me a line or send a PR.

This library is in use on [TriviaBot](https://triviabot.co.uk/) and [Sporks bot](https://sporks.gg) and many other bots!

## 📚 Documentation

The documentation is constantly evolving and improving, generated from the code comments and markdown examples using Doxygen.

#### [View the D++ library documentation](https://dpp.dev/)

### Example

This is a simple ping-pong example using slash commands.

```cpp
#include <dpp/dpp.h>
#include <cstdlib>

int main() {
	dpp::cluster bot(std::getenv("BOT_TOKEN"));

	bot.on_slashcommand([](auto event) {
		if (event.command.get_command_name() == "ping") {
			event.reply("Pong!");
		}
	});

	bot.on_ready([&bot](auto event) {
		if (dpp::run_once<struct register_bot_commands>()) {
			bot.global_command_create(
				dpp::slashcommand("ping", "Ping pong!", bot.me.id)
			);
		}
	});

	bot.start(dpp::st_wait);
	return 0;
}
```

You can find more examples in our [example page](https://dpp.dev/example-programs.html).

## 💻 Supported Systems

### Linux

The library runs ideally on **Linux**.

### Mac OS X, FreeBSD, and OpenBSD

The library is well-functional and stable on **Mac OS X**, **FreeBSD**, and **OpenBSD** too!

### Raspberry Pi

For running your bot on a **Raspberry Pi**, we offer a prebuilt .deb package for ARM64, ARM6, and ARM7 so that you do not have to wait for it to compile.

### Windows

**Windows** is well-supported with ready-made compiled DLL and LIB files, please check out our [Windows Bot Template repository](https://github.com/brainboxdotcc/windows-bot-template). The Windows Bot repository can be cloned and integrated immediately into any Visual Studio 2019 and 2022 project in a matter of minutes.

### Other OS

The library should work fine on other operating systems as well, and if you run a D++ bot on something not listed here, please let us know!

## 🔰 Getting Started

### Installation

D++ can be easily installed using various package managers. Please refer [to our documentation](https://dpp.dev/md_docpages_01_installing.html) for installation tutorials based on your preferred package manager.

### Building from Source

If you prefer to build the library from source, detailed instructions are available [here](https://dpp.dev/install-from-source.html).

### FAQ

For frequently asked questions and their answers, please visit our [FAQ page](https://dpp.dev/md_docpages_01_frequently_asked_questions.html).

### Nightly Builds

If you prefer to use Nightly Builds (This is only if you know what you are doing!) then you can use either our [master nightly builds](https://nightly.link/brainboxdotcc/DPP/workflows/ci/master) or our [dev nightly builds](https://nightly.link/brainboxdotcc/DPP/workflows/ci/dev).

## 🤝 Contributing

Contributions, issues and feature requests are welcome. After cloning and setting up the project locally, you can just submit a PR to this repo and it will be deployed once it's accepted.

Please read the [D++ Code Style Guide](https://dpp.dev/coding-standards.html) for more information on how we format pull requests.

## 💬 Get in touch

If you have various suggestions, questions or want to discuss things with our community, [Join our discord server](https://discord.gg/dpp)! Make a humorous reference to brains in your nickname to get access to a secret brain cult channel! :)

[![Discord](https://img.shields.io/discord/825407338755653642?style=flat)](https://discord.gg/dpp)

## 💕 Show your support

We love people's support in growing and improving. Be sure to leave a ⭐️ if you like the project and also be sure to contribute, if you're interested!

## 📂 Dependencies

### Build requirements

* [cmake](https://cmake.org/) (version 3.13+)
* A supported C++ compiler from the list below

### Supported compilers

* [g++](https://gcc.gnu.org) (version 8 or higher)
* [clang](https://clang.llvm.org/) (version 6 or higher)
* AppleClang (12.0 or higher)
* Microsoft Visual Studio 2019 or 2022 (16.x/17.x)
* [clang-cl](https://learn.microsoft.com/en-us/cpp/build/clang-support-msbuild) (version 15 or higher)

Other compilers may work (either newer versions of those listed above, or different compilers entirely) but have not been tested by us.

### External Dependencies (You must install these)

* [OpenSSL](https://openssl.org/) (whichever `-dev` package comes with your OS)
* [zlib](https://zlib.net) (whichever `-dev` package comes with your OS)

#### Optional Dependencies

For voice support you require both of:
* [libopus](https://www.opus-codec.org)
* [libsodium](https://libsodium.org/)

### Included Dependencies (Packaged with the library)

* [JSON for Modern C++](https://json.nlohmann.me/)
