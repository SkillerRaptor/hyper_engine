/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::camera::Camera;

use hyper_math::{
    matrix::Mat4x4f,
    vector::{self, Vec2f, Vec3f},
};
use hyper_platform::{input::Input, key_code::KeyCode, mouse_code::MouseCode, window::Window};
use nalgebra_glm::Quat;

pub struct FpsCamera {
    aspect_ratio: f32,

    last_pressed: bool,
    last_mouse_position: Vec2f,

    position: Vec3f,
    // TODO: Make own math lib
    rotation: Quat,

    near: f32,
    far: f32,

    fov: f32,
}

impl FpsCamera {
    pub fn new(aspect_ratio: f32) -> Self {
        Self {
            aspect_ratio,

            last_pressed: false,
            last_mouse_position: Vec2f::default(),

            position: Vec3f::default(),
            rotation: Quat::identity(),

            near: 0.1,
            far: 1000.0,

            fov: 90.0_f32.to_radians(),
        }
    }

    pub fn update(
        &mut self,
        delta_time: f32,
        input: &Input,
        window_focused: bool,
        // TODO: Don't pass window
        window: &mut Window,
    ) {
        let mut translation = Vec3f::default();

        if input.get_key(KeyCode::W) {
            translation += vector::BACK;
        }

        if input.get_key(KeyCode::S) {
            translation += vector::FRONT;
        }

        if input.get_key(KeyCode::A) {
            translation += vector::LEFT;
        }

        if input.get_key(KeyCode::D) {
            translation += vector::RIGHT;
        }

        if input.get_key(KeyCode::LShift) {
            translation += vector::DOWN;
        }

        if input.get_key(KeyCode::Space) {
            translation += vector::UP;
        }

        // TODO: Check for focus
        let mut delta = Vec2f::default();
        if window_focused && input.get_mouse_button(MouseCode::Middle) {
            if self.last_pressed {
                let delta_x = input.get_mouse_position().x - self.last_mouse_position.x;
                let delta_y = input.get_mouse_position().y - self.last_mouse_position.y;
                delta = Vec2f::new(delta_x, delta_y);
            } else {
                self.last_mouse_position.x = input.get_mouse_position().x;
                self.last_mouse_position.y = input.get_mouse_position().y;
                self.last_pressed = true;
                window.set_cursor_visiblity(false);
            }

            window
                .set_cursor_position(self.last_mouse_position)
                .unwrap();
        } else {
            window.set_cursor_visiblity(true);
            self.last_pressed = false;
        }

        if input.get_mouse_button(MouseCode::Middle) {
            let mouse_speed = 0.025;
            self.rotation =
                nalgebra_glm::quat_angle_axis(delta.x * mouse_speed, &Vec3f::new(0.0, -1.0, 0.0))
                    * self.rotation;
            self.rotation *=
                nalgebra_glm::quat_angle_axis(delta.y * mouse_speed, &Vec3f::new(-1.0, 0.0, 0.0));
            self.rotation = self.rotation.normalize();
        }

        // TODO: Replace constants
        self.position +=
            nalgebra_glm::quat_rotate_vec3(&self.rotation, &(translation * 50.0 * delta_time));
    }

    pub fn on_window_resize(&mut self, width: u32, height: u32) {
        self.aspect_ratio = width as f32 / height as f32;
    }
}

impl Camera for FpsCamera {
    fn projection_matrix(&self) -> Mat4x4f {
        nalgebra_glm::perspective(self.aspect_ratio, self.fov, self.near, self.far)
    }

    fn view_matrix(&self) -> Mat4x4f {
        let translation_matrix = Mat4x4f::new_translation(&self.position);
        let rotation_matrix = nalgebra_glm::quat_to_mat4(&self.rotation);

        nalgebra_glm::inverse(&(translation_matrix * rotation_matrix))
    }
}
