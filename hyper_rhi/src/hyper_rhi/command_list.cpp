/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/command_list.hpp"

#include <hyper_core/assertion.hpp>

#include "hyper_rhi/compute_pass.hpp"
#include "hyper_rhi/render_pass.hpp"

namespace hyper_engine
{
    std::shared_ptr<ComputePass> CommandList::begin_compute_pass(const ComputePassDescriptor &descriptor) const
    {
        return begin_compute_pass_platform(descriptor);
    }

    std::shared_ptr<RenderPass> CommandList::begin_render_pass(const RenderPassDescriptor &descriptor) const
    {
        HE_ASSERT(!descriptor.color_attachments.empty());

        for (const ColorAttachment &color_attachment : descriptor.color_attachments)
        {
            HE_ASSERT(color_attachment.view);
        }

        return begin_render_pass_platform(descriptor);
    }
} // namespace hyper_engine