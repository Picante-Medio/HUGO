#include "UIStructs.h"
void UIElementCache::setVisible(bool visible)
{
    if (visible)
    {
        overlay->show();
        container->show();
        element->show();
    }
    else
    {
        overlay->hide();
        container->hide();
        element->hide();
    }
}

void UISliderCache::setVisible(bool visible)
{
    UIElementCache::setVisible(visible);
    if (visible)
        knob->show();
    else
        knob->hide();
}
