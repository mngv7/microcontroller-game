typedef enum
{
    INIT,
    SIMON,
    PLAYER,
    SUCCESS,
    FAIL
} gameplay_stages;

typedef enum
{
    WAIT,
    BUTTON1,
    BUTTON2,
    BUTTON3,
    BUTTON4
} buttons;

buttons button;
gameplay_stages gameplay_stage;