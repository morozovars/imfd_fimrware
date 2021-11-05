#ifndef EVENTS_H_
#define EVENTS_H_
 
typedef enum
{
  EVENT_IM_ALIVE = 0
}event_t;

void events_init(void);
void event_send(event_t evt);

#endif // ! EVENTS_H_