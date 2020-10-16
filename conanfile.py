from conans import ConanFile, CMake
import traceback

# Just replace `MySimpleProject` with your name
class MySimpleProject(ConanFile):
    name = "MySimpleProject"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    requires = [
        ("boost/1.72.0"),
        ("gtest/1.10.0")
        ]
    generators = "cmake"