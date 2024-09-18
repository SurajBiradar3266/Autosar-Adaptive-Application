#include "gpio.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <string>

GPIO::GPIO() {
    exportGPIO();
    setDirection("out");
}

GPIO::~GPIO() {
    unexportGPIO();
}

/*void GPIO::setValue(int value) {
    write(GPIO_PATH + "gpio" + pin_ + "/", "value", std::to_string(value));
}
*/
void GPIO::setValue_17(int value) {
    write("/sys/class/gpio/gpio17/", "value", std::to_string(value));
}

void GPIO::setValue_27(int value) {
    write("/sys/class/gpio/gpio27/", "value", std::to_string(value));
}

void GPIO::setValue_22(int value) {
    write("/sys/class/gpio/gpio22/", "value", std::to_string(value));
}


void GPIO::exportGPIO() {
    //Exporting GPIO PIN 17, 27, 22
    write(GPIO_PATH, "export", "17");
    write(GPIO_PATH, "export", "27");
    write(GPIO_PATH, "export", "22");
}

void GPIO::unexportGPIO() {
    write(GPIO_PATH, "unexport", "17");
    write(GPIO_PATH, "unexport", "27");
    write(GPIO_PATH, "unexport", "22");
}

void GPIO::setDirection(const std::string& direction) {
    write("/sys/class/gpio/gpio17/", "direction", direction);
    write("/sys/class/gpio/gpio27/", "direction", direction);
    write("/sys/class/gpio/gpio22/", "direction", direction);
}

void GPIO::write(const std::string& path, const std::string& filename, const std::string& value) {
    std::ofstream fs;
    fs.open((path + filename).c_str());
    if (!fs.is_open()) {
        perror("Failed to open file for writing");
        return;
    }
    fs << value;
    fs.close();
}

