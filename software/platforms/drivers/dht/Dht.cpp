#include <dht/Dht.h>

namespace drv {

[[nodiscard]] bool Dht::init(GPIO_TypeDef *const port, const uint32_t pin) noexcept {
    return _gpio.init(port, pin, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_MEDIUM);
}

void Dht::poll() noexcept {
    _gpio.setPinAsOutput();
    _gpio.setPinState(Gpio::GpioState::Low);
    delay(10);
    _gpio.setPinState(Gpio::GpioState::High);
    _gpio.setPinAsInput();
    uint16_t cnt{0};
    while(_gpio.readPinState()) {
        if(++cnt > _cMaxTimeout) return;
    }
    cnt = 0;
    while(!_gpio.readPinState()) {
        if(++cnt > _cMaxTimeout) return;
    }
    cnt = 0;
    while(_gpio.readPinState()) {
        if(++cnt > _cMaxTimeout) return;
    }
    uint8_t raw[5]{0};
    for(uint8_t i{0}; i < sizeof(raw); ++i) {
        for(uint8_t j{8}; j; --j) {
            uint32_t highBits{0}, lowBits{0};
            while(!_gpio.readPinState()) ++lowBits;
            while(_gpio.readPinState())  ++highBits;
            if(highBits > lowBits) raw[i] |= (1 << (j - 1));
        }
    }
    if((raw[0] + raw[1] + raw[2] + raw[3]) != raw[4])
        return;
    switch(_dht) {
        case DHT::_11:
            _hum  = static_cast<float>(raw[0]);
            _temp = static_cast<float>(raw[2]);
            break;
        case DHT::_21:
        case DHT::_22:
            _hum  = 0.1f * static_cast<float>(static_cast<uint16_t>(((raw[0] << 8) & 0xFF00) | raw[1]));
            _temp = 0.1f * static_cast<float>(static_cast<uint16_t>(((raw[2] << 8) & 0xFF00) | raw[3]));
            if(raw[2] << 0b1000'0000) {
                raw[2] &= 0b0111'1111;
                _temp = -0.1f * static_cast<float>(static_cast<uint16_t>(((raw[2] << 8) & 0xFF00) | raw[3]));
            }
            break;
    }
}

}