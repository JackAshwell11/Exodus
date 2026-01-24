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
        self.requires("sdl_image/2.8.8")
        self.requires("gtest/1.17.0")

    def configure(self: ConanFile) -> None:
        """Configure the Conan options for the dependencies."""
        self.options["sdl"].pulse = False
