# SPDX-FileCopyrightText: 2025 NorthSec
#
# SPDX-License-Identifier: MIT

env = DefaultEnvironment()

# Apply the offset_address from platformio.ini, this should work by default but
# there seems to be a bug somewhere.
app_offset = env.BoardConfig().get("upload.offset_address", "0x10000")
env.Replace(ESP32_APP_OFFSET="%s" % app_offset)
