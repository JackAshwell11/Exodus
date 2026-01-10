"""Manages the Conan configuration for the application."""

from __future__ import annotations

# Builtin
from typing import TYPE_CHECKING

# Pip
from conan import ConanFile

if TYPE_CHECKING:
    from typing import ClassVar


class Exodus(ConanFile):  # type: ignore[misc]
    """The Conan configuration for the application."""

    # The metadata for the package
    generators: ClassVar[list[str]] = ["CMakeDeps"]
    settings: ClassVar[list[str]] = ["build_type"]

    def requirements(self: ConanFile) -> None:
        """Add the required dependencies to the Conan configuration."""
        self.requires("sdl/2.32.10")

