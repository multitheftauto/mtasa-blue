FROM jetbrains/teamcity-minimal-agent:latest

# Set to 1 to configure this image as Teamcity build agent
# Default value is 0 (manual build)
ENV AS_BUILDAGENT 0

# Set default target platform to 64-bits
ENV BUILD_BITS 64

# This is important for using apt-get
USER root

# Install latest gcc and libs
RUN dpkg --add-architecture i386 && apt-get update && \
    apt-get install -y software-properties-common wget ca-certificates git build-essential \
        gcc-multilib g++-multilib gcc-10-multilib g++-10-multilib curl subversion ncftp \
        libncursesw5-dev libmysqlclient-dev \
        lib32ncursesw5-dev libncursesw5-dev:i386 

# Set build directory
VOLUME /build
WORKDIR /build

# Copy entrypoint script
COPY utils/docker-entrypoint.sh /docker-entrypoint.sh

# Add GLIB compat
COPY utils/compat /compat

# Set entrypoint
ENTRYPOINT bash /docker-entrypoint.sh
