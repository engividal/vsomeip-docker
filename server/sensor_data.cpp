#include "sensor_data.h"
#include <vsomeip/vsomeip.hpp>
#include <cstring>
#include <iostream>
#include <iomanip>

// Global message counter
int message_count = 0;

// Specialized deserialization functions
SpeedData deserialize_speed_data(const std::vector<uint8_t>& payload) {
    SpeedData data = {0};
    if (payload.size() >= 8) {
        // Extract speed (4 bytes)
        std::memcpy(&data.speed_kmh, payload.data(), 4);
        // Extract timestamp (4 bytes)
        std::memcpy(&data.timestamp, payload.data() + 4, 4);
    }
    return data;
}

EngineTemperatureData deserialize_engine_temp_data(const std::vector<uint8_t>& payload) {
    EngineTemperatureData data = {0};
    if (payload.size() >= 8) {
        // Extract temperature (4 bytes)
        std::memcpy(&data.temperature_celsius, payload.data(), 4);
        // Extract timestamp (4 bytes)
        std::memcpy(&data.timestamp, payload.data() + 4, 4);
    }
    return data;
}

AmbientTemperatureData deserialize_ambient_temp_data(const std::vector<uint8_t>& payload) {
    AmbientTemperatureData data = {0};
    if (payload.size() >= 8) {
        // Extract temperature (4 bytes)
        std::memcpy(&data.temperature_celsius, payload.data(), 4);
        // Extract timestamp (4 bytes)
        std::memcpy(&data.timestamp, payload.data() + 4, 4);
    }
    return data;
}

// Message handler functions
void on_speed_message(const std::shared_ptr<vsomeip::message> &request) {
    auto payload = request->get_payload();
    std::vector<uint8_t> data(payload->get_data(), payload->get_data() + payload->get_length());
    
    auto speed_data = deserialize_speed_data(data);
    message_count++;
    
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "[#" << std::setw(4) << message_count << "] ";
    std::cout << "🏃 SPEED: " << std::setw(5) << speed_data.speed_kmh << " km/h";
    if (speed_data.speed_kmh > 100.0) std::cout << " ⚠️ HIGH SPEED!";
    std::cout << " [Method 0x0001]" << std::endl;
}

void on_engine_temp_message(const std::shared_ptr<vsomeip::message> &request) {
    auto payload = request->get_payload();
    std::vector<uint8_t> data(payload->get_data(), payload->get_data() + payload->get_length());
    
    auto engine_data = deserialize_engine_temp_data(data);
    message_count++;
    
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "[#" << std::setw(4) << message_count << "] ";
    std::cout << "🔥 ENGINE: " << std::setw(5) << engine_data.temperature_celsius << "°C";
    if (engine_data.temperature_celsius > 100.0) std::cout << " 🚨 OVERHEAT!";
    std::cout << " [Method 0x0002]" << std::endl;
}

void on_ambient_temp_message(const std::shared_ptr<vsomeip::message> &request) {
    auto payload = request->get_payload();
    std::vector<uint8_t> data(payload->get_data(), payload->get_data() + payload->get_length());
    
    auto ambient_data = deserialize_ambient_temp_data(data);
    message_count++;
    
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "[#" << std::setw(4) << message_count << "] ";
    std::cout << "🌡️ AMBIENT: " << std::setw(5) << ambient_data.temperature_celsius << "°C";
    if (ambient_data.temperature_celsius < 0.0) std::cout << " ❄️ FREEZING!";
    std::cout << " [Method 0x0003]" << std::endl;
}
