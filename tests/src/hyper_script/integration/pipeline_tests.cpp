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

static constexpr bool s_generate_golden_files = true;

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

static void dump_tokens(
    std::ostream &stream, const script::SourceManager &source_manager, const std::span<const script::Token> tokens)
{
    if (tokens.empty()) {
        return;
    }

    for (const script::Token &token : tokens) {
        const script::SourcePosition position
            = source_manager.get_position(token.span.source_id, token.span.start_offset);
        stream << fmt::format(
            "{} \"{}\" @{}:{}\n",
            script::token_kind_to_string(token.kind),
            token.lexeme,
            position.line,
            position.column);
    }
}

static void
    dump_node(std::ostream &stream, const script::AstNode &node, const std::string &prefix, const bool is_root = false)
{
    const std::string connector = is_root ? "" : "  ";
    const std::string child_prefix = prefix + (is_root ? "" : "  ");

    switch (node.kind) {
    case script::AstNodeKind::FunctionDeclaration: {
        const script::FunctionDeclaration &declaration = static_cast<const script::FunctionDeclaration &>(node);
        stream
            << fmt::format("{}{}FunctionDeclaration {{ identifier: {} }}\n", prefix, connector, declaration.identifier);
        dump_node(stream, *declaration.body, child_prefix);
        break;
    }
    case script::AstNodeKind::TranslationUnitDeclaration: {
        const script::TranslationUnitDeclaration &declaration
            = static_cast<const script::TranslationUnitDeclaration &>(node);
        stream << fmt::format("{}{}TranslationUnitDeclaration\n", prefix, connector);
        if (declaration.declarations.empty()) {
            break;
        }
        for (usize i = 0; i < declaration.declarations.size(); ++i) {
            dump_node(stream, *declaration.declarations[i], child_prefix);
        }
        break;
    }
    case script::AstNodeKind::VariableDeclaration: {
        const script::VariableDeclaration &declaration = static_cast<const script::VariableDeclaration &>(node);
        stream
            << fmt::format("{}{}VariableDeclaration {{ identifier: {} }}\n", prefix, connector, declaration.identifier);
        dump_node(stream, *declaration.initializer, child_prefix);
        break;
    }
    case script::AstNodeKind::BinaryExpression: {
        const script::BinaryExpression &expression = static_cast<const script::BinaryExpression &>(node);
        const std::string_view operation = [&]() {
            switch (expression.operation) {
            case script::BinaryOperation::Addition:
                return "Addition";
            case script::BinaryOperation::Subtraction:
                return "Subtraction";
            case script::BinaryOperation::Multiplication:
                return "Multiplication";
            case script::BinaryOperation::Division:
                return "Division";
            case script::BinaryOperation::Equal:
                return "Equal";
            case script::BinaryOperation::NotEqual:
                return "NotEqual";
            case script::BinaryOperation::LessThan:
                return "LessThan";
            case script::BinaryOperation::LessThanOrEqual:
                return "LessThanOrEqual";
            case script::BinaryOperation::GreaterThan:
                return "GreaterThan";
            case script::BinaryOperation::GreaterThanOrEqual:
                return "GreaterThanOrEqual";
            default:
                HE_UNREACHABLE();
            }
        }();
        stream << fmt::format("{}{}BinaryExpression {{ operation: {} }}\n", prefix, connector, operation);
        dump_node(stream, *expression.left, child_prefix);
        dump_node(stream, *expression.right, child_prefix);
        break;
    }
    case script::AstNodeKind::CallExpression: {
        const script::CallExpression &expression = static_cast<const script::CallExpression &>(node);
        stream << fmt::format("{}{}CallExpression {{ identifier: {} }}\n", prefix, connector, expression.identifier);
        if (expression.arguments.empty()) {
            break;
        }
        for (usize i = 0; i < expression.arguments.size(); ++i) {
            dump_node(stream, *expression.arguments[i], child_prefix);
        }
        break;
    }
    case script::AstNodeKind::LiteralExpression: {
        const script::LiteralExpression &expression = static_cast<const script::LiteralExpression &>(node);
        stream << fmt::format("{}{}LiteralExpression\n", prefix, connector);
        dump_node(stream, *expression.literal, child_prefix);
        break;
    }
    case script::AstNodeKind::VariableExpression: {
        const script::VariableExpression &expression = static_cast<const script::VariableExpression &>(node);
        stream
            << fmt::format("{}{}VariableExpression {{ identifier: {} }}\n", prefix, connector, expression.identifier);
        break;
    }
    case script::AstNodeKind::IntegerLiteral: {
        const script::IntegerLiteral &literal = static_cast<const script::IntegerLiteral &>(node);
        stream << fmt::format("{}{}IntegerLiteral {{ value: {} }}\n", prefix, connector, literal.value);
        break;
    }
    case script::AstNodeKind::AssignStatement: {
        const script::AssignStatement &statement = static_cast<const script::AssignStatement &>(node);
        stream << fmt::format("{}{}AssignStatement {{ identifier: {} }}\n", prefix, connector, statement.identifier);
        dump_node(stream, *statement.value, child_prefix);
        break;
    }
    case script::AstNodeKind::CompoundStatement: {
        const script::CompoundStatement &statement = static_cast<const script::CompoundStatement &>(node);
        stream << fmt::format("{}{}CompoundStatement\n", prefix, connector);
        if (statement.statements.empty()) {
            break;
        }
        for (usize i = 0; i < statement.statements.size(); ++i) {
            dump_node(stream, *statement.statements[i], child_prefix);
        }
        break;
    }
    case script::AstNodeKind::DeclarationStatement: {
        const script::DeclarationStatement &statement = static_cast<const script::DeclarationStatement &>(node);
        stream << fmt::format("{}{}DeclarationStatement\n", prefix, connector);
        dump_node(stream, *statement.declaration, child_prefix);
        break;
    }
    case script::AstNodeKind::ExpressionStatement: {
        const script::ExpressionStatement &statement = static_cast<const script::ExpressionStatement &>(node);
        stream << fmt::format("{}{}ExpressionStatement\n", prefix, connector);
        dump_node(stream, *statement.expression, child_prefix);
        break;
    }
    case script::AstNodeKind::IfStatement: {
        const script::IfStatement &statement = static_cast<const script::IfStatement &>(node);
        stream << fmt::format("{}{}IfStatement\n", prefix, connector);
        dump_node(stream, *statement.condition, child_prefix);
        dump_node(stream, *statement.then_body, child_prefix);
        if (statement.else_body) {
            dump_node(stream, *statement.else_body, child_prefix);
        }
        break;
    }
    case script::AstNodeKind::WhileStatement: {
        const script::WhileStatement &statement = static_cast<const script::WhileStatement &>(node);
        stream << fmt::format("{}{}WhileStatement\n", prefix, connector);
        dump_node(stream, *statement.condition, child_prefix);
        dump_node(stream, *statement.body, child_prefix);
        break;
    }
    default:
        HE_PANIC("Encountered an unexpected node while dumping");
    }
}

