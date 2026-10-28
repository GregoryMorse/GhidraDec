#ifndef GHIDRADEC_EXECUTION_POLICY_H
#define GHIDRADEC_EXECUTION_POLICY_H

namespace idaplugin {

// Without a running GUI dispatcher, worker callbacks can deadlock on IDA.
constexpr bool useDecompilerWorker(bool requested, bool graphical, bool autonomous, bool forceSynchronous)
{
    return requested && graphical && !autonomous && !forceSynchronous;
}

} // namespace idaplugin

#endif
