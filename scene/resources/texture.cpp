/**************************************************************************/
/*  texture.cpp                                                           */
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

#include "texture.h"

#include "core/object/callable_mp.h"
#include "core/object/class_db.h"
#include "scene/resources/placeholder_textures.h"
#include "servers/rendering/rendering_server.h"

void ArrayTexture::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_texture_array", "texture_array"), &ArrayTexture::set_texture_array);
	ClassDB::bind_method(D_METHOD("get_texture_array"), &ArrayTexture::get_texture_array);
	ClassDB::bind_method(D_METHOD("set_layer", "layer"), &ArrayTexture::set_layer);
	ClassDB::bind_method(D_METHOD("get_layer"), &ArrayTexture::get_layer);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture_array", PROPERTY_HINT_RESOURCE_TYPE, "Texture2DArray,CompressedTexture2DArray,PlaceholderTexture2DArray,Texture2DArrayRD"), "set_texture_array", "get_texture_array");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "layer", PROPERTY_HINT_RANGE, "0,2047,1,or_greater"), "set_layer", "get_layer");
}

void ArrayTexture::set_texture_array(const Ref<TextureLayered> &p_array) {
	ERR_FAIL_COND(p_array.is_valid() && p_array->get_layered_type() != TextureLayered::LAYERED_TYPE_2D_ARRAY);
	if (texture_array == p_array) {
		return;
	}
	if (texture_array.is_valid()) {
		texture_array->disconnect_changed(callable_mp((Resource *)this, &ArrayTexture::emit_changed));
	}
	texture_array = p_array;
	if (texture_array.is_valid()) {
		texture_array->connect_changed(callable_mp((Resource *)this, &ArrayTexture::emit_changed));
	}
	if (texture.is_valid()) {
		RS::get_singleton()->texture_layer_view_update(texture, texture_array.is_valid() ? texture_array->get_rid() : RID(), layer);
	}
	emit_changed();
}

Ref<TextureLayered> ArrayTexture::get_texture_array() const {
	return texture_array;
}

void ArrayTexture::set_layer(int p_layer) {
	ERR_FAIL_COND(p_layer < 0);
	if (layer == p_layer) {
		return;
	}
	layer = p_layer;
	if (texture.is_valid()) {
		RS::get_singleton()->texture_layer_view_update(texture, texture_array.is_valid() ? texture_array->get_rid() : RID(), layer);
	}
	emit_changed();
}

int ArrayTexture::get_layer() const {
	return layer;
}

int ArrayTexture::get_width() const {
	return texture_array.is_valid() ? MAX(1, texture_array->get_width()) : 1;
}

int ArrayTexture::get_height() const {
	return texture_array.is_valid() ? MAX(1, texture_array->get_height()) : 1;
}

Image::Format ArrayTexture::get_format() const {
	return texture_array.is_valid() ? texture_array->get_format() : Image::FORMAT_MAX;
}

bool ArrayTexture::has_mipmaps() const {
	return texture_array.is_valid() && texture_array->has_mipmaps();
}

int ArrayTexture::get_mipmap_count() const {
	return texture_array.is_valid() ? texture_array->get_mipmap_count() : 0;
}

Ref<Image> ArrayTexture::get_image() const {
	if (texture_array.is_null() || layer >= texture_array->get_layers()) {
		return Ref<Image>();
	}
	return texture_array->get_layer_data(layer);
}

RID ArrayTexture::get_rid() const {
	if (texture.is_null()) {
		texture = RS::get_singleton()->texture_layer_view_create(texture_array.is_valid() ? texture_array->get_rid() : RID(), layer);
	}
	return texture;
}

ArrayTexture::~ArrayTexture() {
	if (texture.is_valid() && RS::get_singleton()) {
		RS::get_singleton()->free_rid(texture);
	}
}

Ref<Image> Texture2D::get_image() const {
	Ref<Image> ret;
	GDVIRTUAL_CALL(_get_image, ret);
	return ret;
}

Image::Format Texture2D::get_format() const {
	Image::Format ret = Image::FORMAT_MAX;
	GDVIRTUAL_CALL(_get_format, ret);
	return ret;
}

int Texture2D::get_mipmap_count() const {
	int ret = 0;
	GDVIRTUAL_CALL(_get_mipmap_count, ret);
	return ret;
}

bool Texture2D::has_mipmaps() const {
	bool ret = false;
	GDVIRTUAL_CALL(_has_mipmaps, ret);
	return ret;
}

int Texture2D::get_width() const {
	int ret = 0;
	GDVIRTUAL_CALL(_get_width, ret);
	return ret;
}

int Texture2D::get_height() const {
	int ret = 0;
	GDVIRTUAL_CALL(_get_height, ret);
	return ret;
}

Size2 Texture2D::get_size() const {
	return Size2(get_width(), get_height());
}

bool Texture2D::is_pixel_opaque(int p_x, int p_y) const {
	bool ret = true;
	GDVIRTUAL_CALL(_is_pixel_opaque, p_x, p_y, ret);
	return ret;
}

