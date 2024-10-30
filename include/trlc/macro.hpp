#pragma once

#include "macro/append_args.hpp"            // IWYU pragma: export
#include "macro/apply_args.hpp"             // IWYU pragma: export
#include "macro/apply_args_with_index.hpp"  // IWYU pragma: export
#include "macro/count_args.hpp"             // IWYU pragma: export
#include "macro/expand_args.hpp"            // IWYU pragma: export
#include "macro/expand_args_with_index.hpp" // IWYU pragma: export

namespace trlc
{
#define TRLC_STRINGIFY(x) #x
} // namespace trlc
