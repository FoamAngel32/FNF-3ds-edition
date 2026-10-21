#include "general.h"
#include "states/play_state.h"
StateFunction func_state_init[TOTAL_STATE] = {
    NULL,
    NULL,
    init_PlayState,
};

StateFunction func_state_update[TOTAL_STATE] = {
    NULL,
    NULL,
    update_PlayState,
};
