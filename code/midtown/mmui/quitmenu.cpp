define_dummy_symbol(mmui_quitmenu);

#include "quitmenu.h"

#include "mmwidget/manager.h"

QuitMenu::QuitMenu(i32 idc)
    : PUMenuBase(idc, 0.3f, 0.3f, 0.625f, 0.15625f, "quit_dlg")
{
    AddBMButton(IDC_QUIT_YES, "dlg_yes", 0.10f, 0.15f, 4);
    AddBMButton(IDC_QUIT_NO, "dlg_no", 0.55f, 0.15f, 4);
}
