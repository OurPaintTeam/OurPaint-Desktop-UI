#ifndef OURPAINT_DESKTOP_UI_TOOLSTYPE_H
#define OURPAINT_DESKTOP_UI_TOOLSTYPE_H

#include <QMetaType>

namespace UI {
    enum class ContainerType {
        Tools,
        Side
    };


    enum class PrimitiveType {
        Point,
        CubicBezier,

        Line,
        Polyline,
        InfiniteLine,
        LineSettings,

        CircleByRadius,
        CircleByDiameter,
        CircleByTwoPoints,
        CircleByThreePoints,
        CircleTangentTwoLines,
        CircleTangentThreeLines,

        ArcByRadius,
        ArcByDiameter,
        ArcThreePoints,
        ArcSettings
    };


    enum class ConstraintType {
        Coincident,
        Horizontal,
        Vertical,
        Parallel,
        Perpendicular,
        Tangent,
        Equal,
        FixUnfix,
        Concentric,
        Collinear,
        Midpoint,
        Symmetric,
        Distance
    };


    enum class ToolsType {
        Cursor,
        Size
    };
} // namespace UI

Q_DECLARE_METATYPE(UI::PrimitiveType)

#endif //OURPAINT_DESKTOP_UI_TOOLSTYPE_H
