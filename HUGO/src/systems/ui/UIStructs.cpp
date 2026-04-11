#include "UIStructs.h"
void UIElementCache::setVisible(bool visible)
{
    if (visible)
    {
        overlay->show();
        container->show();
        if (element != nullptr)
            element->show();
        if (extra != nullptr)
            extra->show();
    }
    else
    {
        overlay->hide();
        container->hide();
        if (element != nullptr)
            element->hide();
        if (extra != nullptr)
            extra->hide();
    }
}