/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_script/diagnostics.hpp"

#include <algorithm>
#include <array>

#include <fmt/format.h>

#include <hyper_core/assertion.hpp>

namespace he::script {

Diagnostic &Diagnostic::with_label(const Span &span, std::optional<std::string> msg, const LabelStyle style)
{
    labels.emplace_back(span, std::move(msg), style);
    return *this;
}

Diagnostic &Diagnostic::with_note(std::string new_note)
{
    note = std::move(new_note);
    return *this;
}

Diagnostic &Diagnostic::with_help(std::string new_help)
{
    help = std::move(new_help);
    return *this;
}

Diagnostic &DiagnosticEngine::emit_note(std::string message)
{
    const Diagnostic diagnostic = {
        .severity = Severity::Note,
        .message = std::move(message),
        .labels = { },
        .note = std::nullopt,
        .help = std::nullopt,
    };

    m_diagnostics.push_back(std::move(diagnostic));

    return m_diagnostics.back();
}

Diagnostic &DiagnosticEngine::emit_help(std::string message)
{
    const Diagnostic diagnostic = {
        .severity = Severity::Help,
        .message = std::move(message),
        .labels = { },
        .note = std::nullopt,
        .help = std::nullopt,
    };

    m_diagnostics.push_back(std::move(diagnostic));

    return m_diagnostics.back();
}

Diagnostic &DiagnosticEngine::emit_warning(std::string message)
{
    const Diagnostic diagnostic = {
        .severity = Severity::Warning,
        .message = std::move(message),
        .labels = { },
        .note = std::nullopt,
        .help = std::nullopt,
    };

    m_diagnostics.push_back(std::move(diagnostic));
    m_warning_count += 1;

    return m_diagnostics.back();
}

Diagnostic &DiagnosticEngine::emit_error(std::string message)
{
    const Diagnostic diagnostic = {
        .severity = Severity::Error,
        .message = std::move(message),
        .labels = { },
        .note = std::nullopt,
        .help = std::nullopt,
    };

    m_diagnostics.push_back(std::move(diagnostic));
    m_error_count += 1;

    return m_diagnostics.back();
}

Diagnostic &DiagnosticEngine::emit_fatal(std::string message)
{
    const Diagnostic diagnostic = {
        .severity = Severity::Fatal,
        .message = std::move(message),
        .labels = { },
        .note = std::nullopt,
        .help = std::nullopt,
    };

    m_diagnostics.push_back(std::move(diagnostic));

    return m_diagnostics.back();
}

void DiagnosticRenderer::render(const Diagnostic &diagnostic) const
{
    HE_ASSERT(!diagnostic.labels.empty());

    const Label &first_primary_label = find_first_primary_label(diagnostic);
    const SourcePosition first_primary_position
        = m_source_manager.get_position(first_primary_label.span.source_id, first_primary_label.span.start_offset);

    const std::vector<RenderBlock> blocks = build_render_blocks(diagnostic);
    const usize line_width = determine_line_width(first_primary_position.line, blocks);

    render_header(diagnostic, first_primary_label, first_primary_position, line_width);
    render_blocks(diagnostic, blocks, line_width);
    render_footer(diagnostic, line_width);
}

void DiagnosticRenderer::render_all(const std::span<const Diagnostic> diagnostics) const
{
    for (const Diagnostic &diagnostic : diagnostics) {
        render(diagnostic);
    }
}

std::string_view DiagnosticRenderer::severity_to_string(const Severity severity)
{
    switch (severity) {
    case Severity::Note:
        return "note";
    case Severity::Help:
        return "help";
    case Severity::Warning:
        return "warning";
    case Severity::Error:
        return "error";
    case Severity::Fatal:
        return "fatal";
    }

    HE_UNREACHABLE();
}

fmt::color DiagnosticRenderer::severity_to_color(const Severity severity)
{
    switch (severity) {
    case Severity::Note:
        return fmt::color::cyan;
    case Severity::Help:
        return fmt::color::magenta;
    case Severity::Warning:
        return fmt::color::yellow;
    case Severity::Error:
        return fmt::color::red;
    case Severity::Fatal:
        return fmt::color::crimson;
    }

    HE_UNREACHABLE();
}

fmt::color DiagnosticRenderer::annotation_to_color(const LabelStyle style, const Diagnostic &diagnostic)
{
    if (style == LabelStyle::Primary) {
        return severity_to_color(diagnostic.severity);
    }

    return fmt::color::cornflower_blue;
}

const Label &DiagnosticRenderer::find_first_primary_label(const Diagnostic &diagnostic)
{
    for (const Label &label : diagnostic.labels) {
        if (label.style == LabelStyle::Primary) {
            return label;
        }
    }

    return diagnostic.labels.front();
}

std::vector<DiagnosticRenderer::RenderBlock> DiagnosticRenderer::build_render_blocks(const Diagnostic &diagnostic) const
{
    std::vector<RenderBlock> render_blocks;

    for (const Label &label : diagnostic.labels) {
        const SourcePosition start = m_source_manager.get_position(label.span.source_id, label.span.start_offset);
        const SourcePosition end = m_source_manager.get_position(label.span.source_id, label.span.end_offset);

        if (start.line != end.line) {
            const MultiLineAnnotation multi_line_annotation = {
                .label = &label,
                .start = start,
                .end = end,
            };

            const RenderBlock render_block = {
                .source_id = label.span.source_id,
                .first_line = start.line,
                .last_line = end.line,
                .single_line_annotations = { },
                .multi_line_annotation = multi_line_annotation,
            };

            render_blocks.push_back(render_block);

            continue;
        }

        RenderBlock *same_render_block = nullptr;
        for (RenderBlock &render_block : render_blocks) {
            if (render_block.single_line_annotations.empty()) {
                continue;
            }

            if (render_block.source_id != label.span.source_id) {
                continue;
            }

            if (render_block.first_line != start.line) {
                continue;
            }

            same_render_block = &render_block;
        }

        if (same_render_block == nullptr) {
            RenderBlock &render_block = render_blocks.emplace_back(
                RenderBlock {
                    .source_id = label.span.source_id,
                    .first_line = start.line,
                    .last_line = end.line,
                    .single_line_annotations = { },
                    .multi_line_annotation = std::nullopt,
                });
            same_render_block = &render_block;
        }

        same_render_block->single_line_annotations.push_back(
            {
                .label = &label,
                .line = start.line,
                .start_column = start.column,
                .end_column = end.column,
            });
    }

    for (RenderBlock &render_block : render_blocks) {
        std::ranges::sort(
            render_block.single_line_annotations,
            [](const SingleLineAnnotation &a, const SingleLineAnnotation &b) {
                return a.start_column < b.start_column;
            });
    }

    std::ranges::sort(render_blocks, [](const RenderBlock &a, const RenderBlock &b) {
        if (a.source_id != b.source_id) {
            return a.source_id < b.source_id;
        }

        return a.first_line < b.first_line;
    });

    return render_blocks;
}

u32 DiagnosticRenderer::determine_line_width(const u32 primary_line, const std::span<const RenderBlock> render_blocks)
{
    u32 max_line = primary_line;
    for (const RenderBlock &render_block : render_blocks) {
        max_line = std::max(max_line, render_block.first_line);
    }

    return std::max<u32>(2, static_cast<u32>(fmt::format("{}", max_line).size()));
}

void DiagnosticRenderer::render_header(
    const Diagnostic &diagnostic,
    const Label &first_primary_label,
    const SourcePosition &first_primary_position,
    const usize line_width) const
{
    fmt::print(
        "{}: {}\n",
        fmt::format(fmt::fg(severity_to_color(diagnostic.severity)), "{}", severity_to_string(diagnostic.severity)),
        fmt::format(fmt::emphasis::bold, "{}", diagnostic.message));
    fmt::print(
        "{:>{}} {} {}:{}:{}\n",
        "",
        line_width,
        fmt::format(fmt::fg(fmt::color::cornflower_blue), "╭▸"),
        m_source_manager.get_path(first_primary_label.span.source_id),
        first_primary_position.line,
        first_primary_position.column);
    fmt::print("{:>{}} {}\n", "", line_width, fmt::format(fmt::fg(fmt::color::cornflower_blue), "│"));
}

void DiagnosticRenderer::render_blocks(
    const Diagnostic &diagnostic, const std::span<const RenderBlock> render_blocks, const usize line_width) const
{
    usize previous_last_line = 0;
    std::optional<SourceId> previous_source = std::nullopt;
    for (const RenderBlock &render_block : render_blocks) {
        if (previous_source.has_value()) {
            if (render_block.source_id != previous_source.value()) {
                u32 first_column = 0;

                if (render_block.multi_line_annotation.has_value()) {
                    first_column = render_block.multi_line_annotation->start.column;
                } else {
                    first_column = render_block.single_line_annotations.front().start_column;

                    for (const SingleLineAnnotation &annotation : render_block.single_line_annotations) {
                        if (annotation.label->style == LabelStyle::Primary) {
                            first_column = annotation.start_column;
                            break;
                        }
                    }
                }

                fmt::print("{:>{}} {}\n", "", line_width, fmt::format(fmt::fg(fmt::color::cornflower_blue), "│"));
                fmt::print(
                    "{:>{}} {} {}:{}:{}\n",
                    "",
                    line_width,
                    fmt::format(fmt::fg(fmt::color::cornflower_blue), "├▸"),
                    m_source_manager.get_path(render_block.source_id),
                    render_block.first_line,
                    first_column);
                fmt::print("{:>{}} {}\n", "", line_width, fmt::format(fmt::fg(fmt::color::cornflower_blue), "│"));
            } else if (render_block.first_line > previous_last_line + 1) {
                fmt::print("{:>{}} {}\n", "", line_width, fmt::format(fmt::fg(fmt::color::cornflower_blue), "│"));
            }
        }

        previous_last_line = render_block.last_line;
        previous_source = render_block.source_id;

        if (render_block.multi_line_annotation.has_value()) {
            render_multi_line_block(diagnostic, render_block.multi_line_annotation.value(), line_width);
        } else {
            render_single_line_block(diagnostic, render_block, line_width);
        }
    }
}

void DiagnosticRenderer::render_single_line_block(
    const Diagnostic &diagnostic, const RenderBlock &render_block, const usize line_width) const
{
    struct Glyphs {
        std::string_view vertical;
        std::string_view horizontal;
        std::string_view tee;
    };

    constexpr Glyphs primary_glyphs = {
        .vertical = "┃",
        .horizontal = "━",
        .tee = "┯",
    };

    constexpr Glyphs secondary_glyphs = {
        .vertical = "│",
        .horizontal = "─",
        .tee = "┬",
    };

    fmt::print(
        "{:>{}} {} {}\n",
        render_block.first_line,
        line_width,
        fmt::format(fmt::fg(fmt::color::cornflower_blue), "│"),
        m_source_manager.get_line(render_block.source_id, render_block.first_line));

    std::string markings;
    usize written_columns = 0;
    for (usize i = 0; i < render_block.single_line_annotations.size(); ++i) {
        const SingleLineAnnotation &annotation = render_block.single_line_annotations[i];
        const Glyphs &glyphs = (annotation.label->style == LabelStyle::Primary) ? primary_glyphs : secondary_glyphs;

        const fmt::color annotation_color = annotation_to_color(annotation.label->style, diagnostic);

        const usize start_column = annotation.start_column - 1;
        const usize width = annotation.end_column - annotation.start_column;
        const bool is_last = (i + 1 == render_block.single_line_annotations.size());

        if (start_column > written_columns) {
            markings += std::string(start_column - written_columns, ' ');
            written_columns = start_column;
        }

        std::string horizontal;
        for (usize j = 0; j < width - (!is_last * 1); ++j) {
            horizontal += glyphs.horizontal;
        }

        if (is_last) {
            markings
                += fmt::format(fmt::fg(annotation_color), "{} {}", horizontal, annotation.label->message.value_or(""));
        } else {
            markings += fmt::format(fmt::fg(annotation_color), "{}{}", glyphs.tee, horizontal);
        }

        written_columns += width;
    }

    fmt::print("{} {}\n", fmt::format(fmt::fg(fmt::color::cornflower_blue), "{:>{}} │", "", line_width), markings);

    for (usize stack_index = 1; stack_index < render_block.single_line_annotations.size(); ++stack_index) {
        const usize target_index = render_block.single_line_annotations.size() - stack_index - 1;

        std::string bar_row;
        usize bar_column = 0;
        for (usize j = 0; j <= target_index; ++j) {
            const SingleLineAnnotation &bar_annotation = render_block.single_line_annotations[j];
            const usize start_column = bar_annotation.start_column - 1;
            if (start_column > bar_column) {
                bar_row += std::string(start_column - bar_column, ' ');
                bar_column = start_column;
            }

            bar_row += fmt::format(fmt::fg(annotation_to_color(bar_annotation.label->style, diagnostic)), "│");
            bar_column += 1;
        }

        fmt::print("{:>{}} {} {}\n", "", line_width, fmt::format(fmt::fg(fmt::color::cornflower_blue), "│"), bar_row);

        const SingleLineAnnotation &annotation = render_block.single_line_annotations[target_index];
        std::string message_row;
        usize message_column = 0;
        for (usize j = 0; j < target_index; ++j) {
            const SingleLineAnnotation &bar_annotation = render_block.single_line_annotations[j];
            const usize start_column = bar_annotation.start_column - 1;
            if (start_column > message_column) {
                message_row += std::string(start_column - message_column, ' ');
                message_column = start_column;
            }

            message_row += fmt::format(fmt::fg(annotation_to_color(bar_annotation.label->style, diagnostic)), "│");
            message_column += 1;
        }

        const usize target_column = annotation.start_column - 1;
        if (target_column > message_column) {
            message_row += std::string(target_column - message_column, ' ');
        }

        if (annotation.label->message.has_value()) {
            message_row += fmt::format(
                fmt::fg(annotation_to_color(annotation.label->style, diagnostic)),
                "{}",
                annotation.label->message.value());
        }

        fmt::print(
            "{:>{}} {} {}\n",
            "",
            line_width,
            fmt::format(fmt::fg(fmt::color::cornflower_blue), "│"),
            message_row);
    }
}

void DiagnosticRenderer::render_multi_line_block(
    const Diagnostic &diagnostic, const MultiLineAnnotation &annotation, const usize line_width) const
{
    struct Glyphs {
        std::string_view vertical;
        std::string_view horizontal;
        std::string_view top_left;
        std::string_view bottom_left;
        std::string_view bottom_right;
        std::string_view dotted;
    };

    constexpr Glyphs primary_glyphs = {
        .vertical = "┃",
        .horizontal = "━",
        .top_left = "┏",
        .bottom_left = "┗",
        .bottom_right = "┛",
        .dotted = "┇",
    };

    constexpr Glyphs secondary_glyphs = {
        .vertical = "│",
        .horizontal = "─",
        .top_left = "┌",
        .bottom_left = "└",
        .bottom_right = "┘",
        .dotted = "┆",
    };

    const Glyphs &glyphs = (annotation.label->style == LabelStyle::Primary) ? primary_glyphs : secondary_glyphs;
    const fmt::color annotation_color = annotation_to_color(annotation.label->style, diagnostic);
    const usize total_lines = annotation.end.line - annotation.start.line + 1;

    fmt::print(
        "{:>{}} {}   {}\n",
        annotation.start.line,
        line_width,
        fmt::format(fmt::fg(fmt::color::cornflower_blue), "│"),
        m_source_manager.get_line(annotation.label->span.source_id, annotation.start.line));

    std::string horizontal_top;
    for (usize i = 0; i < annotation.start.column; ++i) {
        horizontal_top += glyphs.horizontal;
    }

    fmt::print(
        "{:>{}} {} {}\n",
        "",
        line_width,
        fmt::format(fmt::fg(fmt::color::cornflower_blue), "│"),
        fmt::format(fmt::fg(annotation_color), "{}{}{}", glyphs.top_left, horizontal_top, glyphs.bottom_right));

    if (total_lines == 3) {
        fmt::print(
            "{:>{}} {} {} {}\n",
            annotation.start.line + 1,
            line_width,
            fmt::format(fmt::fg(fmt::color::cornflower_blue), "│"),
            fmt::format(fmt::fg(annotation_color), "{}", glyphs.vertical),
            m_source_manager.get_line(annotation.label->span.source_id, annotation.start.line + 1));
    } else if (total_lines > 3) {
        fmt::print(
            "{:>{}} {} {}\n",
            "",
            line_width,
            fmt::format(fmt::fg(fmt::color::cornflower_blue), "┆"),
            fmt::format(fmt::fg(annotation_color), "{}", glyphs.dotted));
    }

    if (annotation.end.line != annotation.start.line) {
        fmt::print(
            "{:>{}} {} {} {}\n",
            annotation.end.line,
            line_width,
            fmt::format(fmt::fg(fmt::color::cornflower_blue), "│"),
            fmt::format(fmt::fg(annotation_color), "{}", glyphs.vertical),
            m_source_manager.get_line(annotation.label->span.source_id, annotation.end.line));
    }

    std::string horizontal_bottom = "";
    for (usize i = 0; i < annotation.end.column; ++i) {
        horizontal_bottom += glyphs.horizontal;
    }

    if (annotation.label->message.has_value()) {
        fmt::print(
            "{:>{}} {} {}",
            "",
            line_width,
            fmt::format(fmt::fg(fmt::color::cornflower_blue), "│"),
            fmt::format(
                fmt::fg(annotation_color),
                "{}{}{} {}",
                glyphs.bottom_left,
                horizontal_bottom,
                glyphs.bottom_right,
                annotation.label->message.value()));
    }

    fmt::print("\n");
}

void DiagnosticRenderer::render_footer(const Diagnostic &diagnostic, const usize line_width) const
{
    const bool has_note = diagnostic.note.has_value();
    const bool has_help = diagnostic.help.has_value();
    if (has_note || has_help) {
        if (has_note) {
            fmt::print(
                "{:>{}} {} {}: {}\n",
                "",
                line_width,
                fmt::format(fmt::fg(fmt::color::cornflower_blue), "{}", has_help ? "├" : "╰"),
                fmt::format(fmt::emphasis::bold, "note"),
                diagnostic.note.value());
        }

        if (has_help) {
            fmt::print(
                "{:>{}} {} {}: {}\n",
                "",
                line_width,
                fmt::format(fmt::fg(fmt::color::cornflower_blue), "╰"),
                fmt::format(fmt::emphasis::bold, "help"),
                diagnostic.help.value());
        }
    } else {
        fmt::print("{:>{}} {}\n", "", line_width, fmt::format(fmt::fg(fmt::color::cornflower_blue), "╰"));
    }

    fmt::print("\n");
}

} // namespace he::script
