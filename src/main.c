
#include <locale.h>
#include "util/assert.h"
#include "util/data.h"
#include "ui/menu.h"
#include "ui/mainMenu.h"
#include "ui/findMenu.h"

int main() {
    // utf8 problem on Windows
    // see `https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-140#utf-8-support`
    setlocale(LC_ALL, ".UTF8");

    mainMenuInit();
    findMenuInit();
    assertWithWarn(dataLoad("data.csv"), "数据加载失败.");

    menuShow(&mainMenu);

    mainMenuFree();
    findMenuFree();
    dataFree();

    return 0;
}