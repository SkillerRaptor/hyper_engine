/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

/// A trait representing a game implementaion
pub trait Game {
    /// Updates the current frame
    ///
    /// This function will be called after the engine has updated the internal
    /// state.
    fn update(&mut self);

    /// Updates the current frame at a fixed rate
    ///
    /// This function will be called at a fixed rate, which is specified by the
    /// engine. The fixed rate is most likely 60 or 120 frames per second.
    ///
    /// Arguments:
    ///
    /// * `delta_time`: Time since the last fixed update
    /// * `time`: Time since the begin of the current frame
    fn update_fixed(&mut self, delta_time: f32, time: f32);

    /// Renders the current frame
    ///
    /// This function will be called after the engine has rendered the scene
    fn render(&mut self);
}
