# vSomeIP Docker Client-Server Project

This project demonstrates a complete SOME/IP communication implementation using vSomeIP in Docker containers. It simulates a server offering a SOME/IP service and a client connecting to it through Service Discovery in an isolated Docker network.

## 🎯 Objective

Create a containerized development environment to test SOME/IP communication between applications, simulating a realistic multi-sensor ECU system. The project demonstrates method-separated SOME/IP messages for speed, engine temperature, and ambient temperature sensors, enabling development and testing of distributed automotive systems that use the SOME/IP protocol for service discovery and inter-ECU communication.

## 🏗️ System Architecture

### Architecture Diagram:

```
┌────────────────────────────────────────────────────────────────────────────────┐
│                            Docker Network (vsomeip_net)                        │
│                              Subnet: 192.168.144.0/20                          │
│                                                                                │
│  ┌─────────────────────────────┐              ┌─────────────────────────────┐  │
│  │     vSomeIP Server          │              │     vSomeIP Client          │  │
│  │    (vsomeip_server)         │              │    (vsomeip_client)         │  │
│  │                             │              │                             │  │
│  │  IP: 192.168.144.2          │              │  IP: 192.168.144.3          │  │
│  │  App ID: 0x0127             │              │  App ID: 0x0100             │  │
│  │                             │              │                             │  │
│  │  ┌─────────────────────┐    │              │  ┌─────────────────────┐    │  │
│  │  │   Service Offer     │    │              │  │   Service Request   │    │  │
│  │  │   Service: 0x1234   │    │              │  │   Service: 0x1234   │    │  │
│  │  │   Instance: 0x0001  │    │              │  │   Instance: 0x0001  │    │  │
│  │  │   Port: 30001       │    │              │  │                     │    │  │
│  │  └─────────────────────┘    │              │  └─────────────────────┘    │  │
│  │             │               │              │             │               │  │
│  └─────────────┼───────────────┘              └─────────────┼───────────────┘  │
│                │                                            │                  │
│                └─────────────────┐   ┌──────────────────────┘                  │
│                                  │   │                                         │
│                     ┌────────────▼───▼──────────────┐                          │
│                     │    Service Discovery (SD)     │                          │
│                     │                               │                          │
│                     │   Multicast: 224.244.224.245  │                          │
│                     │   Port: 30490 (UDP)           │                          │
│                     │                               │                          │
│                     │   1. Server offers service    │                          │
│                     │   2. Client discovers service │                          │
│                     │   3. Direct communication     │                          │
│                     └───────────────────────────────┘                          │
│                                                                                │
└────────────────────────────────────────────────────────────────────────────────┘

                                  Communication Flow:
                                  
    1. Server Start    ──────►  2. Service Discovery  ──────►  3. Client Discovery
         │                            │                             │
         │                            ▼                             │
         │                     Multicast Announce                   │
         │                    (224.244.224.245:30490)               │
         │                            │                             │
         │                            ▼                             ▼
    4. Service Available ◄─────  5. SD Response     ◄─────  6. Service Found
         │                                                          │
         │                                                          ▼
         └────────────────► 7. Direct SOME/IP Communication ◄──────┘
                              (192.168.144.2 ↔ 192.168.144.3)
```

### Main Components:
- **vSomeIP Server**: Multi-sensor service provider with method-specific handlers for speed, engine temperature, and ambient temperature (Service ID: 0x1234, Instance: 0x5678)
- **vSomeIP Client**: Multi-threaded sensor simulator sending realistic automotive data via separate SOME/IP methods
- **Service Discovery**: Communication via UDP multicast for automatic service discovery
- **Docker Network**: Isolated bridge network (`vsomeip_net`) with subnet `192.168.144.0/20`

### Sensor System:
- **Speed Sensor Thread**: Generates realistic speed data (0-250 km/h) every 2 seconds via Method 0x0001
- **Engine Temperature Thread**: Simulates engine temperature (70-110°C) every 3 seconds via Method 0x0002
- **Ambient Temperature Thread**: Provides ambient temperature (-20 to 45°C) every 5 seconds via Method 0x0003

### Communication Flow:
1. **Server** starts and offers the multi-sensor service via Service Discovery
2. **Client** discovers the service and starts three sensor simulation threads
3. **Service Discovery** establishes the connection between client and server
4. **Multi-sensor SOME/IP communication** with method-separated protocols for each sensor type

## 🧱 Project Structure

