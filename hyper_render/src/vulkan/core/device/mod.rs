/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

pub(crate) mod logical_device;
pub(crate) mod physical_device;

use crate::vulkan::{
    command::{command_buffer::CommandBuffer, command_pool::CommandPool},
    core::{
        device::{
            logical_device::{LogicalDevice, LogicalDeviceCreateInfo},
            physical_device::{
                PhysicalDevice, PhysicalDeviceCreateInfo, QueueFamilies, SurfaceDetails,
            },
        },
        instance::debug_utils::DebugName,
        instance::Instance,
        surface::Surface,
        swapchain::{self, Swapchain},
    },
    memory::allocator::{self, Allocator},
    pipeline::{pipeline_layout::PipelineLayout, Pipeline},
    resource::buffer::Buffer,
    sync::{binary_semaphore::BinarySemaphore, timeline_semaphore::TimelineSemaphore},
};

use hyper_platform::window::Window;

use ash::{extensions::khr, vk};
use color_eyre::Result;
use std::{cell::RefCell, ffi::CStr, rc::Rc};

pub(crate) struct Device {
    present_queue: vk::Queue,
    graphics_queue: vk::Queue,

    logical_device: LogicalDevice,
    physical_device: PhysicalDevice,

    instance: Rc<Instance>,
}

