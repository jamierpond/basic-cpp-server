#!/bin/bash
set -e

# Default values
BUILD_TYPE=""
RUN_TESTS=false
RUN_SERVER=true

# Parse command line arguments
while [[ $# -gt 0 ]]; do
  case $1 in
    --asan)
      BUILD_TYPE="--config=asan"
      shift
      ;;
    --tsan)
      BUILD_TYPE="--config=tsan"
      shift
      ;;
    --msan)
      BUILD_TYPE="--config=msan"
      shift
      ;;
    --test)
      RUN_TESTS=true
      shift
      ;;
    --build-only)
      RUN_SERVER=false
      shift
      ;;
    *)
      echo "Unknown option: $1"
      echo "Usage: $0 [--asan|--tsan|--msan] [--test] [--build-only]"
      exit 1
      ;;
  esac
done

# Build the server
echo "Building server..."
bazel build //src:server $BUILD_TYPE

# Run tests if requested
if $RUN_TESTS; then
  echo "Running tests..."
  bazel test //test:tests $BUILD_TYPE
fi

# Run the server unless build-only was specified
if $RUN_SERVER; then
  echo "Starting server on port 3000..."
  bazel run //src:server $BUILD_TYPE
fi

# Usage examples:
# ./run.sh                     - Build and run the server
# ./run.sh --asan              - Build and run with address sanitizer
# ./run.sh --test              - Build, run tests, and run the server
# ./run.sh --test --build-only - Build and run tests only, don't run the server
# ./run.sh --tsan --build-only - Build with thread sanitizer but don't run