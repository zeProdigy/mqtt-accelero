#include "system/dbgio.h"
#include "system/lis302dl.h"


int accelero_test(int argc, char *argv[])
{
    int ret;

    ret = lis302dl_init();
    if (ret) {
        CONSOLE_ERROR("Can't init lis302dl");
        return 1;
    }

    CONSOLE_LOG("lis302dl inited");

    return 0;
}
