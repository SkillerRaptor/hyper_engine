/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::core::{
    device::{physical_device::PhysicalDevice, Device},
    instance::{debug_utils::DebugName, Instance},
};

use ash::vk;
use color_eyre::Result;
use std::{collections::HashSet, rc::Rc};

pub(crate) struct LogicalDevice {
    raw: ash::Device,

    instance: Rc<Instance>,
}

impl LogicalDevice {
    pub(crate) fn new(create_info: LogicalDeviceCreateInfo) -> Result<Self> {
        let LogicalDeviceCreateInfo {
            instance,
            physical_device,
        } = create_info;

        let queue_families = physical_device.queue_families();

        let mut unique_queue_families = HashSet::new();
        unique_queue_families.insert(queue_families.graphics_family());
        unique_queue_families.insert(queue_families.present_family());

        let mut queue_create_infos = Vec::new();
        for queue_family in unique_queue_families {
            let queue_create_info = vk::DeviceQueueCreateInfo::builder()
                .queue_family_index(queue_family)
                .queue_priorities(&[1.0])
                .build();

            queue_create_infos.push(queue_create_info);
        }

        let mut dynamic_rendering =
            vk::PhysicalDeviceDynamicRenderingFeatures::builder().dynamic_rendering(true);
        let mut timline_semaphore =
            vk::PhysicalDeviceTimelineSemaphoreFeatures::builder().timeline_semaphore(true);
        let mut synchronization2 =
            vk::PhysicalDeviceSynchronization2Features::builder().synchronization2(true);
        let mut descriptor_indexing = vk::PhysicalDeviceDescriptorIndexingFeatures::builder()
            .shader_uniform_buffer_array_non_uniform_indexing(true)
            .shader_sampled_image_array_non_uniform_indexing(true)
            .shader_storage_buffer_array_non_uniform_indexing(true)
            .shader_storage_image_array_non_uniform_indexing(true)
            .descriptor_binding_uniform_buffer_update_after_bind(true)
            .descriptor_binding_sampled_image_update_after_bind(true)
            .descriptor_binding_storage_image_update_after_bind(true)
            .descriptor_binding_storage_buffer_update_after_bind(true)
            .descriptor_binding_update_unused_while_pending(true)
            .descriptor_binding_partially_bound(true)
            .descriptor_binding_variable_descriptor_count(true)
            .runtime_descriptor_array(true);

        let mut physical_device_features = vk::PhysicalDeviceFeatures2::builder()
            .push_next(&mut dynamic_rendering)
            .push_next(&mut timline_semaphore)
            .push_next(&mut synchronization2)
            .push_next(&mut descriptor_indexing);

        let extension_names = Device::EXTENSIONS
            .iter()
            .map(|&extension| extension.as_ptr())
            .collect::<Vec<_>>();

        let create_info = vk::DeviceCreateInfo::builder()
            .push_next(&mut physical_device_features)
            .queue_create_infos(&queue_create_infos)
            .enabled_extension_names(&extension_names);

        let raw = instance.create_logical_device(physical_device, &create_info)?;

        Ok(Self { raw, instance })
    }

    pub(crate) fn allocate_command_buffers(
        &self,
        allocate_info: vk::CommandBufferAllocateInfo,
    ) -> Result<Vec<vk::CommandBuffer>> {
        let command_buffers = unsafe { self.raw.allocate_command_buffers(&allocate_info) }?;
        Ok(command_buffers)
    }

    pub(crate) fn allocate_descriptor_sets(
        &self,
        allocate_info: vk::DescriptorSetAllocateInfo,
    ) -> Result<Vec<vk::DescriptorSet>> {
        let descriptor_sets = unsafe { self.raw.allocate_descriptor_sets(&allocate_info) }?;
        Ok(descriptor_sets)
    }

    pub(crate) fn begin_command_buffer(
        &self,
        command_buffer: vk::CommandBuffer,
        begin_info: vk::CommandBufferBeginInfo,
    ) -> Result<()> {
        unsafe {
            self.raw.begin_command_buffer(command_buffer, &begin_info)?;
        }

        Ok(())
    }

