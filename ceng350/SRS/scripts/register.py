#!/usr/bin/env python3
"""Remove the "UNREGISTERED" text from the SVG file.
Takes the path to the SVG file as the argument.
Produces a new file with the same name and "-registered.svg" extension."""

import re
from sys import argv
from pathlib import Path

file_path = Path(argv[1])

with open(file_path, "r", encoding="UTF-8") as file:
    svg = file.read()

REGEX = "<text.*UNREGISTERED</text>"
with open(
    file_path.with_suffix("").as_posix() + "-registered.svg", "w", encoding="UTF-8"
) as file:
    file.write(re.sub(REGEX, "", svg))
