#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
extern "C" {
#endif

extern volatile uint8_t blinkDelay;
extern volatile uint8_t blinkDelayCounter;
extern volatile uint8_t simple_delay_counter;

void ledOn(void);
void ledOff(void);
void ledToggle(void);
void ledBlink(uint8_t delay);
void ledBlinkTask(void);

void simpleDelayCounterTask(void);

#ifdef __cplusplus
}
#endif

#endif
