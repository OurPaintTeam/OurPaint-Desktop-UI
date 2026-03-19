#ifndef OURPAINT_DESKTOP_UI_TOOLSTYPE_H
#define OURPAINT_DESKTOP_UI_TOOLSTYPE_H

#include <QMetaType>

namespace UI {
    enum class PrimitiveType {
        Line,
        Polyline,
        InfiniteLine,
        LineSettings,

        CircleByDiameter,
        CircleTwoPoints,
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
} // namespace UI

Q_DECLARE_METATYPE(UI::PrimitiveType)

#endif //OURPAINT_DESKTOP_UI_TOOLSTYPE_H
