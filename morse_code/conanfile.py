from conans import ConanFile, CMake
import traceback

# Just replace `MySimpleProject` with your name
class MySimpleProject(ConanFile):
    name = "MySimpleProject"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    requires = [        ]
    generators = "cmake"