#include <ds18b20/Ds18b20.h>

namespace drv {

[[nodiscard]] bool Ds18b20::init(OneWire *ow) noexcept {
    if(ow == nullptr)
        return false;
    _ow = ow;
    if(_ow->first()) {
      _ow->getFullRom(DS_ROM);
    }
    if(!isSensor(DS_ROM))
        return false;
    _ow->reset();
    _ow->selectWithPointer(DS_ROM);
    _ow->writeByte(DS18B20_CMD_CONVERTTEMP);
    return true;
}

bool Ds18b20::allDone() noexcept {
    return _ow->readBit();
}

bool Ds18b20::read(float &temp) noexcept {
    uint16_t temperature;
	uint8_t resolution;
	int8_t digit, minus = 0;
	float decimal;
	uint8_t i = 0;
	uint8_t data[9];
	uint8_t crc;
	
	/* Check if device is DS18B20 */
	if (!isSensor(DS_ROM)) {
		return false;
	}
	
	/* Check if line is released, if it is, then conversion is complete */
	if (!_ow->readBit()) {
		/* Conversion is not finished yet */
		return false; 
	}

	/* Reset line */
	_ow->reset();
	/* Select ROM number */
	_ow->selectWithPointer(DS_ROM);
	/* Read scratchpad command by onewire protocol */
	_ow->writeByte(static_cast<uint8_t>(OneWire::Cmd::RSCRATCHPAD));
	
	/* Get data */
	for (i = 0; i < 9; i++) {
		/* Read byte by byte */
		data[i] = _ow->readByte();
	}
	
	/* Calculate CRC */
	crc = _ow->crc8(data, 8);
	
	/* Check if CRC is ok */
	if (crc != data[8]) {
		/* CRC invalid */
		return 0;
	}
	
	/* First two bytes of scratchpad are temperature values */
	temperature = data[0] | (data[1] << 8);

	/* Reset line */
	_ow->reset();
	
	/* Check if temperature is negative */
	if (temperature & 0x8000) {
		/* Two's complement, temperature is negative */
		temperature = ~temperature + 1;
		minus = 1;
	}

	
	/* Get sensor resolution */
	resolution = ((data[4] & 0x60) >> 5) + 9;

	
	/* Store temperature integer digits and decimal digits */
	digit = temperature >> 4;
	digit |= ((temperature >> 8) & 0x7) << 4;
	
	/* Store decimal digits */
	switch (resolution) {
		case 9: {
			decimal = (temperature >> 3) & 0x01;
			decimal *= (float)DS18B20_DECIMAL_STEPS_9BIT;
		} break;
		case 10: {
			decimal = (temperature >> 2) & 0x03;
			decimal *= (float)DS18B20_DECIMAL_STEPS_10BIT;
		} break;
		case 11: {
			decimal = (temperature >> 1) & 0x07;
			decimal *= (float)DS18B20_DECIMAL_STEPS_11BIT;
		} break;
		case 12: {
			decimal = temperature & 0x0F;
			decimal *= (float)DS18B20_DECIMAL_STEPS_12BIT;
		} break;
		default: {
			decimal = 0xFF;
			digit = 0;
		}
	}
	
	/* Check for negative part */
	decimal = digit + decimal;
	if (minus) {
		decimal = 0 - decimal;
	}
	
	/* Set to pointer */
	temp = decimal;
	
	/* Return 1, temperature valid */
	return true;
}

void Ds18b20::startAll() noexcept {
    _ow->reset();
	/* Skip rom */
	_ow->writeByte(static_cast<uint8_t>(OneWire::Cmd::SKIPROM));
	/* Start conversion on all connected devices */
	_ow->writeByte(DS18B20_CMD_CONVERTTEMP);
}

}