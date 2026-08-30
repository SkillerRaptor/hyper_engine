/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <span>

#include <fmt/color.h>

#include <hyper_core/types.hpp>

#include "hyper_script/source_manager.hpp"

namespace he::script {

struct Span {
    SourceId source_id = 0;
    usize start_offset = 0;
    usize end_offset = 0;
};

enum class Severity : u8 {
    Note,
    Help,
    Warning,
    Error,
    Fatal,
};

enum class LabelStyle : u8 {
    Primary,
    Secondary
};

struct Label {
    Span span;
    std::optional<std::string> message;
    LabelStyle style = LabelStyle::Primary;
};

struct Diagnostic {
    Severity severity = Severity::Error;
    std::string message;
    std::vector<Label> labels;
    std::optional<std::string> note;
    std::optional<std::string> help;

    static Diagnostic new_note(std::string msg);
    static Diagnostic new_help(std::string msg);
    static Diagnostic new_warning(std::string msg);
    static Diagnostic new_error(std::string msg);
    static Diagnostic new_fatal(std::string msg);

    Diagnostic &with_label(const Span &, std::optional<std::string> msg, LabelStyle style = LabelStyle::Primary);
    Diagnostic &with_note(std::string new_note);
    Diagnostic &with_help(std::string new_help);
};

class DiagnosticEngine {
public:
    void emit(const Diagnostic &);

    std::span<const Diagnostic> diagnostics() const { return m_diagnostics; }

    u32 warning_count() const { return m_warning_count; }
    bool has_warnings() const { return m_warning_count > 0; }

    u32 error_count() const { return m_error_count; }
    bool has_errors() const { return m_error_count > 0; }

private:
    std::vector<Diagnostic> m_diagnostics;
    u32 m_warning_count = 0;
    u32 m_error_count = 0;
};

class DiagnosticRenderer {
private:
    struct SingleLineAnnotation {
        const Label *label = nullptr;
        u32 line = 0;
        u32 start_column = 0;
        u32 end_column = 0;
    };

    struct MultiLineAnnotation {
        const Label *label = nullptr;
        SourcePosition start;
        SourcePosition end;
    };

    struct RenderBlock {
        SourceId source_id = 0;
        u32 first_line = 0;
        u32 last_line = 0;
        std::vector<SingleLineAnnotation> single_line_annotations;
        std::optional<MultiLineAnnotation> multi_line_annotation = std::nullopt;
    };

public:
    explicit DiagnosticRenderer(const SourceManager &source_manager)
        : m_source_manager(source_manager)
    {
    }

    void render(const Diagnostic &) const;
    void render_all(std::span<const Diagnostic>) const;

private:
    static std::string_view severity_to_string(Severity);
    static fmt::color severity_to_color(Severity);
    static fmt::color annotation_to_color(LabelStyle, const Diagnostic &);

    static const Label &find_first_primary_label(const Diagnostic &);
    std::vector<RenderBlock> build_render_blocks(const Diagnostic &) const;
    static u32 determine_line_width(u32 primary_line, std::span<const RenderBlock> render_blocks);

    void render_header(
        const Diagnostic &,
        const Label &first_primary_label,
        const SourcePosition &first_primary_position,
        usize line_width) const;
    void render_blocks(const Diagnostic &, std::span<const RenderBlock> render_blocks, usize line_width) const;

    void render_single_line_block(const Diagnostic &, const RenderBlock &, usize line_width) const;
    void render_multi_line_block(const Diagnostic &, const MultiLineAnnotation &, usize line_width) const;
    void render_footer(const Diagnostic &, usize line_width) const;

private:
    const SourceManager &m_source_manager;
};

} // namespace he::script
