typedef struct Timer{
    void (*funcptr)(void);
    uint32_t interval;
    uint32_t downscaled;
    uint32_t countdown;
    int8_t id;	//for identifying timer at removal
    struct Timer* next; //we'll have a linked list
} timer;

void timer_init(void);
int8_t register_timer(void (*fptr)(void), uint32_t ival);
void deregister_timer(int8_t id);
