#ifndef SCOMP_EVENT_BASE_H_
#define SCOMP_EVENT_BASE_H_

#include "scomp_command_base.h"

typedef scomp_command_base_t scomp_event_base_t;

/** @def SCOMP_DEFINE_EVENT
 *  @brief Statically define a event.
 *
 *  Helper macro to statically define a event.
 *
 *  @param _label event name.
 *  @param code   code value.
 */
#define SCOMP_DEFINE_EVENT(_label, code, attribute_count, ...)       \
        static scomp_event_base_t evt_##_label = { code, attribute_count, { __VA_ARGS__ } };


#endif // ! SCOMP_EVENT_BASE_H_