/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use winit::keyboard;

#[derive(Clone, Copy, Debug, Hash, PartialEq, Eq)]
pub enum KeyCode {
    Backquote,
    Backslash,
    BracketLeft,
    BracketRight,
    Comma,
    Digit0,
    Digit1,
    Digit2,
    Digit3,
    Digit4,
    Digit5,
    Digit6,
    Digit7,
    Digit8,
    Digit9,
    Equal,
    IntlBackslash,
    IntlRo,
    IntlYen,
    KeyA,
    KeyB,
    KeyC,
    KeyD,
    KeyE,
    KeyF,
    KeyG,
    KeyH,
    KeyI,
    KeyJ,
    KeyK,
    KeyL,
    KeyM,
    KeyN,
    KeyO,
    KeyP,
    KeyQ,
    KeyR,
    KeyS,
    KeyT,
    KeyU,
    KeyV,
    KeyW,
    KeyX,
    KeyY,
    KeyZ,
    Minus,
    Period,
    Quote,
    Semicolon,
    Slash,
    AltLeft,
    AltRight,
    Backspace,
    CapsLock,
    ContextMenu,
    ControlLeft,
    ControlRight,
    Enter,
    SuperLeft,
    SuperRight,
    ShiftLeft,
    ShiftRight,
    Space,
    Tab,
    Convert,
    KanaMode,
    Lang1,
    Lang2,
    Lang3,
    Lang4,
    Lang5,
    NonConvert,
    Delete,
    End,
    Help,
    Home,
    Insert,
    PageDown,
    PageUp,
    ArrowDown,
    ArrowLeft,
    ArrowRight,
    ArrowUp,
    NumLock,
    Numpad0,
    Numpad1,
    Numpad2,
    Numpad3,
    Numpad4,
    Numpad5,
    Numpad6,
    Numpad7,
    Numpad8,
    Numpad9,
    NumpadAdd,
    NumpadBackspace,
    NumpadClear,
    NumpadClearEntry,
    NumpadComma,
    NumpadDecimal,
    NumpadDivide,
    NumpadEnter,
    NumpadEqual,
    NumpadHash,
    NumpadMemoryAdd,
    NumpadMemoryClear,
    NumpadMemoryRecall,
    NumpadMemoryStore,
    NumpadMemorySubtract,
    NumpadMultiply,
    NumpadParenLeft,
    NumpadParenRight,
    NumpadStar,
    NumpadSubtract,
    Escape,
    Fn,
    FnLock,
    PrintScreen,
    ScrollLock,
    Pause,
    BrowserBack,
    BrowserFavorites,
    BrowserForward,
    BrowserHome,
    BrowserRefresh,
    BrowserSearch,
    BrowserStop,
    Eject,
    LaunchApp1,
    LaunchApp2,
    LaunchMail,
    MediaPlayPause,
    MediaSelect,
    MediaStop,
    MediaTrackNext,
    MediaTrackPrevious,
    Power,
    Sleep,
    AudioVolumeDown,
    AudioVolumeMute,
    AudioVolumeUp,
    WakeUp,
    Meta,
    Hyper,
    Turbo,
    Abort,
    Resume,
    Suspend,
    Again,
    Copy,
    Cut,
    Find,
    Open,
    Paste,
    Props,
    Select,
    Undo,
    Hiragana,
    Katakana,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    F13,
    F14,
    F15,
    F16,
    F17,
    F18,
    F19,
    F20,
    F21,
    F22,
    F23,
    F24,
    F25,
    F26,
    F27,
    F28,
    F29,
    F30,
    F31,
    F32,
    F33,
    F34,
    F35,
}

