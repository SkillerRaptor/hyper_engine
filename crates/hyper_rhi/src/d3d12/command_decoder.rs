//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::mem::{self, ManuallyDrop};

use windows::Win32::{
    Foundation::RECT,
    Graphics::{
        Direct3D::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        Direct3D12::{
            ID3D12CommandAllocator,
            ID3D12GraphicsCommandList,
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
    d3d12::{
        graphics_device::GraphicsDevice,
        graphics_pipeline::GraphicsPipeline,
        texture::Texture,
    },
    texture::Texture as _,
};

pub(crate) struct CommandDecoder<'a> {
    command_list: &'a ID3D12GraphicsCommandList,
    command_allocator: &'a ID3D12CommandAllocator,
    graphics_device: &'a GraphicsDevice,
}

impl<'a> CommandDecoder<'a> {
    pub(super) fn new(
        graphics_device: &'a GraphicsDevice,
        command_allocator: &'a ID3D12CommandAllocator,
        command_list: &'a ID3D12GraphicsCommandList,
    ) -> Self {
        Self {
            command_list,
            command_allocator,
            graphics_device,
        }
    }
}

impl<'a> crate::commands::command_decoder::CommandDecoder for CommandDecoder<'a> {
    fn begin_render_pass(&self, texture: &dyn crate::texture::Texture) {
        let texture = texture.downcast_ref::<Texture>().unwrap();

        unsafe {
            self.command_list
                .Reset(self.command_allocator, None)
                .unwrap();

            self.command_list.ResourceBarrier(&[D3D12_RESOURCE_BARRIER {
                Type: D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                Flags: D3D12_RESOURCE_BARRIER_FLAG_NONE,
                Anonymous: D3D12_RESOURCE_BARRIER_0 {
                    Transition: ManuallyDrop::new(D3D12_RESOURCE_TRANSITION_BARRIER {
                        pResource: mem::transmute_copy(texture.resource()),
                        StateBefore: D3D12_RESOURCE_STATE_PRESENT,
                        StateAfter: D3D12_RESOURCE_STATE_RENDER_TARGET,
                        Subresource: D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                    }),
                },
            }]);
        }
    }

    fn end_render_pass(&self, texture: &dyn crate::texture::Texture) {
        let texture = texture.downcast_ref::<Texture>().unwrap();

        unsafe {
            self.command_list.ResourceBarrier(&[D3D12_RESOURCE_BARRIER {
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

    fn bind_bindings(&self, _buffer: &dyn crate::buffer::Buffer) {
        todo!();
    }

    fn bind_pipeline(
        &self,
        graphics_pipeline: &dyn crate::graphics_pipeline::GraphicsPipeline,
        texture: &dyn crate::texture::Texture,
    ) {
        let graphics_pipeline = graphics_pipeline
            .downcast_ref::<GraphicsPipeline>()
            .unwrap();
        let texture = texture.downcast_ref::<Texture>().unwrap();

        let width = texture.width();
        let height = texture.height();

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

        unsafe {
            self.command_list
                .SetGraphicsRootSignature(self.graphics_device.root_signature());
            // TODO: Is cloning really necessary?
            self.command_list.SetDescriptorHeaps(&[Some(
                self.graphics_device
                    .cbv_srv_uav_heap()
                    .descriptor_heap()
                    .clone(),
            )]);
            self.command_list
                .SetPipelineState(graphics_pipeline.pipeline_state());
            self.command_list.RSSetViewports(&[viewport]);
            self.command_list.RSSetScissorRects(&[scissor_rect]);

            self.command_list
                .IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            let descriptor_heap = self.graphics_device.rtv_heap();
            let rtv_handle = D3D12_CPU_DESCRIPTOR_HANDLE {
                ptr: descriptor_heap.handle().ptr
                    + texture.index() as usize * descriptor_heap.size(),
            };

            self.command_list
                .OMSetRenderTargets(1, Some(&rtv_handle), false, None);

            self.command_list.ClearRenderTargetView(
                rtv_handle,
                &[0.0_f32, 0.2_f32, 0.4_f32, 1.0_f32],
                None,
            );
        }
    }

    fn bind_index_buffer(&self, _buffer: &dyn crate::buffer::Buffer) {
        todo!();
    }

    fn draw(&self, vertex_count: u32, instance_count: u32, first_vertex: u32, first_instance: u32) {
        unsafe {
            self.command_list.DrawInstanced(
                vertex_count,
                instance_count,
                first_vertex,
                first_instance,
            );
        }
    }

    fn draw_indexed(
        &self,
        index_count: u32,
        instance_count: u32,
        first_index: u32,
        vertex_offset: i32,
        first_instance: u32,
    ) {
        unsafe {
            self.command_list.DrawIndexedInstanced(
                index_count,
                instance_count,
                first_index,
                vertex_offset,
                first_instance,
            );
        }
    }
}
