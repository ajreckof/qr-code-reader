#pragma once

#include "godot_cpp/classes/control.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/classes/camera_texture.hpp"
#include "godot_cpp/classes/camera_feed.hpp"
#include "godot_cpp/classes/shader_material.hpp"
#include "godot_cpp/classes/aspect_ratio_container.hpp"
#include "godot_cpp/classes/color_rect.hpp"
#include "godot_cpp/classes/image.hpp"
#include "thirdparty/quirc/lib/quirc.h"

using namespace godot;

class QRCodeReader : public Control {
	GDCLASS(QRCodeReader, Control)

private:
	AspectRatioContainer *aspect_ratio_container;
	ColorRect *color_rect;
	Ref<CameraFeed> camera_feed;
	Ref<ShaderMaterial> shader_material;
	int feed_idx;
	Ref<CameraTexture> camera_y_texture;
	Ref<CameraTexture> camera_CbCr_texture;
	struct quirc *qr_decoder;
	PackedStringArray detected_codes;

protected:
	static void _bind_methods();
	
	public:
	QRCodeReader();
	~QRCodeReader() override;
	
	void _ready() override;
	void _process(double delta) override;
	void set_feed_idx(int p_idx);
	int get_feed_idx() const;
	
	PackedStringArray detect_qr_codes();
	PackedStringArray get_detected_codes() const;
};
