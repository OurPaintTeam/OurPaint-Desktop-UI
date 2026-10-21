#ifndef OURPAINT_EVENT_H
#define OURPAINT_EVENT_H

#include <vector>

namespace OurPaint {
    enum class EventType {
        ButtonClicked,
        KeyPressed,
        KeyReleased,
    };


    enum class Tool {
        None,

        Point,

        Line,

        CircleByRadius,
        CircleByDiameter,
        CircleByTwoPoints,
        CircleByThreePoints,
        CircleTangentToTwoLines,
        CircleTangentToThreeLines,

        ArcByRadius,
        ArcByDiameter,
        ArcByThreePoints,

        CubicBezier,

        Distance,
        HVDistance,
        Angle,

        Coincidence,
        Verticality,
        Horizontality,
        Parallelism,
        Perpendicularity,
        Equality,
        Tangent,
        FixUnfix,
        Concentricity,
        Collinearity,
        Symmetry,
        Midpoint,
    };


    struct Event {
        EventType type = EventType::ButtonClicked;
        Tool tool = Tool::None;
        // GLFW key code for KeyPressed/KeyReleased events.
        int key = 0;
        // GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, etc.
        int mods = 0;
        // True when GLFW reports a key-repeat event.
        bool repeat = false;
        // Snapshot of all currently held keys, including the event's key
        // on press and excluding it on release.
        std::vector<int> keysDown;
    };
} // namespace OurPaint

#endif // OURPAINT_EVENT_H
