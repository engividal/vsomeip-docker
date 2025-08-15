#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sstream>
#include <iostream>
#include "../message_handlers.h"
#include "../sensor_data.h"

// Mock class for vsomeip::payload
class MockPayload {
public:
    MockPayload(const std::vector<uint8_t>& data) : data_(data) {}
    
    const uint8_t* get_data() const { return data_.data(); }
    vsomeip::length_t get_length() const { return static_cast<vsomeip::length_t>(data_.size()); }
    
private:
    std::vector<uint8_t> data_;
};

// Mock class for vsomeip::message
class MockMessage {
public:
    MockMessage(const std::vector<uint8_t>& payload_data) 
        : payload_(std::make_shared<MockPayload>(payload_data)) {}
    
    std::shared_ptr<MockPayload> get_payload() const { return payload_; }
    
private:
    std::shared_ptr<MockPayload> payload_;
};

// Helper function to create test payload
std::vector<uint8_t> create_test_payload(float value, uint32_t timestamp) {
    std::vector<uint8_t> payload(8);
    std::memcpy(payload.data(), &value, 4);
    std::memcpy(payload.data() + 4, &timestamp, 4);
    return payload;
}

// Helper function to capture console output
std::string capture_console_output(std::function<void()> func) {
    std::ostringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    func();
    std::cout.rdbuf(old);
    return buffer.str();
}

// ==================== MESSAGE HANDLER TESTS ====================

class MessageHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        reset_message_count();
    }
    
    void TearDown() override {
        reset_message_count();
    }
};

TEST_F(MessageHandlerTest, SpeedMessageHandlerNormalSpeed) {
    float test_speed = 85.5f;
    uint32_t timestamp = 12345;
    auto payload_data = create_test_payload(test_speed, timestamp);
    
    auto mock_message = std::make_shared<MockMessage>(payload_data);
    
    std::string output = capture_console_output([&]() {
        on_speed_message(std::reinterpret_pointer_cast<vsomeip::message>(mock_message));
    });
    
    EXPECT_EQ(get_message_count(), 1);
    EXPECT_THAT(output, ::testing::HasSubstr("SPEED:"));
    EXPECT_THAT(output, ::testing::HasSubstr("85.5"));
    EXPECT_THAT(output, ::testing::HasSubstr("km/h"));
    EXPECT_THAT(output, ::testing::HasSubstr("[Method 0x0001]"));
    EXPECT_THAT(output, ::testing::Not(::testing::HasSubstr("HIGH SPEED")));
}

TEST_F(MessageHandlerTest, SpeedMessageHandlerHighSpeed) {
    float high_speed = 120.0f;
    uint32_t timestamp = 98765;
    auto payload_data = create_test_payload(high_speed, timestamp);
    
    auto mock_message = std::make_shared<MockMessage>(payload_data);
    
    std::string output = capture_console_output([&]() {
        on_speed_message(std::reinterpret_pointer_cast<vsomeip::message>(mock_message));
    });
    
    EXPECT_EQ(get_message_count(), 1);
    EXPECT_THAT(output, ::testing::HasSubstr("SPEED:"));
    EXPECT_THAT(output, ::testing::HasSubstr("120.0"));
    EXPECT_THAT(output, ::testing::HasSubstr("⚠️ HIGH SPEED!"));
}

TEST_F(MessageHandlerTest, EngineTemperatureHandlerNormalTemp) {
    float normal_temp = 85.0f;
    uint32_t timestamp = 54321;
    auto payload_data = create_test_payload(normal_temp, timestamp);
    
    auto mock_message = std::make_shared<MockMessage>(payload_data);
    
    std::string output = capture_console_output([&]() {
        on_engine_temp_message(std::reinterpret_pointer_cast<vsomeip::message>(mock_message));
    });
    
    EXPECT_EQ(get_message_count(), 1);
    EXPECT_THAT(output, ::testing::HasSubstr("ENGINE:"));
    EXPECT_THAT(output, ::testing::HasSubstr("85.0"));
    EXPECT_THAT(output, ::testing::HasSubstr("°C"));
    EXPECT_THAT(output, ::testing::HasSubstr("[Method 0x0002]"));
    EXPECT_THAT(output, ::testing::Not(::testing::HasSubstr("OVERHEAT")));
}

TEST_F(MessageHandlerTest, EngineTemperatureHandlerOverheat) {
    float overheat_temp = 105.0f;
    uint32_t timestamp = 11111;
    auto payload_data = create_test_payload(overheat_temp, timestamp);
    
    auto mock_message = std::make_shared<MockMessage>(payload_data);
    
    std::string output = capture_console_output([&]() {
        on_engine_temp_message(std::reinterpret_pointer_cast<vsomeip::message>(mock_message));
    });
    
    EXPECT_EQ(get_message_count(), 1);
    EXPECT_THAT(output, ::testing::HasSubstr("ENGINE:"));
    EXPECT_THAT(output, ::testing::HasSubstr("105.0"));
    EXPECT_THAT(output, ::testing::HasSubstr("🚨 OVERHEAT!"));
}

