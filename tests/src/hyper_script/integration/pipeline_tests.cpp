/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include <hyper_core/filesystem.hpp>
#include <hyper_script/debug.hpp>
#include <hyper_script/diagnostics.hpp>
#include <hyper_script/ir_builder.hpp>
#include <hyper_script/lexer.hpp>
#include <hyper_script/parser.hpp>
#include <hyper_script/source_manager.hpp>

#include "dynamic_test_invoker.hpp"

using namespace he;

static constexpr bool s_generate_golden_files = false;

static std::string sanitize(std::string string)
{
    std::erase(string, '\0');
    std::erase(string, '\r');
    while (!string.empty() && (string.back() == '\n' || string.back() == ' ' || string.back() == '\t')) {
        string.pop_back();
    }

    return string;
}

static std::vector<std::string> split_lines(const std::string &text)
{
    std::vector<std::string> lines;

    std::string line;
    std::stringstream string_stream(text);
    while (std::getline(string_stream, line)) {
        lines.push_back(line);
    }

    return lines;
}

static void assert_golden_match(
    const std::filesystem::path &golden_path, const std::string &actual_content, const std::string_view stage_name)
{
    if (s_generate_golden_files) {
        std::ofstream out(golden_path);
        out << actual_content;
        return;
    }

    if (!std::filesystem::exists(golden_path)) {
        return;
    }

    const std::string expected_content = filesystem::read_to_string(golden_path.string()).value();

    const std::string normalized_actual = sanitize(actual_content);
    const std::string normalized_expected = sanitize(expected_content);
    if (normalized_actual == normalized_expected) {
        return;
    }

    const std::vector<std::string> actual_lines = split_lines(normalized_actual);
    const std::vector<std::string> expected_lines = split_lines(normalized_expected);
    const size_t max_lines = std::max(actual_lines.size(), expected_lines.size());
    for (size_t i = 0; i < max_lines; ++i) {
        const std::string actual_line = (i < actual_lines.size()) ? actual_lines[i] : "<EOF>";
        const std::string expected_line = (i < expected_lines.size()) ? expected_lines[i] : "<EOF>";

        if (actual_line != expected_line) {
            std::string diff_summary = fmt::format(
                "Difference in {} at line {}:\n"
                "  Expected: \"{}\"\n"
                "  Actual:   \"{}\"\n",
                stage_name,
                i + 1,
                expected_line,
                actual_line);

            FAIL_CHECK(diff_summary);

            return;
        }
    }
}

static void run_pipeline_test(const std::filesystem::path &test_dir)
{
    const std::filesystem::path test_directory = test_dir.lexically_normal();
    const std::string test_name = test_directory.filename().string();

    const std::filesystem::path source_file = test_directory / (test_name + ".hyper");

    if (!std::filesystem::exists(source_file)) {
        FAIL("Missing .hyper source file: " << source_file.string());
    }

    script::SourceManager source_manager;
    script::DiagnosticEngine diagnostic_engine;

    const script::SourceId source_id
        = source_manager.add_file(test_name + ".hyper", filesystem::read_to_string(source_file.string()).value());

    INFO(source_manager.get_path(source_id));
    INFO(source_manager.get_source(source_id));

    script::Lexer lexer(source_manager, diagnostic_engine, source_id);
    const std::vector<script::Token> tokens = lexer.lex();

    SECTION("Test Lexer")
    {
        REQUIRE(!diagnostic_engine.has_errors());

        std::ostringstream token_stream;
        script::dump_tokens(token_stream, tokens, false);

        const std::filesystem::path tokens_file = test_directory / (test_name + ".tokens");
        assert_golden_match(tokens_file, token_stream.str(), "Lexer");
    }

    script::Parser parser(diagnostic_engine, tokens);
    const std::unique_ptr<script::AstNode> ast = parser.parse();

    SECTION("Test Parser")
    {
        REQUIRE(!diagnostic_engine.has_errors());

        std::ostringstream ast_stream;
        script::dump_ast(ast_stream, *ast, false);

        const std::filesystem::path ast_file = test_directory / (test_name + ".ast");
        assert_golden_match(ast_file, ast_stream.str(), "Parser");
    }

    SECTION("Test IR")
    {
        const script::TranslationUnitDeclaration &translation_unit_declaration
            = static_cast<const script::TranslationUnitDeclaration &>(*ast);

        std::ostringstream ir_stream;
        for (const std::unique_ptr<script::Declaration> &declaration : translation_unit_declaration.declarations) {
            HE_ASSERT(declaration->kind == script::AstNodeKind::FunctionDeclaration);

            const script::FunctionDeclaration &function_declaration
                = static_cast<const script::FunctionDeclaration &>(*declaration);

            script::IrBuilder builder(function_declaration);
            const std::unique_ptr<script::IrFunction> function = builder.build();

            script::dump_ir(ir_stream, *function);
        }

        const std::filesystem::path ir_file = test_directory / (test_name + ".ir");
        assert_golden_match(ir_file, ir_stream.str(), "IR");
    }
}

struct PipelineTestsRegistry {
    PipelineTestsRegistry()
    {
        const std::filesystem::path data_path = "./test_data/hyper_script";
        if (!std::filesystem::exists(data_path)) {
            return;
        }

        for (const std::filesystem::directory_entry &test_entry : std::filesystem::directory_iterator(data_path)) {
            if (!test_entry.is_directory()) {
                continue;
            }

            const std::string folder_name = test_entry.path().filename().string();
            const std::string test_name = "HyperScript Integration: " + folder_name;

            Catch::AutoReg(
                create_dynamic_test_invoker([test_path = test_entry.path()]() { run_pipeline_test(test_path); }),
                CATCH_INTERNAL_LINEINFO,
                Catch::StringRef(),
                Catch::NameAndTags(test_name, "[hyper_script][integration][pipeline]"));
        }
    }
};

static PipelineTestsRegistry registry;
