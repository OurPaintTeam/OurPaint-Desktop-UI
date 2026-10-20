#ifndef OURPAINT_EVENTS_H
#define OURPAINT_EVENTS_H

#include <functional>
#include "Event.h"

namespace OurPaint
{
    using EventCallback = std::function<void(const Event&)>;

    void SetEventCallback(EventCallback callback);

    void EmitEvent(const Event& event);

} // namespace OurPaint

#endif // OURPAINT_EVENTS_H