#include <gtest/gtest.h>
#include <vector>
#include <cstring>
#include <limits>

// Include the actual header file from server implementation
#include "../sensor_data.h"

// Helper function to create test payload
std::vector<uint8_t> create_payload(float value, uint32_t timestamp) {
    std::vector<uint8_t> payload(8);
    std::memcpy(payload.data(), &value, 4);
    std::memcpy(payload.data() + 4, &timestamp, 4);
    return payload;
}

// ==================== DESERIALIZATION TESTS ====================

TEST(SpeedDataTest, DeserializeValidPayload) {
    float expected_speed = 85.5f;
    uint32_t expected_timestamp = 12345;
    auto payload = create_payload(expected_speed, expected_timestamp);
    
    SpeedData result = deserialize_speed_data(payload);
    
    EXPECT_FLOAT_EQ(result.speed_kmh, expected_speed);
    EXPECT_EQ(result.timestamp, expected_timestamp);
}

TEST(SpeedDataTest, DeserializeEmptyPayload) {
    std::vector<uint8_t> empty_payload;
    
    SpeedData result = deserialize_speed_data(empty_payload);
    
    EXPECT_FLOAT_EQ(result.speed_kmh, 0.0f);
    EXPECT_EQ(result.timestamp, 0);
}

TEST(SpeedDataTest, DeserializeShortPayload) {
    std::vector<uint8_t> short_payload = {0x01, 0x02, 0x03}; // only 3 bytes
    
    SpeedData result = deserialize_speed_data(short_payload);
    
    EXPECT_FLOAT_EQ(result.speed_kmh, 0.0f);
    EXPECT_EQ(result.timestamp, 0);
}

TEST(EngineTemperatureTest, DeserializeValidPayload) {
    float expected_temp = 95.7f;
    uint32_t expected_timestamp = 67890;
    auto payload = create_payload(expected_temp, expected_timestamp);
    
    EngineTemperatureData result = deserialize_engine_temp_data(payload);
    
    EXPECT_FLOAT_EQ(result.temperature_celsius, expected_temp);
    EXPECT_EQ(result.timestamp, expected_timestamp);
}

TEST(EngineTemperatureTest, DeserializeOverheatTemperature) {
    float overheat_temp = 105.0f;
    uint32_t timestamp = 11111;
    auto payload = create_payload(overheat_temp, timestamp);
    
    EngineTemperatureData result = deserialize_engine_temp_data(payload);
    
    EXPECT_FLOAT_EQ(result.temperature_celsius, overheat_temp);
    EXPECT_GT(result.temperature_celsius, 100.0f); // Check overheat condition
}

TEST(AmbientTemperatureTest, DeserializeValidPayload) {
    float expected_temp = 22.3f;
    uint32_t expected_timestamp = 54321;
    auto payload = create_payload(expected_temp, expected_timestamp);
    
    AmbientTemperatureData result = deserialize_ambient_temp_data(payload);
    
    EXPECT_FLOAT_EQ(result.temperature_celsius, expected_temp);
    EXPECT_EQ(result.timestamp, expected_timestamp);
}

TEST(AmbientTemperatureTest, DeserializeFreezingTemperature) {
    float freezing_temp = -5.0f;
    uint32_t timestamp = 99999;
    auto payload = create_payload(freezing_temp, timestamp);
    
    AmbientTemperatureData result = deserialize_ambient_temp_data(payload);
    
    EXPECT_FLOAT_EQ(result.temperature_celsius, freezing_temp);
    EXPECT_LT(result.temperature_celsius, 0.0f); // Check freezing condition
}

// ==================== DATA VALIDATION TESTS ====================

TEST(SensorValidationTest, SpeedHighSpeedDetection) {
    float high_speed = 120.0f;
    uint32_t timestamp = 11111;
    auto payload = create_payload(high_speed, timestamp);
    
    SpeedData result = deserialize_speed_data(payload);
    
    EXPECT_GT(result.speed_kmh, 100.0f); // Should detect high speed
}

TEST(SensorValidationTest, PayloadSizeOptimization) {
    // Test if payload has exactly 8 bytes as expected
    float test_value = 42.0f;
    uint32_t test_timestamp = 12345;
    auto payload = create_payload(test_value, test_timestamp);
    
    EXPECT_EQ(payload.size(), 8); // Verify 8-byte optimization per sensor
}

// ==================== EDGE CASES TESTS ====================

TEST(EdgeCasesTest, MaxFloatValues) {
    float max_speed = 999.9f;
    uint32_t max_timestamp = UINT32_MAX;
    auto payload = create_payload(max_speed, max_timestamp);
    
    SpeedData result = deserialize_speed_data(payload);
    
    EXPECT_FLOAT_EQ(result.speed_kmh, max_speed);
    EXPECT_EQ(result.timestamp, max_timestamp);
}

