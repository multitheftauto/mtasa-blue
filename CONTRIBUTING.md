So you've decided to become a contributor to our project. Excellent!
But before we can start accepting your code, there are a couple of
things you should know about how we work. These are mostly guidelines
and rules as to how your code should be structured and how it can be
committed without upsetting any fellow contributors.

Our project's code repository can be found on the [multitheftauto/mtasa-blue](https://github.com/multitheftauto/mtasa-blue/) Git repository at [GitHub](https://github.com/). We are always looking for new developers, so if you're interested, here are some useful links:

* [Nightly Builds](https://nightly.mtasa.com/)
* [Issue Tracker](https://github.com/multitheftauto/mtasa-blue/issues)
* [Wiki Roadmap](https://wiki.mtasa.com/wiki/Roadmap) <!--TODO: this page is mostly useless now -->

# Overview

## Directory Structure

Our project is organised into a number of different
directories which serve different purposes:

-   **Client**
-   **Server**
-   **Shared**: contains code shared between both the client and the
    server
-   **utils**: utilities used to automate certain tasks
-   **vendor**: unmodified third-party code and libraries (optionally
    linked to the appropriate third-party Git repository through Git
    submodules).

Our branches contain groundbreaking research, radical ideas and other
work-in-progress changes that are meant to be merged into `master` at
a later point in time.

The master branch is the main development branch containing the
latest, bleeding-edge code.

Additional information can befound on our "[Coding info]" page,
including stuff like:

- individual projects (modules) within the above folders
- simplification of data types
- debug commands
- more

<!-- TODO: consider moving 'Coding info' to GitHub wiki.
Also, the above directory structure should just be merged into the 'Coding info' page.
It feels out of place. -->

[Coding info]: https://wiki.multitheftauto.com/wiki/Coding_info

## Gaining and losing commit access

Commit access is granted after pull requests have been submitted, and the
coder has proven themselves to be competent. The subject matter of the patches does
not matter, we are more interested in whether if you are granted commit
access, you will be capable of maintaining a high standard of code and
remaining cohesive with other project collaborators.

Patches can be submitted using [GitHub's pull request
system](https://github.com/multitheftauto/mtasa-blue/pull/new). Usually
commit access is gained after 2-3 pull requests, but this is not fixed and
depends on the extent of the contributions. This requires you to fork
our repository and make commits to your own branch first.

After gaining commit access, if the contributor's commits are of a
consistently low standard, or the user fails to stick to the rules,
their commit access will be stripped and will be required to submit pull
requests again.

## Committing code

<!-- TODO: below clashes a bit with the 'What to code' section -->
<!-- oh and it's also referring to mantis which we don't use -->
Keep in mind that your commits should initially be fixing or
implementing existing issues on our [bug
tracker](http://bugs.mtasa.com). The
[roadmap](http://bugs.mtasa.com/roadmap_page.php) is particularly
important since it allows contributors to track down priority issues.

Please follow these guidelines for all your contributions:

-   Commits should be thoroughly tested when added to master. Commits
    that \'need to be fixed later\' which directly affect the state of
    the mod will be reverted other than in exceptional circumstances.
-   If writing unstable or experimental code, a private branch should be
    added in your own fork. Branches should not be \"personal\" to each
    user. This means that a branch should be created for a new feature,
    not for a user specific playground.
-   Commit messages should always give a clear indication of the content
    of the change, without having to look at the code at all. Where
    appropriate, include the Mantis Issue number in your log message and
    keep your log messages consistent, e.g. **Fix \#1234: description
    and notes here**. [Follow the seven rules identified
    here.](http://chris.beams.io/posts/git-commit/)
-   When committing updates to previous commits, include the previous
    commit SHA (and a summarised commit message) in the new commit
    message. Doing this will help identify related commits if they are
    viewed at a later date.

## Ratings and comments

<!-- TODO: needs review guide -->

Ratings and comments are open for the public to review code and provide
feedback. Please be mature and civilised when posting comments.
Developers should try to review other contributor\'s commits and provide
feedback as much as possible.

Make sure you make appropriate use of the GitHub Reactions feature to
rate commits or express agreement/disagreement to a comment. This avoids
spammy comments such as \"+1\", \"-1\", \"Nice one!\", etc.

Since you can only react to comments, not commits, feel free to create
the initial \"+1\" comment in response to a commit. However, future
reactions to a commit should be to the first response comment.

## What to code

Generally, developers should try to only send pull requests that resolve existing
[issues](https://github.com/multitheftauto/mtasa-blue/issues).

If you're looking for something to work on, take a look at:
- our ["good first issue"] label, and
- our [milestones]

["good first issue"]: https://github.com/multitheftauto/mtasa-blue/issues?q=is%3Aissue+is%3Aopen+sort%3Aupdated-desc+label%3A%22good+first+issue%22
[milestones]: https://github.com/multitheftauto/mtasa-blue/milestones?direction=asc&sort=due_date

<!-- TODO: below may need to be rephrased -->
Of course, if you're interested in something else, feel free to experiment
and submit it.

## Style

<!--

TODO: talk about editorconfig.

Also, consider moving this to its own document on the wiki? It seems too long.

-->

-   We use 4 spaces instead of tabs.
-   Hungarian notation for variable names.

    ```cpp
    float         fValue;               // Local variable
    unsigned char m_ucValue;            // Class member variable
    char          ms_cValue;            // Class static variable
    bool          g_bCrashTwiceAnHour;  // Global variable
    char*         szUsername;           // Zero terminated string
    SString       strUsername;          // String
    CVector       vecPosition;          // 3D Vector
    ```

-   Lower camel case for variable names of types like custom structs and
    enums:

    ```cpp
    SSomeStruct   valueOne;
    ESomeEnum     m_valueTwo;
    ```

-   Function names use UpperCamelCase:

    ```cpp
    void UpperCamelCase()
    ```

-   Functions with no arguments are declared and defined with (), and
    called with ()

    ```cpp
    void MyTest();
    void MyTest() { return; }
    MyTest();
    ```

-   Do not use nested structures without braces:

    ```cpp
    // This is disallowed:
    for (dont)
        for (do)
            for (this)
                ...

    // Either of these are allowed
    if (x) {
        y;
    }

    if (x)
        y;
    ```

-   Put `#pragma once` preprocessor directive next to the copyright
    comment for new header files and the ones you are modifying for the
    pull request. Make sure to remove include guard if you are using
    `#pragma once`:

    ```cpp
    /*****************************************************************************
    \
    *
    *  PROJECT:     Multi Theft Auto
    *  LICENSE:     See LICENSE in the top level directory
    *  FILE:        Client/mods/deathmatch/logic/CClientIFP.h
    *
    *  Multi Theft Auto is available from http://www.multitheftauto.com/
    *
    *****************************************************************************/

    #pragma once
    ```

-   Once you're done writing code, and you're about to create a pull
    request, apply clang formatting:
    -   Download
        [clang-format-r325576.exe](http://prereleases.llvm.org/win-snapshots/clang-format-r325576.exe),
        and move it to **utils** folder.
    -   Download [fnr.zip](http://findandreplace.io/downloads/fnr.zip),
        extract fnr.exe to **utils** folder.
    -   Run `utils/win-apply-clang-format.bat` <!-- TODO: this updates all files. we shouldn't recommend this. -->

-   For anything else, follow the style of the code that already exists.

-   Tip: In Visual Studio go to `Tools -> Options -> Text Editor -> C/C++ -> Formatting -> Spacing`
    and you can configure it to automatically apply the right spacing.
