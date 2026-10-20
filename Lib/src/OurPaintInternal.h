#ifndef OURPAINT_OURPAINTINTERNAL_H
#define OURPAINT_OURPAINTINTERNAL_H

#include <OurPaint.h>
#include "Events.h"
namespace OurPaint::Internal {
const ViewportRenderer& renderer();
void setTitle(const char* title);
void notifyToolButtonClicked(Tool tool);
}

#endif // OURPAINT_OURPAINTINTERNAL_H
