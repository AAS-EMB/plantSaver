#pragma once
#include <oneWire/OneWire.h>

namespace drv {

#define DS18B20_FAMILY_CODE				0x28
#define DS18B20_CMD_ALARMSEARCH			0xEC

/* DS18B20 read temperature command */
#define DS18B20_CMD_CONVERTTEMP			0x44 	/* Convert temperature */
#define DS18B20_DECIMAL_STEPS_12BIT		0.0625
#define DS18B20_DECIMAL_STEPS_11BIT		0.125
#define DS18B20_DECIMAL_STEPS_10BIT		0.25
#define DS18B20_DECIMAL_STEPS_9BIT		0.5

/* Bits locations for resolution */
#define DS18B20_RESOLUTION_R1			6
#define DS18B20_RESOLUTION_R0			5

/* CRC enabled */
#ifdef DS18B20_USE_CRC	
#define DS18B20_DATA_LEN				9
#else
#define DS18B20_DATA_LEN				2
#endif

class Ds18b20 {
public:
    Ds18b20() = default;
    ~Ds18b20() = default;
    enum class Resolution : uint8_t {
        bits9,
        bits10,
        bits11,
        bits12
    };
    [[nodiscard]] bool init(OneWire *ow) noexcept;
    bool start() noexcept;
    void startAll() noexcept;
    bool read(float &temp) noexcept;
    uint8_t getResolution() noexcept;
    bool setResolution(Resolution resolution) noexcept;
    bool isSensor(uint8_t *ROM) noexcept {if (*ROM == DS18B20_FAMILY_CODE) {
		return true;
	}
	return false;}
    bool allDone() noexcept;

private:
    OneWire *_ow;
    uint8_t DS_ROM[8];
};

}