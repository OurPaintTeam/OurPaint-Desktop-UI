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

        CircleByDiameter,
        CircleByRadius,
        EllipseThreePoints,
        CircleSettings,

        ArcByRadius,
        ArcByDiameter,
        ArcThreePoints,
        ArcSettings
    };


    enum class ConstraintType {
        PointLineDistance,
        PointOnLine,
        PointPointDistance,
        CoincidentPoints,
        LineCircleDistance,
        LineOnCircle,
        LineInCircle,
        ParallelLines,
        PerpendicularLines,
        AngleBetweenLines
    };


    enum class ToolsType {
        Cursor,
        Size
    };
} // namespace UI

Q_DECLARE_METATYPE(UI::PrimitiveType)

#endif //OURPAINT_DESKTOP_UI_TOOLSTYPE_H