TEST_F(MessageHandlerTest, AmbientTemperatureHandlerNormalTemp) {
    float normal_temp = 22.5f;
    uint32_t timestamp = 99999;
    auto payload_data = create_test_payload(normal_temp, timestamp);
    
    auto mock_message = std::make_shared<MockMessage>(payload_data);
    
    std::string output = capture_console_output([&]() {
        on_ambient_temp_message(std::reinterpret_pointer_cast<vsomeip::message>(mock_message));
    });
    
    EXPECT_EQ(get_message_count(), 1);
    EXPECT_THAT(output, ::testing::HasSubstr("AMBIENT:"));
    EXPECT_THAT(output, ::testing::HasSubstr("22.5"));
    EXPECT_THAT(output, ::testing::HasSubstr("°C"));
    EXPECT_THAT(output, ::testing::HasSubstr("[Method 0x0003]"));
    EXPECT_THAT(output, ::testing::Not(::testing::HasSubstr("FREEZING")));
}

TEST_F(MessageHandlerTest, AmbientTemperatureHandlerFreezing) {
    float freezing_temp = -10.0f;
    uint32_t timestamp = 77777;
    auto payload_data = create_test_payload(freezing_temp, timestamp);
    
    auto mock_message = std::make_shared<MockMessage>(payload_data);
    
    std::string output = capture_console_output([&]() {
        on_ambient_temp_message(std::reinterpret_pointer_cast<vsomeip::message>(mock_message));
    });
    
    EXPECT_EQ(get_message_count(), 1);
    EXPECT_THAT(output, ::testing::HasSubstr("AMBIENT:"));
    EXPECT_THAT(output, ::testing::HasSubstr("-10.0"));
    EXPECT_THAT(output, ::testing::HasSubstr("❄️ FREEZING!"));
}

// ==================== MESSAGE COUNT TESTS ====================

TEST_F(MessageHandlerTest, MessageCountIncrementsCorrectly) {
    float test_speed = 50.0f;
    uint32_t timestamp = 12345;
    auto payload_data = create_test_payload(test_speed, timestamp);
    auto mock_message = std::make_shared<MockMessage>(payload_data);
    
    EXPECT_EQ(get_message_count(), 0);
    
    // Process multiple messages and verify count increments
    for (int i = 1; i <= 5; ++i) {
        capture_console_output([&]() {
            on_speed_message(std::reinterpret_pointer_cast<vsomeip::message>(mock_message));
        });
        EXPECT_EQ(get_message_count(), i);
    }
}

TEST_F(MessageHandlerTest, MessageCountPersistsAcrossDifferentHandlers) {
    float speed = 60.0f;
    float engine_temp = 90.0f;
    float ambient_temp = 25.0f;
    uint32_t timestamp = 12345;
    
    auto speed_payload = create_test_payload(speed, timestamp);
    auto engine_payload = create_test_payload(engine_temp, timestamp);
    auto ambient_payload = create_test_payload(ambient_temp, timestamp);
    
    auto speed_message = std::make_shared<MockMessage>(speed_payload);
    auto engine_message = std::make_shared<MockMessage>(engine_payload);
    auto ambient_message = std::make_shared<MockMessage>(ambient_payload);
    
    EXPECT_EQ(get_message_count(), 0);
    
    // Process one message from each handler
    capture_console_output([&]() {
        on_speed_message(std::reinterpret_pointer_cast<vsomeip::message>(speed_message));
    });
    EXPECT_EQ(get_message_count(), 1);
    
    capture_console_output([&]() {
        on_engine_temp_message(std::reinterpret_pointer_cast<vsomeip::message>(engine_message));
    });
    EXPECT_EQ(get_message_count(), 2);
    
    capture_console_output([&]() {
        on_ambient_temp_message(std::reinterpret_pointer_cast<vsomeip::message>(ambient_message));
    });
    EXPECT_EQ(get_message_count(), 3);
}

// ==================== BOUNDARY VALUE TESTS ====================

TEST_F(MessageHandlerTest, SpeedHandlerExactBoundaryValues) {
    // Test exactly 100.0 km/h (boundary case)
    float boundary_speed = 100.0f;
    uint32_t timestamp = 12345;
    auto payload_data = create_test_payload(boundary_speed, timestamp);
    auto mock_message = std::make_shared<MockMessage>(payload_data);
    
    std::string output = capture_console_output([&]() {
        on_speed_message(std::reinterpret_pointer_cast<vsomeip::message>(mock_message));
    });
    
    EXPECT_THAT(output, ::testing::HasSubstr("100.0"));
    EXPECT_THAT(output, ::testing::Not(::testing::HasSubstr("HIGH SPEED")));
    
    // Test 100.1 km/h (just over boundary)
    float over_boundary_speed = 100.1f;
    auto over_payload = create_test_payload(over_boundary_speed, timestamp);
    auto over_message = std::make_shared<MockMessage>(over_payload);
    
    std::string over_output = capture_console_output([&]() {
        on_speed_message(std::reinterpret_pointer_cast<vsomeip::message>(over_message));
    });
    
    EXPECT_THAT(over_output, ::testing::HasSubstr("100.1"));
    EXPECT_THAT(over_output, ::testing::HasSubstr("HIGH SPEED"));
}