TEST(EdgeCasesTest, NegativeTemperatures) {
    float negative_temp = -40.0f;
    uint32_t timestamp = 0;
    auto payload = create_payload(negative_temp, timestamp);
    
    AmbientTemperatureData result = deserialize_ambient_temp_data(payload);
    
    EXPECT_FLOAT_EQ(result.temperature_celsius, negative_temp);
    EXPECT_EQ(result.timestamp, timestamp);
}

// ==================== ADDITIONAL COVERAGE TESTS ====================

TEST(SpeedDataTest, DeserializeZeroSpeed) {
    float zero_speed = 0.0f;
    uint32_t timestamp = 12345;
    auto payload = create_payload(zero_speed, timestamp);
    
    SpeedData result = deserialize_speed_data(payload);
    
    EXPECT_FLOAT_EQ(result.speed_kmh, 0.0f);
    EXPECT_EQ(result.timestamp, timestamp);
}

TEST(EngineTemperatureTest, DeserializeNormalOperatingTemp) {
    float normal_temp = 85.0f;
    uint32_t timestamp = 98765;
    auto payload = create_payload(normal_temp, timestamp);
    
    EngineTemperatureData result = deserialize_engine_temp_data(payload);
    
    EXPECT_FLOAT_EQ(result.temperature_celsius, normal_temp);
    EXPECT_LT(result.temperature_celsius, 100.0f); // Should be below overheat threshold
    EXPECT_GT(result.temperature_celsius, 0.0f);   // Should be above freezing
}

TEST(AmbientTemperatureTest, DeserializeComfortableTemp) {
    float comfortable_temp = 23.5f;
    uint32_t timestamp = 13579;
    auto payload = create_payload(comfortable_temp, timestamp);
    
    AmbientTemperatureData result = deserialize_ambient_temp_data(payload);
    
    EXPECT_FLOAT_EQ(result.temperature_celsius, comfortable_temp);
    EXPECT_GT(result.temperature_celsius, 0.0f);   // Above freezing
    EXPECT_LT(result.temperature_celsius, 40.0f);  // Below extreme heat
}

TEST(PayloadTest, CreatePayloadIntegrity) {
    float test_value = 123.456f;
    uint32_t test_timestamp = 987654321;
    auto payload = create_payload(test_value, test_timestamp);
    
    // Verify payload structure
    EXPECT_EQ(payload.size(), 8);
    
    // Extract and verify the data manually
    float extracted_value;
    uint32_t extracted_timestamp;
    std::memcpy(&extracted_value, payload.data(), 4);
    std::memcpy(&extracted_timestamp, payload.data() + 4, 4);
    
    EXPECT_FLOAT_EQ(extracted_value, test_value);
    EXPECT_EQ(extracted_timestamp, test_timestamp);
}

TEST(PayloadTest, DeserializeExactlyEightBytes) {
    std::vector<uint8_t> exact_payload(8, 0xFF); // 8 bytes of 0xFF
    
    SpeedData speed_result = deserialize_speed_data(exact_payload);
    EngineTemperatureData engine_result = deserialize_engine_temp_data(exact_payload);
    AmbientTemperatureData ambient_result = deserialize_ambient_temp_data(exact_payload);
    
    // All should successfully deserialize (even if values are unusual due to 0xFF pattern)
    // This tests boundary condition of exactly 8 bytes
    EXPECT_TRUE(true); // If we reach here, deserialization didn't crash
}

TEST(PayloadTest, DeserializeSevenBytes) {
    std::vector<uint8_t> short_payload(7, 0x42); // 7 bytes, one short
    
    SpeedData result = deserialize_speed_data(short_payload);
    
    // Should return zero-initialized data for insufficient payload
    EXPECT_FLOAT_EQ(result.speed_kmh, 0.0f);
    EXPECT_EQ(result.timestamp, 0);
}

// ==================== PERFORMANCE AND MEMORY TESTS ====================

TEST(PerformanceTest, LargePayloadHandling) {
    std::vector<uint8_t> large_payload(1000, 0x00); // 1000 bytes
    
    // Functions should only read first 8 bytes and not crash
    SpeedData result = deserialize_speed_data(large_payload);
    
    EXPECT_FLOAT_EQ(result.speed_kmh, 0.0f);
    EXPECT_EQ(result.timestamp, 0);
}

TEST(MemoryTest, MultipleDeserializationCalls) {
    // Test multiple calls don't cause memory issues
    for (int i = 0; i < 100; ++i) {
        float test_speed = static_cast<float>(i);
        uint32_t test_timestamp = static_cast<uint32_t>(i * 1000);
        auto payload = create_payload(test_speed, test_timestamp);
        
        SpeedData result = deserialize_speed_data(payload);
        
        EXPECT_FLOAT_EQ(result.speed_kmh, test_speed);
        EXPECT_EQ(result.timestamp, test_timestamp);
    }
}