    pub(crate) fn bind_buffer_memory(
        &self,
        buffer: vk::Buffer,
        memory: vk::DeviceMemory,
        offset: u64,
    ) -> Result<()> {
        unsafe {
            self.raw.bind_buffer_memory(buffer, memory, offset)?;
        }

        Ok(())
    }

    pub(crate) fn bind_image_memory(
        &self,
        image: vk::Image,
        memory: vk::DeviceMemory,
        offset: u64,
    ) -> Result<()> {
        unsafe {
            self.raw.bind_image_memory(image, memory, offset)?;
        }

        Ok(())
    }

    pub(crate) fn cmd_begin_rendering(
        &self,
        command_buffer: vk::CommandBuffer,
        rendering_info: vk::RenderingInfo,
    ) {
        unsafe {
            self.raw
                .cmd_begin_rendering(command_buffer, &rendering_info);
        }
    }

    pub(crate) fn cmd_end_rendering(&self, command_buffer: vk::CommandBuffer) {
        unsafe {
            self.raw.cmd_end_rendering(command_buffer);
        }
    }

    pub(crate) fn cmd_bind_pipeline(
        &self,
        command_buffer: vk::CommandBuffer,
        pipeline_bind_point: vk::PipelineBindPoint,
        pipeline: vk::Pipeline,
    ) {
        unsafe {
            self.raw
                .cmd_bind_pipeline(command_buffer, pipeline_bind_point, pipeline);
        }
    }

    pub(crate) fn cmd_pipeline_barrier2(
        &self,
        command_buffer: vk::CommandBuffer,
        dependency_info: vk::DependencyInfo,
    ) {
        unsafe {
            self.raw
                .cmd_pipeline_barrier2(command_buffer, &dependency_info);
        }
    }

    pub(crate) fn cmd_set_viewport(
        &self,
        command_buffer: vk::CommandBuffer,
        first_viewport: u32,
        viewports: &[vk::Viewport],
    ) {
        unsafe {
            self.raw
                .cmd_set_viewport(command_buffer, first_viewport, viewports);
        }
    }

    pub(crate) fn cmd_set_scissor(
        &self,
        command_buffer: vk::CommandBuffer,
        first_scissor: u32,
        scissors: &[vk::Rect2D],
    ) {
        unsafe {
            self.raw
                .cmd_set_scissor(command_buffer, first_scissor, scissors);
        }
    }

    pub(crate) fn cmd_bind_descriptor_sets(
        &self,
        command_buffer: vk::CommandBuffer,
        pipeline_bind_point: vk::PipelineBindPoint,
        layout: vk::PipelineLayout,
        first_set: u32,
        descriptor_sets: &[vk::DescriptorSet],
        dynamic_offsets: &[u32],
    ) {
        unsafe {
            self.raw.cmd_bind_descriptor_sets(
                command_buffer,
                pipeline_bind_point,
                layout,
                first_set,
                descriptor_sets,
                dynamic_offsets,
            );
        }
    }

    pub(crate) fn cmd_push_constants(
        &self,
        command_buffer: vk::CommandBuffer,
        layout: vk::PipelineLayout,
        stage_flags: vk::ShaderStageFlags,
        offset: u32,
        constants: &[u8],
    ) {
        unsafe {
            self.raw
                .cmd_push_constants(command_buffer, layout, stage_flags, offset, constants);
        }
    }

    pub(crate) fn cmd_draw(
        &self,
        command_buffer: vk::CommandBuffer,
        vertex_count: u32,
        instance_count: u32,
        first_vertex: u32,
        first_instance: u32,
    ) {
        unsafe {
            self.raw.cmd_draw(
                command_buffer,
                vertex_count,
                instance_count,
                first_vertex,
                first_instance,
            );
        }
    }

    pub(crate) fn cmd_draw_indexed(
        &self,
        command_buffer: vk::CommandBuffer,
        index_count: u32,
        instance_count: u32,
        first_index: u32,
        vertex_offset: i32,
        first_instance: u32,
    ) {
        unsafe {
            self.raw.cmd_draw_indexed(
                command_buffer,
                index_count,
                instance_count,
                first_index,
                vertex_offset,
                first_instance,
            );
        }
    }

