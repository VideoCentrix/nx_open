// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <core/resource/resource_fwd.h>
#include <nx/vms/rules/basic_action.h>

#include "../rules_fwd.h"

namespace nx::vms::rules {

inline bool isProlonged(const ActionPtr& action)
{
    const auto actionState = action->state();
    return actionState == State::started || actionState == State::stopped;
}

/** Returns whether the action is prolonged and has no fixed duration. */
NX_VMS_RULES_API bool isProlonged(const Engine* engine, const ActionBuilder* builder);

NX_VMS_RULES_API bool hasTargetCamera(const vms::rules::ItemDescriptor& actionDescriptor);

NX_VMS_RULES_API bool hasTargetLayout(const vms::rules::ItemDescriptor& actionDescriptor);

NX_VMS_RULES_API bool hasTargetUser(const vms::rules::ItemDescriptor& actionDescriptor);

NX_VMS_RULES_API bool hasTargetServer(const vms::rules::ItemDescriptor& actionDescriptor);

NX_VMS_RULES_API bool needAcknowledge(const ActionPtr& action);

/** Returns available states that can be used with the given action builder. */
NX_VMS_RULES_API QSet<State> getAvailableStates(
    const Engine* engine, const ActionBuilder* actionBuilder);

} // namespace nx::vms::rules
