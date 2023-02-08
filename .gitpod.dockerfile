FROM gitpod/workspace-full-vnc

# Install Linux dependencies
USER root
RUN apt-get update \
    && apt-get -y install build-essential snapd ninja-build xcb libx11-dev libx11-xcb-dev libxcb-sync-dev libxcb-randr0-dev \
    && sudo snap install cmake --classic \
    && apt-get clean && rm -rf /var/cache/apt/* && rm -rf /var/lib/apt/lists/* && rm -rf /tmp/*

USER gitpod
