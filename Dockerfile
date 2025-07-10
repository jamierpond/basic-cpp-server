# Builder stage
FROM ubuntu:22.04 AS builder

# Install all dependencies in a single layer and boost
RUN apt-get update && apt-get install -y \
    cmake git wget software-properties-common gnupg ninja-build \
    libssl-dev lsb-release clang \
    libboost-all-dev \
    --no-install-recommends && \
    rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source files
COPY . .

RUN ls

# Build the project
RUN mkdir -p build && \
    cmake -G Ninja \
    -B build \
    -S . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \

RUN cd build && \
    cmake --build . --target install

# Run server
CMD ["./build/PondAudio"]