```
.
├── docker-compose.yml          # Container and network configuration
├── client/
│   ├── Dockerfile             # Client Docker image
│   ├── CMakeLists.txt         # Build configuration
│   ├── client.cpp             # vSomeIP client application
│   ├── client-config.json     # vSomeIP client configuration
│   ├── entrypoint.sh          # Initialization script
│   └── logs/                  # Log directory
└── server/
    ├── Dockerfile             # Server Docker image
    ├── CMakeLists.txt         # Build configuration
    ├── server.cpp             # vSomeIP server application
    ├── server-config.json     # vSomeIP server configuration
    ├── entrypoint.sh          # Initialization script
    └── logs/                  # Log directory
```

## ⚙️ vSomeIP Configuration

### Server Configuration (server-config.json):
- **Application ID**: 0x0127
- **Service ID**: 0x1234
- **Instance ID**: 0x0001
- **Unicast**: Specific IP in Docker network
- **Service Discovery**: Multicast 224.244.224.245:30490

### Client Configuration (client-config.json):
- **Application ID**: 0x0100
- **Service Discovery**: Searches for service 0x1234
- **Unicast**: Specific IP in Docker network

## 🐳 How to Use

### Prerequisites:
- Docker and Docker Compose installed
- Linux system (tested on Ubuntu/Debian)
- Permissions to run Docker

### 1. Clone and prepare the project

```bash
git clone https://github.com/engividal/vsomeip-docker.git
cd vsomeip-docker
```

### 2. Build Docker images

```bash
docker-compose build
```

This command will:
- Download Ubuntu 22.04 base image
- Install dependencies (cmake, g++, boost, etc.)
- Compile and install vSomeIP 3.5.6
- Configure environment with correct LD_LIBRARY_PATH

### 3. Run the system

```bash
docker-compose up
```

### 4. Monitor logs in real-time

```bash
# In separate terminals:
docker-compose logs -f server
docker-compose logs -f client

# Or view all together:
docker-compose logs -f

# Alternative: Monitor application logs directly from files
tail -f server/logs/server.log
tail -f client/logs/client.log

# Monitor both logs simultaneously
tail -f server/logs/server.log client/logs/client.log
```

### 5. Monitor sensor data in real-time

```bash
# Monitor server logs directly from host (recommended)
tail -f server/logs/server.log

# Monitor client logs directly from host
tail -f client/logs/client.log

# Monitor both server and client logs simultaneously
tail -f server/logs/server.log client/logs/client.log

# Filter only sensor data from server logs
tail -f server/logs/server.log | grep -E "(Speed|Engine|Ambient)"

# Monitor specific sensors from server logs
tail -f server/logs/server.log | grep "Speed"
tail -f server/logs/server.log | grep "Engine.*temp"
tail -f server/logs/server.log | grep "Ambient"

# Monitor sensor alerts (abnormal values)
tail -f server/logs/server.log | grep -E "(ALERT|WARNING)"

# Alternative: Using docker-compose logs
docker-compose logs -f server | grep -E "(Speed|Engine|Ambient)"
docker-compose logs -f client
```

### 6. Expected success logs:

**Server (Multi-sensor output):**
```log
[info] OFFER(0127): [1234.5678:0.0] (true)
Server: Offering service 0x1234, instance 0x5678
Speed sensor data: 87.45 km/h (timestamp: 1703123456)
Engine temp data: 89.12°C (timestamp: 1703123457)
Ambient temp data: 22.78°C (timestamp: 1703123458)
ALERT: Engine temperature high: 106.34°C
```

**Client (Multi-threaded output):**
```log
on_availability: service=0x1234, instance=0x5678, available=true
Speed thread: Sending 87.45 km/h
Engine temp thread: Sending 89.12°C
Ambient temp thread: Sending 22.78°C
```

## 🔧 Development and Debugging

### Execute commands inside containers:

```bash
# Access server shell
docker exec -it vsomeip_server bash

# Access client shell  
docker exec -it vsomeip_client bash

# View current configuration
docker exec -it vsomeip_server cat /app/server-config.json
docker exec -it vsomeip_client cat /app/client-config.json
```

### Verify network connectivity:

```bash
# View container IPs
docker network inspect vsomeip-docker_vsomeip_net

# Test connectivity between containers
docker exec -it vsomeip_client nc -zv vsomeip_server 30001
```

## 📊 Network Analysis and Protocol Monitoring

### Find the correct Docker network interface:

```bash
# Identify the Docker bridge interface for vsomeip network
docker network ls
docker network inspect vsomeip-docker_vsomeip_net | grep -E "(NetworkID|Gateway)"

# Auto-detect the correct interface
DOCKER_IF=$(docker network inspect vsomeip-docker_vsomeip_net --format '{{.Id}}' | head -c 12)
INTERFACE="br-$DOCKER_IF"
echo "Docker interface: $INTERFACE"

# Verify interface exists
ip addr show | grep $INTERFACE
```

### Capture SOME/IP traffic with tcpdump:

