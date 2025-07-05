#!/bin/bash
set -e

# Ensure LD_LIBRARY_PATH includes the vSomeIP library location
export LD_LIBRARY_PATH="/usr/local/lib:${LD_LIBRARY_PATH:-}"

# Export VSOMEIP_LOG_LEVEL and configuration file
export VSOMEIP_LOG_LEVEL=${VSOMEIP_LOG_LEVEL:-trace}
export VSOMEIP_CONFIGURATION=/app/client-config.json

echo "LD_LIBRARY_PATH is set to: $LD_LIBRARY_PATH"
echo "VSOMEIP_LOG_LEVEL is set to: $VSOMEIP_LOG_LEVEL"
echo "Using VSOMEIP configuration file: $VSOMEIP_CONFIGURATION"

# Verify that vSomeIP libraries are accessible
echo "Checking vSomeIP library availability:"
ldconfig -p | grep vsomeip || echo "Warning: vSomeIP libraries not found in ldconfig cache"

# Wait a moment to ensure the server has time to start
echo "Waiting for server to be ready..."
sleep 10

mkdir -p build logs
cd build
cmake ..
make -j$(nproc)

echo "Starting client..."
exec /app/build/client > /app/logs/client.log 2>&1
