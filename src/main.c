#include <avr/interrupt.h>
#include "buzzer.h"
#include "display.h"
#include <util/delay.h>
#include "display_macros.h"
#include "initialisation.h"
#include "timer.h"
#include "types.h"
#include "uart.h"

// Variables for pushbutton/key press handling:
uint8_t pb_sample = 0xFF;
uint8_t pb_sample_r = 0xFF;
uint8_t pb_changed;
uint8_t pb_falling;
uint8_t pb_rising;
volatile uint8_t key_pressed = 0;
uint8_t pb_released = 0;
uint8_t pushbutton_received = 0;

// Variables for gameplay state:
uint8_t user_input = 0;
uint8_t input_count = 0;
uint8_t user_correct = 1;

// Linear Shift Feedback Register (LSFR) variables:
uint32_t mask = 0xE2023CAB;
uint32_t re_init_state = 0x11592931;
uint32_t state_lsfr;
uint8_t step;
uint8_t result;

// Display variables:
uint8_t right_digit;
uint8_t left_digit;

// Initialise state machines.
buttons button = WAIT;
gameplay_stages gameplay_stage = INIT;

// Function: Linear Shift Feedback Register
// Description: Produces a deterministic pseudo-random number.
// Parameters:
//  - state: Pointer to the current state of the LSFR
//  - step: Pointer to the variable to store the next step
//  - result: Pointer to store the least significant bit of the statevoid
void LSFR(uint32_t *state, uint8_t *step, uint8_t *result)
{
    *result = *state & 1;
    *state >>= 1;

    if (*result)
    {
        *state ^= mask;
    }
    *step = (uint8_t)(*state & 0b11);
}

// Function: check_edge
// Description: Checks for rising or falling edges from the pushbuttons.
void check_edge(void)
{
    pb_sample_r = pb_sample;
    pb_sample = pb_debounced_state;

    pb_changed = pb_sample_r ^ pb_sample;

    pb_falling = pb_changed & pb_sample_r;
    pb_rising = pb_changed & pb_sample;
}

// Struct containing enum type buttons and their associated bitmasks.
typedef struct buttons_pins
{
    uint8_t pin;
    buttons button;
} button_pin;

// Array of mapped bitmasks and buttons.
button_pin arr[4] = {{PIN4_bm, BUTTON1}, {PIN5_bm, BUTTON2}, {PIN6_bm, BUTTON3}, {PIN7_bm, BUTTON4}};

// Function: extract_digits
// Description: Separates an integer into its tens and units digits
// Parameters:
//  - number: The number to be split into digits
//  - left_digit: Pointer to store the tens digit
//  - right_digit: Pointer to store the units digit
void extract_digits(uint32_t number, uint8_t *left_digit, uint8_t *right_digit)
{
    uint8_t tens_count;

    while (number > 10)
    {
        number -= 10;
        tens_count++;
    }

    if (tens_count < 1)
    {
        *left_digit = 10; // Setting as ten will leave the display blank.
    }
    else
    {
        *left_digit = tens_count;
    }

    *right_digit = number;
}

// Indexed digits encoded within hexadecimal values.
volatile uint8_t segs[] = {0x08, 0x6B, 0x44, 0x41, 0x23, 0x11, 0x10, 0x4B, 0x00, 0x01, 0xFF};

// Function: handle_button
// Description: Handles button press events during the PLAYER stage
// Parameters:
//  - button_index: Index of the button pressed
void handle_button(uint8_t button_index)
{
    buzzer_on(button_index);
    display_segment(button_index);

    // Check if user's input was correct
    if (step != button_index)
    {
        user_correct = 0;
    }

    if (!pb_released)
    {
        if (pb_rising & arr[button_index].pin) // If the pushbutton was released.
        {
            pb_released = 1;
            pushbutton_received = 0; // Reset flag.
        }
        else if (!pushbutton_received) // If there was not a pushbutton input (UART input).
        {
            pb_released = 1;
        }
    }
    else
    {
        // If playback delay has elapsed since the button press.
        if (elapsed_time >= (playback_delay_ms >> 1))
        {
            buzzer_off();
            display_segment(4);
            user_input = 1;  // Set user input flage.
            pb_released = 0; // Reset pushbutton released flag.
            button = WAIT;
        }
    }
}

