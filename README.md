# SignalStream (working title)

Backend playground in modern C++ for learning real backend development:
HTTP API, PostgreSQL, Docker, and later gRPC, background workers, and more.

Right now it's just a skeleton with a single health check endpoint.

## Tech stack

- C++20
- Drogon (HTTP framework)
- Conan 2 (dependency management)
- CMake
- (Later) PostgreSQL, Docker, gRPC, Kafka, etc.

## Status

- ✅ Backend builds
- ✅ `/health` endpoint returns `{"status": "ok"}`
- ⏳ Database connection (PostgreSQL)
- ⏳ Auth, users, feed
- ⏳ Docker setup for app + DB

## Build & run (CLI)

Requirements:
- Conan 2
- CMake
- A C++20 compiler (AppleClang / clang++)

```bash
# From project root
conan install . -s build_type=Debug -b=missing

mkdir -p build/Debug
cd build/Debug

cmake ../.. -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
cmake --build .

./server/signalstream
