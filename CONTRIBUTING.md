# Contributors Guide

So you've decided to become a contributor to our project. Excellent!

We are always looking for new developers, so if you're new,
please check out our [Getting Started guide](https://wiki.multitheftauto.com/wiki/Coding_info).

But before we can start accepting your code, there are a couple of
things you should know about how we work. 

This document mostly contains guidelines and rules as to how your
code should be structured and how it can be committed without
upsetting any fellow contributors.

## Where to code

As a new potential contributor, you will need to fork our repository and make
commits to your own "branch". Then you can send us a pull request.

Our _`master`_ branch is the main development branch containing the
latest, bleeding-edge code.

Our _other_ branches contain groundbreaking research, radical ideas and other
work-in-progress changes that are meant to be merged into `master` at
a later point in time.

If you're a collaborator, it's your choice whether to push branches to this
repository or to your own fork.

**Branches are "topical" and snd should not be "personal" to each
user.** This means that a branch should be created for a new feature,
not for a user specific playground.

## What to code

Generally, please try submit pull requests that resolve existing
[issues](https://github.com/multitheftauto/mtasa-blue/issues).

If you're looking for something to work on, take a look at the ["good first issue"]
label, or our [milestones].

["good first issue"]: https://github.com/multitheftauto/mtasa-blue/issues?q=is%3Aissue+is%3Aopen+sort%3Aupdated-desc+label%3A%22good+first+issue%22
[milestones]: https://github.com/multitheftauto/mtasa-blue/milestones?direction=asc&sort=due_date

Of course, if you're interested in something else, feel free to experiment
and submit it. But discussing the feature beforehand, in an issue, will
make your PR more likely to be merged in a timely fashion.

## Committing code

**Make sure your code contributions follow the [Style Guide]**.

[Style Guide]: https://github.com/multitheftauto/mtasa-blue/wiki/Style-Guide

**Commits should be tested when added to master.** Commits
that 'need to be fixed later' which directly affect the state of
the mod will be reverted other than in exceptional circumstances.

**Commit messages should**

- be consistent
- always give a clear indication of what has been changed without having to look at the code
- include issue numbers, using [GitHub keywords](https://help.github.com/en/github/managing-your-work-on-github/linking-a-pull-request-to-an-issue#linking-a-pull-request-to-an-issue-using-a-keyword) where necessary
- [follow the seven rules identified here](http://chris.beams.io/posts/git-commit/)
    
The most important of the [seven rules](http://chris.beams.io/posts/git-commit/) has been copied below, but please read the article:

1. Separate subject from body with a blank line
2. Limit the subject line to 50 characters
3. Use the imperative mood in the subject line
4. Use the body to explain what and why vs. how

**Follow up (addendum) commits should refer to the previous commit.** Do this by 
including the previous commit SHA and a summarised commit message in
the new commit message. Doing this will help identify related commits
if they are viewed at a later date.

**Try to keep PRs small — they should be about one thing.** you do multiple things
in one PR, it's hard to review. If you're fixing stuff as you go, you might want
to make atomic commits and then cherry-pick those commits into separate branches,
leaving the PR clean.

**Read the ["Code Review"] guide** for more guidelines about the code review process.

## Reviewing code

Contributors should try to review other contributor's commits and provide
feedback as much as possible.

Please read our ["Code Review"] article for information on how to review code effectively.

["Code Review"]: https://github.com/multitheftauto/mtasa-blue/wiki/Code-Review

<!--

TODO(qaisjp): the below content should be part of a code of conduct instead

Ratings and comments are open for the public to review code and provide
feedback. Please be mature and civilised when posting comments.

Make sure you make appropriate use of the GitHub Reactions feature to
rate commits or express agreement/disagreement to a comment. This avoids
spammy comments such as "+1", "-1", "Nice one!", etc.

Since you can only react to comments, not commits, feel free to create
the initial "+1" comment in response to a commit. However, future
similar reactions to a commit should be to the first response comment.

-->

## Gaining and losing merge rights

Merge rights allow you to merge your own approved pull requests and 
review other people's pull requests.

We grant merge rights after you have proven yourself to be competent,
which is generally after 3-5 pull requests. This is not fixed and depends
on the extent of your contributions, community status and other factors.

The subject matter of your PRs do not matter—we are more interested in,
once granted merge rights, whether you are capable of maintaining
a high standard of code and remaining cohesive with other project collaborators.

After gaining merge rights, if your contributions are of a consistently low standard,
or you fail to stick to the rules, your permissions will be stripped.

## Merging pull requests

Before merging, enforced by GitHub's branch protection, pull requests **require**:
- Linux and Windows status checks to pass
- 1 pull request review

If the pull request is large, try and only merge if there at least 2 pull request reviews.
This isn't enforced via branch protection, but please try and stick to this convention
(... unless nobody else is reviewing your PR).

Branch protection is **not enforced** for repository administrators,
and those people are therefore not required to send pull requests. Individual repo admins may,
for the greater good, pledge to submit pull requests despite this lack of enforcement.

For informational purposes, the current repository administrators are a _subset of_ [forum admins].
It is not an authoritative list.

[forum admins]: https://forum.mtasa.com/staff/
