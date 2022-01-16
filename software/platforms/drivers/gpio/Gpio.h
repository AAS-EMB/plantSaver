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
    [[nodiscard]] bool init(GPIO_TypeDef *const port, const uint32_t pin, const uint32_t mode,
                            const uint32_t pull = GPIO_NOPULL, const uint32_t speed = GPIO_SPEED_FREQ_MEDIUM) noexcept;
    void setPinAsInput() noexcept;
    void setPinAsOutput() noexcept;
    void setPinState(const GpioState state) noexcept;
    void togglePin() noexcept;
    [[nodiscard]] bool readPinState() const noexcept;

private:
    static constexpr uint8_t _maxGpioPinsOnPort{16};
    [[nodiscard]] bool rccEnable(const GPIO_TypeDef *const port) const noexcept;
    GPIO_TypeDef *_port{nullptr};
    GPIO_InitTypeDef _cfg{};
};

}