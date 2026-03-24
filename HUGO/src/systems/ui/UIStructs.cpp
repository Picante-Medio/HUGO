#include "UIStructs.h"
void UIElementCache::setVisible(bool visible)
{
    if (visible)
    {
        overlay->show();
        container->show();
        if (element != nullptr)
            element->show();
    }
    else
    {
        overlay->hide();
        container->hide();
        if (element != nullptr)
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
