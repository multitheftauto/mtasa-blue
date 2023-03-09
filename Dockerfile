FROM jetbrains/teamcity-minimal-agent:latest

# This is important for using apt-get
USER root

# Default build configuration
ENV AS_BUILDAGENT=0 \
    BUILD_ARCHITECTURE=x64 \
    BUILD_CONFIG=release

# Install build-time dependencies
RUN dpkg --add-architecture i386 && apt-get update && \
    apt-get install -y software-properties-common wget ca-certificates git build-essential \
        gcc-multilib g++-multilib gcc-10-multilib g++-10-multilib curl subversion ncftp \
        libncurses-dev libncursesw5 \
        libncurses-dev:i386 libncursesw5:i386 \
        libmysqlclient-dev

# Set build directory
VOLUME /build
WORKDIR /build

# Copy entrypoint script
COPY utils/docker-entrypoint.sh /docker-entrypoint.sh

# Add GLIB compat 
COPY utils/compat /compat

# Set entrypoint
ENTRYPOINT bash /docker-entrypoint.sh
