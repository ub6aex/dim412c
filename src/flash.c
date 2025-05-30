#include "stm32f0xx.h"
#include "flash.h"

__attribute__((section(".user_config"))) const uint32_t user_config[USER_CONFIG_LENGTH] = {
    USER_CONFIG_DMX_ADDRESS_DEFAULT,
    USER_CONFIG_BRIGHTNESS_DEFAULT,
    USER_CONFIG_DMX_ADDRESS_OFFSET_DEFAULT,
    USER_CONFIG_DMX_DISABLE_TIMEOUT_DEFAULT
};

void _FLASH_unlock(void) {
    while (FLASH->SR & FLASH_SR_BSY); // wait till no operation is on going
    if (FLASH->CR & FLASH_CR_LOCK) { // check that the Flash is locked
        FLASH->KEYR = FLASH_FKEY1; // perform unlock sequence
        FLASH->KEYR = FLASH_FKEY2;
    }
}

void _FLASH_lock(void) {
    FLASH->CR |= FLASH_CR_LOCK;
}

bool _FLASH_ready(void) {
    return !(FLASH->SR & FLASH_SR_BSY); // BSY bit is reset in the FLASH_SR register
}

bool _FLASH_checkEOP(void) {
    if(FLASH->SR & FLASH_SR_EOP) { // check the 'End of operation' flag
        FLASH->SR |= FLASH_SR_EOP; //  clear 'End of operation' flag by software
        return true;
    }
    return false;
}

uint32_t _FLASH_read(uint32_t flash_addr) {
    return (*(__IO uint32_t*)flash_addr);
}

bool _FLASH_erasePage(uint32_t page_addr) {
    while(!_FLASH_ready()); // wait flash ready

    FLASH->CR |= FLASH_CR_PER; // enable page erasing
    FLASH->AR = page_addr; // set page address
    FLASH->CR |= FLASH_CR_STRT; // start the erasing
    while(!_FLASH_ready());  // wait for page erase complete
    FLASH->CR &= ~FLASH_CR_PER; // disable the page erase

    return _FLASH_checkEOP(); // complete, clear flag
}

bool _FLASH_write(uint32_t flash_addr, uint32_t data) {
    while(!_FLASH_ready()); // wait flash ready

    FLASH->CR |= FLASH_CR_PG; // Set the PG bit in the FLASH_CR register to enable programming
    *(__IO uint16_t*)flash_addr = (uint16_t)data; // write two L bytes
    while(!_FLASH_ready()); // wait flash ready
    if(!_FLASH_checkEOP())
        return false;

    flash_addr += 2;
    data >>= 16;
    *(__IO uint16_t*)flash_addr = (uint16_t)data; // write two H bytes
    while(!_FLASH_ready()); // wait flash ready
    FLASH->CR &= ~(FLASH_CR_PG); // Reset the PG Bit to disable programming

    return _FLASH_checkEOP();
}

uint32_t FLASH_getUserConfig(uint16_t parameter) {
    return user_config[parameter];
}

bool FLASH_setUserConfig(uint16_t parameter, uint32_t value) {
    bool success = true;

    // bufferize existing config
    uint32_t config[USER_CONFIG_LENGTH];
    for (uint8_t i=0; i<USER_CONFIG_LENGTH; i++)
        config[i] = FLASH_getUserConfig(i);

    // modify desired value
    config[parameter] = value;

    _FLASH_unlock();
    // erase config page
    if (!_FLASH_erasePage((uint32_t)user_config)) {
        success = false;
    } else {
        // write updated config
        for (uint8_t i=0; i<USER_CONFIG_LENGTH; i++) {
            if (!_FLASH_write((uint32_t)(user_config+i), config[i]))
                success = false;
        }
    }
    _FLASH_lock();
    return success;
}
