FROM jetbrains/teamcity-minimal-agent:latest

# This is important for using apt-get
USER root

# Default build configuration
ENV AS_BUILDAGENT=0 \
    BUILD_ARCHITECTURE=x64 \
    BUILD_CONFIG=release \
    AR=x86_64-linux-gnu-gcc-ar-10 \
    CC=x86_64-linux-gnu-gcc-10 \
    CXX=x86_64-linux-gnu-g++-10

# Install build-time dependencies
RUN apt-get update && \
    apt-get install -y software-properties-common wget ca-certificates git build-essential \
        gcc-10 g++-10 curl subversion ncftp \
        libncurses-dev libncursesw6 libmysqlclient-dev

# Set build directory
VOLUME /build
WORKDIR /build

# Copy entrypoint script
COPY utils/docker-entrypoint.sh /docker-entrypoint.sh
RUN chmod +x /docker-entrypoint.sh

# Add GLIB compat 
COPY utils/compat /compat

# Set entrypoint
ENTRYPOINT bash /docker-entrypoint.sh
