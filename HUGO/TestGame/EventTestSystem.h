#pragma once

#include "HUGO.h"

struct TestComponent;

HSYSTEM(EventTestSystem)

    double timer = 0.;
public:
    void init() override;
    void onLoad() override;

    void fixedUpdate(uint32_t dt) override;
    void update(uint32_t dt) override;

    void testEventA(TestComponent& cmp);
    void testEventB(TestComponent& cmp);
    void testEventC(TestComponent& cmp);

END_HSYSTEM()
