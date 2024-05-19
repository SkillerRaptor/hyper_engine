/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::{
    mem::{self, ManuallyDrop},
    sync::atomic::Ordering,
};

use windows::Win32::{
    Foundation::RECT,
    Graphics::{
        Direct3D::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        Direct3D12::{
            D3D12_CPU_DESCRIPTOR_HANDLE,
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
    },
};

use crate::{
    d3d12::{GraphicsDevice, Texture},
    render_pass::RenderPassDescriptor,
    render_pipeline::RenderPipeline,
};

pub(crate) struct RenderPass {
    texture: Texture,

    graphics_device: GraphicsDevice,
}

impl RenderPass {
    pub(crate) fn new(graphics_device: &GraphicsDevice, descriptor: &RenderPassDescriptor) -> Self {
        let texture = descriptor.texture.d3d12_texture().unwrap();

        Self {
            texture: texture.clone(),

            graphics_device: graphics_device.clone(),
        }
    }

    pub(crate) fn bind_pipeline(&self, pipeline: &RenderPipeline) {
        let pipeline = pipeline.d3d12_render_pipeline().unwrap();

        let width = self.texture.width();
        let height = self.texture.height();

        // TODO: Get depth info from pipeline
        let viewport = D3D12_VIEWPORT {
            TopLeftX: 0.0,
            TopLeftY: 0.0,
            Width: width as f32,
            Height: height as f32,
            MinDepth: D3D12_MIN_DEPTH,
            MaxDepth: D3D12_MAX_DEPTH,
        };

        let scissor_rect = RECT {
            left: 0,
            top: 0,
            right: width as i32,
            bottom: height as i32,
        };

        let current_frame = self.graphics_device.current_frame();
        let command_allocator = &current_frame.command_allocator;
        let command_list = &current_frame.command_list;

        unsafe {
            command_list
                .Reset(command_allocator, pipeline.pipeline_state())
                .expect("failed to reset command list");

            command_list.SetGraphicsRootSignature(self.graphics_device.root_signature());
            command_list.RSSetViewports(&[viewport]);
            command_list.RSSetScissorRects(&[scissor_rect]);

            command_list.ResourceBarrier(&[D3D12_RESOURCE_BARRIER {
                Type: D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                Flags: D3D12_RESOURCE_BARRIER_FLAG_NONE,
                Anonymous: D3D12_RESOURCE_BARRIER_0 {
                    Transition: ManuallyDrop::new(D3D12_RESOURCE_TRANSITION_BARRIER {
                        pResource: mem::transmute_copy(self.texture.resource()),
                        StateBefore: D3D12_RESOURCE_STATE_PRESENT,
                        StateAfter: D3D12_RESOURCE_STATE_RENDER_TARGET,
                        Subresource: D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                    }),
                },
            }]);

            let frame_index = self
                .graphics_device
                .current_texture_index()
                .load(Ordering::Relaxed);

            let descriptor_heap = self.graphics_device.rtv_heap();
            let rtv_handle = D3D12_CPU_DESCRIPTOR_HANDLE {
                ptr: descriptor_heap.handle().ptr + frame_index as usize * descriptor_heap.size(),
            };

            command_list.OMSetRenderTargets(1, Some(&rtv_handle), false, None);

            command_list.ClearRenderTargetView(
                rtv_handle,
                &[0.0_f32, 0.2_f32, 0.4_f32, 1.0_f32],
                None,
            );
        }
    }

    pub(crate) fn draw(
        &self,
        vertex_count: u32,
        instance_count: u32,
        first_vertex: u32,
        first_instance: u32,
    ) {
        let current_frame = self.graphics_device.current_frame();
        let command_list = &current_frame.command_list;

        unsafe {
            command_list.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            // command_list.IASetVertexBuffers(0, Some(&[vertex_buffer_view]));
            command_list.DrawInstanced(vertex_count, instance_count, first_vertex, first_instance);
        }
    }
}

impl Drop for RenderPass {
    fn drop(&mut self) {
        let current_frame = self.graphics_device.current_frame();
        let command_list = &current_frame.command_list;

        unsafe {
            command_list.ResourceBarrier(&[D3D12_RESOURCE_BARRIER {
                Type: D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                Flags: D3D12_RESOURCE_BARRIER_FLAG_NONE,
                Anonymous: D3D12_RESOURCE_BARRIER_0 {
                    Transition: ManuallyDrop::new(D3D12_RESOURCE_TRANSITION_BARRIER {
                        pResource: mem::transmute_copy(self.texture.resource()),
                        StateBefore: D3D12_RESOURCE_STATE_RENDER_TARGET,
                        StateAfter: D3D12_RESOURCE_STATE_PRESENT,
                        Subresource: D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                    }),
                },
            }]);
        }
    }
}
