/*
 * Copyright (c) 2023-2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use hyper_math::{Mat4, Quat, Vec2, Vec3};
use hyper_platform::{input::Input, key_code::KeyCode, mouse_code::MouseCode, window::Window};

use crate::camera::Camera;

pub struct FpsCamera {
    aspect_ratio: f32,

    last_pressed: bool,
    last_mouse_position: Vec2,

    position: Vec3,
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
            last_mouse_position: Vec2::default(),

            position: Vec3::default(),
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
        // TODO: Don't pass window
        window: &mut Window,
    ) {
        let mut translation = Vec3::default();

        if input.is_key_pressed(KeyCode::KeyW) {
            translation += Vec3::new(0.0, 0.0, -1.0);
        }

        if input.is_key_pressed(KeyCode::KeyS) {
            translation += Vec3::new(0.0, 0.0, 1.0);
        }

        if input.is_key_pressed(KeyCode::KeyA) {
            translation += Vec3::new(-1.0, 0.0, 0.0);
        }

        if input.is_key_pressed(KeyCode::KeyD) {
            translation += Vec3::new(1.0, 0.0, 0.0);
        }

        if input.is_key_pressed(KeyCode::ShiftLeft) {
            translation += Vec3::new(0.0, -1.0, 0.0);
        }

        if input.is_key_pressed(KeyCode::Space) {
            translation += Vec3::new(0.0, 1.0, 0.0);
        }

        let mut delta = Vec2::default();
        if input.is_mouse_button_pressed(MouseCode::Middle) {
            if self.last_pressed {
                let delta_x = input.mouse_position().x - self.last_mouse_position.x;
                let delta_y = input.mouse_position().y - self.last_mouse_position.y;
                delta = Vec2::new(delta_x, delta_y);
            } else {
                self.last_mouse_position.x = input.mouse_position().x;
                self.last_mouse_position.y = input.mouse_position().y;
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

        if input.is_mouse_button_pressed(MouseCode::Middle) {
            let mouse_speed = 0.025;
            self.rotation =
                hyper_math::quat_angle_axis(delta.x * mouse_speed, &Vec3::new(0.0, -1.0, 0.0))
                    * self.rotation;
            self.rotation *=
                hyper_math::quat_angle_axis(delta.y * mouse_speed, &Vec3::new(-1.0, 0.0, 0.0));
            self.rotation = self.rotation.normalize();
        }

        // TODO: Replace constants
        self.position +=
            hyper_math::quat_rotate_vec3(&self.rotation, &(translation * 50.0 * delta_time));
    }

    pub fn on_window_resize(&mut self, width: u32, height: u32) {
        self.aspect_ratio = width as f32 / height as f32;
    }
}

impl Camera for FpsCamera {
    fn projection_matrix(&self) -> Mat4 {
        hyper_math::perspective(self.aspect_ratio, self.fov, self.near, self.far)
    }

    fn view_matrix(&self) -> Mat4 {
        let translation_matrix = Mat4::new_translation(&self.position);
        let rotation_matrix = hyper_math::quat_to_mat4(&self.rotation);

        hyper_math::inverse(&(translation_matrix * rotation_matrix))
    }
}
