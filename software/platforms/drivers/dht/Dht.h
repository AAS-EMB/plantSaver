#pragma once
#include <gpio/Gpio.h>

namespace drv {

class Dht {
public:
    Dht() = delete;
    enum class DHT : uint8_t {
        _11,
        _12,
        _21,
        _22
    };
    explicit Dht(const DHT dht)
        : _dht{dht}
    {}
    ~Dht() = default;
    [[nodiscard]] bool init(GPIO_TypeDef *const port, const uint32_t pin) noexcept;
    void poll() noexcept;
    auto convertionCtoF(const float c) const noexcept { return c * 1.8f + 32.0f; }
    auto convertionFtoC(const float f) const noexcept { return (f - 32.0f) * 0.55555f; }
    auto getHum()  const noexcept { return _hum;  }
    auto getTemp() const noexcept { return _temp; }
private:
    static constexpr uint16_t _cMaxTimeout{1000};
    void delay(const uint16_t timeout) const noexcept { HAL_Delay(timeout); }
    DHT _dht{DHT::_22};
    Gpio _gpio{};
    float _hum{0.0f};
    float _temp{0.0f};

};

}