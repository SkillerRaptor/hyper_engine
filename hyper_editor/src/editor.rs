/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_engine::game::Game;

use egui::Context;
use hyper_math::vector::Vec3f;

pub(crate) struct Editor {
    rotation: f32,
    light_position: Vec3f,
    text: String,
}

impl Editor {
    pub(crate) fn new() -> Self {
        Self {
            rotation: 0.0,
            light_position: Vec3f::default(),
            text: String::new(),
        }
    }
}

impl Game for Editor {
    fn update(&mut self) {}

    fn update_fixed(&mut self, _delta_time: f32, _time: f32) {}

    fn render(&mut self) {}

    fn render_gui(&mut self, context: &Context) {
        egui::SidePanel::left("my_side_panel").show(context, |ui| {
            ui.heading("Hello");
            ui.label("Hello Hyper Engine!");
            ui.separator();
            ui.hyperlink("https://github.com/SkillerRaptor/hyper_engine");
            ui.separator();
            ui.label("Rotation");
            ui.add(egui::widgets::DragValue::new(&mut self.rotation));
            ui.add(egui::widgets::Slider::new(
                &mut self.rotation,
                -180.0..=180.0,
            ));
            ui.label("Light Position");
            ui.horizontal(|ui| {
                ui.label("x:");
                ui.add(egui::widgets::DragValue::new(&mut self.light_position.x));
                ui.label("y:");
                ui.add(egui::widgets::DragValue::new(&mut self.light_position.y));
                ui.label("z:");
                ui.add(egui::widgets::DragValue::new(&mut self.light_position.z));
            });
            ui.separator();
            ui.text_edit_singleline(&mut self.text);
        });
        egui::Window::new("My Window")
            .resizable(true)
            .scroll2([true, true])
            .show(context, |ui| {
                ui.heading("Hello");
                ui.label("Hello Hyper Engine!");
                ui.separator();
                ui.hyperlink("https://github.com/SkillerRaptor/hyper_engine");
                ui.separator();
                ui.label("Rotation");
                ui.add(egui::widgets::DragValue::new(&mut self.rotation));
                ui.add(egui::widgets::Slider::new(
                    &mut self.rotation,
                    -180.0..=180.0,
                ));
                ui.label("Light Position");
                ui.horizontal(|ui| {
                    ui.label("x:");
                    ui.add(egui::widgets::DragValue::new(&mut self.light_position.x));
                    ui.label("y:");
                    ui.add(egui::widgets::DragValue::new(&mut self.light_position.y));
                    ui.label("z:");
                    ui.add(egui::widgets::DragValue::new(&mut self.light_position.z));
                });
                ui.separator();
                ui.text_edit_singleline(&mut self.text);
            });
    }
}