```bash
# Capture all SOME/IP traffic (Service Discovery + sensor data)
sudo tcpdump -i $INTERFACE -v

# Capture only Service Discovery (SD) multicast traffic
sudo tcpdump -i $INTERFACE -v dst 224.244.224.245

# Capture sensor data traffic between containers
sudo tcpdump -i $INTERFACE -v net 192.168.144.0/20

# Save capture for Wireshark analysis
sudo tcpdump -i $INTERFACE -w someip_capture.pcap

# Filter by specific method IDs (sensor types)
sudo tcpdump -i $INTERFACE -v -X | grep -A 10 -B 5 "1234"
```

### Analyze sensor protocol patterns:

```bash
# Monitor speed sensor traffic (Method 0x0001)
sudo tcpdump -i $INTERFACE -v -X | grep -A 5 "0001"

# Monitor engine temperature traffic (Method 0x0002)  
sudo tcpdump -i $INTERFACE -v -X | grep -A 5 "0002"

# Monitor ambient temperature traffic (Method 0x0003)
sudo tcpdump -i $INTERFACE -v -X | grep -A 5 "0003"

# Count messages per sensor type
sudo tcpdump -i $INTERFACE -c 100 | grep -c "30001"  # Service port
```

### Advanced tcpdump filters for multi-sensor SOME/IP:

```bash
# Capture specific service ID (0x1234) traffic
sudo tcpdump -i $INTERFACE -X -s 0 | grep -B 2 -A 10 "1234"

# Monitor only client-to-server communication
sudo tcpdump -i $INTERFACE src 192.168.144.3 and dst 192.168.144.2

# Monitor only server responses
sudo tcpdump -i $INTERFACE src 192.168.144.2 and dst 192.168.144.3

# Time-stamped capture with sensor frequency analysis
sudo tcpdump -i $INTERFACE -ttt net 192.168.144.0/20
```

```bash
# Capture multicast Service Discovery messages
sudo tcpdump -i br-xxxxxxxxx -w sd_multicast.pcap dst 224.244.224.245

# Capture both Service Discovery and data communication
sudo tcpdump -i br-xxxxxxxxx -w full_someip.pcap '(udp port 30490) or (host 192.168.144.2 and host 192.168.144.3)'

# Monitor real-time with timestamps
sudo tcpdump -i br-xxxxxxxxx -tttt -v udp port 30490
```

### Analyze with Wireshark:

After capturing packets with tcpdump, you can analyze them using Wireshark:

```bash
# Install Wireshark (if not already installed)
sudo apt install wireshark

# Open capture file in Wireshark
wireshark someip_capture.pcap
```

**Wireshark Analysis Tips:**
- **Filter for SOME/IP**: Use filter `someip` if Wireshark has SOME/IP dissector
- **Service Discovery filter**: `udp.port == 30490`
- **Container communication**: `ip.addr == 192.168.144.2 && ip.addr == 192.168.144.3`
- **Multicast filter**: `ip.dst == 224.244.224.245`

### Real-time monitoring during testing:

First, find your Docker bridge interface:

```bash
# Get the network ID and find corresponding interface
NETWORK_ID=$(docker network ls | grep vsomeip-docker_vsomeip_net | awk '{print $1}')
echo "Network ID: $NETWORK_ID"
echo "Bridge interface: br-${NETWORK_ID:0:12}"
```

Then use this interface for monitoring:

```bash
# Terminal 1: Start packet capture (replace br-xxxxxxxxx with your bridge interface)
sudo tcpdump -i br-${NETWORK_ID:0:12} -w live_analysis.pcap

# Terminal 2: Run the application
docker-compose up

# Terminal 3: Monitor in real-time
sudo tcpdump -i br-${NETWORK_ID:0:12} -l udp port 30490 | tee tcpdump_output.log

# Terminal 4: Monitor application logs
tail -f server/logs/server.log client/logs/client.log
```

## 🚀 Extensions and Improvements

### Add more sensor types:

1. **Fuel Level Sensor (Method 0x0004):**
   ```cpp
   // Add to client: fuel level simulation (0-100%)
   // Add to server: fuel level handler with low fuel alerts
   ```

2. **Tire Pressure Sensors (Methods 0x0005-0x0008):**
   ```cpp
   // Four separate methods for each wheel
   // Pressure simulation (1.8-2.5 bar) with alerts
   ```

3. **GPS Coordinates (Method 0x0009):**
   ```cpp
   // Latitude/longitude simulation with movement patterns
   ```

### Add more clients (Multi-ECU simulation):

1. Duplicate the `client` service in `docker-compose.yml`
2. Change container name and application ID
3. Configure different sensor types per ECU
4. Simulate ECU-specific behavior patterns

### Advanced sensor configurations:

