# vSomeIP Docker Client-Server Project

This project demonstrates a complete SOME/IP communication implementation using vSomeIP in Docker containers. It simulates a server offering a SOME/IP service and a client connecting to it through Service Discovery in an isolated Docker network.

## 🎯 Objective

Create a containerized development environment to test SOME/IP communication between applications, enabling development and testing of distributed systems that use the SOME/IP protocol for service discovery and communication.

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
- **vSomeIP Server**: Container offering a SOME/IP service (ID: 0x1234, Instance: 0x0001)
- **vSomeIP Client**: Container that discovers and consumes the service offered by the server
- **Service Discovery**: Communication via UDP multicast for automatic service discovery
- **Docker Network**: Isolated bridge network (`vsomeip_net`) with subnet `192.168.144.0/20`

### Communication Flow:
1. **Server** starts and offers the service via Service Discovery
2. **Client** searches for available services via multicast
3. **Service Discovery** establishes the connection between client and server
4. **SOME/IP message exchange** between applications

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

### 5. Expected success logs:

**Server:**
```log
[info] OFFER(0127): [1234.0001:0.0] (true)
Server: Offering service 0x1234, instance 0x0001
```

**Client:**
```log
on_availability: service=0x1234, instance=0x1, available=true
Server: Received message from client!
Total clients communicated: 1
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

## 📊 Network Analysis with tcpdump and Wireshark

### Find the correct Docker network interface:

```bash
# First, identify the Docker bridge interface for vsomeip network
docker network ls
ip addr show | grep -A 3 "br-"

# The interface will be something like br-xxxxxxxxx matching your network ID
```

### Capture SOME/IP traffic with tcpdump:

```bash
# Replace br-xxxxxxxxx with your actual Docker bridge interface
# You can find it with: docker network inspect vsomeip-docker_vsomeip_net

# Capture all traffic on Docker network interface
sudo tcpdump -i br-xxxxxxxxx -w someip_capture.pcap

# Capture only UDP traffic on Service Discovery port
sudo tcpdump -i br-xxxxxxxxx -w sd_capture.pcap udp port 30490

# Capture with verbose output (real-time analysis)
sudo tcpdump -i br-xxxxxxxxx -v udp port 30490

# Capture specific IP communication between containers
sudo tcpdump -i br-xxxxxxxxx -w client_server.pcap host 192.168.144.2 and host 192.168.144.3
```

### Advanced tcpdump filters for SOME/IP:

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

### Add more clients:

1. Duplicate the `client` service in `docker-compose.yml`
2. Change container name and application ID
3. Configure a different `client-config.json`

### Advanced configurations:

- **Reliable/TCP**: Add TCP communication besides UDP
- **Methods**: Implement request/response methods
- **Events**: Implement event notifications  
- **Fields**: Implement getter/setter fields
- **Security**: Enable vSomeIP authentication

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

### Common problems:

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
- **Automotive development**: Simulation of ECUs communicating via Ethernet
- **Integration testing**: Validation of SOME/IP services
- **Prototyping**: Rapid development of distributed applications
- **Education**: Learning SOME/IP protocol and Service Discovery
- **Network analysis**: Understanding SOME/IP traffic patterns and debugging communication issues