impl Device {
    pub(crate) const EXTENSIONS: [&'static CStr; 1] = [khr::Swapchain::name()];

    pub(crate) fn new(create_info: DeviceCreateInfo) -> Result<Rc<Self>> {
        let DeviceCreateInfo { instance, surface } = create_info;

        let physical_device = PhysicalDevice::new(PhysicalDeviceCreateInfo {
            instance: &instance,
            surface,
        })?;

        let logical_device = LogicalDevice::new(LogicalDeviceCreateInfo {
            instance: instance.clone(),
            physical_device: &physical_device,
        })?;

        let queue_families = physical_device.queue_families();
        let graphics_queue = logical_device.get_device_queue(queue_families.graphics_family());
        let present_queue = logical_device.get_device_queue(queue_families.present_family());

        Ok(Rc::new(Self {
            present_queue,
            graphics_queue,

            logical_device,
            physical_device,

            instance,
        }))
    }

    pub(crate) fn create_allocator(
        &self,
        create_info: AllocatorCreateInfo,
    ) -> Result<Rc<RefCell<Allocator>>> {
        let AllocatorCreateInfo { debug } = create_info;

        let allocator = Allocator::new(allocator::AllocatorCreateInfo {
            instance: &self.instance,
            device: self,
            log_leaks_on_shutdown: debug,
        })?;

        Ok(Rc::new(RefCell::new(allocator)))
    }

    pub(crate) fn create_swapchain(
        self: &Rc<Self>,
        create_info: SwapchainCreateInfo,
    ) -> Result<Swapchain> {
        let SwapchainCreateInfo {
            window,
            surface,
            allocator,
        } = create_info;

        let swaphain = Swapchain::new(swapchain::SwapchainCreateInfo {
            window,
            instance: &self.instance,
            surface,
            device: self.clone(),
            allocator,
        })?;

        Ok(swaphain)
    }

    pub(crate) fn allocate_vk_command_buffers(
        &self,
        allocate_info: vk::CommandBufferAllocateInfo,
    ) -> Result<Vec<vk::CommandBuffer>> {
        let command_buffers = self
            .logical_device
            .allocate_command_buffers(allocate_info)?;
        Ok(command_buffers)
    }

    pub(crate) fn allocate_vk_descriptor_sets(
        &self,
        allocate_info: vk::DescriptorSetAllocateInfo,
    ) -> Result<Vec<vk::DescriptorSet>> {
        let descriptor_sets = self
            .logical_device
            .allocate_descriptor_sets(allocate_info)?;
        Ok(descriptor_sets)
    }

    pub(crate) fn begin_command_buffer(
        &self,
        command_buffer: &CommandBuffer,
        begin_info: vk::CommandBufferBeginInfo,
    ) -> Result<()> {
        self.logical_device
            .begin_command_buffer(command_buffer.raw(), begin_info)?;
        Ok(())
    }

    pub(crate) fn bind_buffer_memory(&self, buffer: &Buffer) -> Result<()> {
        self.logical_device.bind_buffer_memory(
            buffer.raw(),
            buffer.allocation().memory(),
            buffer.allocation().offset(),
        )?;
        Ok(())
    }

    pub(crate) fn bind_image_memory(
        &self,
        image: vk::Image,
        memory: vk::DeviceMemory,
        offset: u64,
    ) -> Result<()> {
        self.logical_device
            .bind_image_memory(image, memory, offset)?;
        Ok(())
    }

    pub(crate) fn cmd_begin_rendering(
        &self,
        command_buffer: &CommandBuffer,
        rendering_info: vk::RenderingInfo,
    ) {
        self.logical_device
            .cmd_begin_rendering(command_buffer.raw(), rendering_info);
    }

    pub(crate) fn cmd_end_rendering(&self, command_buffer: &CommandBuffer) {
        self.logical_device.cmd_end_rendering(command_buffer.raw());
    }

    pub(crate) fn cmd_bind_pipeline(
        &self,
        command_buffer: &CommandBuffer,
        pipeline_bind_point: vk::PipelineBindPoint,
        pipeline: &dyn Pipeline,
    ) {
        self.logical_device.cmd_bind_pipeline(
            command_buffer.raw(),
            pipeline_bind_point,
            pipeline.raw(),
        );
    }

    pub(crate) fn cmd_pipeline_barrier2(
        &self,
        command_buffer: &CommandBuffer,
        dependency_info: vk::DependencyInfo,
    ) {
        self.logical_device
            .cmd_pipeline_barrier2(command_buffer.raw(), dependency_info);
    }

    pub(crate) fn cmd_set_viewport(
        &self,
        command_buffer: &CommandBuffer,
        first_viewport: u32,
        viewports: &[vk::Viewport],
    ) {
        self.logical_device
            .cmd_set_viewport(command_buffer.raw(), first_viewport, viewports);
    }

    pub(crate) fn cmd_set_scissor(
        &self,
        command_buffer: &CommandBuffer,
        first_scissor: u32,
        scissors: &[vk::Rect2D],
    ) {
        self.logical_device
            .cmd_set_scissor(command_buffer.raw(), first_scissor, scissors);
    }

    pub(crate) fn cmd_bind_descriptor_sets(
        &self,
        command_buffer: &CommandBuffer,
        pipeline_bind_point: vk::PipelineBindPoint,
        layout: &PipelineLayout,
        first_set: u32,
        descriptor_sets: &[vk::DescriptorSet],
        dynamic_offsets: &[u32],
    ) {
        self.logical_device.cmd_bind_descriptor_sets(
            command_buffer.raw(),
            pipeline_bind_point,
            layout.raw(),
            first_set,
            descriptor_sets,
            dynamic_offsets,
        );
    }

    pub(crate) fn cmd_push_constants(
        &self,
        command_buffer: &CommandBuffer,
        layout: vk::PipelineLayout,
        stage_flags: vk::ShaderStageFlags,
        offset: u32,
        constants: &[u8],
    ) {
        self.logical_device.cmd_push_constants(
            command_buffer.raw(),
            layout,
            stage_flags,
            offset,
            constants,
        );
    }

    pub(crate) fn cmd_draw(
        &self,
        command_buffer: &CommandBuffer,
        vertex_count: u32,
        instance_count: u32,
        first_vertex: u32,
        first_instance: u32,
    ) {
        self.logical_device.cmd_draw(
            command_buffer.raw(),
            vertex_count,
            instance_count,
            first_vertex,
            first_instance,
        );
    }

    pub(crate) fn cmd_draw_indexed(
        &self,
        command_buffer: &CommandBuffer,
        index_count: u32,
        instance_count: u32,
        first_index: u32,
        vertex_offset: i32,
        first_instance: u32,
    ) {
        self.logical_device.cmd_draw_indexed(
            command_buffer.raw(),
            index_count,
            instance_count,
            first_index,
            vertex_offset,
            first_instance,
        );
    }

    pub(crate) fn cmd_bind_index_buffer(
        &self,
        command_buffer: &CommandBuffer,
        buffer: &Buffer,
        offset: vk::DeviceSize,
        index_type: vk::IndexType,
    ) {
        self.logical_device.cmd_bind_index_buffer(
            command_buffer.raw(),
            buffer.raw(),
            offset,
            index_type,
        );
    }

    pub(crate) fn cmd_copy_buffer(
        &self,
        command_buffer: &CommandBuffer,
        source_buffer: &Buffer,
        destination_buffer: &Buffer,
        regions: &[vk::BufferCopy],
    ) {
        self.logical_device.cmd_copy_buffer(
            command_buffer.raw(),
            source_buffer.raw(),
            destination_buffer.raw(),
            regions,
        );
    }

    pub(crate) fn cmd_copy_buffer_to_image(
        &self,
        command_buffer: &CommandBuffer,
        source_buffer: &Buffer,
        destination_image: vk::Image,
        destination_image_layout: vk::ImageLayout,
        regions: &[vk::BufferImageCopy],
    ) {
        self.logical_device.cmd_copy_buffer_to_image(
            command_buffer.raw(),
            source_buffer.raw(),
            destination_image,
            destination_image_layout,
            regions,
        );
    }

    pub(crate) fn create_vk_buffer(&self, create_info: vk::BufferCreateInfo) -> Result<vk::Buffer> {
        let buffer = self.logical_device.create_buffer(create_info)?;
        Ok(buffer)
    }

    pub(crate) fn create_vk_command_pool(
        &self,
        create_info: vk::CommandPoolCreateInfo,
    ) -> Result<vk::CommandPool> {
        let command_pool = self.logical_device.create_command_pool(create_info)?;
        Ok(command_pool)
    }

    pub(crate) fn create_vk_compute_pipelines(
        &self,
        create_infos: &[vk::ComputePipelineCreateInfo],
    ) -> Result<Vec<vk::Pipeline>> {
        let compute_pipelines = self.logical_device.create_compute_pipelines(create_infos)?;
        Ok(compute_pipelines)
    }

    pub(crate) fn create_vk_descriptor_pool(
        &self,
        create_info: vk::DescriptorPoolCreateInfo,
    ) -> Result<vk::DescriptorPool> {
        let descriptor_pool = self.logical_device.create_descriptor_pool(create_info)?;
        Ok(descriptor_pool)
    }

    pub(crate) fn create_vk_descriptor_set_layout(
        &self,
        create_info: vk::DescriptorSetLayoutCreateInfo,
    ) -> Result<vk::DescriptorSetLayout> {
        let descriptor_set_layout = self
            .logical_device
            .create_descriptor_set_layout(create_info)?;
        Ok(descriptor_set_layout)
    }

    pub(crate) fn create_vk_graphics_pipelines(
        &self,
        create_infos: &[vk::GraphicsPipelineCreateInfo],
    ) -> Result<Vec<vk::Pipeline>> {
        let graphics_pipelines = self
            .logical_device
            .create_graphics_pipelines(create_infos)?;
        Ok(graphics_pipelines)
    }

    pub(crate) fn create_vk_image(&self, create_info: vk::ImageCreateInfo) -> Result<vk::Image> {
        let image = self.logical_device.create_image(create_info)?;
        Ok(image)
    }

    pub(crate) fn create_vk_image_view(
        &self,
        create_info: vk::ImageViewCreateInfo,
    ) -> Result<vk::ImageView> {
        let image_view = self.logical_device.create_image_view(create_info)?;
        Ok(image_view)
    }

    pub(crate) fn create_vk_pipeline_layout(
        &self,
        create_info: vk::PipelineLayoutCreateInfo,
    ) -> Result<vk::PipelineLayout> {
        let pipeline_layout = self.logical_device.create_pipeline_layout(create_info)?;
        Ok(pipeline_layout)
    }

    pub(crate) fn create_vk_sampler(
        &self,
        create_info: vk::SamplerCreateInfo,
    ) -> Result<vk::Sampler> {
        let sampler = self.logical_device.create_sampler(create_info)?;
        Ok(sampler)
    }

    pub(crate) fn create_vk_shader_module(
        &self,
        create_info: vk::ShaderModuleCreateInfo,
    ) -> Result<vk::ShaderModule> {
        let shader_module = self.logical_device.create_shader_module(create_info)?;
        Ok(shader_module)
    }

    pub(crate) fn create_vk_semaphore(
        &self,
        create_info: vk::SemaphoreCreateInfo,
    ) -> Result<vk::Semaphore> {
        let semaphore = self.logical_device.create_semaphore(create_info)?;
        Ok(semaphore)
    }

    pub(crate) fn destroy_buffer(&self, buffer: vk::Buffer) {
        self.logical_device.destroy_buffer(buffer);
    }

    pub(crate) fn destroy_command_pool(&self, command_pool: vk::CommandPool) {
        self.logical_device.destroy_command_pool(command_pool);
    }

    pub(crate) fn destroy_descriptor_pool(&self, descriptor_pool: vk::DescriptorPool) {
        self.logical_device.destroy_descriptor_pool(descriptor_pool);
    }

    pub(crate) fn destroy_descriptor_set_layout(
        &self,
        descriptor_set_layout: vk::DescriptorSetLayout,
    ) {
        self.logical_device
            .destroy_descriptor_set_layout(descriptor_set_layout);
    }

    pub(crate) fn destroy_image(&self, image: vk::Image) {
        self.logical_device.destroy_image(image);
    }

    pub(crate) fn destroy_image_view(&self, image_view: vk::ImageView) {
        self.logical_device.destroy_image_view(image_view);
    }

    pub(crate) fn destroy_pipeline(&self, pipeline: vk::Pipeline) {
        self.logical_device.destroy_pipeline(pipeline);
    }

    pub(crate) fn destroy_pipeline_layout(&self, pipeline_layout: vk::PipelineLayout) {
        self.logical_device.destroy_pipeline_layout(pipeline_layout);
    }

    pub(crate) fn destroy_shader_module(&self, shader_module: vk::ShaderModule) {
        self.logical_device.destroy_shader_module(shader_module);
    }

    pub(crate) fn destroy_semaphore(&self, semaphore: vk::Semaphore) {
        self.logical_device.destroy_semaphore(semaphore);
    }

    pub(crate) fn end_command_buffer(&self, command_buffer: &CommandBuffer) -> Result<()> {
        self.logical_device
            .end_command_buffer(command_buffer.raw())?;
        Ok(())
    }

    pub(crate) fn get_buffer_memory_requirements(&self, buffer: &Buffer) -> vk::MemoryRequirements {
        self.logical_device
            .get_buffer_memory_requirements(buffer.raw())
    }

    pub(crate) fn get_image_memory_requirements(&self, image: vk::Image) -> vk::MemoryRequirements {
        self.logical_device.get_image_memory_requirements(image)
    }

    pub(crate) fn reset_command_buffer(
        &self,
        command_buffer: &CommandBuffer,
        flags: vk::CommandBufferResetFlags,
    ) -> Result<()> {
        self.logical_device
            .reset_command_buffer(command_buffer.raw(), flags)?;

        Ok(())
    }

    pub(crate) fn reset_command_pool(
        &self,
        command_pool: &CommandPool,
        flags: vk::CommandPoolResetFlags,
    ) -> Result<()> {
        self.logical_device
            .reset_command_pool(command_pool.raw(), flags)?;
        Ok(())
    }

    pub(crate) fn set_object_name<T>(&self, debug_name: DebugName<T>) -> Result<()>
    where
        T: vk::Handle,
    {
        self.logical_device.set_object_name(debug_name)?;
        Ok(())
    }

    pub(crate) fn submit_render_queue(
        &self,
        command_buffer: &CommandBuffer,
        present_semaphore: &BinarySemaphore,
        render_semaphore: &BinarySemaphore,
        submit_semaphore: &TimelineSemaphore,
        frame_id: u64,
    ) -> Result<()> {
        self.logical_device.submit_render_queue(
            self.graphics_queue,
            command_buffer.raw(),
            present_semaphore.raw(),
            render_semaphore.raw(),
            submit_semaphore.raw(),
            frame_id,
        )?;
        Ok(())
    }

    pub(crate) fn submit_upload_queue(
        &self,
        command_buffer: &CommandBuffer,
        upload_semaphore: &TimelineSemaphore,
        last_upload_value: u64,
    ) -> Result<()> {
        self.logical_device.submit_upload_queue(
            self.graphics_queue,
            command_buffer.raw(),
            upload_semaphore.raw(),
            last_upload_value,
        )?;
        Ok(())
    }

    pub(crate) fn update_descriptor_sets(
        &self,
        writes: &[vk::WriteDescriptorSet],
        copies: &[vk::CopyDescriptorSet],
    ) {
        self.logical_device.update_descriptor_sets(writes, copies);
    }

    pub(crate) fn wait_idle(&self) -> Result<()> {
        self.logical_device.wait_idle()?;
        Ok(())
    }

    pub(crate) fn wait_semaphores(&self, wait_info: vk::SemaphoreWaitInfo) -> Result<()> {
        self.logical_device.wait_semaphores(wait_info)?;
        Ok(())
    }

    pub(crate) fn queue_families(&self) -> QueueFamilies {
        self.physical_device.queue_families()
    }

    pub(crate) fn surface_details(&self, surface: &Surface) -> SurfaceDetails {
        self.physical_device.surface_details(surface)
    }

    pub(crate) fn physical_device(&self) -> &PhysicalDevice {
        &self.physical_device
    }

    pub(crate) fn logical_device(&self) -> &LogicalDevice {
        &self.logical_device
    }

    pub(crate) fn graphics_queue(&self) -> vk::Queue {
        self.graphics_queue
    }

    pub(crate) fn present_queue(&self) -> vk::Queue {
        self.present_queue
    }
}

pub(crate) struct DeviceCreateInfo<'a> {
    pub(crate) instance: Rc<Instance>,
    pub(crate) surface: &'a Surface,
}

pub(crate) struct AllocatorCreateInfo {
    pub(crate) debug: bool,
}

pub(crate) struct SwapchainCreateInfo<'a> {
    pub(crate) window: &'a Window,
    pub(crate) surface: &'a Surface,
    pub(crate) allocator: Rc<RefCell<Allocator>>,
}
