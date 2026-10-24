#include "general.h"
#include "states/play_state.h"
#include "states/title_state.h"
#include "events.h"
StateFunction func_state_init[TOTAL_STATE] = {
    init_TitleState,
    NULL,
    NULL,
    NULL,
    init_PlayState,
};

StateFunction func_state_update[TOTAL_STATE] = {
    update_TitleState,
    NULL,
    NULL,
    NULL,
    update_PlayState,
};

EventTrigger events_trigger[TOTAL_EVENT_TYPES] = {
    event_FocusCamera,
};
