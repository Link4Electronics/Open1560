#pragma once

struct EAX_REVERBPROPERTIES {
    unsigned long environment;
    float fVolume;
    float fDecayTime_sec;
    float fDamping;
};

constexpr unsigned long EAX_ENVIRONMENT_CITY = 3;
