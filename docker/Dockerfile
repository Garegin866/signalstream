FROM debian:12 AS build

# Install deps: build tools, cmake, git, Drogon dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libjsoncpp-dev \
    uuid-dev \
    zlib1g-dev \
    libssl-dev \
    libbrotli-dev \
    && rm -rf /var/lib/apt/lists/*

# Build Drogon from source (simple way; later you can optimize)
WORKDIR /opt
RUN git clone --depth=1 https://github.com/drogonframework/drogon.git
WORKDIR /opt/drogon
RUN mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build . --target install

# Build your app
WORKDIR /app
COPY server /app/server
COPY CMakeLists.txt /app/CMakeLists.txt

RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build . --target signalstream

# Runtime image
FROM debian:12 AS runtime

RUN apt-get update && apt-get install -y \
    libjsoncpp25 \
    uuid-runtime \
    zlib1g \
    libssl3 \
    libbrotli1 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=build /usr/local/lib /usr/local/lib
COPY --from=build /usr/local/include/drogon /usr/local/include/drogon
COPY --from=build /usr/local/include/trantor /usr/local/include/trantor
COPY --from=build /app/build/server/signalstream /app/signalstream

ENV LD_LIBRARY_PATH=/usr/local/lib

EXPOSE 8080

CMD ["./signalstream"]