bool Texture2D::has_alpha() const {
	bool ret = true;
	GDVIRTUAL_CALL(_has_alpha, ret);
	return ret;
}

void Texture2D::draw(RID p_canvas_item, const Point2 &p_pos, const Color &p_modulate, bool p_transpose) const {
	if (GDVIRTUAL_CALL(_draw, p_canvas_item, p_pos, p_modulate, p_transpose)) {
		return;
	}
	RenderingServer::get_singleton()->canvas_item_add_texture_rect(p_canvas_item, Rect2(p_pos, get_size()), get_rid(), false, p_modulate, p_transpose);
}

void Texture2D::draw_rect(RID p_canvas_item, const Rect2 &p_rect, bool p_tile, const Color &p_modulate, bool p_transpose) const {
	if (GDVIRTUAL_CALL(_draw_rect, p_canvas_item, p_rect, p_tile, p_modulate, p_transpose)) {
		return;
	}
	RenderingServer::get_singleton()->canvas_item_add_texture_rect(p_canvas_item, p_rect, get_rid(), p_tile, p_modulate, p_transpose);
}

void Texture2D::draw_rect_region(RID p_canvas_item, const Rect2 &p_rect, const Rect2 &p_src_rect, const Color &p_modulate, bool p_transpose, bool p_clip_uv) const {
	if (GDVIRTUAL_CALL(_draw_rect_region, p_canvas_item, p_rect, p_src_rect, p_modulate, p_transpose, p_clip_uv)) {
		return;
	}
	RenderingServer::get_singleton()->canvas_item_add_texture_rect_region(p_canvas_item, p_rect, get_rid(), p_src_rect, p_modulate, p_transpose, p_clip_uv);
}

bool Texture2D::get_rect_region(const Rect2 &p_rect, const Rect2 &p_src_rect, Rect2 &r_rect, Rect2 &r_src_rect) const {
	r_rect = p_rect;
	r_src_rect = p_src_rect;
	return true;
}

Ref<Resource> Texture2D::create_placeholder() const {
	Ref<PlaceholderTexture2D> placeholder;
	placeholder.instantiate();
	placeholder->set_size(get_size());
	return placeholder;
}

void Texture2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_format"), &Texture2D::get_format);
	ClassDB::bind_method(D_METHOD("get_mipmap_count"), &Texture2D::get_mipmap_count);
	ClassDB::bind_method(D_METHOD("get_width"), &Texture2D::get_width);
	ClassDB::bind_method(D_METHOD("get_height"), &Texture2D::get_height);
	ClassDB::bind_method(D_METHOD("get_size"), &Texture2D::get_size);
	ClassDB::bind_method(D_METHOD("has_alpha"), &Texture2D::has_alpha);
	ClassDB::bind_method(D_METHOD("has_mipmaps"), &Texture2D::has_mipmaps);
	ClassDB::bind_method(D_METHOD("draw", "canvas_item", "position", "modulate", "transpose"), &Texture2D::draw, DEFVAL(Color(1, 1, 1)), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("draw_rect", "canvas_item", "rect", "tile", "modulate", "transpose"), &Texture2D::draw_rect, DEFVAL(Color(1, 1, 1)), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("draw_rect_region", "canvas_item", "rect", "src_rect", "modulate", "transpose", "clip_uv"), &Texture2D::draw_rect_region, DEFVAL(Color(1, 1, 1)), DEFVAL(false), DEFVAL(true));
	ClassDB::bind_method(D_METHOD("get_image"), &Texture2D::get_image);
	ClassDB::bind_method(D_METHOD("create_placeholder"), &Texture2D::create_placeholder);

	ADD_GROUP("", "");

	GDVIRTUAL_BIND(_get_image);
	GDVIRTUAL_BIND(_get_format);
	GDVIRTUAL_BIND(_get_mipmap_count);
	GDVIRTUAL_BIND(_get_width);
	GDVIRTUAL_BIND(_get_height);
	GDVIRTUAL_BIND(_is_pixel_opaque, "x", "y");
	GDVIRTUAL_BIND(_has_alpha);
	GDVIRTUAL_BIND(_has_mipmaps);

	GDVIRTUAL_BIND(_draw, "to_canvas_item", "pos", "modulate", "transpose")
	GDVIRTUAL_BIND(_draw_rect, "to_canvas_item", "rect", "tile", "modulate", "transpose")
	GDVIRTUAL_BIND(_draw_rect_region, "to_canvas_item", "rect", "src_rect", "modulate", "transpose", "clip_uv");
}

Texture2D::Texture2D() {
}

TypedArray<Image> Texture3D::_get_datai() const {
	Vector<Ref<Image>> data = get_data();

	TypedArray<Image> ret;
	ret.resize(data.size());
	for (int i = 0; i < data.size(); i++) {
		ret[i] = data[i];
	}
	return ret;
}

Image::Format Texture3D::get_format() const {
	Image::Format ret = Image::FORMAT_MAX;
	GDVIRTUAL_CALL(_get_format, ret);
	return ret;
}

