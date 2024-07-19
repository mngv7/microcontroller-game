#include "timer.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "display.h"
#include "display_macros.h"

volatile uint8_t pb_debounced_state = 0xFF;

// Function: pb_debounce
// Description: Debounces the push buttons using a vertical counter method.
static void pb_debounce(void)
{
    // Vertical counter bits.
    static uint8_t count0 = 0; // Counter bit 0
    static uint8_t count1 = 0; // Counter bit 1

    uint8_t pb_sample = PORTA.IN; // Read the current state of the push buttons

    uint8_t pb_changed = (pb_sample ^ pb_debounced_state); // Detect changes

    // Vertical counter update.
    count1 = (count1 ^ count0) & pb_changed;
    count0 = ~count0 & pb_changed;

    pb_debounced_state ^= (count0 & count1); // Update the debounced state
}

// Function: spi_write
// Description: Writes data to the SPI bus, alternating between left and right bytes.
static void spi_write(void)
{
    static uint8_t current_side = 0; // Current side to write (0 for left, 1 for right)

    if (current_side)
    {
        SPI0.DATA = left_byte; // Write left byte to SPI
    }
    else
    {
        SPI0.DATA = right_byte; // Write right byte to SPI
    }

    // Toggle the current side.
    current_side = !current_side;
}

// Function: prepare_delay
// Description: Prepares the playback delay based on ADC result.
void prepare_delay(void)
{
    // Start ADC conversion
    ADC0.COMMAND |= ADC_START_IMMEDIATE_gc;

    delay_ready = ADC0.INTFLAGS & ADC_RESRDY_bm; // Check if ADC conversion is complete

    if (delay_ready)
    {
        // Read the result
        uint8_t adc_result = ADC0.RESULT;

        // Clear the Result Ready flag
        ADC0.INTFLAGS = ADC_RESRDY_bm;

        playback_delay_ms = (6.8 * adc_result) + 250; // Calculate playback delay
    }
}

// Function: playback_delay
// Description: Delays for the duration of playback_delay_ms.
void playback_delay(void)
{
    elapsed_time = 0;
    while (elapsed_time < playback_delay_ms)
    {
    } // Do nothing for the playback delay.
}

// Function: half_playback_delay
// Description: Delays for half the duration of playback_delay_ms.
void half_playback_delay(void)
{
    elapsed_time = 0;
    while (elapsed_time < (playback_delay_ms >> 1))
    {
    } // Do nothing for half the playback delay.
}

// ISR: TCB1_INT_vect
// Description: Timer interrupt service routine triggered every 5ms.
ISR(TCB1_INT_vect)
{
    pb_debounce();
    spi_write();   // Write data to SPI

    TCB1.INTFLAGS = TCB_CAPT_bm; // Clear interrupt flag
}

// ISR: TCB0_INT_vect
// Description: Timer interrupt service routine triggered every 1ms.
ISR(TCB0_INT_vect)
{
    elapsed_time++;              // Increment elapsed time
    TCB0.INTFLAGS = TCB_CAPT_bm; // Clear interrupt flag
}
