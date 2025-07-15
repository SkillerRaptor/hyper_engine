/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "platform/event_server.hpp"

#include <SDL3/SDL.h>

#include "core/logger.hpp"
#include "platform/key_events.hpp"
#include "platform/mouse_events.hpp"
#include "platform/window_events.hpp"

std::unique_ptr<EventServer> EventServer::create()
{
    HE_INFO("Initialized event server");
    return std::unique_ptr<EventServer>(new EventServer());
}

EventServer::~EventServer() { HE_INFO("Shutdown event server"); }

void EventServer::poll()
{
    SDL_Event event {};
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT: dispatch<WindowCloseEvent>(); break;
        case SDL_EVENT_WINDOW_MOVED:
            dispatch<WindowMoveEvent>(static_cast<u32>(event.window.data1), static_cast<u32>(event.window.data2));
            break;
        case SDL_EVENT_WINDOW_RESIZED:
            dispatch<WindowResizeEvent>(static_cast<u32>(event.window.data1), static_cast<u32>(event.window.data2));
            break;
        case SDL_EVENT_KEY_DOWN: dispatch<KeyPressEvent>(static_cast<KeyCode>(event.key.key)); break;
        case SDL_EVENT_KEY_UP: dispatch<KeyReleaseEvent>(static_cast<KeyCode>(event.key.key)); break;
        case SDL_EVENT_MOUSE_MOTION: dispatch<MouseMoveEvent>(event.motion.x, event.motion.y); break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            dispatch<MouseButtonPressEvent>(static_cast<MouseCode>(event.button.button));
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            dispatch<MouseButtonReleaseEvent>(static_cast<MouseCode>(event.button.button));
            break;
        case SDL_EVENT_MOUSE_WHEEL: dispatch<MouseScrollEvent>(event.wheel.x, event.wheel.y); break;
        default: break;
        }
    }
}
