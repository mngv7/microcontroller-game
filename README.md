# Simon Says Dev Board Game

This repository shows a project completed for a subject at QUT. The project involved extensive knowledge of the QUTy development board (https://cab202.github.io/quty/), as well as C programming. 

## Table of Contents
- [Description](#description)
- [Achievements](#achievements)
- [Installation](#installation)
- [Usage](#usage)
- [Configuration](#configuration)
- [Contributing](#contributing)
- [License](#license)
- [Contact Information](#contact-information)

## Description

The game developed is similar to a 'Simon Says' game, where one of four segments are displated on the 7-segment displays, along with a correspongding tone. The user then must recreate the sequence of segments and tones with their corresponding pushbuttons. The dev board features a potentiometer, the position of the potentiometer is accessible through an ADC. The value returned by the ADC affects the playback delay, the playback delay determines how long segments are lit for when 'Simon' displays the sequence.

The sequence is generated using a linear feedback shift register, a pseudo-random number generator. The LSFR uses a seed to generate a sequence, this deterministic behaviour allows the program to seemingly generate an infinitely long sequence, without the worry for the limited memory constraints of the ATtiny1626.

Along with interaction with the pushbuttons, the user can also access the game through UART gameplay. Using serial communication, keyboard strokes (1,2,3,4), correlates to pushing the corresponding pushbutton. The addition of UART gameplay 'piggybacks' the pushbutton code, by using various flags and logical checks. This design allowed for efficient program execution as minimal steps were adding to the original pushbutton gameplay. Furthemore, the octave of the tone played for each segment can be increased or decreased, this is limited to a frequency within the human hearing range (20Hz to 20kHz).

Unfortunately due to time constraints I was unable to complete this project and could not integrate all specifications, see task_sheet.pdf for full specifications.

## Achievements

This project received a grade of 24.8/30.

**Emulator test cases (15/20):**

- Gameplay Test (4/4)
- Pushbutton Hold Test (0.5/0.5)
- Pushbutton Debouncing Test (0.5/0.5)
- Sequencing Test (3/3)
- Playback Delay Test (3/3)
- Playback Frequency Test (3/3)
- UART Octave Test (1/1)
- UART Gameplay Test (1/2)
- High Score Test (0/0.8)
- Reset Test (0/0.8)
- Seed Test (0/0.8)
- Mixed Input Test (0/0.6)

**Manual Tests (9.8/10)**

- Code, comments and logical structure (2.5/3)
- Use of pre-defined bitmasks and group configurations (1/1)
- Integer types and appropriate arithmetic operations (2/2)
- State machine usage (2/2)
- Efficient coding practices (1.3/2)

## Installation


## Usage

usage

## Configuration

config

## Contributing

contribute

## License

The license for this project is currently unspecified.

## Contact Information

**Contributors:**

Zackariya Taylor - zackariya.taylor@gmail.com

Project Link: https://github.com/mngv7/microcontroller-game
