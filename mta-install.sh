#!/bin/bash
#
# Must be called after mta-build.sh the first time, can be called anytime after that
#

# Install data files
utils/premake5_x64 install_data
utils/premake5_x64 install_resources

echo "Server ready in Bin/server/"
