// client.cpp - Vehicle ECU Multi-Sensor System
#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <mutex>
#include <atomic>
#include <iomanip>
#include <memory>
#include <vector>

std::shared_ptr<vsomeip::application> app;
std::atomic<bool> service_available(false);
std::atomic<bool> running(true);
std::mutex cout_mutex;

// Optimized sensor data structures - one per sensor type
struct SpeedData {
    float speed_kmh;
    uint32_t timestamp;
};

struct EngineTemperatureData {
    float temperature_celsius;
    uint32_t timestamp;
};

struct AmbientTemperatureData {
    float temperature_celsius;
    uint32_t timestamp;
};

// Vehicle sensor simulator class with method-specific data
class VehicleSensors {
private:
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> speed_dist;
    std::uniform_real_distribution<> engine_temp_dist;
    std::uniform_real_distribution<> ambient_temp_dist;
    
    // Variables to simulate realistic trends
    float current_speed = 0.0f;
    float current_engine_temp = 80.0f;
    float current_ambient_temp = 20.0f;
    
public:
    VehicleSensors() : gen(rd()), 
                       speed_dist(-5.0, 5.0),      // Gradual speed variation
                       engine_temp_dist(-2.0, 2.0), // Gradual temp variation
                       ambient_temp_dist(-1.0, 1.0) {} // Slow ambient change
    
    // Generate speed sensor data only
    SpeedData get_speed_data() {
        current_speed += speed_dist(gen);
        current_speed = std::max(0.0f, std::min(120.0f, current_speed));
        
        return {
            current_speed,
            static_cast<uint32_t>(std::time(nullptr))
        };
    }
    
    // Generate engine temperature sensor data only
    EngineTemperatureData get_engine_temp_data() {
        current_engine_temp += engine_temp_dist(gen);
        current_engine_temp = std::max(60.0f, std::min(110.0f, current_engine_temp));
        
        return {
            current_engine_temp,
            static_cast<uint32_t>(std::time(nullptr))
        };
    }
    
    // Generate ambient temperature sensor data only
    AmbientTemperatureData get_ambient_temp_data() {
        current_ambient_temp += ambient_temp_dist(gen);
        current_ambient_temp = std::max(-20.0f, std::min(50.0f, current_ambient_temp));
        
        return {
            current_ambient_temp,
            static_cast<uint32_t>(std::time(nullptr))
        };
    }
};

// Specialized serialization functions for each sensor type
std::vector<uint8_t> serialize_speed_data(const SpeedData& data) {
    std::vector<uint8_t> payload;
    payload.reserve(8); // 4 + 4 bytes
    
    // Speed (4 bytes)
    auto speed_bytes = reinterpret_cast<const uint8_t*>(&data.speed_kmh);
    payload.insert(payload.end(), speed_bytes, speed_bytes + 4);
    
    // Timestamp (4 bytes)
    auto time_bytes = reinterpret_cast<const uint8_t*>(&data.timestamp);
    payload.insert(payload.end(), time_bytes, time_bytes + 4);
    
    return payload;
}

std::vector<uint8_t> serialize_engine_temp_data(const EngineTemperatureData& data) {
    std::vector<uint8_t> payload;
    payload.reserve(8); // 4 + 4 bytes
    
    // Temperature (4 bytes)
    auto temp_bytes = reinterpret_cast<const uint8_t*>(&data.temperature_celsius);
    payload.insert(payload.end(), temp_bytes, temp_bytes + 4);
    
    // Timestamp (4 bytes)
    auto time_bytes = reinterpret_cast<const uint8_t*>(&data.timestamp);
    payload.insert(payload.end(), time_bytes, time_bytes + 4);
    
    return payload;
}

std::vector<uint8_t> serialize_ambient_temp_data(const AmbientTemperatureData& data) {
    std::vector<uint8_t> payload;
    payload.reserve(8); // 4 + 4 bytes
    
    // Temperature (4 bytes)
    auto temp_bytes = reinterpret_cast<const uint8_t*>(&data.temperature_celsius);
    payload.insert(payload.end(), temp_bytes, temp_bytes + 4);
    
    // Timestamp (4 bytes)
    auto time_bytes = reinterpret_cast<const uint8_t*>(&data.timestamp);
    payload.insert(payload.end(), time_bytes, time_bytes + 4);
    
    return payload;
}

