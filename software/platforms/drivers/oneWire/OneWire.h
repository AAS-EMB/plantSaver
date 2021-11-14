#pragma once
#include <gpio/Gpio.h>

namespace drv {

class OneWire {
public:
    OneWire() = default;
    ~OneWire() = default;
    enum class Cmd : uint8_t {
        RSCRATCHPAD   = 0xBE,
        WSCRATCHPAD	  = 0x4E,
        CPYSCRATCHPAD = 0x48,
        RECEEPROM     = 0xB8,
        RPWRSUPPLY    = 0xB4,
        SEARCHROM     = 0xF0,
        READROM	      = 0x33,
        MATCHROM      = 0x55,
        SKIPROM       = 0xCC
    };
    [[nodiscard]] bool init(GPIO_TypeDef *port, uint32_t pin);
    [[nodiscard]] bool reset();
    void writeBit(uint8_t bit);
    uint8_t readBit();
    void writeByte(uint8_t byte);
    uint8_t readByte();
    uint8_t first();
    uint8_t next();
    void resetSearch();
    [[nodiscard]] bool search(Cmd cmd);
    [[nodiscard]] bool verify();
    void targetSetup(uint8_t familyCode);
    void familySkipSetup();
    uint8_t getRom(uint8_t index);
    void select(uint8_t *addr);
    void selectWithPointer(uint8_t *rom);
    void getFullRom(uint8_t *firstIndex);
    uint8_t crc8(uint8_t *addr, uint8_t len);

protected:
    uint8_t _romNumber[8];

private:
    void delay(uint16_t timeout) {
        uint32_t startTick = DWT->CYCCNT;
        uint32_t delayTicks = timeout * (SystemCoreClock / 1000000);
        while (DWT->CYCCNT - startTick < delayTicks);
    }
    Gpio    _gpio;
    uint8_t _lastDiscrepancy;
	uint8_t _lastFamilyDiscrepancy;
	bool    _lastDeviceFlag;
    uint32_t DWT_CYCCNT  = *(volatile uint32_t *)0xE0001004;
    uint32_t DWT_CONTROL = *(volatile uint32_t *)0xE0001000;
    uint32_t SCB_DEMCR   = *(volatile uint32_t *)0xE000EDFC;
};

}