# Base image
FROM ubuntu:22.04

# Avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive
ENV VSOMEIP_LOG_LEVEL=trace

# Install build dependencies
RUN apt-get update && apt-get install -y \
    git cmake g++ build-essential \
    libboost-all-dev libsystemd-dev \
    pkg-config jq netcat \
 && apt-get clean

# Clone & build vSomeIP once
WORKDIR /opt
RUN git clone https://github.com/COVESA/vsomeip.git && \
    cd vsomeip && mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo \
          -DENABLE_SIGNAL_HANDLING=1 \
          -DENABLE_CONFIGURATION=1 \
          -DBUILD_SHARED_LIBS=ON \
          -DCMAKE_INSTALL_PREFIX=/usr/local .. && \
    make -j$(nproc) && make install

# Ensure loader can find vSomeIP libraries
ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
RUN ldconfig /usr/local/lib

# Bring in client app sources
WORKDIR /app
COPY . .

# Make entrypoint runnable
RUN chmod +x ./entrypoint.sh

# Always start via our entrypoint
ENTRYPOINT ["./entrypoint.sh"]
