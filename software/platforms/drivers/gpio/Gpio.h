#pragma once
#include "stm32f1xx_hal.h"

namespace drv {

class Gpio {
public:
    Gpio() = default;
    ~Gpio() = default;
    enum class GpioState : uint8_t {
        Low,
        High
    };
    [[nodiscard]] bool init(GPIO_TypeDef *port, uint32_t pin, uint32_t mode,
                            uint32_t pull, uint32_t speed);
    void setPinAsInput();
    void setPinAsOutput();
    void setPinState(GpioState state);
    void togglePin();
    [[nodiscard]] bool readPinState() const;

private:
    static constexpr uint8_t _maxGpioPinsOnPort = 16;
    [[nodiscard]] bool rccEnable(const GPIO_TypeDef *port);
    GPIO_TypeDef *_port;
    GPIO_InitTypeDef _cfg;
};

}