    pub(crate) fn cmd_bind_index_buffer(
        &self,
        command_buffer: vk::CommandBuffer,
        buffer: vk::Buffer,
        offset: vk::DeviceSize,
        index_type: vk::IndexType,
    ) {
        unsafe {
            self.raw
                .cmd_bind_index_buffer(command_buffer, buffer, offset, index_type);
        }
    }

    pub(crate) fn cmd_copy_buffer(
        &self,
        command_buffer: vk::CommandBuffer,
        source_buffer: vk::Buffer,
        destination_buffer: vk::Buffer,
        regions: &[vk::BufferCopy],
    ) {
        unsafe {
            self.raw
                .cmd_copy_buffer(command_buffer, source_buffer, destination_buffer, regions);
        }
    }

    pub(crate) fn cmd_copy_buffer_to_image(
        &self,
        command_buffer: vk::CommandBuffer,
        source_buffer: vk::Buffer,
        destination_image: vk::Image,
        destination_image_layout: vk::ImageLayout,
        regions: &[vk::BufferImageCopy],
    ) {
        unsafe {
            self.raw.cmd_copy_buffer_to_image(
                command_buffer,
                source_buffer,
                destination_image,
                destination_image_layout,
                regions,
            );
        }
    }

    pub(crate) fn create_buffer(&self, create_info: vk::BufferCreateInfo) -> Result<vk::Buffer> {
        let buffer = unsafe { self.raw.create_buffer(&create_info, None) }?;
        Ok(buffer)
    }

    pub(crate) fn create_command_pool(
        &self,
        create_info: vk::CommandPoolCreateInfo,
    ) -> Result<vk::CommandPool> {
        let command_pool = unsafe { self.raw.create_command_pool(&create_info, None) }?;
        Ok(command_pool)
    }

    #[allow(dead_code)]
    pub(crate) fn create_compute_pipelines(
        &self,
        create_infos: &[vk::ComputePipelineCreateInfo],
    ) -> Result<Vec<vk::Pipeline>> {
        let compute_pipelines = unsafe {
            self.raw
                .create_compute_pipelines(vk::PipelineCache::null(), create_infos, None)
        }
        .map_err(|error| error.1)?;
        Ok(compute_pipelines)
    }

    pub(crate) fn create_descriptor_pool(
        &self,
        create_info: vk::DescriptorPoolCreateInfo,
    ) -> Result<vk::DescriptorPool> {
        let descriptor_pool = unsafe { self.raw.create_descriptor_pool(&create_info, None) }?;
        Ok(descriptor_pool)
    }

    pub(crate) fn create_descriptor_set_layout(
        &self,
        create_info: vk::DescriptorSetLayoutCreateInfo,
    ) -> Result<vk::DescriptorSetLayout> {
        let descriptor_set_layout =
            unsafe { self.raw.create_descriptor_set_layout(&create_info, None) }?;
        Ok(descriptor_set_layout)
    }

    pub(crate) fn create_graphics_pipelines(
        &self,
        create_infos: &[vk::GraphicsPipelineCreateInfo],
    ) -> Result<Vec<vk::Pipeline>> {
        let graphics_pipelines = unsafe {
            self.raw
                .create_graphics_pipelines(vk::PipelineCache::null(), create_infos, None)
        }
        .map_err(|error| error.1)?;
        Ok(graphics_pipelines)
    }

    pub(crate) fn create_image(&self, create_info: vk::ImageCreateInfo) -> Result<vk::Image> {
        let image = unsafe { self.raw.create_image(&create_info, None) }?;
        Ok(image)
    }

    pub(crate) fn create_image_view(
        &self,
        create_info: vk::ImageViewCreateInfo,
    ) -> Result<vk::ImageView> {
        let image_view = unsafe { self.raw.create_image_view(&create_info, None) }?;
        Ok(image_view)
    }

    pub(crate) fn create_pipeline_layout(
        &self,
        create_info: vk::PipelineLayoutCreateInfo,
    ) -> Result<vk::PipelineLayout> {
        let pipeline_layout = unsafe { self.raw.create_pipeline_layout(&create_info, None) }?;
        Ok(pipeline_layout)
    }

    pub(crate) fn create_sampler(&self, create_info: vk::SamplerCreateInfo) -> Result<vk::Sampler> {
        let sampler = unsafe { self.raw.create_sampler(&create_info, None) }?;
        Ok(sampler)
    }

