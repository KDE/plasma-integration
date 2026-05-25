# SPDX-FileCopyrightText: 2026 Nicolas Fella <nicolas.fella@gmx.de>
# SPDX-License-Identifier: BSD-2-Clause

find_file(noto_font share/fonts/google-noto/NotoSans-Regular.ttf)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FontNotoSans
    FOUND_VAR
        FontNotoSans_FOUND
    REQUIRED_VARS
        noto_font
)

unset(noto_font)