static void dump_ast(std::ostream &stream, const script::AstNode &node) { dump_node(stream, node, "", true); }

static void dump_ir(std::ostream &stream, const script::IrFunction &function)
{
    stream << fmt::format("define @{}() {{\n", function.name);

    for (usize i = 0; i < function.blocks.size(); ++i) {
        const std::unique_ptr<script::IrBlock> &block = function.blocks[i];

        if (block->id > 0) {
            stream << "\n";
        }
        stream << fmt::format("block_{}:\n", block->id);

        for (const script::IrValue *instruction : block->instructions) {
            switch (instruction->kind) {
            case script::IrValueKind::Unknown:
            case script::IrValueKind::Undef:
                break;
            case script::IrValueKind::Constant: {
                const script::IrConstant *constant = static_cast<const script::IrConstant *>(instruction);
                stream << fmt::format("  %{} = iconst {}\n", instruction->id, constant->value);
                break;
            }
            case script::IrValueKind::Addition: {
                const script::IrAdd *add = static_cast<const script::IrAdd *>(instruction);
                stream << fmt::format("  %{} = iadd %{} %{}\n", instruction->id, add->left->id, add->right->id);
                break;
            }
            case script::IrValueKind::Subtraction: {
                const script::IrSub *sub = static_cast<const script::IrSub *>(instruction);
                stream << fmt::format("  %{} = isub %{} %{}\n", instruction->id, sub->left->id, sub->right->id);
                break;
            }
            case script::IrValueKind::Multiplication: {
                const script::IrMul *mul = static_cast<const script::IrMul *>(instruction);
                stream << fmt::format("  %{} = imul %{} %{}\n", instruction->id, mul->left->id, mul->right->id);
                break;
            }
            case script::IrValueKind::Division: {
                const script::IrDiv *div = static_cast<const script::IrDiv *>(instruction);
                stream << fmt::format("  %{} = idiv %{} %{}\n", instruction->id, div->left->id, div->right->id);
                break;
            }
            case script::IrValueKind::Compare: {
                const script::IrCmp *cmp = static_cast<const script::IrCmp *>(instruction);
                std::string_view operation_string;
                switch (cmp->operation) {
                case script::CompareOperation::Equal:
                    operation_string = "eq";
                    break;
                case script::CompareOperation::NotEqual:
                    operation_string = "ne";
                    break;
                case script::CompareOperation::LessThan:
                    operation_string = "lt";
                    break;
                case script::CompareOperation::LessThanOrEqual:
                    operation_string = "le";
                    break;
                case script::CompareOperation::GreaterThan:
                    operation_string = "gt";
                    break;
                case script::CompareOperation::GreaterThanOrEqual:
                    operation_string = "ge";
                    break;
                }
                stream << fmt::format(
                    "  %{} = icmp {} %{} %{}\n",
                    instruction->id,
                    operation_string,
                    cmp->left->id,
                    cmp->right->id);
                break;
            }
            case script::IrValueKind::Branch: {
                const script::IrBranch *branch = static_cast<const script::IrBranch *>(instruction);
                if (branch->condition != nullptr) {
                    stream << fmt::format(
                        "  br %{}, %block_{}, %block_{}\n",
                        branch->condition->id,
                        branch->true_target->id,
                        branch->false_target->id);
                } else {
                    stream << fmt::format("  br %block_{}\n", branch->true_target->id);
                }
                break;
            }
            case script::IrValueKind::Call: {
                const script::IrCall *call = static_cast<const script::IrCall *>(instruction);
                std::string arguments;
                for (usize j = 0; j < call->arguments.size(); ++j) {
                    arguments += fmt::format("%{}", call->arguments[j]->id);
                    if (j + 1 < call->arguments.size()) {
                        arguments += ", ";
                    }
                }
                stream << fmt::format("  %{} = call @{}({})\n", instruction->id, call->callee, arguments);
                break;
            }
            case script::IrValueKind::Phi: {
                const script::IrPhi *phi = static_cast<const script::IrPhi *>(instruction);
                std::string operands;
                for (usize j = 0; j < phi->operands.size(); ++j) {
                    operands
                        += fmt::format("[ %{}, %block_{} ]", phi->operands[j]->id, phi->parent->predecessors[j]->id);
                    if (j + 1 < phi->operands.size()) {
                        operands += ", ";
                    }
                }
                stream << fmt::format("  %{} = φ {}\n", instruction->id, operands);
                break;
            }
            }
        }
    }

    stream << "}\n";
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
        dump_tokens(token_stream, source_manager, tokens);

        const std::filesystem::path tokens_file = test_directory / (test_name + ".tokens");
        assert_golden_match(tokens_file, token_stream.str(), "Lexer");
    }

    script::Parser parser(diagnostic_engine, tokens);
    const std::unique_ptr<script::AstNode> ast = parser.parse();

    SECTION("Test Parser")
    {
        REQUIRE(!diagnostic_engine.has_errors());

        std::ostringstream ast_stream;
        dump_ast(ast_stream, *ast);

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

            dump_ir(ir_stream, *function);
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