- **Sensor fusion**: Combine multiple sensor readings for derived values
- **Error simulation**: Introduce sensor failures and error conditions
- **Data validation**: Add checksum and data integrity validation
- **Reliable/TCP**: Add TCP communication for critical sensor data
- **Events**: Implement event notifications for sensor threshold violations
- **Fields**: Implement getter/setter fields for sensor calibration
- **Security**: Enable vSomeIP authentication for sensor data protection

## 🧼 Cleanup and Maintenance

### Stop and remove containers:

```bash
# Stop containers
docker-compose down

# Remove orphaned volumes and networks
docker-compose down --volumes --remove-orphans

# Complete rebuild after changes
docker-compose down && docker-compose build --no-cache && docker-compose up
```

### Clear logs:

```bash
# Clear local logs
rm -rf client/logs/* server/logs/*

# View container space usage
docker system df
docker system prune  # Remove unused resources
```

## 🛠️ Troubleshooting

### Multi-sensor specific issues:

1. **Sensor data not appearing:**
   ```bash
   # Check if all three threads are running
   docker-compose logs client | grep -E "(Speed|Engine|Ambient).*thread"
   
   # Verify method registration on server
   docker-compose logs server | grep -E "register.*0x000[123]"
   
   # Check for method-specific errors
   docker-compose logs server | grep -E "Method.*not.*found"
   ```

2. **Missing sensor alerts:**
   ```bash
   # Force engine temperature alert by checking thresholds
   docker-compose logs server | grep -E "(temp|°C)" | tail -10
   
   # Monitor for temperature values > 105°C
   docker-compose logs -f server | grep -E "ALERT.*temp"
   ```

3. **Sensor frequency issues:**
   ```bash
   # Check thread timing
   docker-compose logs client | grep -E "thread.*Sending" | tail -20
   
   # Verify different update frequencies (2s, 3s, 5s)
   docker-compose logs -f client --timestamps
   ```

4. **Method separation problems:**
   ```bash
   # Verify each method ID is handled separately
   docker-compose logs server | grep -E "Method.*0x000[123]"
   
   # Check payload sizes (should be 8 bytes per sensor)
   docker-compose logs server | grep -E "payload.*size"
   ```

### General SOME/IP issues:

1. **Service Discovery doesn't work:**
   - Verify containers are in the same Docker network
   - Check multicast configurations in JSON files
   - Verify logs from both containers

2. **Library path error:**
   - Verify `LD_LIBRARY_PATH` is configured
   - Run `ldconfig -p | grep vsomeip` inside container

3. **Container doesn't start:**
   - Verify ports are free on host
   - Check logs with `docker-compose logs`

### Advanced debugging:

```bash
# View all Docker networks
docker network ls

# Inspect specific container  
docker inspect vsomeip_server

# View processes inside container
docker exec -it vsomeip_server ps aux
```

## 📚 References and Documentation

### vSomeIP:
- [vSomeIP GitHub](https://github.com/COVESA/vsomeip)
- [SOME/IP Specification](https://www.autosar.org/standards)
- [vSomeIP User Guide](https://github.com/COVESA/vsomeip/wiki)

### Docker:
- [Docker Compose Networking](https://docs.docker.com/compose/networking/)
- [Docker Multi-container Apps](https://docs.docker.com/compose/)

### Network Analysis:
- [tcpdump Manual](https://www.tcpdump.org/manpages/tcpdump.1.html)
- [Wireshark User Guide](https://www.wireshark.org/docs/wsug_html/)
- [SOME/IP Protocol Analysis](https://www.wireshark.org/docs/dfref/s/someip.html)

### SOME/IP Protocol:
- Service Discovery via multicast UDP
- Application-to-Application communication
- Automotive Ethernet communication standard
- Part of AUTOSAR Adaptive Platform

---

## 🎯 Use Cases

This project serves as a foundation for:
- **Automotive ECU simulation**: Multi-sensor vehicle systems with realistic speed, engine temperature, and ambient temperature data
- **Method-separated SOME/IP protocols**: Demonstrating how to handle different sensor types via separate method IDs
- **Multi-threaded sensor systems**: Simulation of real-world sensor frequencies and data generation patterns
- **Integration testing**: Validation of SOME/IP services with multiple concurrent data streams
- **Prototyping**: Rapid development of distributed automotive applications with realistic sensor behavior
- **Education**: Learning SOME/IP protocol, Service Discovery, and automotive sensor communication patterns
- **Network analysis**: Understanding SOME/IP traffic patterns for multi-sensor systems and debugging communication issues
- **Performance testing**: Analyzing system behavior under multiple concurrent sensor data streams

## Attribution

If you use this code in your project, please credit: **Ismael Vidal**.
