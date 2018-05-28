#include "shader_factory.h"

redox::Resource<redox::graphics::Shader> redox::graphics::ShaderFactory::load_impl(
	const String& path, const Graphics& graphics) {

	io::File fstream(path, io::File::Mode::READ);
	auto buffer = fstream.read();

	return { construct_tag{}, std::move(buffer), graphics };
}
