/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_script/source_manager.hpp"

#include "hyper_core/assertion.hpp"
#include "hyper_core/logger.hpp"

namespace he::script {

SourceId SourceManager::add_file(std::string path, std::string source)
{
    const SourceId id = m_files.size();
    std::vector<usize> line_starts = compute_line_starts(source);
    m_files.push_back(
        {
            .path = std::move(path),
            .source = std::move(source),
            .line_starts = std::move(line_starts),
        });
    return id;
}

std::string_view SourceManager::get_path(const SourceId source_id) const { return m_files[source_id].path; }

std::string_view SourceManager::get_source(const SourceId source_id) const { return m_files[source_id].source; }

std::string_view SourceManager::get_line(const SourceId source_id, const u32 line) const
{
    const File &file = m_files[source_id];
    HE_ASSERT(line > 0 && line < file.line_starts.size());

    const usize index = line - 1;
    const usize line_start = file.line_starts[index];
    const usize next_line_start = (line < file.line_starts.size()) ? file.line_starts[line] : file.source.size();
    return std::string_view(file.source).substr(line_start, next_line_start - 1 - line_start);
}

SourcePosition SourceManager::get_position(const SourceId source_id, const usize offset) const
{
    const File &file = m_files[source_id];
    HE_ASSERT(offset < file.source.size());

    u32 line = 1;
    for (usize i = 1; i < file.line_starts.size() && file.line_starts[i] <= offset; ++i) {
        ++line;
    }

    const u32 column = offset - file.line_starts[line - 1] + 1;

    return SourcePosition {
        .line = line,
        .column = column,
    };
}

std::vector<usize> SourceManager::compute_line_starts(const std::string_view source)
{
    std::vector<usize> starts;

    starts.push_back(0);
    for (usize i = 0; i < source.size(); ++i) {
        if (source[i] == '\n') {
            starts.push_back(i + 1);
        }
    }

    return starts;
}

} // namespace he::script
