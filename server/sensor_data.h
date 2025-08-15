#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <vector>
#include <cstdint>
#include <memory>
#include <vsomeip/vsomeip.hpp>

// Sensor data structures
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

// Deserialization function declarations
SpeedData deserialize_speed_data(const std::vector<uint8_t>& payload);
EngineTemperatureData deserialize_engine_temp_data(const std::vector<uint8_t>& payload);
AmbientTemperatureData deserialize_ambient_temp_data(const std::vector<uint8_t>& payload);

// Message handler function declarations (for testing)
void on_speed_message(const std::shared_ptr<vsomeip::message> &request);
void on_engine_temp_message(const std::shared_ptr<vsomeip::message> &request);
void on_ambient_temp_message(const std::shared_ptr<vsomeip::message> &request);

// Global message counter (for testing)
extern int message_count;

#endif // SENSOR_DATA_H
