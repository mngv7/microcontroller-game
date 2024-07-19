#include <stdint.h>

volatile uint16_t playback_delay_ms;
extern volatile uint8_t pb_debounced_state;
volatile uint16_t elapsed_time;
void prepare_delay(void);
void playback_delay(void);
void half_playback_delay(void);
uint8_t delay_ready;