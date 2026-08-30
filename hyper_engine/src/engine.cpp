/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "engine.hpp"

#include <chrono>

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>
#include <hyper_script/debug.hpp>
#include <hyper_script/diagnostics.hpp>
#include <hyper_script/ir_builder.hpp>
#include <hyper_script/lexer.hpp>
#include <hyper_script/parser.hpp>
#include <hyper_script/source_manager.hpp>

namespace he {

Engine::Engine()
{
    const std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();

    m_window = std::make_unique<Window>("HyperEngine", 1280, 720);
    m_graphics_device = GraphicsDevice::create(GraphicsApi::Vulkan, *m_window);

    constexpr std::string_view source = R"(
fn foo() {
    let a = 0;
    let b = a + 10;
    let c = a + b;
    let d = a + b + c;

    let x = 10;
    let y = 20;
    print(x);
    print(y);

    let z = 0;
    if (x > y) {
      z = x - y;
    } else {
      z = x + y;
    }

    print(z);
}
    )";

    const File file = {
        .path = "./test.hyper",
        .source = std::string(source),
    };

    const std::vector<File> files = { file };
    compile({ files });

    const std::chrono::time_point<std::chrono::steady_clock> end_time = std::chrono::steady_clock::now();
    const std::chrono::duration<f32> elapsed_seconds = end_time - start_time;

    HE_INFO("Completed engine initialization (time={:.2}s)", elapsed_seconds.count());
}

void Engine::run()
{
    constexpr f32 delta_time = 1.0f / 60.0f;

    f32 total_time = 0.0f;
    f32 accumulator = 0.0f;
    std::chrono::time_point<std::chrono::steady_clock> current_time = std::chrono::steady_clock::now();
    while (!m_window->is_close_requested()) {
        const std::chrono::time_point<std::chrono::steady_clock> new_time = std::chrono::steady_clock::now();
        const std::chrono::duration<f32> frame_time = new_time - current_time;
        current_time = new_time;

        accumulator += frame_time.count();

        m_window->update();
        m_input.update();

        while (accumulator >= delta_time) {
            // Fixed Update
            fixed_update(delta_time);

            accumulator -= delta_time;
            total_time += delta_time;
        }

        // Update
        update(delta_time);

        // Render
        render();
    }
}

void Engine::fixed_update(const f32 delta_time) { (void) delta_time; }

void Engine::update(const f32 delta_time) { (void) delta_time; }

void Engine::render() const { }

void Engine::compile(const std::span<const File> files)
{
    script::SourceManager source_manager;
    script::DiagnosticEngine diagnostic_engine;

    for (const File file : files) {
        const script::SourceId source_id = source_manager.add_file(file.path, file.source);
        const usize errors_before = diagnostic_engine.error_count();

        script::Lexer lexer(source_manager, diagnostic_engine, source_id);
        const std::vector<script::Token> tokens = lexer.lex();
        if (diagnostic_engine.error_count() > errors_before) {
            continue;
        }

        script::dump_tokens(tokens);

        script::Parser parser(diagnostic_engine, tokens);
        const std::unique_ptr<script::AstNode> ast = parser.parse();
        if (diagnostic_engine.error_count() > errors_before) {
            continue;
        }

        script::dump_ast(*ast);

        const script::TranslationUnitDeclaration &translation_unit_declaration
            = static_cast<const script::TranslationUnitDeclaration &>(*ast);

        for (const std::unique_ptr<script::Declaration> &declaration : translation_unit_declaration.declarations) {
            HE_ASSERT(declaration->kind == script::AstNodeKind::FunctionDeclaration);

            const script::FunctionDeclaration &function_declaration
                = static_cast<const script::FunctionDeclaration &>(*declaration);

            script::IrBuilder builder(function_declaration);
            const std::unique_ptr<script::IrFunction> function = builder.build();

            script::dump_ir(*function);
        }
    }

    script::DiagnosticRenderer renderer(source_manager);
    renderer.render_all(diagnostic_engine.diagnostics());
}

} // namespace he
