// client.cpp
// vSomeIP client application

#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <memory>
#include <vector>

static std::shared_ptr<vsomeip::application> app;

void on_availability(vsomeip::service_t service,
                     vsomeip::instance_t instance,
                     bool is_available) {
    std::cout << "on_availability: service=0x" << std::hex << service
              << ", instance=0x" << instance
              << ", available=" << std::boolalpha << is_available << std::dec << std::endl;

    if (is_available) {
        std::cout << "Client: Service available. Sending request..." << std::endl;
        
        // Build request
        auto request = vsomeip::runtime::get()->create_request();
        request->set_service(service);
        request->set_instance(instance);
        request->set_method(0x0001);

        // Create payload
        auto payload = vsomeip::runtime::get()->create_payload();
        std::vector<vsomeip::byte_t> data = {0x01, 0x02, 0x03};
        payload->set_data(data);
        request->set_payload(payload);

        // Send request
        app->send(request);
        std::cout << "Client: Request sent." << std::endl;
    }
}

void on_response(const std::shared_ptr<vsomeip::message>& response) {
    std::cout << "Client: Received response for method 0x"
              << std::hex << response->get_method() << std::dec << std::endl;

    auto payload = response->get_payload();
    const vsomeip::byte_t* data = payload->get_data();
    size_t length = payload->get_length();

    std::cout << "Response payload:";
    for (size_t i = 0; i < length; ++i) {
        std::cout << " " << std::hex << static_cast<int>(data[i]);
    }
    std::cout << std::dec << std::endl;
}

int main() {
    // Initialize application
    app = vsomeip::runtime::get()->create_application("client");
    if (!app->init()) {
        std::cerr << "Client: Application initialization failed!" << std::endl;
        return 1;
    }

    // Register handlers
    app->register_availability_handler(0x1234, 0x0001, on_availability);
    app->register_message_handler(0x1234, 0x0001, 0x0001, on_response);

    // Discover service
    app->request_service(0x1234, 0x0001);

    std::cout << "Client: Starting main loop..." << std::endl;
    app->start();
    return 0;
}
