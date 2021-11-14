#pragma once
#include "stm32f1xx_hal.h"
#include <optional>

namespace drv { 

class AdcTemp {
public:
    AdcTemp() = delete;
    explicit AdcTemp(float k, float b) : _k(k), _b(b), _temp(0) {}
    ~AdcTemp() = default;
    [[nodiscard]] bool init(ADC_TypeDef *instance);
    ADC_HandleTypeDef& handle() { return _adc; }
    float getTemperature() const { return _temp; }
    void irq() { _temp = _b - HAL_ADC_GetValue(&_adc) * _k; }

private:
    [[nodiscard]] bool rccEnable(const ADC_TypeDef *instance);
    std::optional<IRQn_Type> getIrq(const ADC_TypeDef *instance);
    float _k;
    float _b;
    float _temp;
    ADC_HandleTypeDef _adc;
};

}