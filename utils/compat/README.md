## What is this?

These two files allow us to build binaries which work old Linux distributions with 
outdated glibc versions on a modern host machine.

## How does it work?

When a symbol in glibc is superseeded by a newer one (e.g. memcpy@2.2.5 vs memcpy@2.14) the old one is not actually 
removed from glibc for backwards-compatibilty. However by default new builds will use the new symbol version. This 
causes errors when attempting to load a new build on a machine which does not have the new symbol version. As a 
workaround we can specify which version of a symbol we want to use. 

As in most cases libstdc++ is also linked against the new glibc symbols these need to be changed as well. For this 
objcopy is a handyy tool to make a new static library from the system library while only using old symbols. Therefore
you'll need to run create_static_libraries.sh at least once before using this.

## What does not work?
You cannot use a symbol for which there is no corresponding symbol in the earlier glibc version

Also extra caution is advised when linking system libraries staically, as they might import symbols 
with a higher glibc version. Apply the same thing as for libstdc++ for them to solve the issue.
