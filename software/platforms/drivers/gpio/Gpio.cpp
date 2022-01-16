#include <gpio/Gpio.h>

namespace drv {

[[nodiscard]] bool Gpio::init(GPIO_TypeDef *const port, const uint32_t pin, const uint32_t mode,
                              const uint32_t pull, const uint32_t speed) noexcept {
    if(port == nullptr)
        return false;
    if(!rccEnable(port))
        return false;
    GPIO_InitTypeDef cfg{pin, mode, pull, speed};
    HAL_GPIO_Init(port, &cfg);
    _port = port;
    _cfg  = cfg;
    return true;
}

void Gpio::setPinAsInput() noexcept {
    _cfg.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(_port, &_cfg);
}

void Gpio::setPinAsOutput() noexcept {
    _cfg.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(_port, &_cfg);
}

void Gpio::setPinState(const GpioState state) noexcept {
    if (state == GpioState::High) {
        _port->BSRR = _cfg.Pin;
    } else {
        _port->BSRR = _cfg.Pin << _maxGpioPinsOnPort;
    }
}

void Gpio::togglePin() noexcept {
    const auto prevState{_port->ODR};
    _port->BSRR = ((prevState & _cfg.Pin) << _maxGpioPinsOnPort) | (~prevState & _cfg.Pin);
}

[[nodiscard]] bool Gpio::readPinState() const noexcept {
    return GpioState{_port->IDR & _cfg.Pin} == GpioState::High;
}

[[nodiscard]] bool Gpio::rccEnable(const GPIO_TypeDef *const port) const noexcept {
    if(port == GPIOA) { __HAL_RCC_GPIOA_CLK_ENABLE(); return true; }
    if(port == GPIOB) { __HAL_RCC_GPIOB_CLK_ENABLE(); return true; }
    if(port == GPIOC) { __HAL_RCC_GPIOC_CLK_ENABLE(); return true; }
    if(port == GPIOD) { __HAL_RCC_GPIOD_CLK_ENABLE(); return true; }
    return false;
}

}