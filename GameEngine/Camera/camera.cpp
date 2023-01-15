#include "camera.h"

Camera::Camera(glm::vec3 cameraPosition) {
	this->camera_position = cameraPosition;
	this->camera_view_direction = glm::vec3(0.0f, 0.0f, -1.0f);
	this->camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->camera_right = glm::cross(camera_view_direction, camera_up);
	this->rotationOx = 0.0f;
	this->rotationOy = -90.0f;
}
Camera::Camera() {
	this ->camera_position = glm::vec3(0.0f, 0.0f, 0.0f);
	this ->camera_view_direction = glm::vec3(0.0f, 0.0f, -1.0f);
	this ->camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->camera_right = glm::cross(camera_view_direction, camera_up);
	this->rotationOx = 0.0f;
	this->rotationOy = -90.0f;
}
Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraViewDirection, glm::vec3 cameraUp) {
	this->camera_position = cameraPosition;
	this->camera_view_direction = cameraViewDirection;
	this->camera_up = cameraUp;
	this->camera_right = glm::cross(cameraViewDirection, cameraUp);
}
Camera::~Camera()
{
}

void Camera::set_camera_y(float y) {
	this->camera_position.y = y + this->height_offset;
}

void Camera::keyboard_move_front(float camera_speed) {
	glm::vec3 new_camera_pos = camera_position + camera_view_direction * camera_speed;
	camera_position = this->check_pos(new_camera_pos);
}

void Camera::keyboard_move_back(float camera_speed) {
	glm::vec3 new_camera_pos = camera_position - camera_view_direction * camera_speed;
	camera_position = this->check_pos(new_camera_pos);
}

void Camera::keyboard_move_left(float camera_speed) {
	glm::vec3 new_camera_pos = camera_position - glm::normalize(glm::cross(camera_view_direction, camera_up)) * camera_speed;
	camera_position = this->check_pos(new_camera_pos);
}

void Camera::keyboard_move_right(float camera_speed) {
	glm::vec3 new_camera_pos = camera_position + glm::normalize(glm::cross(camera_view_direction, camera_up)) * camera_speed;
	camera_position = this->check_pos(new_camera_pos);
}

void Camera::keyboard_move_up(float camera_speed) {
	camera_position += camera_up * camera_speed;
}

void Camera::keyboard_move_down(float camera_speed) {
	camera_position -= camera_up * camera_speed;
}

void Camera::rotate_ox(float angle) {	
	camera_view_direction = glm::normalize(glm::vec3((glm::rotate(glm::mat4(1.0f), angle, camera_right) * glm::vec4(camera_view_direction, 1))));
	camera_up = glm::normalize(glm::cross(camera_right, camera_view_direction));
	camera_right = glm::cross(camera_view_direction, camera_up);
}

void Camera::rotate_oy (float angle) {
	camera_view_direction = glm::normalize(glm::vec3((glm::rotate(glm::mat4(1.0f), angle, camera_up) * glm::vec4(camera_view_direction, 1))));
	camera_right = glm::normalize(glm::vec3((glm::rotate(glm::mat4(1.0f), angle, camera_up) * glm::vec4(camera_right, 1))));
	camera_up = glm::normalize(glm::cross(camera_right, camera_view_direction));
}

bool Camera::point_in_polygon(float x, float z, std::vector<float> bound_x, std::vector<float> bound_z) {
	int poly_corners = bound_x.size();
	int i, j = poly_corners - 1;
	bool oddNodes = false;

	for (i = 0; i < poly_corners; i++) {
		if ((bound_z[i] < z && bound_z[j] >= z
			|| bound_z[j] < z && bound_z[i] >= z)
			&& (bound_x[i] <= x || bound_x[j] <= x)) {
			oddNodes ^= (bound_x[i] + (z - bound_z[i]) / (bound_z[j] - bound_z[i]) * (bound_x[j] - bound_x[i]) < x);
		}
		j = i;
	}

	return oddNodes;
}

