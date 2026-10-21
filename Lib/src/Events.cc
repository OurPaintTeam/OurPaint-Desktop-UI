#include "Events.h"

#include <utility>

namespace OurPaint
{
    namespace
    {
        EventCallback g_eventCallback;
    } // namespace

    void SetEventCallback(EventCallback callback)
    {
        g_eventCallback = std::move(callback);
    }

    void EmitEvent(const Event& event)
    {
        if (g_eventCallback)
        {
            g_eventCallback(event);
        }
    }

} // namespace OurPaint