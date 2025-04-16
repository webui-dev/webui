from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain

class WebuiConan(ConanFile):
    name = "webui"
    version = "2.5.0-beta.4"
    license = "MIT"
    url = "https://github.com/webui-dev/webui"
    homepage = "https://webui.me"
    description = "Use any web browser or WebView as GUI, with your preferred language in the backend and modern web technologies in the frontend, all in a lightweight portable library."

    settings = "os", "compiler", "build_type", "arch"
    options = {"tls": [True, False], "shared": [True, False]}
    default_options = {"tls": False, "shared": False}

    generators = "CMakeDeps"

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["WEBUI_USE_TLS"] = self.options.tls
        tc.variables["BUILD_SHARED_LIBS"] = self.options.shared
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def requirements(self):
        self.tool_requires("cmake/[>=3.18.0]")

        if self.options.tls:
            self.requires("openssl/[>3.0.0]")