int main(void)
{
    cli(); // Disable interrupts for initialisation functions
    adc_init();
    button_init();
    spi_init();
    pwm_init();
    timers_init();
    uart_init();
    sei(); // Enable interrupts

    uint16_t sequence_length;

    while (1)
    {
        check_edge();

        switch (gameplay_stage)
        {
        case INIT:
            sequence_length = 1; // Unitialise the sequence length to 1 on reset/initialisation.
            gameplay_stage = SIMON;
            break;
        case SIMON:
            state_lsfr = re_init_state; // Initialise state to recreate the same sequence of steps as re_init_state.
            prepare_delay();
            if (delay_ready)
            {
                for (int i = 0; i < sequence_length; i++)
                {
                    LSFR(&state_lsfr, &step, &result); // Create new step
                    buzzer_on(step);
                    display_segment(step);
                    half_playback_delay(); // Half delay
                    buzzer_off();
                    display_segment(4);    // Display off.
                    half_playback_delay(); // Half delay
                }
                state_lsfr = re_init_state; // Re-initialise state to recreate the same sequence, for the user's, as displayed by Simon.
                gameplay_stage = PLAYER;
            }
            break;
        case PLAYER:
            switch (button)
            {
            case WAIT:
                for (int i = 0; i < 4; i++) // Loop through each button/key.
                {
                    if ((pb_falling | key_pressed) & arr[i].pin) // Check if pushbutton or key pressed.
                    {
                        LSFR(&state_lsfr, &step, &result); // Update the step to compare the user's input to.
                        key_pressed = 0;                   // Rest key pressed flag bitmask.
                        elapsed_time = 0;                  // Start timer.
                        input_count++;                     // Log input.
                        button = arr[i].button;            // Change states.
                    }

                    // Check if the input came from the pushbutton.
                    if (pb_falling & arr[i].pin)
                    {
                        pushbutton_received = 1;
                    }
                }
                break;
            case BUTTON1:
                handle_button(0);
                break;
            case BUTTON2:
                handle_button(1);
                break;
            case BUTTON3:
                handle_button(2);
                break;
            case BUTTON4:
                handle_button(3);
                break;
            default:
                button = WAIT;
                break;
            }

            if (user_input) // If user input is detected.
            {
                if (!user_correct)
                {
                    user_correct = 1; // Reset flag.
                    input_count = 0;  // Reset flag.
                    gameplay_stage = FAIL;
                }
                else
                {
                    if (input_count == sequence_length) // If the number of inputs matches the sequence length.
                    {
                        input_count = 0; // Reset count.
                        gameplay_stage = SUCCESS;
                    }
                }
                user_input = 0; // Reset user input flag.
            }
            break;
        case SUCCESS:
            update_display(0, 0); // Success pattern.
            playback_delay();
            display_segment(4); // Display off.
            sequence_length++;
            gameplay_stage = SIMON;
            break;
        case FAIL:
            update_display(0b01110111, 0b01110111); // Fail pattern.
            playback_delay();
            extract_digits(sequence_length, &left_digit, &right_digit); // Extract digits from the sequence length (user's score) to be displayed.
            update_display(segs[left_digit], segs[right_digit]);
            playback_delay();
            display_segment(4); // Display off.
            playback_delay();
            LSFR(&state_lsfr, &step, &result); // Get next step to re-initialise to.
            re_init_state = state_lsfr;        // Re-initialise sequence to where it was left off.
            gameplay_stage = INIT;
            break;
        default:
            gameplay_stage = INIT;
            break;
        }
    }
}