// Service availability callback
void on_availability(vsomeip::service_t service, vsomeip::instance_t instance, bool available) {
    if (service == 0x1234 && instance == 0x0001) {
        service_available = available;
        std::lock_guard<std::mutex> lock(cout_mutex);
        if (available) {
            std::cout << "🚗 ECU Client: Central Gateway ONLINE. Starting sensors..." << std::endl;
        } else {
            std::cout << "⚠️  ECU Client: Central Gateway OFFLINE." << std::endl;
        }
    }
}

// Specialized send functions for each sensor method
void send_speed_data(const SpeedData& data) {
    if (!service_available) return;
    
    auto payload_data = serialize_speed_data(data);
    
    auto request = vsomeip::runtime::get()->create_request();
    request->set_service(0x1234);
    request->set_instance(0x0001);
    request->set_method(0x0001);  // Speed method
    request->set_payload(vsomeip::runtime::get()->create_payload(payload_data));
    
    app->send(request);
    
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "🏃 SPEED: " << data.speed_kmh << " km/h [Method 0x0001]" << std::endl;
}

void send_engine_temp_data(const EngineTemperatureData& data) {
    if (!service_available) return;
    
    auto payload_data = serialize_engine_temp_data(data);
    
    auto request = vsomeip::runtime::get()->create_request();
    request->set_service(0x1234);
    request->set_instance(0x0001);
    request->set_method(0x0002);  // Engine temperature method
    request->set_payload(vsomeip::runtime::get()->create_payload(payload_data));
    
    app->send(request);
    
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "🔥 ENGINE: " << data.temperature_celsius << "°C [Method 0x0002]" << std::endl;
}

void send_ambient_temp_data(const AmbientTemperatureData& data) {
    if (!service_available) return;
    
    auto payload_data = serialize_ambient_temp_data(data);
    
    auto request = vsomeip::runtime::get()->create_request();
    request->set_service(0x1234);
    request->set_instance(0x0001);
    request->set_method(0x0003);  // Ambient temperature method
    request->set_payload(vsomeip::runtime::get()->create_payload(payload_data));
    
    app->send(request);
    
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "🌡️ AMBIENT: " << data.temperature_celsius << "°C [Method 0x0003]" << std::endl;
}

// Updated sensor threads using method-specific functions
void speed_sensor_thread(VehicleSensors& sensors) {
    while (running) {
        if (service_available) {
            auto data = sensors.get_speed_data();
            send_speed_data(data);
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));  // 2 segundos - velocidade
    }
}

void engine_temp_sensor_thread(VehicleSensors& sensors) {
    while (running) {
        if (service_available) {
            auto data = sensors.get_engine_temp_data();
            send_engine_temp_data(data);
        }
        std::this_thread::sleep_for(std::chrono::seconds(3));  // 3 segundos - temp motor
    }
}

void ambient_temp_sensor_thread(VehicleSensors& sensors) {
    while (running) {
        if (service_available) {
            auto data = sensors.get_ambient_temp_data();
            send_ambient_temp_data(data);
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));  // 5 segundos - temp ambiente
    }
}

int main() {
    // Initialize vehicle ECU application
    app = vsomeip::runtime::get()->create_application("vehicle_ecu");
    app->init();
    
    std::cout << "🚗 Vehicle ECU: Multi-Method Sensor System..." << std::endl;
    std::cout << "📊 Methods: 0x0001(Speed), 0x0002(Engine), 0x0003(Ambient)" << std::endl;
    
    // Register service availability handler
    app->register_availability_handler(0x1234, 0x0001, on_availability);
    app->request_service(0x1234, 0x0001);
    
    VehicleSensors sensors;
    
    // Create threads for each sensor with different frequencies BEFORE app->start()
    std::thread speed_thread(speed_sensor_thread, std::ref(sensors));
    std::thread engine_temp_thread(engine_temp_sensor_thread, std::ref(sensors));
    std::thread ambient_temp_thread(ambient_temp_sensor_thread, std::ref(sensors));
    
    // Main control thread
    std::cout << "🔄 All sensor threads started with dedicated methods!" << std::endl;
    std::cout << "   • Speed: 2s cycle → Method 0x0001" << std::endl;
    std::cout << "   • Engine Temp: 3s cycle → Method 0x0002" << std::endl;
    std::cout << "   • Ambient Temp: 5s cycle → Method 0x0003" << std::endl;
    
    // Start vSomeIP (this blocks forever - threads run independently)
    app->start();
    
    return 0;
}
