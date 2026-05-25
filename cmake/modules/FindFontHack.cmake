# SPDX-FileCopyrightText: 2026 Nicolas Fella <nicolas.fella@gmx.de>
# SPDX-License-Identifier: BSD-2-Clause

find_file(hack_font share/fonts/source-foundry-hack-fonts/Hack-Regular.ttf)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FontHack
    FOUND_VAR
        FontHack_FOUND
    REQUIRED_VARS
        hack_font
)

unset(hack_font)
