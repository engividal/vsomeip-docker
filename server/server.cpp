// server.cpp
#include <vsomeip/vsomeip.hpp>
#include <iostream>

std::shared_ptr<vsomeip::application> app;
int client_count = 0;

void on_message(const std::shared_ptr<vsomeip::message> &request) {
    std::cout << "Server: Received message from client!" << std::endl;
    client_count++;
    std::cout << "Total clients communicated: " << client_count << std::endl;
}

int main() {
    app = vsomeip::runtime::get()->create_application("server");
    app->init();
    
    std::cout << "Server: Initializing..." << std::endl;
    
    app->register_message_handler(0x1234, 0x0001, 0x0001, on_message);
    app->offer_service(0x1234, 0x0001);

    std::cout << "Server: Offering service 0x1234, instance 0x0001" << std::endl;
    app->start();
}
