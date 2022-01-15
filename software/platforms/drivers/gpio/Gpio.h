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
                            uint32_t pull, uint32_t speed) noexcept;
    void setPinAsInput() noexcept;
    void setPinAsOutput() noexcept;
    void setPinState(GpioState state) noexcept;
    void togglePin() noexcept;
    [[nodiscard]] bool readPinState() const noexcept;

private:
    static constexpr uint8_t _maxGpioPinsOnPort{16};
    [[nodiscard]] bool rccEnable(const GPIO_TypeDef *port) const noexcept;
    GPIO_TypeDef *_port{nullptr};
    GPIO_InitTypeDef _cfg{};
};

}