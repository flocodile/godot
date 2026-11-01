/**************************************************************************/
/*  test_array_texture.cpp                                                */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "tests/test_macros.h"

TEST_FORCE_LINK(test_array_texture)

#include "scene/resources/image_texture.h"
#include "scene/resources/texture.h"

namespace TestArrayTexture {

TEST_CASE("[ArrayTexture] Source and layer properties") {
	Ref<Image> image = Image::create_empty(8, 8, false, Image::FORMAT_RGBA8);
	REQUIRE(image->generate_mipmaps() == OK);
	Vector<Ref<Image>> images;
	images.push_back(image);
	images.push_back(image);
	Ref<Texture2DArray> array;
	array.instantiate();
	REQUIRE(array->create_from_images(images) == OK);
	Ref<ArrayTexture> view;
	view.instantiate();
	view->set_texture_array(array);
	view->set_layer(1);
	CHECK(view->get_texture_array() == array);
	CHECK(view->get_layer() == 1);
	CHECK(view->get_width() == 8);
	CHECK(view->get_height() == 8);
	CHECK(view->has_mipmaps());
	CHECK(view->get_mipmap_count() == image->get_mipmap_count());
	view->set_layer(5);
	CHECK(view->get_image().is_null());
	ERR_PRINT_OFF;
	view->set_layer(-1);
	Ref<Cubemap> cubemap;
	cubemap.instantiate();
	view->set_texture_array(cubemap);
	ERR_PRINT_ON;
	CHECK(view->get_layer() == 5);
	CHECK(view->get_texture_array() == array);
}

} // namespace TestArrayTexture
