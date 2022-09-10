/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use ash::{
    vk::{
        self, BufferUsageFlags, DescriptorBufferInfo, DescriptorSetAllocateInfo,
        DescriptorSetLayout, DescriptorSetVariableDescriptorCountAllocateInfo, DescriptorType,
        PhysicalDeviceLimits, WriteDescriptorSet,
    },
    Device,
};
use log::debug;
use std::{cell::RefCell, mem, rc::Rc};
use tracing::instrument;

use crate::{
    allocator::Allocator,
    buffers::buffer::{Buffer, BufferCreateInfo},
    descriptors::descriptor_pool::DescriptorPool,
    vertex::Vertex,
};

// NOTE: TEMP
pub(crate) struct Bindings {
    pub _vertices: u32,
    pub _transforms: u32,
}

pub(crate) struct DescriptorSetCreateInfo<'a> {
    pub logical_device: &'a Device,
    pub allocator: &'a Rc<RefCell<Allocator>>,

    pub descriptor_pool: &'a vk::DescriptorPool,
    pub descriptor_set_layout: &'a DescriptorSetLayout,
    pub descriptor_type: &'a DescriptorType,

    pub limits: &'a PhysicalDeviceLimits,
}

pub(crate) struct DescriptorSet {
    _first_buffer: Buffer,
    _second_buffer: Buffer,
    _third_buffer: Buffer,

    descriptor_set: vk::DescriptorSet,
}

impl DescriptorSet {
    #[instrument(skip_all)]
    pub fn new(create_info: &DescriptorSetCreateInfo) -> Self {
        let count = DescriptorPool::find_descriptor_type_limit(
            create_info.descriptor_type,
            create_info.limits,
        );

        let descriptor_counts = [count];

        let mut descriptor_set_variable_descriptor_count_allocate_info =
            DescriptorSetVariableDescriptorCountAllocateInfo::builder()
                .descriptor_counts(&descriptor_counts);

        let descriptor_set_layouts = [*create_info.descriptor_set_layout];

        let descriptor_set_allocate_info = DescriptorSetAllocateInfo::builder()
            .push_next(&mut descriptor_set_variable_descriptor_count_allocate_info)
            .descriptor_pool(*create_info.descriptor_pool)
            .set_layouts(descriptor_set_layouts.as_slice());

        let descriptor_set = unsafe {
            create_info
                .logical_device
                .allocate_descriptor_sets(&descriptor_set_allocate_info)
                .expect("Failed to allocate descriptor set")[0]
        };

        let buffer_create_info = BufferCreateInfo {
            logical_device: create_info.logical_device,
            allocator: create_info.allocator,
            allocation_size: mem::size_of::<Bindings>() as u64,
            usage: BufferUsageFlags::STORAGE_BUFFER,
        };

        let first_buffer = Buffer::new(&buffer_create_info);

        let buffer_create_info = BufferCreateInfo {
            logical_device: create_info.logical_device,
            allocator: create_info.allocator,
            allocation_size: (mem::size_of::<Vertex>() * 3) as u64,
            usage: BufferUsageFlags::STORAGE_BUFFER,
        };

        let second_buffer = Buffer::new(&buffer_create_info);

        let buffer_create_info = BufferCreateInfo {
            logical_device: create_info.logical_device,
            allocator: create_info.allocator,
            allocation_size: mem::size_of::<nalgebra_glm::Mat4>() as u64,
            usage: BufferUsageFlags::STORAGE_BUFFER,
        };

        let third_buffer = Buffer::new(&buffer_create_info);

        if *create_info.descriptor_type == DescriptorType::STORAGE_BUFFER {
            // TODO: Abstract and more flexible
            {
                let bindings = Bindings {
                    _vertices: 1,
                    _transforms: 2,
                };

                first_buffer.set_data(&[bindings]);

                let descriptor_buffer_info = DescriptorBufferInfo::builder()
                    .buffer(*first_buffer.internal_buffer())
                    .offset(0)
                    .range(vk::WHOLE_SIZE);

                let descriptor_buffer_infos = &[*descriptor_buffer_info];

                let write_descriptor_set = WriteDescriptorSet::builder()
                    .dst_set(descriptor_set)
                    .dst_binding(0)
                    .dst_array_element(0)
                    .descriptor_type(DescriptorType::STORAGE_BUFFER)
                    .buffer_info(descriptor_buffer_infos);

                unsafe {
                    create_info
                        .logical_device
                        .update_descriptor_sets(&[*write_descriptor_set], &[]);
                }
            }

            {
                let triangle_vertices = vec![
                    Vertex::new(
                        nalgebra_glm::vec3(1.0, 1.0, 0.5),
                        nalgebra_glm::vec3(1.0, 0.0, 0.0),
                        //vec3(0.0, 0.0, 0.0),
                    ),
                    Vertex::new(
                        nalgebra_glm::vec3(-1.0, 1.0, 0.5),
                        nalgebra_glm::vec3(0.0, 1.0, 0.0),
                        //vec3(0.0, 0.0, 0.0),
                    ),
                    Vertex::new(
                        nalgebra_glm::vec3(0.0, -1.0, 0.5),
                        nalgebra_glm::vec3(0.0, 0.0, 1.0),
                        //vec3(0.0, 0.0, 0.0),
                    ),
                ];

                second_buffer.set_data(&triangle_vertices);

                let descriptor_buffer_info = DescriptorBufferInfo::builder()
                    .buffer(*second_buffer.internal_buffer())
                    .offset(0)
                    .range(vk::WHOLE_SIZE);

                let descriptor_buffer_infos = &[*descriptor_buffer_info];

                let write_descriptor_set = WriteDescriptorSet::builder()
                    .dst_set(descriptor_set)
                    .dst_binding(0)
                    .dst_array_element(1)
                    .descriptor_type(DescriptorType::STORAGE_BUFFER)
                    .buffer_info(descriptor_buffer_infos);

                unsafe {
                    create_info
                        .logical_device
                        .update_descriptor_sets(&[*write_descriptor_set], &[]);
                }
            }

            {
                let camera_position = nalgebra_glm::vec3(0.0, 0.0, -2.0);

                let view_matrix = nalgebra_glm::translate(
                    &nalgebra_glm::mat4(
                        1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
                        1.0,
                    ),
                    &camera_position,
                );

                let mut projection_matrix =
                    nalgebra_glm::perspective(f32::to_radians(90.0), 1280.0 / 720.0, 0.1, 200.0);

                projection_matrix.m22 *= -1_f32;

                let transform = projection_matrix * view_matrix;

                third_buffer.set_data(&[transform]);

                let descriptor_buffer_info = DescriptorBufferInfo::builder()
                    .buffer(*third_buffer.internal_buffer())
                    .offset(0)
                    .range(vk::WHOLE_SIZE);

                let descriptor_buffer_infos = &[*descriptor_buffer_info];

                let write_descriptor_set = WriteDescriptorSet::builder()
                    .dst_set(descriptor_set)
                    .dst_binding(0)
                    .dst_array_element(2)
                    .descriptor_type(DescriptorType::STORAGE_BUFFER)
                    .buffer_info(descriptor_buffer_infos);

                unsafe {
                    create_info
                        .logical_device
                        .update_descriptor_sets(&[*write_descriptor_set], &[]);
                }
            }
        }

        debug!("Created descriptor set");

        Self {
            _first_buffer: first_buffer,
            _second_buffer: second_buffer,
            _third_buffer: third_buffer,
            descriptor_set,
        }
    }

    pub fn descriptor_set(&self) -> &vk::DescriptorSet {
        &self.descriptor_set
    }
}
