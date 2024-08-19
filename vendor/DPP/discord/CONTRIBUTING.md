# Contributing

When contributing to this repository, please do not feel intimidated! We welcome PRs from developers of all levels of experience and we were all new once.

## Pull Request Process

1. Pull requests should be made against the `dev` branch.
2. Ensure that the changed library can be built on your target system. Do not introduce any platform specific code.
3. Ensure that all methods and functions you add are **fully documented** using doxygen style comments.
4. Test your commit! Make a simple single-file test bot to demonstrate the change, include this with the PR as an attached file on a comment, so we can test and see how it works.
5. Ensure that you do not break any existing API calls without discussing on Discord first!
6. Be sure to follow the coding style guide (if you are not sure, match the code style of existing files including indent style etc.).
7. Your PR must pass the CI actions before being allowed to be merged. Our PR actions check that the build will compile on various platforms before release and make precompiled versions of the library.
8. Automated changes e.g. via Grammarly or a static analysis tool will not usually be accepted into the code without proper thought out justification (by a human being, not an AI or an App) as to why the changes are required. Generally a PR should do more than fix a single spelling error for example as this just takes precious time for something which could have been resolved by a direct commit to the dev branch.
