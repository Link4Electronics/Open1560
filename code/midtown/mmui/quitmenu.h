#pragma once

#include "pu_menu.h"

enum
{
    IDC_QUIT_YES = 200,
    IDC_QUIT_NO = 201,
};

class QuitMenu final : public PUMenuBase
{
public:
    QuitMenu(i32 idc);
    ~QuitMenu() override = default;
};
