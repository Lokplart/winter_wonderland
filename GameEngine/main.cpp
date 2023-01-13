#include "Graphics\window.h"
#include "Camera\camera.h"
#include "Shaders\shader.h"
#include "Model Loading\mesh.h"
#include "Model Loading\texture.h"
#include "Model Loading\meshLoaderObj.h"
#include "map"

void processKeyboardInput ();
float get_closest_vert(Mesh mesh);

std::ostream& operator<< (std::ostream& out, const glm::vec3& vec) {
	out << "[" << vec.x << " " << vec.y << " " << vec.z << "]\n";
	return out;
}

float delta_time = 0.0f;	// time between current frame and last frame
float last_frame = 0.0f;

float inteact_time = 0.0f;

Window window("Game Engine", 800, 800);
Camera camera;

glm::vec3 lightColor = glm::vec3(1.0f);
glm::vec3 lightPos = glm::vec3(-180.0f, 100.0f, -200.0f);

std::map<std::string, std::vector<Texture>> textures;

void add_textures(std::string name, GLuint id) {
	std::vector<Texture> new_texture;
	new_texture.push_back(Texture());
	new_texture[0].id = id;
	new_texture[0].type = "texture_diffuse";

	textures.insert(std::pair<std::string, std::vector<Texture>>(name, new_texture));
}

int main()
{
	glClearColor(0.2f, 0.8f, 1.0f, 1.0f);

	//building and compiling shader program
	Shader shader("Shaders/vertex_shader.glsl", "Shaders/fragment_shader.glsl");
	Shader sunShader("Shaders/sun_vertex_shader.glsl", "Shaders/sun_fragment_shader.glsl");

	//Textures
	GLuint tex_house = loadBMP("Resources/Textures/house.bmp", 'b');
	GLuint tex_snow = loadBMP("Resources/Textures/snow.bmp", 'r');
	GLuint tex_water = loadBMP("Resources/Textures/water.bmp", 'r');
	GLuint tex_trees = loadBMP("Resources/Textures/tree.bmp", 'b');
	GLuint tex_pedesal = loadBMP("Resources/Textures/pedestal.bmp", 'b');
	
	GLuint tex_bed = loadBMP("Resources/Textures/bed.bmp", 'b');
	GLuint tex_rug = loadBMP("Resources/Textures/rug.bmp", 'b');
	GLuint tex_books = loadBMP("Resources/Textures/books.bmp", 'b');
	GLuint tex_bookshelf = loadBMP("Resources/Textures/bookshelf.bmp", 'b');
	GLuint tex_fire_base = loadBMP("Resources/Textures/fireplace_base.bmp", 'b');
	GLuint tex_fire_wood = loadBMP("Resources/Textures/fireplace_wood.bmp", 'b');



	glEnable(GL_DEPTH_TEST);

	add_textures("house", tex_house);
	add_textures("snow", tex_snow);
	add_textures("water", tex_water);
	add_textures("trees", tex_trees);
	add_textures("pedestal", tex_pedesal);

	add_textures("bed", tex_bed);
	add_textures("rug", tex_rug);
	add_textures("books", tex_books);
	add_textures("bookshelf", tex_bookshelf);
	add_textures("fireplace", tex_fire_base);

	MeshLoaderObj loader;
	Mesh sun = loader.loadObj("Resources/Models/sphere.obj");
	Mesh ground = loader.loadObj("Resources/Models/ground.obj", textures["snow"]);
	Mesh water = loader.loadObj("Resources/Models/water.obj", textures["water"]);
	Mesh trees = loader.loadObj("Resources/Models/tree.obj", textures["trees"]);
	Mesh house = loader.loadObj("Resources/Models/house.obj", textures["house"]);
	Mesh pedestal = loader.loadObj("Resources/Models/pedestal.obj", textures["pedestal"]);
	
	Mesh bed = loader.loadObj("Resources/Models/bed.obj", textures["bed"]);
	Mesh rug = loader.loadObj("Resources/Models/rug.obj", textures["rug"]);
	Mesh books = loader.loadObj("Resources/Models/books.obj", textures["books"]);
	Mesh bookshelf = loader.loadObj("Resources/Models/bookshelf.obj", textures["bookshelf"]);
	Mesh fireplace = loader.loadObj("Resources/Models/fireplace.obj", textures["fireplace"]);

	camera.spawn();

	//check if we close the window or press the escape button
	while (!window.isPressed(GLFW_KEY_ESCAPE) &&
		glfwWindowShouldClose(window.getWindow()) == 0)
	{
		window.clear();
		float currentFrame = (float)(glfwGetTime());
		delta_time = currentFrame - last_frame;
		last_frame = currentFrame;


		processKeyboardInput();

		//test mouse input
		if (window.isMousePressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			std::cout << "Pressing mouse button" << std::endl;
		}
		 //// Code for the light ////

		sunShader.use();

		glm::mat4 ProjectionMatrix = glm::perspective(90.0f, window.getWidth() * 1.0f / window.getHeight(), 0.1f, 10000.0f);
		glm::mat4 ViewMatrix = glm::lookAt(camera.get_camera_position(), camera.get_camera_position() + camera.get_camera_view_direction(), camera.get_camera_up());

		GLuint MatrixID = glGetUniformLocation(sunShader.getId(), "MVP");

		//Test for one Obj loading = light source

		glm::mat4 ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, lightPos);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		sun.draw(sunShader);

		//// End code for the light ////

		shader.use();

		GLuint MatrixID2 = glGetUniformLocation(shader.getId(), "MVP");
		GLuint ModelMatrixID = glGetUniformLocation(shader.getId(), "model");

		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.get_camera_position().x, camera.get_camera_position().y, camera.get_camera_position().z);

		house.draw(shader);
		if (camera.location == "outside") {
			ground.draw(shader);
			water.draw(shader);
			trees.draw(shader);
			pedestal.draw(shader);
			camera.setCameraY(ground.find_player_pos(camera.get_camera_position()));
		}
		else if (camera.location == "inside") {
			bed.draw(shader);
			rug.draw(shader);
			books.draw(shader);
			bookshelf.draw(shader);
			fireplace.draw(shader);
		}

		//std::cout << camera.get_camera_position();

		window.update();
	}
}

