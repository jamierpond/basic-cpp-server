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

# Build the project
RUN mkdir -p build && cd build && \
    cmake -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=clang \
    -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
    -DCMAKE_CXX_STANDARD=23 .. && \
    ninja

# Final stage - using smaller image
FROM ubuntu:22.04

# Install only necessary runtime dependencies
RUN apt-get update && apt-get install -y \
    libssl3 libc++1 --no-install-recommends && \
    rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy only the built binary
COPY --from=builder /app/build/PondAudio .

# Expose port
EXPOSE 3000

# Run server
CMD ["./PondAudio"]