impl From<keyboard::KeyCode> for KeyCode {
    fn from(value: keyboard::KeyCode) -> Self {
        match value {
            keyboard::KeyCode::Backquote => Self::Backquote,
            keyboard::KeyCode::Backslash => Self::Backslash,
            keyboard::KeyCode::BracketLeft => Self::BracketLeft,
            keyboard::KeyCode::BracketRight => Self::BracketRight,
            keyboard::KeyCode::Comma => Self::Comma,
            keyboard::KeyCode::Digit0 => Self::Digit0,
            keyboard::KeyCode::Digit1 => Self::Digit1,
            keyboard::KeyCode::Digit2 => Self::Digit2,
            keyboard::KeyCode::Digit3 => Self::Digit3,
            keyboard::KeyCode::Digit4 => Self::Digit4,
            keyboard::KeyCode::Digit5 => Self::Digit5,
            keyboard::KeyCode::Digit6 => Self::Digit6,
            keyboard::KeyCode::Digit7 => Self::Digit7,
            keyboard::KeyCode::Digit8 => Self::Digit8,
            keyboard::KeyCode::Digit9 => Self::Digit9,
            keyboard::KeyCode::Equal => Self::Equal,
            keyboard::KeyCode::IntlBackslash => Self::IntlBackslash,
            keyboard::KeyCode::IntlRo => Self::IntlRo,
            keyboard::KeyCode::IntlYen => Self::IntlYen,
            keyboard::KeyCode::KeyA => Self::KeyA,
            keyboard::KeyCode::KeyB => Self::KeyB,
            keyboard::KeyCode::KeyC => Self::KeyC,
            keyboard::KeyCode::KeyD => Self::KeyD,
            keyboard::KeyCode::KeyE => Self::KeyE,
            keyboard::KeyCode::KeyF => Self::KeyF,
            keyboard::KeyCode::KeyG => Self::KeyG,
            keyboard::KeyCode::KeyH => Self::KeyH,
            keyboard::KeyCode::KeyI => Self::KeyI,
            keyboard::KeyCode::KeyJ => Self::KeyJ,
            keyboard::KeyCode::KeyK => Self::KeyK,
            keyboard::KeyCode::KeyL => Self::KeyL,
            keyboard::KeyCode::KeyM => Self::KeyM,
            keyboard::KeyCode::KeyN => Self::KeyN,
            keyboard::KeyCode::KeyO => Self::KeyO,
            keyboard::KeyCode::KeyP => Self::KeyP,
            keyboard::KeyCode::KeyQ => Self::KeyQ,
            keyboard::KeyCode::KeyR => Self::KeyR,
            keyboard::KeyCode::KeyS => Self::KeyS,
            keyboard::KeyCode::KeyT => Self::KeyT,
            keyboard::KeyCode::KeyU => Self::KeyU,
            keyboard::KeyCode::KeyV => Self::KeyV,
            keyboard::KeyCode::KeyW => Self::KeyW,
            keyboard::KeyCode::KeyX => Self::KeyX,
            keyboard::KeyCode::KeyY => Self::KeyY,
            keyboard::KeyCode::KeyZ => Self::KeyZ,
            keyboard::KeyCode::Minus => Self::Minus,
            keyboard::KeyCode::Period => Self::Period,
            keyboard::KeyCode::Quote => Self::Quote,
            keyboard::KeyCode::Semicolon => Self::Semicolon,
            keyboard::KeyCode::Slash => Self::Slash,
            keyboard::KeyCode::AltLeft => Self::AltLeft,
            keyboard::KeyCode::AltRight => Self::AltRight,
            keyboard::KeyCode::Backspace => Self::Backspace,
            keyboard::KeyCode::CapsLock => Self::CapsLock,
            keyboard::KeyCode::ContextMenu => Self::ContextMenu,
            keyboard::KeyCode::ControlLeft => Self::ControlLeft,
            keyboard::KeyCode::ControlRight => Self::ControlRight,
            keyboard::KeyCode::Enter => Self::Enter,
            keyboard::KeyCode::SuperLeft => Self::SuperLeft,
            keyboard::KeyCode::SuperRight => Self::SuperRight,
            keyboard::KeyCode::ShiftLeft => Self::ShiftLeft,
            keyboard::KeyCode::ShiftRight => Self::ShiftRight,
            keyboard::KeyCode::Space => Self::Space,
            keyboard::KeyCode::Tab => Self::Tab,
            keyboard::KeyCode::Convert => Self::Convert,
            keyboard::KeyCode::KanaMode => Self::KanaMode,
            keyboard::KeyCode::Lang1 => Self::Lang1,
            keyboard::KeyCode::Lang2 => Self::Lang2,
            keyboard::KeyCode::Lang3 => Self::Lang3,
            keyboard::KeyCode::Lang4 => Self::Lang4,
            keyboard::KeyCode::Lang5 => Self::Lang5,
            keyboard::KeyCode::NonConvert => Self::NonConvert,
            keyboard::KeyCode::Delete => Self::Delete,
            keyboard::KeyCode::End => Self::End,
            keyboard::KeyCode::Help => Self::Help,
            keyboard::KeyCode::Home => Self::Home,
            keyboard::KeyCode::Insert => Self::Insert,
            keyboard::KeyCode::PageDown => Self::PageDown,
            keyboard::KeyCode::PageUp => Self::PageUp,
            keyboard::KeyCode::ArrowDown => Self::ArrowDown,
            keyboard::KeyCode::ArrowLeft => Self::ArrowLeft,
            keyboard::KeyCode::ArrowRight => Self::ArrowRight,
            keyboard::KeyCode::ArrowUp => Self::ArrowUp,
            keyboard::KeyCode::NumLock => Self::NumLock,
            keyboard::KeyCode::Numpad0 => Self::Numpad0,
            keyboard::KeyCode::Numpad1 => Self::Numpad1,
            keyboard::KeyCode::Numpad2 => Self::Numpad2,
            keyboard::KeyCode::Numpad3 => Self::Numpad3,
            keyboard::KeyCode::Numpad4 => Self::Numpad4,
            keyboard::KeyCode::Numpad5 => Self::Numpad5,
            keyboard::KeyCode::Numpad6 => Self::Numpad6,
            keyboard::KeyCode::Numpad7 => Self::Numpad7,
            keyboard::KeyCode::Numpad8 => Self::Numpad8,
            keyboard::KeyCode::Numpad9 => Self::Numpad9,
            keyboard::KeyCode::NumpadAdd => Self::NumpadAdd,
            keyboard::KeyCode::NumpadBackspace => Self::NumpadBackspace,
            keyboard::KeyCode::NumpadClear => Self::NumpadClear,
            keyboard::KeyCode::NumpadClearEntry => Self::NumpadClearEntry,
            keyboard::KeyCode::NumpadComma => Self::NumpadComma,
            keyboard::KeyCode::NumpadDecimal => Self::NumpadDecimal,
            keyboard::KeyCode::NumpadDivide => Self::NumpadDivide,
            keyboard::KeyCode::NumpadEnter => Self::NumpadEnter,
            keyboard::KeyCode::NumpadEqual => Self::NumpadEqual,
            keyboard::KeyCode::NumpadHash => Self::NumpadHash,
            keyboard::KeyCode::NumpadMemoryAdd => Self::NumpadMemoryAdd,
            keyboard::KeyCode::NumpadMemoryClear => Self::NumpadMemoryClear,
            keyboard::KeyCode::NumpadMemoryRecall => Self::NumpadMemoryRecall,
            keyboard::KeyCode::NumpadMemoryStore => Self::NumpadMemoryStore,
            keyboard::KeyCode::NumpadMemorySubtract => Self::NumpadMemorySubtract,
            keyboard::KeyCode::NumpadMultiply => Self::NumpadMultiply,
            keyboard::KeyCode::NumpadParenLeft => Self::NumpadParenLeft,
            keyboard::KeyCode::NumpadParenRight => Self::NumpadParenRight,
            keyboard::KeyCode::NumpadStar => Self::NumpadStar,
            keyboard::KeyCode::NumpadSubtract => Self::NumpadSubtract,
            keyboard::KeyCode::Escape => Self::Escape,
            keyboard::KeyCode::Fn => Self::Fn,
            keyboard::KeyCode::FnLock => Self::FnLock,
            keyboard::KeyCode::PrintScreen => Self::PrintScreen,
            keyboard::KeyCode::ScrollLock => Self::ScrollLock,
            keyboard::KeyCode::Pause => Self::Pause,
            keyboard::KeyCode::BrowserBack => Self::BrowserBack,
            keyboard::KeyCode::BrowserFavorites => Self::BrowserFavorites,
            keyboard::KeyCode::BrowserForward => Self::BrowserForward,
            keyboard::KeyCode::BrowserHome => Self::BrowserHome,
            keyboard::KeyCode::BrowserRefresh => Self::BrowserRefresh,
            keyboard::KeyCode::BrowserSearch => Self::BrowserSearch,
            keyboard::KeyCode::BrowserStop => Self::BrowserStop,
            keyboard::KeyCode::Eject => Self::Eject,
            keyboard::KeyCode::LaunchApp1 => Self::LaunchApp1,
            keyboard::KeyCode::LaunchApp2 => Self::LaunchApp2,
            keyboard::KeyCode::LaunchMail => Self::LaunchMail,
            keyboard::KeyCode::MediaPlayPause => Self::MediaPlayPause,
            keyboard::KeyCode::MediaSelect => Self::MediaSelect,
            keyboard::KeyCode::MediaStop => Self::MediaStop,
            keyboard::KeyCode::MediaTrackNext => Self::MediaTrackNext,
            keyboard::KeyCode::MediaTrackPrevious => Self::MediaTrackPrevious,
            keyboard::KeyCode::Power => Self::Power,
            keyboard::KeyCode::Sleep => Self::Sleep,
            keyboard::KeyCode::AudioVolumeDown => Self::AudioVolumeDown,
            keyboard::KeyCode::AudioVolumeMute => Self::AudioVolumeMute,
            keyboard::KeyCode::AudioVolumeUp => Self::AudioVolumeUp,
            keyboard::KeyCode::WakeUp => Self::WakeUp,
            keyboard::KeyCode::Meta => Self::Meta,
            keyboard::KeyCode::Hyper => Self::Hyper,
            keyboard::KeyCode::Turbo => Self::Turbo,
            keyboard::KeyCode::Abort => Self::Abort,
            keyboard::KeyCode::Resume => Self::Resume,
            keyboard::KeyCode::Suspend => Self::Suspend,
            keyboard::KeyCode::Again => Self::Again,
            keyboard::KeyCode::Copy => Self::Copy,
            keyboard::KeyCode::Cut => Self::Cut,
            keyboard::KeyCode::Find => Self::Find,
            keyboard::KeyCode::Open => Self::Open,
            keyboard::KeyCode::Paste => Self::Paste,
            keyboard::KeyCode::Props => Self::Props,
            keyboard::KeyCode::Select => Self::Select,
            keyboard::KeyCode::Undo => Self::Undo,
            keyboard::KeyCode::Hiragana => Self::Hiragana,
            keyboard::KeyCode::Katakana => Self::Katakana,
            keyboard::KeyCode::F1 => Self::F1,
            keyboard::KeyCode::F2 => Self::F2,
            keyboard::KeyCode::F3 => Self::F3,
            keyboard::KeyCode::F4 => Self::F4,
            keyboard::KeyCode::F5 => Self::F5,
            keyboard::KeyCode::F6 => Self::F6,
            keyboard::KeyCode::F7 => Self::F7,
            keyboard::KeyCode::F8 => Self::F8,
            keyboard::KeyCode::F9 => Self::F9,
            keyboard::KeyCode::F10 => Self::F10,
            keyboard::KeyCode::F11 => Self::F11,
            keyboard::KeyCode::F12 => Self::F12,
            keyboard::KeyCode::F13 => Self::F13,
            keyboard::KeyCode::F14 => Self::F14,
            keyboard::KeyCode::F15 => Self::F15,
            keyboard::KeyCode::F16 => Self::F16,
            keyboard::KeyCode::F17 => Self::F17,
            keyboard::KeyCode::F18 => Self::F18,
            keyboard::KeyCode::F19 => Self::F19,
            keyboard::KeyCode::F20 => Self::F20,
            keyboard::KeyCode::F21 => Self::F21,
            keyboard::KeyCode::F22 => Self::F22,
            keyboard::KeyCode::F23 => Self::F23,
            keyboard::KeyCode::F24 => Self::F24,
            keyboard::KeyCode::F25 => Self::F25,
            keyboard::KeyCode::F26 => Self::F26,
            keyboard::KeyCode::F27 => Self::F27,
            keyboard::KeyCode::F28 => Self::F28,
            keyboard::KeyCode::F29 => Self::F29,
            keyboard::KeyCode::F30 => Self::F30,
            keyboard::KeyCode::F31 => Self::F31,
            keyboard::KeyCode::F32 => Self::F32,
            keyboard::KeyCode::F33 => Self::F33,
            keyboard::KeyCode::F34 => Self::F34,
            keyboard::KeyCode::F35 => Self::F35,
            _ => unreachable!(),
        }
    }
}
