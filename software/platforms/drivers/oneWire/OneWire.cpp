#include <oneWire/OneWire.h>

namespace drv {

[[nodiscard]] bool OneWire::init(GPIO_TypeDef *port, uint32_t pin) {
	if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
	return _gpio.init(port, pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM);
}

[[nodiscard]] bool OneWire::reset() {
	bool res = true;
	_gpio.setPinState(Gpio::GpioState::Low);
	_gpio.setPinAsOutput();
	delay(480);
	_gpio.setPinAsInput();
	delay(70);
	res &= !_gpio.readPinState();
	delay(410);
	return res;
}

void OneWire::writeBit(uint8_t bit) {
	if(bit) {
		_gpio.setPinState(Gpio::GpioState::Low);
		_gpio.setPinAsOutput();
		delay(10);
		_gpio.setPinAsInput();
		delay(55);
		_gpio.setPinAsInput();
	} else {
		_gpio.setPinState(Gpio::GpioState::Low);
		_gpio.setPinAsOutput();
		delay(65);
		_gpio.setPinAsInput();
		delay(5);
		_gpio.setPinAsInput();
	}
}

uint8_t OneWire::readBit() {
	uint8_t bit = 0;
	_gpio.setPinState(Gpio::GpioState::Low);
	_gpio.setPinAsOutput();
	delay(3);
	_gpio.setPinAsInput();
	delay(10);
	if(_gpio.readPinState())
		bit = 1;
	delay(50);
	return bit;
}

void OneWire::writeByte(uint8_t byte) {
	for(uint8_t i = 8; i != 0; --i) {
		writeBit(byte & 0b0000'0001);
		byte >>= 1;
	}
}

uint8_t OneWire::readByte() {
	uint8_t byte = 0;
	for(uint8_t i = 8; i != 0; --i) {
		byte >>= 1;
		byte |= (readBit() << 7);
	}
	return byte;
}

uint8_t OneWire::first() {
	resetSearch();
	return search(Cmd::SEARCHROM);
} 

uint8_t OneWire::next() {
	return search(Cmd::SEARCHROM);
}

void OneWire::resetSearch() {
	_lastDiscrepancy = 0;
	_lastDeviceFlag = 0;
	_lastFamilyDiscrepancy = 0;
}

[[nodiscard]] bool OneWire::search(Cmd cmd) {
	bool res = false;
	uint8_t idBitNum = 1;
	uint8_t lastZero = 0, romByteNum = 0;
	uint8_t idBit = 0, cmpIdBit = 0;
	uint8_t romByteMask = 1, searchDir = 0;
	if (!_lastDeviceFlag) {
		if (!reset()) {
			_lastDiscrepancy = 0;
			_lastDeviceFlag = false;
			_lastFamilyDiscrepancy = 0;
			return false;
		}
		writeByte(static_cast<uint8_t>(cmd));  
		do {
			idBit = readBit();
			cmpIdBit = readBit();
			if ((idBit == 1) && (cmpIdBit == 1)) {
				break;
			} else {
				if (idBit != cmpIdBit) {
					searchDir = idBit;
				} else {
					if (idBitNum < _lastDiscrepancy) {
						searchDir = ((_romNumber[romByteNum] & romByteMask) > 0);
					} else {
						searchDir = (idBitNum == _lastDiscrepancy);
					}
					if (searchDir == 0) {
						lastZero = idBitNum;
						if (lastZero < 9) {
							_lastFamilyDiscrepancy = lastZero;
						}
					}
				}
				if (searchDir == 1) {
					_romNumber[romByteNum] |= romByteMask;
				} else {
					_romNumber[romByteNum] &= ~romByteMask;
				}
				writeBit(searchDir);
				++idBitNum;
				romByteMask <<= 1;
				if (!romByteMask) {
					++romByteNum;
					romByteMask = 1;
				}
			}
		} while (romByteNum < 8);
		if (!(idBitNum < 65)) {
			_lastDiscrepancy = lastZero;
			if (_lastDiscrepancy == 0) {
				_lastDeviceFlag = true;
			}
			res = true;
		}
	}
	if (!res || !_romNumber[0]) {
		_lastDiscrepancy = 0;
		_lastDeviceFlag = false;
		_lastFamilyDiscrepancy = 0;
		res = false;
	}
	return res;
}

[[nodiscard]] bool OneWire::verify() {
	bool res = false;
	unsigned char romBackup[8] = {0};
	int ldBackup = 0, ldfBackup = 0, lfdBackup = 0;
	for(uint8_t i = 0; i < 8; ++i)
		romBackup[i] = _romNumber[i];
	ldBackup = _lastDiscrepancy;
	ldfBackup = _lastDeviceFlag;
	lfdBackup = _lastFamilyDiscrepancy;
	_lastDiscrepancy = 64;
	_lastDeviceFlag = false;
	if (search(Cmd::SEARCHROM)) {
		res = true;
		for (uint8_t i = 0; i < 8; ++i) {
			if (romBackup[i] != _romNumber[i]) {
				res = true;
				break;
			}
		}
	} else {
		res = false;
	}
	for (uint8_t i = 0; i < 8; ++i) {
		_romNumber[i] = romBackup[i];
	}
	_lastDiscrepancy = ldBackup;
	_lastDeviceFlag = ldfBackup;
	_lastFamilyDiscrepancy = lfdBackup;
	return res;
}

void OneWire::targetSetup(uint8_t familyCode) {
	_romNumber[0] = familyCode;
	for (uint8_t i = 1; i < 8; ++i) {
		_romNumber[i] = 0;
	}
	_lastDiscrepancy = 64;
	_lastFamilyDiscrepancy = 0;
	_lastDeviceFlag = false;
}

void OneWire::familySkipSetup() {
	_lastDiscrepancy = _lastFamilyDiscrepancy;
	_lastFamilyDiscrepancy = 0;
	if (!_lastDiscrepancy) {
		_lastDeviceFlag = true;
	}
}

uint8_t OneWire::getRom(uint8_t index) {
	return _romNumber[index];
}

void OneWire::select(uint8_t *addr) {
	writeByte(static_cast<uint8_t>(Cmd::MATCHROM));
	for(uint8_t i = 0; i < 8; ++i)
		writeByte(*(addr + i));
}

void OneWire::selectWithPointer(uint8_t *rom) {
	writeByte(static_cast<uint8_t>(Cmd::MATCHROM));
	for(uint8_t i = 0; i < 8; ++i)
		writeByte(*(rom + i));
}

void OneWire::getFullRom(uint8_t *firstIndex) {
	for(uint8_t i = 0; i < 8; ++i)
		*(firstIndex + i) = _romNumber[i];
}

uint8_t OneWire::crc8(uint8_t *addr, uint8_t len) {
	uint8_t crc = 0;
	uint8_t byte = 0;
	uint8_t mix = 0;
	while(len--) {
		byte = *addr++;
		for(uint8_t i = 8; i != 0; --i) {
			mix = (crc ^ byte) & 0b0000'0001;
			crc >>= 1;
			if(mix)
				crc ^= 0b10001100;
			byte >>= 1;
		}
	}
	return crc;
}

}