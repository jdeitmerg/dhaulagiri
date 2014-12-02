typedef struct {
    void (*funcptr)(void);
    uint32_t interval;
    uint32_t downscaled;
    uint32_t countdown;
} timer;

void timer_init(void);
void register_timer(void (*fptr)(void), uint32_t ival);
