#pragma once
#include "stm32f1xx_hal.h"
#include <optional>

namespace drv {

class Adc {
public:
    Adc() = delete;
    explicit Adc(const float coef, const float offset)
        : _adc{}
        , _raw{0}
        , _val{0.0f}
        , _coef{coef}
        , _offset{offset}
    {}
    ~Adc() = default;
    [[nodiscard]] bool init(ADC_TypeDef *const adc, const uint32_t ch, const uint32_t rank,
                            const uint32_t sampl) noexcept;
    // Getters
    auto& getHandle() noexcept    { return _adc; }
    auto  getRaw() const noexcept { return _raw; }
    auto  getVal() const noexcept { return _val; }

    void irq() noexcept { _raw = HAL_ADC_GetValue(&_adc); _val = _raw * _coef + _offset; }

    template <typename T>
    auto map(T val, T minIn, T maxIn, T minOut, T maxOut) {
        return (val - minIn) * (maxOut - minOut) / (maxIn - minIn) + minOut;
    }
private:
    [[nodiscard]] bool rccEnable(const ADC_TypeDef *const adc) const noexcept;
    std::optional<IRQn_Type> getIrq(const ADC_TypeDef *const adc) const noexcept;
    ADC_HandleTypeDef _adc;
    uint32_t _raw;
    float _val;
    float _coef;
    float _offset;
};

}