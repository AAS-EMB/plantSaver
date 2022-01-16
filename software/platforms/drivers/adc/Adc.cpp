#include <adc/Adc.h>

namespace drv {

[[nodiscard]] bool Adc::init(ADC_TypeDef *const adc, const uint32_t ch, const uint32_t rank,
                             const uint32_t sampl) noexcept {
    if(adc == nullptr)
        return false;
    if(!rccEnable(adc))
        return false;
    _adc.Instance = adc;
    _adc.Init.ScanConvMode = ADC_SCAN_DISABLE;
    _adc.Init.ContinuousConvMode = ENABLE;
    _adc.Init.DiscontinuousConvMode = DISABLE;
    _adc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    _adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    _adc.Init.NbrOfConversion = 1;
    if (HAL_ADC_Init(&_adc) != HAL_OK)
        return false;
    ADC_ChannelConfTypeDef cfg{ch, rank, sampl};
    if(HAL_ADC_ConfigChannel(&_adc, &cfg) != HAL_OK)
        return false;
    const auto irq = getIrq(adc);
    if(!irq.has_value())
        return false;
    HAL_NVIC_SetPriority(irq.value(), 0, 0);
    HAL_NVIC_EnableIRQ(irq.value());
    return HAL_ADC_Start_IT(&_adc) == HAL_OK;
}

[[nodiscard]] bool Adc::rccEnable(const ADC_TypeDef *const adc) const noexcept {
    if(adc == ADC1) { __HAL_RCC_ADC1_CLK_ENABLE(); return true; }
    if(adc == ADC2) { __HAL_RCC_ADC2_CLK_ENABLE(); return true; }
    return false;
}

std::optional<IRQn_Type> Adc::getIrq(const ADC_TypeDef *const adc) const noexcept {
    if(adc == ADC1 || adc == ADC2) { return {ADC1_2_IRQn}; }
    return std::nullopt;
}

}