#include <adcTemp/AdcTemp.h>

namespace drv {

[[nodiscard]] bool AdcTemp::init(ADC_TypeDef *instance) {

    if(instance == nullptr)
        return false;
    if(!rccEnable(instance))
        return false;
    _adc.Instance = instance;
    _adc.Init.ScanConvMode = ADC_SCAN_DISABLE;
    _adc.Init.ContinuousConvMode = ENABLE;
    _adc.Init.DiscontinuousConvMode = DISABLE;
    _adc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    _adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    _adc.Init.NbrOfConversion = 1;
    if (HAL_ADC_Init(&_adc) != HAL_OK)
        return false;
    ADC_ChannelConfTypeDef sConfig {ADC_CHANNEL_TEMPSENSOR, ADC_REGULAR_RANK_1, ADC_SAMPLETIME_239CYCLES_5};
    if(HAL_ADC_ConfigChannel(&_adc, &sConfig) != HAL_OK)
        return false;
    auto irq = getIrq(instance);
    if(!irq.has_value())
        return false;
    HAL_NVIC_SetPriority(irq.value(), 0, 0);
    HAL_NVIC_EnableIRQ(irq.value());
    return HAL_ADC_Start_IT(&_adc) == HAL_OK;
}

[[nodiscard]] bool AdcTemp::rccEnable(const ADC_TypeDef *instance) {
    if(instance == ADC1) { __HAL_RCC_ADC1_CLK_ENABLE(); return true; }
    if(instance == ADC2) { __HAL_RCC_ADC2_CLK_ENABLE(); return true; }
    return false;
}

std::optional<IRQn_Type> AdcTemp::getIrq(const ADC_TypeDef *instance) {
    if(instance == ADC1 || instance == ADC2) { return {ADC1_2_IRQn}; }
    return std::nullopt;
}

}