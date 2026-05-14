// von Paul Stoffregen adaptiert, https://github.com/PaulStoffregen/OneWire/tree/master/util
#include <driver/rtc_io.h>
#include <soc/gpio_struct.h>

#pragma once

#define IO_REG_TYPE uint32_t
#define IO_REG_BASE_ATTR
#define IO_REG_MASK_ATTR

// static inline __attribute__((always_inline))
IO_REG_TYPE directRead(IO_REG_TYPE pin)
{
#if CONFIG_IDF_TARGET_ESP32C3
    return (GPIO.in.val >> pin) & 0x1;
#else // plain ESP32
    if ( pin < 32 )
        return (GPIO.in >> pin) & 0x1;
    else if ( pin < 46 )
        return (GPIO.in1.val >> (pin - 32)) & 0x1;
#endif
  return 0;
}

// static inline __attribute__((always_inline))
void directWriteLow(IO_REG_TYPE pin)
{
#if CONFIG_IDF_TARGET_ESP32C3
    GPIO.out_w1tc.val = ((uint32_t)1 << pin);
#else // plain ESP32
    if ( pin < 32 )
        GPIO.out_w1tc = ((uint32_t)1 << pin);
    else if ( pin < 46 )
        GPIO.out1_w1tc.val = ((uint32_t)1 << (pin - 32));
#endif
}

// static inline __attribute__((always_inline))
void directWriteHigh(IO_REG_TYPE pin)
{
#if CONFIG_IDF_TARGET_ESP32C3
    GPIO.out_w1ts.val = ((uint32_t)1 << pin);
#else // plain ESP32
    if ( pin < 32 )
        GPIO.out_w1ts = ((uint32_t)1 << pin);
    else if ( pin < 46 )
        GPIO.out1_w1ts.val = ((uint32_t)1 << (pin - 32));
#endif
}

// static inline __attribute__((always_inline))
void directModeInput(IO_REG_TYPE pin)
{
#if CONFIG_IDF_TARGET_ESP32C3
    GPIO.enable_w1tc.val = ((uint32_t)1 << (pin));
#else
    if ( true )// digitalPinIsValid(pin)
    {

	// Input
        if ( pin < 32 )
            GPIO.enable_w1tc = ((uint32_t)1 << pin);
        else
            GPIO.enable1_w1tc.val = ((uint32_t)1 << (pin - 32));
    }
#endif
}

// static inline __attribute__((always_inline))
void directModeOutput(IO_REG_TYPE pin)
{
#if CONFIG_IDF_TARGET_ESP32C3
    GPIO.enable_w1ts.val = ((uint32_t)1 << (pin));
#else
    if ( pin <= 33 ) // pins above 33 can be only inputs
    {

        // Output
        if ( pin < 32 )
            GPIO.enable_w1ts = ((uint32_t)1 << pin);
        else // already validated to pins <= 33
            GPIO.enable1_w1ts.val = ((uint32_t)1 << (pin - 32));
    }
#endif
}

