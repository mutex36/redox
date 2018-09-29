/*
redox
-----------
MIT License

Copyright (c) 2018 Luis von der Eltz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "model.h"

redox::graphics::Model::Model(mesh_buffer meshBuffer, material_buffer materialBuffer) : 
	_meshes(std::move(meshBuffer)),
	_materials(std::move(materialBuffer)) {
}

void redox::graphics::Model::upload() {
	for (auto& mat : _materials)
		mat->upload();

	for (auto& mesh : _meshes)
		mesh->upload();
}

const redox::graphics::Model::mesh_buffer& redox::graphics::Model::meshes() const {
	return _meshes;
}

const redox::graphics::Model::material_buffer& redox::graphics::Model::materials() const {
	return _materials;
}