int Texture3D::get_width() const {
	int ret = 0;
	GDVIRTUAL_CALL(_get_width, ret);
	return ret;
}

int Texture3D::get_height() const {
	int ret = 0;
	GDVIRTUAL_CALL(_get_height, ret);
	return ret;
}

int Texture3D::get_depth() const {
	int ret = 0;
	GDVIRTUAL_CALL(_get_depth, ret);
	return ret;
}

bool Texture3D::has_mipmaps() const {
	bool ret = false;
	GDVIRTUAL_CALL(_has_mipmaps, ret);
	return ret;
}

Vector<Ref<Image>> Texture3D::get_data() const {
	TypedArray<Image> ret;
	GDVIRTUAL_CALL(_get_data, ret);
	Vector<Ref<Image>> data;
	data.resize(ret.size());
	for (int i = 0; i < data.size(); i++) {
		data.write[i] = ret[i];
	}
	return data;
}

void Texture3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_format"), &Texture3D::get_format);
	ClassDB::bind_method(D_METHOD("get_width"), &Texture3D::get_width);
	ClassDB::bind_method(D_METHOD("get_height"), &Texture3D::get_height);
	ClassDB::bind_method(D_METHOD("get_depth"), &Texture3D::get_depth);
	ClassDB::bind_method(D_METHOD("has_mipmaps"), &Texture3D::has_mipmaps);
	ClassDB::bind_method(D_METHOD("get_data"), &Texture3D::_get_datai);
	ClassDB::bind_method(D_METHOD("create_placeholder"), &Texture3D::create_placeholder);

	GDVIRTUAL_BIND(_get_format);
	GDVIRTUAL_BIND(_get_width);
	GDVIRTUAL_BIND(_get_height);
	GDVIRTUAL_BIND(_get_depth);
	GDVIRTUAL_BIND(_has_mipmaps);
	GDVIRTUAL_BIND(_get_data);
}

Ref<Resource> Texture3D::create_placeholder() const {
	Ref<PlaceholderTexture3D> placeholder;
	placeholder.instantiate();
	placeholder->set_size(Vector3i(get_width(), get_height(), get_depth()));
	return placeholder;
}

Image::Format TextureLayered::get_format() const {
	Image::Format ret = Image::FORMAT_MAX;
	GDVIRTUAL_CALL(_get_format, ret);
	return ret;
}

TextureLayered::LayeredType TextureLayered::get_layered_type() const {
	uint32_t ret = LAYERED_TYPE_2D_ARRAY;
	GDVIRTUAL_CALL(_get_layered_type, ret);
	return (LayeredType)ret;
}

int TextureLayered::get_width() const {
	int ret = 0;
	GDVIRTUAL_CALL(_get_width, ret);
	return ret;
}

int TextureLayered::get_height() const {
	int ret = 0;
	GDVIRTUAL_CALL(_get_height, ret);
	return ret;
}

int TextureLayered::get_layers() const {
	int ret = 0;
	GDVIRTUAL_CALL(_get_layers, ret);
	return ret;
}

bool TextureLayered::has_mipmaps() const {
	bool ret = false;
	GDVIRTUAL_CALL(_has_mipmaps, ret);
	return ret;
}

int TextureLayered::get_mipmap_count() const {
	return has_mipmaps() ? Image::get_image_required_mipmaps(get_width(), get_height(), get_format()) : 0;
}

Ref<Image> TextureLayered::get_layer_data(int p_layer) const {
	Ref<Image> ret;
	GDVIRTUAL_CALL(_get_layer_data, p_layer, ret);
	return ret;
}

void TextureLayered::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_format"), &TextureLayered::get_format);
	ClassDB::bind_method(D_METHOD("get_layered_type"), &TextureLayered::get_layered_type);
	ClassDB::bind_method(D_METHOD("get_width"), &TextureLayered::get_width);
	ClassDB::bind_method(D_METHOD("get_height"), &TextureLayered::get_height);
	ClassDB::bind_method(D_METHOD("get_layers"), &TextureLayered::get_layers);
	ClassDB::bind_method(D_METHOD("has_mipmaps"), &TextureLayered::has_mipmaps);
	ClassDB::bind_method(D_METHOD("get_layer_data", "layer"), &TextureLayered::get_layer_data);

	BIND_ENUM_CONSTANT(LAYERED_TYPE_2D_ARRAY);
	BIND_ENUM_CONSTANT(LAYERED_TYPE_CUBEMAP);
	BIND_ENUM_CONSTANT(LAYERED_TYPE_CUBEMAP_ARRAY);

	GDVIRTUAL_BIND(_get_format);
	GDVIRTUAL_BIND(_get_layered_type);
	GDVIRTUAL_BIND(_get_width);
	GDVIRTUAL_BIND(_get_height);
	GDVIRTUAL_BIND(_get_layers);
	GDVIRTUAL_BIND(_has_mipmaps);
	GDVIRTUAL_BIND(_get_layer_data, "layer_index");
}