TEST_F(MessageHandlerTest, EngineTemperatureHandlerBoundaryValues) {
    // Test exactly 100.0°C (boundary case)
    float boundary_temp = 100.0f;
    uint32_t timestamp = 12345;
    auto payload_data = create_test_payload(boundary_temp, timestamp);
    auto mock_message = std::make_shared<MockMessage>(payload_data);
    
    std::string output = capture_console_output([&]() {
        on_engine_temp_message(std::reinterpret_pointer_cast<vsomeip::message>(mock_message));
    });
    
    EXPECT_THAT(output, ::testing::HasSubstr("100.0"));
    EXPECT_THAT(output, ::testing::Not(::testing::HasSubstr("OVERHEAT")));
    
    // Test 100.1°C (just over boundary)
    float over_boundary_temp = 100.1f;
    auto over_payload = create_test_payload(over_boundary_temp, timestamp);
    auto over_message = std::make_shared<MockMessage>(over_payload);
    
    std::string over_output = capture_console_output([&]() {
        on_engine_temp_message(std::reinterpret_pointer_cast<vsomeip::message>(over_message));
    });
    
    EXPECT_THAT(over_output, ::testing::HasSubstr("100.1"));
    EXPECT_THAT(over_output, ::testing::HasSubstr("OVERHEAT"));
}

TEST_F(MessageHandlerTest, AmbientTemperatureHandlerBoundaryValues) {
    // Test exactly 0.0°C (boundary case)
    float boundary_temp = 0.0f;
    uint32_t timestamp = 12345;
    auto payload_data = create_test_payload(boundary_temp, timestamp);
    auto mock_message = std::make_shared<MockMessage>(payload_data);
    
    std::string output = capture_console_output([&]() {
        on_ambient_temp_message(std::reinterpret_pointer_cast<vsomeip::message>(mock_message));
    });
    
    EXPECT_THAT(output, ::testing::HasSubstr("0.0"));
    EXPECT_THAT(output, ::testing::Not(::testing::HasSubstr("FREEZING")));
    
    // Test -0.1°C (just below boundary)
    float below_boundary_temp = -0.1f;
    auto below_payload = create_test_payload(below_boundary_temp, timestamp);
    auto below_message = std::make_shared<MockMessage>(below_payload);
    
    std::string below_output = capture_console_output([&]() {
        on_ambient_temp_message(std::reinterpret_pointer_cast<vsomeip::message>(below_message));
    });
    
    EXPECT_THAT(below_output, ::testing::HasSubstr("-0.1"));
    EXPECT_THAT(below_output, ::testing::HasSubstr("FREEZING"));
}

// ==================== HELPER FUNCTION TESTS ====================

TEST_F(MessageHandlerTest, ResetMessageCountWorks) {
    // Increment counter
    float test_speed = 60.0f;
    uint32_t timestamp = 12345;
    auto payload_data = create_test_payload(test_speed, timestamp);
    auto mock_message = std::make_shared<MockMessage>(payload_data);
    
    capture_console_output([&]() {
        on_speed_message(std::reinterpret_pointer_cast<vsomeip::message>(mock_message));
    });
    
    EXPECT_EQ(get_message_count(), 1);
    
    // Reset and verify
    reset_message_count();
    EXPECT_EQ(get_message_count(), 0);
}

// ==================== INTEGRATION TESTS ====================

TEST_F(MessageHandlerTest, MixedMessageProcessingSequence) {
    // Simulate a realistic sequence of mixed sensor messages
    struct TestMessage {
        std::string type;
        float value;
        std::function<void(std::shared_ptr<vsomeip::message>)> handler;
    };
    
    std::vector<TestMessage> messages = {
        {"speed", 45.0f, on_speed_message},
        {"engine", 85.0f, on_engine_temp_message},
        {"ambient", 23.0f, on_ambient_temp_message},
        {"speed", 110.0f, on_speed_message},  // High speed
        {"engine", 105.0f, on_engine_temp_message},  // Overheat
        {"ambient", -5.0f, on_ambient_temp_message}   // Freezing
    };
    
    for (size_t i = 0; i < messages.size(); ++i) {
        auto payload = create_test_payload(messages[i].value, static_cast<uint32_t>(i + 1000));
        auto mock_message = std::make_shared<MockMessage>(payload);
        
        capture_console_output([&]() {
            messages[i].handler(std::reinterpret_pointer_cast<vsomeip::message>(mock_message));
        });
        
        EXPECT_EQ(get_message_count(), static_cast<int>(i + 1));
    }
    
    EXPECT_EQ(get_message_count(), 6);
}
