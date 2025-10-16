#include <iostream>
#include <chrono>
#include <iomanip> // Required for std::put_time

int main() {

    // Get current time using system_clock
    auto now = std::chrono::system_clock::now();
    // Convert to time_t (seconds since epoch)
    std::time_t time_now = std::chrono::system_clock::to_time_t(now);
    // Convert to local time structure
    std::tm local_tm = *std::localtime(&time_now);
    // Format and print the time with timezone    
    auto time = std::put_time(&local_tm, "%b-%d-%Y %H:%M:%S %Z");
    //print the time in seconds since epoch
    std::cout << "Current time: " << time << std::endl;

    return 0;
}