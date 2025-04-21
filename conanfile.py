from conan import ConanFile
from conan.tools.cmake import cmake_layout


class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("protobuf/5.27.0")
        self.requires("boost/1.88.0")


    def build_requirements(self):
        self.tool_requires("cmake/3.31.8")
        self.tool_requires("protobuf/5.27.0")


    def layout(self):
        cmake_layout(self)
