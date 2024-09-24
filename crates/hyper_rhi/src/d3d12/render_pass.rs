//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{
    mem::{self, ManuallyDrop},
    sync::Arc,
};

use windows::Win32::{
    Foundation::RECT,
    Graphics::{
        Direct3D::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        Direct3D12::{
            D3D12_CPU_DESCRIPTOR_HANDLE,
            D3D12_INDEX_BUFFER_VIEW,
            D3D12_MAX_DEPTH,
            D3D12_MIN_DEPTH,
            D3D12_RESOURCE_BARRIER,
            D3D12_RESOURCE_BARRIER_0,
            D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
            D3D12_RESOURCE_BARRIER_FLAG_NONE,
            D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_TRANSITION_BARRIER,
            D3D12_VIEWPORT,
        },
        Dxgi::Common::DXGI_FORMAT_R32_UINT,
    },
};

use crate::{
    buffer::Buffer as _,
    d3d12::{
        buffer::Buffer,
        graphics_device::GraphicsDeviceShared,
        graphics_pipeline::GraphicsPipeline,
        pipeline_layout::PipelineLayout,
        texture::Texture,
    },
    render_pass::{DrawIndexedArguments, RenderPassDescriptor},
    texture::Texture as _,
};

pub(crate) struct RenderPass {
    depth_stencil_attachment: Option<Arc<dyn crate::texture::Texture>>,
    color_attachment: Arc<dyn crate::texture::Texture>,

    graphics_device: Arc<GraphicsDeviceShared>,
}

impl RenderPass {
    pub(crate) fn new(
        graphics_device: &Arc<GraphicsDeviceShared>,
        descriptor: &RenderPassDescriptor,
    ) -> Self {
        let color_attachment = descriptor
            .color_attachment
            .downcast_ref::<Texture>()
            .unwrap();

        let command_allocator = &graphics_device.current_frame().command_allocator;
        let command_list = &graphics_device.current_frame().command_list;

        unsafe {
            command_list.Reset(command_allocator, None).unwrap();

            command_list.ResourceBarrier(&[D3D12_RESOURCE_BARRIER {
                Type: D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                Flags: D3D12_RESOURCE_BARRIER_FLAG_NONE,
                Anonymous: D3D12_RESOURCE_BARRIER_0 {
                    Transition: ManuallyDrop::new(D3D12_RESOURCE_TRANSITION_BARRIER {
                        pResource: mem::transmute_copy(color_attachment.resource()),
                        StateBefore: D3D12_RESOURCE_STATE_PRESENT,
                        StateAfter: D3D12_RESOURCE_STATE_RENDER_TARGET,
                        Subresource: D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                    }),
                },
            }]);
        }

        let viewport = D3D12_VIEWPORT {
            TopLeftX: 0.0,
            TopLeftY: 0.0,
            Width: color_attachment.width() as f32,
            Height: color_attachment.height() as f32,
            MinDepth: D3D12_MIN_DEPTH,
            MaxDepth: D3D12_MAX_DEPTH,
        };

        let scissor_rect = RECT {
            left: 0,
            top: 0,
            right: color_attachment.width() as i32,
            bottom: color_attachment.height() as i32,
        };

        unsafe {
            command_list.RSSetViewports(&[viewport]);
            command_list.RSSetScissorRects(&[scissor_rect]);
        }

        // TODO: Find a better way to do this
        let depth_stencil_attachment = if descriptor.depth_stencil_attachment.is_some() {
            Some(Arc::clone(
                descriptor.depth_stencil_attachment.as_ref().unwrap(),
            ))
        } else {
            None
        };

        Self {
            depth_stencil_attachment: depth_stencil_attachment,
            color_attachment: Arc::clone(descriptor.color_attachment),

            graphics_device: Arc::clone(graphics_device),
        }
    }
}

impl crate::render_pass::RenderPass for RenderPass {
    fn set_pipeline(&self, pipeline: &Arc<dyn crate::graphics_pipeline::GraphicsPipeline>) {
        let pipeline = pipeline.downcast_ref::<GraphicsPipeline>().unwrap();
        let layout = pipeline.layout().downcast_ref::<PipelineLayout>().unwrap();

        let color_attachment = self.color_attachment.downcast_ref::<Texture>().unwrap();

        let command_list = &self.graphics_device.current_frame().command_list;

        unsafe {
            // TODO: Is cloning really necessary?
            command_list.SetDescriptorHeaps(&[Some(
                self.graphics_device
                    .cbv_srv_uav_heap()
                    .descriptor_heap()
                    .clone(),
            )]);
            command_list.SetGraphicsRootSignature(layout.root_signature());
            command_list.SetPipelineState(pipeline.pipeline_state());

            command_list.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            let descriptor_heap = self.graphics_device.rtv_heap();
            let rtv_handle = D3D12_CPU_DESCRIPTOR_HANDLE {
                ptr: descriptor_heap.handle().ptr
                    + color_attachment.index() as usize * descriptor_heap.size(),
            };

            command_list.OMSetRenderTargets(1, Some(&rtv_handle), false, None);

            command_list.ClearRenderTargetView(
                rtv_handle,
                &[0.0_f32, 0.2_f32, 0.4_f32, 1.0_f32],
                None,
            );
        }
    }

    fn set_index_buffer(&self, buffer: &Arc<dyn crate::buffer::Buffer>) {
        let buffer = buffer.downcast_ref::<Buffer>().unwrap();

        let index_buffer_view = D3D12_INDEX_BUFFER_VIEW {
            BufferLocation: buffer.gpu_address(),
            SizeInBytes: buffer.size() as u32,
            Format: DXGI_FORMAT_R32_UINT,
        };

        let command_list = &self.graphics_device.current_frame().command_list;

        unsafe {
            command_list.IASetIndexBuffer(Some(&index_buffer_view));
        }
    }

    fn set_push_constants(&self, data: &[u8]) {
        let command_list = &self.graphics_device.current_frame().command_list;

        // TODO: Ensure data is same size as push constant
        unsafe {
            command_list.SetGraphicsRoot32BitConstants(
                0,
                data.len() as u32 / 4,
                data.as_ptr() as *const _,
                0,
            );
        }
    }

    fn draw_indexed(&self, arguments: &DrawIndexedArguments) {
        let command_list = &self.graphics_device.current_frame().command_list;

        unsafe {
            command_list.DrawIndexedInstanced(
                arguments.index_count,
                arguments.instance_count,
                arguments.first_index,
                arguments.vertex_offset,
                arguments.first_instance,
            );
        }
    }
}

impl Drop for RenderPass {
    fn drop(&mut self) {
        let texture = self.color_attachment.downcast_ref::<Texture>().unwrap();

        let command_list = &self.graphics_device.current_frame().command_list;

        unsafe {
            command_list.ResourceBarrier(&[D3D12_RESOURCE_BARRIER {
                Type: D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                Flags: D3D12_RESOURCE_BARRIER_FLAG_NONE,
                Anonymous: D3D12_RESOURCE_BARRIER_0 {
                    Transition: ManuallyDrop::new(D3D12_RESOURCE_TRANSITION_BARRIER {
                        pResource: mem::transmute_copy(texture.resource()),
                        StateBefore: D3D12_RESOURCE_STATE_RENDER_TARGET,
                        StateAfter: D3D12_RESOURCE_STATE_PRESENT,
                        Subresource: D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                    }),
                },
            }]);
        }
    }
}