    pub(crate) fn create_shader_module(
        &self,
        create_info: vk::ShaderModuleCreateInfo,
    ) -> Result<vk::ShaderModule> {
        let shader_module = unsafe { self.raw.create_shader_module(&create_info, None) }?;
        Ok(shader_module)
    }

    pub(crate) fn create_semaphore(
        &self,
        create_info: vk::SemaphoreCreateInfo,
    ) -> Result<vk::Semaphore> {
        let semaphore = unsafe { self.raw.create_semaphore(&create_info, None) }?;
        Ok(semaphore)
    }

    pub(crate) fn destroy_buffer(&self, buffer: vk::Buffer) {
        unsafe {
            self.raw.destroy_buffer(buffer, None);
        }
    }

    pub(crate) fn destroy_command_pool(&self, command_pool: vk::CommandPool) {
        unsafe {
            self.raw.destroy_command_pool(command_pool, None);
        }
    }

    pub(crate) fn destroy_descriptor_pool(&self, descriptor_pool: vk::DescriptorPool) {
        unsafe {
            self.raw.destroy_descriptor_pool(descriptor_pool, None);
        }
    }

    pub(crate) fn destroy_descriptor_set_layout(
        &self,
        descriptor_set_layout: vk::DescriptorSetLayout,
    ) {
        unsafe {
            self.raw
                .destroy_descriptor_set_layout(descriptor_set_layout, None);
        }
    }

    pub(crate) fn destroy_image(&self, image: vk::Image) {
        unsafe {
            self.raw.destroy_image(image, None);
        }
    }

    pub(crate) fn destroy_image_view(&self, image_view: vk::ImageView) {
        unsafe {
            self.raw.destroy_image_view(image_view, None);
        }
    }

    pub(crate) fn destroy_pipeline(&self, pipeline: vk::Pipeline) {
        unsafe {
            self.raw.destroy_pipeline(pipeline, None);
        }
    }

    pub(crate) fn destroy_pipeline_layout(&self, pipeline_layout: vk::PipelineLayout) {
        unsafe {
            self.raw.destroy_pipeline_layout(pipeline_layout, None);
        }
    }

    pub(crate) fn destroy_shader_module(&self, shader_module: vk::ShaderModule) {
        unsafe {
            self.raw.destroy_shader_module(shader_module, None);
        }
    }

    pub(crate) fn destroy_semaphore(&self, semaphore: vk::Semaphore) {
        unsafe {
            self.raw.destroy_semaphore(semaphore, None);
        }
    }

    pub(crate) fn end_command_buffer(&self, command_buffer: vk::CommandBuffer) -> Result<()> {
        unsafe {
            self.raw.end_command_buffer(command_buffer)?;
        }

        Ok(())
    }

    pub(crate) fn get_buffer_memory_requirements(
        &self,
        buffer: vk::Buffer,
    ) -> vk::MemoryRequirements {
        unsafe { self.raw.get_buffer_memory_requirements(buffer) }
    }

    pub(crate) fn get_image_memory_requirements(&self, image: vk::Image) -> vk::MemoryRequirements {
        unsafe { self.raw.get_image_memory_requirements(image) }
    }

    pub(crate) fn reset_command_buffer(
        &self,
        command_buffer: vk::CommandBuffer,
        flags: vk::CommandBufferResetFlags,
    ) -> Result<()> {
        unsafe {
            self.raw.reset_command_buffer(command_buffer, flags)?;
        }

        Ok(())
    }

    pub(crate) fn reset_command_pool(
        &self,
        command_pool: vk::CommandPool,
        flags: vk::CommandPoolResetFlags,
    ) -> Result<()> {
        unsafe {
            self.raw.reset_command_pool(command_pool, flags)?;
        }

        Ok(())
    }

    pub(crate) fn set_object_name<T>(&self, debug_name: DebugName<T>) -> Result<()>
    where
        T: vk::Handle,
    {
        if let Some(debug_utils) = self.instance.debug_utils().as_ref() {
            debug_utils.set_object_name(self, debug_name)?;
        }

        Ok(())
    }

