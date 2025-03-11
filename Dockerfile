FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source files
COPY . .

# Build the project
RUN mkdir -p build && cd build && \
    cmake .. && \
    make -j$(nproc)

# run tests for lolz
RUN cd build && \
    ctest

# Expose the port the server runs on
EXPOSE 3000

# Run the server
CMD ["./build/MyProject"]
