#pragma once

#include <glm.hpp>
#include <gtx\transform.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include "..\Graphics\window.h"
#include <vector>

class Camera {
	private:
		glm::vec3 camera_position;
		glm::vec3 camera_view_direction;
		glm::vec3 camera_up;
		glm::vec3 camera_right;

		std::vector<float> map_bound = { -45.5f, -45.5f, 45.5f, 45.5f };
		std::vector<float> lake_bound_x = { -2.49f, -1.34f, 3.06f, 2.32f, -2.21f, -1.6f, 17.67f, 29.04f, 35.85f, 25.35f, 2.62f };
		std::vector<float> lake_bound_z = { -36.46f, -19.44f, -11.51f, 5.02f, 19.52f, 31.2f, 38.99f, 26.62f, -9.79f, -31.05f, -39.53f };
		std::vector<float> house_outside_bound_x = { -7.98f, -9.61f, -10.9f, -9.25f, -8.70f, -8.82f, -9.16f, -9.29f, -8.44f, -8.35f, -8.65f, -8.55f };
		std::vector<float> house_outside_bound_z = { 10.97f, 13.37f, 12.51f, 10.12f, 10.54f, 10.73f, 10.51f, 10.71f, 11.31f, 11.08f, 10.80f, 10.64f };
		std::vector<float> house_inside_bound_x = { -9.48f, -8.81f, -9.35f, -9.18f, -9.28f, -9.44f, -9.88f, -10.25f, -9.89f, -10.21f };
		std::vector<float> house_inside_bound_z = { 10.97f, 11.43f, 12.09f, 12.23f, 12.43f, 12.32f, 12.90f, 12.666f, 12.16f, 11.924f };
		std::vector<float> house_bound_enter = { this->house_outside_bound_x[6], this->house_outside_bound_z[6], this->house_outside_bound_x[8], this->house_outside_bound_z[8] };
		std::vector<float> house_bound_exit = { -9.14f, 11.11f, -8.92f, 11.41f };
		glm::vec3 pedestal_mountain = glm::vec3(-31.88f, 24.70f, 5.23f);
		glm::vec3 pedestal_lakeside = glm::vec3(42.09f, 1.58f, 1.44f);
		glm::vec3 bookshelf = glm::vec3(-9.74589f, 1.05275f, 12.7125f);
		glm::vec3 fireplace = glm::vec3(-9.11807f, 1.05275f, 11.6974f);

		float height_offset = 0.35f;

		//rotation - to be removed
		float rotationOx;
		float rotationOy;

	public:
		std::string location = "inside";
		
		int questline_progress = 0;
		bool got_key = false;
		bool got_doll = false;

		Camera();
		Camera(glm::vec3 camera_position);
		Camera(glm::vec3 camera_position, glm::vec3 camera_view_direction, glm::vec3 camera_up);
		~Camera();
		glm::mat4 get_view_matrix();
		glm::vec3 get_camera_position();
		glm::vec3 get_camera_view_direction();
		glm::vec3 get_camera_up();

		void set_camera_y(float y);

		void keyboard_move_front(float camera_speed);
		void keyboard_move_back(float camera_speed);
		void keyboard_move_left(float camera_speed);
		void keyboard_move_right(float camera_speed);
		void keyboard_move_up(float camera_speed);
		void keyboard_move_down(float camera_speed);

		void keyboard_interact();

		void rotate_ox(float angle);
		void rotate_oy(float angle); 

		void spawn();

		void at_edge_of_world();

		bool point_in_polygon(float x, float z, std::vector<float> bound_x, std::vector<float> bound_z);
		bool point_in_square(std::vector<float> coords);

		glm::vec3 check_pos(glm::vec3 new_camera_position);
};

