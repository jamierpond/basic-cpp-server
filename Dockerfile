FROM ubuntu:22.04

# Install dependencies including modern Clang and Ninja
RUN apt-get update && apt-get install -y \
    cmake \
    git \
    lsb-release \
    wget \
    software-properties-common \
    gnupg \
    ninja-build \
    && rm -rf /var/lib/apt/lists/*

# Add LLVM/Clang repository and install Clang 16
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
    add-apt-repository "deb http://apt.llvm.org/$(lsb_release -cs)/ llvm-toolchain-$(lsb_release -cs)-16 main" && \
    apt-get update && \
    apt-get install -y clang-16 lldb-16 lld-16 libc++-16-dev libc++abi-16-dev && \
    rm -rf /var/lib/apt/lists/*

# Set Clang as the default compiler
ENV CC=clang-16
ENV CXX=clang++-16

# Set working directory
WORKDIR /app

# Copy source files
COPY . .

# Build the project using Ninja
RUN mkdir -p build && cd build && \
    cmake -G Ninja -DCMAKE_C_COMPILER=clang-16 -DCMAKE_CXX_COMPILER=clang++-16 .. && \
    ninja

# Run tests
RUN cd build && \
    ctest

# Expose the port the server runs on
EXPOSE 3000

# Run the server
CMD ["./build/MyProject"]
