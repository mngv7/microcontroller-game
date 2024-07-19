#include <stdint.h>

extern volatile uint8_t left_byte;
extern volatile uint8_t right_byte;

void update_display(const uint8_t left, const uint8_t right);
void display_segment(uint8_t step);