    pub(crate) fn submit_render_queue(
        &self,
        graphics_queue: vk::Queue,
        command_buffer: vk::CommandBuffer,
        present_semaphore: vk::Semaphore,
        render_semaphore: vk::Semaphore,
        submit_semaphore: vk::Semaphore,
        frame_id: u64,
    ) -> Result<()> {
        let present_wait_semaphore_info = vk::SemaphoreSubmitInfo::builder()
            .semaphore(present_semaphore)
            .value(0)
            .stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .device_index(0);

        let command_buffer_info = vk::CommandBufferSubmitInfo::builder()
            .command_buffer(command_buffer)
            .device_mask(0);

        let submit_signal_semaphore_info = vk::SemaphoreSubmitInfo::builder()
            .semaphore(submit_semaphore)
            .value(frame_id)
            .stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .device_index(0);

        let render_signal_semaphore_info = vk::SemaphoreSubmitInfo::builder()
            .semaphore(render_semaphore)
            .value(0)
            .stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .device_index(0);

        let wait_semaphore_infos = &[*present_wait_semaphore_info];
        let command_buffer_infos = &[*command_buffer_info];
        let signal_semaphore_infos =
            &[*submit_signal_semaphore_info, *render_signal_semaphore_info];
        let submit_info = vk::SubmitInfo2::builder()
            .wait_semaphore_infos(wait_semaphore_infos)
            .command_buffer_infos(command_buffer_infos)
            .signal_semaphore_infos(signal_semaphore_infos);

        unsafe {
            self.raw
                .queue_submit2(graphics_queue, &[*submit_info], vk::Fence::null())?;
        }

        Ok(())
    }

    pub(crate) fn submit_upload_queue(
        &self,
        graphics_queue: vk::Queue,
        command_buffer: vk::CommandBuffer,
        upload_semaphore: vk::Semaphore,
        last_upload_value: u64,
    ) -> Result<()> {
        let command_buffer_info = vk::CommandBufferSubmitInfo::builder()
            .command_buffer(command_buffer)
            .device_mask(0);

        let wait_semaphore_info = vk::SemaphoreSubmitInfo::builder()
            .semaphore(upload_semaphore)
            .value(last_upload_value)
            .stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .device_index(0);

        let signal_semaphore_info = vk::SemaphoreSubmitInfo::builder()
            .semaphore(upload_semaphore)
            .value(last_upload_value + 1)
            .stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .device_index(0);

        let wait_semaphore_infos = &[*wait_semaphore_info];
        let signal_semaphore_infos = &[*signal_semaphore_info];
        let command_buffer_infos = &[*command_buffer_info];
        let submit_info = vk::SubmitInfo2::builder()
            .command_buffer_infos(command_buffer_infos)
            .wait_semaphore_infos(wait_semaphore_infos)
            .signal_semaphore_infos(signal_semaphore_infos);

        unsafe {
            self.raw
                .queue_submit2(graphics_queue, &[*submit_info], vk::Fence::null())?;
        }

        Ok(())
    }

    pub(crate) fn update_descriptor_sets(
        &self,
        writes: &[vk::WriteDescriptorSet],
        copies: &[vk::CopyDescriptorSet],
    ) {
        unsafe {
            self.raw.update_descriptor_sets(writes, copies);
        }
    }

    pub(crate) fn wait_idle(&self) -> Result<()> {
        unsafe {
            self.raw.device_wait_idle()?;
        }

        Ok(())
    }

    pub(crate) fn wait_semaphores(&self, wait_info: vk::SemaphoreWaitInfo) -> Result<()> {
        unsafe {
            self.raw.wait_semaphores(&wait_info, u64::MAX)?;
        }

        Ok(())
    }

    pub(crate) fn get_device_queue(&self, queue_family_index: u32) -> vk::Queue {
        unsafe { self.raw.get_device_queue(queue_family_index, 0) }
    }

    pub(crate) fn raw(&self) -> &ash::Device {
        &self.raw
    }
}

impl Drop for LogicalDevice {
    fn drop(&mut self) {
        unsafe {
            self.raw.destroy_device(None);
        }
    }
}

pub(crate) struct LogicalDeviceCreateInfo<'a> {
    pub(crate) instance: Rc<Instance>,
    pub(crate) physical_device: &'a PhysicalDevice,
}
