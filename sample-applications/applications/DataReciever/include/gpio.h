#ifndef GPIO_H
#define GPIO_H

#include <string>

#define GPIO_PATH "/sys/class/gpio/"
// #define GPIO_PIN  "17"

class GPIO {
public:
    GPIO();
    ~GPIO();

    void setValue_17(int value);
    void setValue_27(int value);
    void setValue_22(int value);

private:
    std::string pin_;  // Change pin_ to std::string

    void exportGPIO();
    void unexportGPIO();
    void setDirection(const std::string& direction);
    void write(const std::string& path, const std::string& filename, const std::string& value);
};

#endif // GPIO_H

