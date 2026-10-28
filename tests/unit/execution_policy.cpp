#include "execution_policy.h"

static_assert(idaplugin::useDecompilerWorker(true, true, false, false), "Interactive GUI uses a worker");
static_assert(!idaplugin::useDecompilerWorker(true, false, false, false), "idat must not wait for GUI callbacks");
static_assert(!idaplugin::useDecompilerWorker(true, true, true, false), "Autonomous GUI scripts are synchronous");
static_assert(!idaplugin::useDecompilerWorker(true, true, false, true), "Explicit synchronous override wins");
static_assert(!idaplugin::useDecompilerWorker(false, true, false, false), "Preserve caller's synchronous setting");

int main()
{
    return 0;
}