bool Camera::point_in_square(std::vector<float> coords) {
	if (this->camera_position.x > coords[0] && this->camera_position.x < coords[2] &&
		this->camera_position.z > coords[1] && this->camera_position.z < coords[3]) {
		return true;
	}
	return false;
}

void Camera::keyboard_interact() {
	if (this->location == "outside") {
		if (this->point_in_square(this->house_bound_enter)) {
			this->location = "inside";
			this->camera_position.x = -9.07f;
			this->camera_position.z = 11.35f;
			std::cout << "DEBUG: entered house\n";
		}
		if (glm::distance(this->camera_position, this->pedestal_mountain) < 1.5f) {
			if (this->questline_progress > 0 && this->questline_progress < 3 && !this->got_key) {
				this->questline_progress += 1;
				this->got_key = true;

				std::cout << "DEBUG: questline advanced\n";
			}
			std::cout << "DEBUG: interaction - mountain pedestal\n";
		}
		if (glm::distance(this->camera_position, this->pedestal_lakeside) < 1.5f) {
			if (this->questline_progress > 0 && this->questline_progress < 3 && !this->got_doll) {
				this->questline_progress += 1;
				this->got_doll = true;

				std::cout << "DEBUG: questline advanced\n";
			}
			std::cout << "DEBUG: interaction - lakeside pedestal\n";
		}
	}
	else {
		if (this->point_in_square(this->house_bound_exit)) {
			this->location = "outside";
			this->camera_position.x = -8.77f;
			this->camera_position.z = 10.91f;
			std::cout << "DEBUG: exited house\n";
		}
		if (glm::distance(this->camera_position, this->bookshelf) < 0.3f) {
			if (this->questline_progress == 0) {
				this->questline_progress = 1;
				std::cout << "DEBUG: questline advanced\n";
			}
			std::cout << "DEBUG: interaction - bookshelf\n";
		}
		if (glm::distance(this->camera_position, this->fireplace) < 0.3f) {
			if (this->questline_progress == 3) {
				this->questline_progress = 4;
				std::cout << "DEBUG: questline advanced\n";
			}
			std::cout << "DEBUG: interaction - fireplace\n";
		}
	}
}

glm::vec3 Camera::check_pos(glm::vec3 new_camera_pos) {
	if (this->location == "outside") {
		if (!this->point_in_square(this->map_bound)) {
			return this->camera_position;
		}
		if (this->point_in_polygon(new_camera_pos.x, new_camera_pos.z, this->lake_bound_x, this->lake_bound_z)) {
			return this->camera_position;
		}
		if (this->point_in_polygon(new_camera_pos.x, new_camera_pos.z, this->house_outside_bound_x, this->house_outside_bound_z)) {
			return this->camera_position;
		}
	}
	else {
		if (!this->point_in_polygon(new_camera_pos.x, new_camera_pos.z, this->house_inside_bound_x, this->house_inside_bound_z)) {
			return this->camera_position;
		}
	}
	return new_camera_pos;
}

void Camera::spawn() {
	this->location = "inside";

	this->camera_position.x = -9.9736f;
	this->camera_position.y = 1.05275f;
	this->camera_position.z = 12.6552f;
	this->rotate_oy(-35.0f);

	this->got_key = false;
	this->got_doll = false;
	this->questline_progress = 0;
}

void Camera::at_edge_of_world() {
	if (std::abs(this->camera_position.x) > 42.0f || std::abs(this->camera_position.z) > 42.0f) {
		this->spawn();
	}
}

glm::mat4 Camera::get_view_matrix() {
	return glm::lookAt(camera_position, camera_position + camera_view_direction, camera_up);
}

glm::vec3 Camera::get_camera_position() {
	return camera_position;
}

glm::vec3 Camera::get_camera_view_direction() {
	return camera_view_direction;
}

glm::vec3 Camera::get_camera_up() {
	return camera_up;
}


