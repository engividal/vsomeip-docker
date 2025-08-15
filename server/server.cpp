// server.cpp - Central Gateway for Vehicle ECU Sensors with Method Separation
#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <iomanip>
#include "sensor_data.h"

std::shared_ptr<vsomeip::application> app;

int main() {
    app = vsomeip::runtime::get()->create_application("central_gateway");
    app->init();
    
    std::cout << "🏭 Central Gateway: Multi-Method Sensor Processor" << std::endl;
    std::cout << "📡 Methods: 0x0001(Speed), 0x0002(Engine), 0x0003(Ambient)" << std::endl;
    std::cout << "💾 Payload optimized: 8 bytes per sensor (vs 17 bytes before)" << std::endl;
    
    // Register specialized handlers for each method
    app->register_message_handler(0x1234, 0x0001, 0x0001, on_speed_message);        // Speed sensor
    app->register_message_handler(0x1234, 0x0001, 0x0002, on_engine_temp_message);  // Engine temperature
    app->register_message_handler(0x1234, 0x0001, 0x0003, on_ambient_temp_message); // Ambient temperature
    
    app->offer_service(0x1234, 0x0001);

    std::cout << "✅ Gateway ready with 3 specialized method handlers" << std::endl;
    app->start();
}
