#include "qr_code_reader.h"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/camera_server.hpp"
#include "godot_cpp/classes/resource_loader.hpp"

QRCodeReader::QRCodeReader() {
	aspect_ratio_container = nullptr;
}

QRCodeReader::~QRCodeReader() {
	if (camera_feed.is_valid()) {
		// Deactivate previous feed
		if (camera_feed->is_active()) {
			camera_feed->set_active(false);
		}
	}
}

void QRCodeReader::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_feed_idx", "idx"), &QRCodeReader::set_feed_idx);
	ClassDB::bind_method(D_METHOD("get_feed_idx"), &QRCodeReader::get_feed_idx);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "feed_idx"), "set_feed_idx", "get_feed_idx");
}

void QRCodeReader::_ready() {
	if(Engine::get_singleton()->is_editor_hint()) {
		return;
	}


	// Create TextureRect to display camera feed
	aspect_ratio_container = memnew(AspectRatioContainer);
	add_child(aspect_ratio_container);
	aspect_ratio_container->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	aspect_ratio_container->set_stretch_mode(AspectRatioContainer::STRETCH_FIT);

	color_rect = memnew(ColorRect);
	aspect_ratio_container->add_child(color_rect);

	shader_material.instantiate();
	color_rect->set_material(shader_material);
	shader_material->set_shader(ResourceLoader::get_singleton()->load("res://addons/qr_code_reader/camera.gdshader"));

	camera_y_texture.instantiate();
	camera_y_texture->set_which_feed(CameraServer::FEED_Y_IMAGE);
	shader_material->set_shader_parameter("camera_y", camera_y_texture);

	camera_CbCr_texture.instantiate();
	camera_CbCr_texture->set_which_feed(CameraServer::FEED_CBCR_IMAGE);
	shader_material->set_shader_parameter("camera_CbCr", camera_CbCr_texture);
	set_feed_idx(0);
	
}

void QRCodeReader::_process(double delta) {
	if(Engine::get_singleton()->is_editor_hint()) {
		return;
	}
	if (!camera_y_texture.is_valid() || !camera_CbCr_texture.is_valid()) {
		return;
	}
	Vector2 size = camera_y_texture->get_size();
	if (size.x > 0 && size.y > 0) {
		aspect_ratio_container->set_ratio(size.x / size.y);
		color_rect->set_visible(true);
	} else {
		color_rect->set_visible(false);
	}
}

void QRCodeReader::set_feed_idx(int p_idx) {
	feed_idx = p_idx;

	if (Engine::get_singleton()->is_editor_hint()) {
		return;
	}
	CameraServer *camera_server = CameraServer::get_singleton();
	if (!camera_server) {
		UtilityFunctions::print("CameraServer not available");
		return;
	}
	if(!camera_server->is_monitoring_feeds()) {
		camera_server->set_monitoring_feeds(true);
	}
	// Get the number of available feeds
	int feed_count = camera_server->get_feed_count();
	if (feed_count == 0) {
		UtilityFunctions::print("No camera feeds available");
		return;
	}
	while (feed_idx > feed_count)
	{
		feed_idx -= feed_count;
	}
	
	if (camera_feed.is_valid()) {
		// Deactivate previous feed
		if (camera_feed->is_active()) {
			camera_feed->set_active(false);
		}
	}
	// Get the first camera feed
	camera_feed = camera_server->get_feed(feed_idx);
	
	// Activate the camera feed
	if (!camera_feed->is_active()) {
		camera_feed->set_active(true);
	}
	camera_y_texture = shader_material->get_shader_parameter("camera_y");;
	camera_CbCr_texture = shader_material->get_shader_parameter("camera_CbCr");

	camera_y_texture->set_camera_feed_id(camera_feed->get_id());
	camera_CbCr_texture->set_camera_feed_id(camera_feed->get_id());

	shader_material->set_shader_parameter("camera_y", camera_y_texture);
	shader_material->set_shader_parameter("camera_CbCr", camera_CbCr_texture);
}

int QRCodeReader::get_feed_idx() const {
	return feed_idx;
}


