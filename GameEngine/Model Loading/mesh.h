#pragma once
#include <glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "..\Shaders\shader.h"
#include "..\Camera\camera.h"

struct Vertex  {
	glm::vec3 pos;
	glm::vec3 normals;
	glm::vec2 textureCoords;

	Vertex() {}

	Vertex(float pos_x, float pos_y, float pos_z)
	{
		pos.x = pos_x;
		pos.y = pos_y;
		pos.z = pos_z;
	}

	Vertex(float pos_x, float pos_y, float pos_z, float norm_x, float norm_y, float norm_z)
	{
		pos.x = pos_x;
		pos.y = pos_y;
		pos.z = pos_z;

		normals.x = norm_x;
		normals.y = norm_y;
		normals.z = norm_z;
	}

	Vertex(float pos_x, float pos_y, float pos_z, float text_x, float text_y)
	{
		pos.x = pos_x;
		pos.y = pos_y;
		pos.z = pos_z;

		textureCoords.x = text_x;
		textureCoords.y = text_y;
	}

	Vertex(float pos_x, float pos_y, float pos_z, float norm_x, float norm_y, float norm_z, float text_x, float text_y)
	{
		pos.x = pos_x;
		pos.y = pos_y;
		pos.z = pos_z;

		normals.x = norm_x;
		normals.y = norm_y;
		normals.z = norm_z;

		textureCoords.x = text_x;
		textureCoords.y = text_y;
	}

	void print() {
		std::cout << this->pos.x << " " << this->pos.y << " " << this->pos.z << "\n";
	}
};

struct Texture  {
	unsigned int id = 0;
	std::string type;
};

class Mesh {
	public:
		std::vector<Vertex> vertices;
		std::vector<int> indices;
		std::vector<Texture> textures;
		std::map<float, float> heights;

		unsigned int vao = 0, vbo = 0, ibo = 0;

		Mesh();	
		Mesh(std::vector<Vertex> vertices, std::vector<int> indices, std::vector<Texture> textures);
		Mesh(std::vector<Vertex> vertices, std::vector<int> indices);
		~Mesh();

		void setTextures(std::vector<Texture> textures);
		void setup();
		void setup2();
		void draw(Shader shader);

		float get_barrycentric_coords(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos);
		float sign(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
		bool point_in_triangle(glm::vec3 pt, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
		float find_player_pos(glm::vec3 camera_pos);
};

