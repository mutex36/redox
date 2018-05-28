#include "gltf_importer.h"

#define RDX_LOG_TAG "Importer"
#include "core/logging/log.h"

redox::GLTFImporter::GLTFImporter(const String& path) {
	io::File file(path, io::File::Mode::READ);
	auto buffer = file.read();

	cgltf_options options{};
	cgltf_result result = cgltf_parse(&options, buffer.data(), buffer.size(), &_data);
	if (result != cgltf_result_success)
		throw Exception("failed to load gltf file");
}

std::size_t redox::GLTFImporter::mesh_count() const {
	return _data.meshes_count;
}

void redox::GLTFImporter::import_mesh(std::size_t index, Buffer<graphics::MeshVertex>& vertices,
	Buffer<uint16_t>& indices) {

	if (index >= _data.meshes_count)
		throw Exception("mesh index not found");

	const auto& mesh = _data.meshes[index];

	for (std::size_t primIndex = 0; primIndex < mesh.primitives_count; primIndex++) {

		const auto& primitive = mesh.primitives[primIndex];

		for (std::size_t attrIndex = 0; attrIndex < primitive.attributes_count; attrIndex++) {
			const auto& attribute = primitive.attributes[attrIndex];

			const auto& bufferView = attribute.data->buffer_view;
			if (bufferView->type != cgltf_buffer_view_type_vertices)
				throw Exception("invalid buffer type");

			auto bufferUri = bufferView->buffer->uri;
			auto cachedBuffer = _buffers.get(bufferUri);
			if (!cachedBuffer) {
				io::File blobFile(R"(C:\Users\luis9\Desktop\redox\redox\resources\meshes\BoxTextured0.bin)",
					io::File::Mode::READ);

				_buffers.push(bufferUri, blobFile.read());
			}

			const auto& buffer = *cachedBuffer;

			auto offset = bufferView->offset + attribute.data->offset;
			auto stride = attribute.data->stride;
			auto size = attribute.data->count * stride;

			for (size_t index = offset; index < size; index +=stride) {

				switch (attribute.name) {

				case cgltf_attribute_type_position:
				{
					const auto& pos = reinterpret_cast<const math::Vec3f&>(buffer[index]);

				}


				break;

				}

			}
		}
	}

}