void processKeyboardInput() {
	float camera_speed = 2 * delta_time;
	if (camera.location == "inside") {
		camera_speed = delta_time;
	}

	//interaction
	if (window.isPressed(GLFW_KEY_E)) {
		float current_time = (float)(glfwGetTime());
		if (current_time - inteact_time > 1.0f) {
			inteact_time = current_time;
			camera.keyboard_interact();
		}
	}
		

	//translation
	if (window.isPressed(GLFW_KEY_W))
		camera.keyboard_move_front(camera_speed);
	if (window.isPressed(GLFW_KEY_S))
		camera.keyboard_move_back(camera_speed);
	if (window.isPressed(GLFW_KEY_A))
		camera.keyboard_move_left(camera_speed);
	if (window.isPressed(GLFW_KEY_D))
		camera.keyboard_move_right(camera_speed);
	if (window.isPressed(GLFW_KEY_R))
		camera.keyboard_move_up(camera_speed);
	if (window.isPressed(GLFW_KEY_F))
		camera.keyboard_move_down(camera_speed);

	//rotation
	if (window.isPressed(GLFW_KEY_LEFT))
		camera.rotate_oy(camera_speed*50);
	if (window.isPressed(GLFW_KEY_RIGHT))
		camera.rotate_oy(-camera_speed*50);
	if (window.isPressed(GLFW_KEY_UP))
		camera.rotate_ox(camera_speed);
	if (window.isPressed(GLFW_KEY_DOWN))
		camera.rotate_ox(-camera_speed);
}
