/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
#include <vector>

#include <hyper_core/types.hpp>

namespace he::script {

// TODO: Make this type safe
using SourceId = u32;

struct SourcePosition {
    u32 line = 0;
    u32 column = 0;
};

class SourceManager {
private:
    struct File {
        std::string path;
        std::string source;
        std::vector<usize> line_starts;
    };

public:
    SourceId add_file(std::string path, const std::string &source);

    std::string_view get_path(SourceId) const;
    std::string_view get_source(SourceId) const;

    std::string_view get_line(SourceId, u32 line) const;
    SourcePosition get_position(SourceId, usize offset) const;

private:
    static std::vector<usize> compute_line_starts(std::string_view source);

private:
    std::vector<File> m_files;
};

} // namespace he::script
