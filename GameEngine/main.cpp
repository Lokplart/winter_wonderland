#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

#include "Graphics\window.h"
#include "Camera\camera.h"
#include "Shaders\shader.h"
#include "Model Loading\mesh.h"
#include "Model Loading\texture.h"
#include "Model Loading\meshLoaderObj.h"
#include "map"

void processKeyboardInput (bool* p_open, int* p_Jdelay, bool* p_openi, int* p_Idelay, bool* p_items, bool* p_openm, int* M_delay, bool* p_close);
void showJournal(bool* p_open);
void showInventory(bool* p_openi, bool* p_items);
void showMenu(bool* p_openm, bool* p_open, bool* p_openi, int* p_Idelay, bool* p_items, int* p_Jdelay,bool* p_close);
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
	Shader sun_shader("Shaders/sun_vertex_shader.glsl", "Shaders/sun_fragment_shader.glsl");
	Shader anim_shader("Shaders/object_vertex_shader.glsl", "Shaders/object_fragment_shader.glsl");

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

	GLuint tex_key = loadBMP("Resources/Textures/key.bmp", 'b');
	GLuint tex_doll = loadBMP("Resources/Textures/doll.bmp", 'b');

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
	
	add_textures("key", tex_key);
	add_textures("doll", tex_doll);


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

	Mesh key = loader.loadObj("Resources/Models/key.obj", textures["key"]);
	Mesh doll = loader.loadObj("Resources/Models/doll.obj", textures["doll"]);

	camera.spawn();

	int anim_dir = 1;
	float anim_y = 0.0f;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 460");
	glfwMakeContextCurrent(window.getWindow());

	bool isJournal = true;
	bool* p_open = &isJournal;
	int J_delay = 0;
	int* p_Jdelay = &J_delay;
	bool isInventory = false;
	bool* p_openi = &isInventory;
	int I_delay = 0;
	int* p_Idelay = &I_delay;
	bool isMenu = false;
	bool* p_openm = &isMenu;
	int M_delay = 0;
	int* p_Mdelay = &M_delay;
	bool close = false;
	bool* p_close = &close;
	bool items = false;
	bool* p_items = &items;
	//check if we close the window or press the escape button
	while (!*p_close && glfwWindowShouldClose(window.getWindow()) == 0) {
		window.clear();
		float currentFrame = (float)(glfwGetTime());
		delta_time = currentFrame - last_frame;
		last_frame = currentFrame;
		J_delay++;
		I_delay++;
		M_delay++;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (window.isMousePressed(GLFW_MOUSE_BUTTON_LEFT)) {
			std::cout << "Pressing mouse button" << std::endl;
		}

		sun_shader.use();

		glm::mat4 ProjectionMatrix = glm::perspective(90.0f, window.getWidth() * 1.0f / window.getHeight(), 0.1f, 10000.0f);
		glm::mat4 ViewMatrix = glm::lookAt(camera.get_camera_position(), camera.get_camera_position() + camera.get_camera_view_direction(), camera.get_camera_up());

		GLuint MatrixID = glGetUniformLocation(sun_shader.getId(), "MVP");


		glm::mat4 ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, lightPos);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		sun.draw(sun_shader);


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
			if (camera.questline_progress > 0) {
				pedestal.draw(shader);
			}
			camera.set_camera_y(ground.find_player_pos(camera.get_camera_position()));
		}
		else if (camera.location == "inside") {
			bed.draw(shader);
			rug.draw(shader);
			books.draw(shader);
			bookshelf.draw(shader);
			fireplace.draw(shader);
		}

		if (camera.location == "outside" && camera.questline_progress > 0 && camera.questline_progress < 3) {
			anim_shader.use();
			MatrixID2 = glGetUniformLocation(anim_shader.getId(), "MVP");
			ModelMatrixID = glGetUniformLocation(anim_shader.getId(), "model");
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
			if (anim_y > 0.05f)		  { anim_dir = -1; }
			else if (anim_y < -0.05f) { anim_dir = 1; }
			anim_y = anim_y + (delta_time / 10.0f * anim_dir);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, anim_y, 0.0f));
			MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniform3f(glGetUniformLocation(anim_shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
			glUniform3f(glGetUniformLocation(anim_shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
			glUniform3f(glGetUniformLocation(anim_shader.getId(), "viewPos"), camera.get_camera_position().x, camera.get_camera_position().y, camera.get_camera_position().z);

			if (!camera.got_key) {
				key.draw(anim_shader);
			}
			if (!camera.got_doll) {
				doll.draw(anim_shader);
			}
		}
		
		processKeyboardInput(p_open,p_Jdelay,p_openi,p_Idelay, p_items,p_openm,p_Mdelay,p_close);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		window.update();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void processKeyboardInput(bool* p_open,int* p_Jdelay, bool* p_openi, int* p_Idelay,bool* p_items, bool* p_openm, int* p_Mdelay, bool* p_close) {
	float camera_speed = 10 * delta_time;
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

	//tasks
	if (!*p_open && window.isPressed(GLFW_KEY_J) && *p_Jdelay > 100)
	{
		*p_open = true;
		*p_Jdelay = 0;
	}
	if (*p_open && window.isPressed(GLFW_KEY_J) && *p_Jdelay > 100)
	{
		*p_open = false;
		*p_Jdelay = 0;
	}
	if (*p_open)
	{
		showJournal(p_open);
	}

	if (!*p_openi && window.isPressed(GLFW_KEY_I) && *p_Idelay > 100)
	{
		*p_openi = true;
		*p_Idelay = 0;
	}
	if (*p_openi && window.isPressed(GLFW_KEY_I) && *p_Idelay > 100)
	{
		*p_openi = false;
		*p_Idelay = 0;
	}
	if (*p_openi)
	{
		showInventory(p_openi,p_items);
	}

	if (!*p_openm && window.isPressed(GLFW_KEY_ESCAPE) && *p_Mdelay > 100)
	{
		*p_openm = true;
		*p_Mdelay = 0;
	}
	if (*p_openm && window.isPressed(GLFW_KEY_ESCAPE) && *p_Mdelay > 100)
	{
		*p_openm = false;
		*p_Mdelay = 0;
	}
	if (*p_openm)
	{
		showMenu(p_openm, p_open, p_openi, p_Idelay,p_items, p_Jdelay, p_close);
	}

	
}

void showJournal(bool* p_open)
{
	if (!ImGui::Begin("Journal", p_open))
	{
		ImGui::End();
	}
	else
	{
		if (camera.questline_progress == 0)
			ImGui::Text("Screw this place. Lets try that old book again...\nMaybe it's more then just fairytales...");
		if (camera.questline_progress == 1)
			ImGui::Text("The book mentioned a 'key' and a 'doll' that keep people bound to this world.\nI need to find them!");
		if (camera.questline_progress == 2) {
			if (camera.got_key) {
				ImGui::Text("I got the key, now to find the doll");
			}
			else if (camera.got_doll) {
				ImGui::Text("I got the doll, now to find the key");
			}
		}
		if (camera.questline_progress == 3)
			ImGui::Text("I found them! Now I need their... 'contents'?\nLets try burning them, what's the worst that could happen?");
		if (camera.questline_progress == 4)
			ImGui::Text("This is it! The book said to consume their 'contents' at 'the Edge of the World'...\nWhat could that mean?");
		
		ImGui::End();
	}
}

void showInventory(bool* p_openi, bool* p_items)
{
	if (!ImGui::Begin("Inventory", p_openi))
	{
		ImGui::End();
	}
	else
	{
		if (camera.questline_progress == 4)
		{
			if (ImGui::Selectable("Potion", false)) {
				camera.at_edge_of_world();
			}
		}
		else if (camera.got_doll && !camera.got_key && camera.questline_progress !=5)
		{
			ImGui::Selectable("Doll", false);
			*p_items = true;
		}
		else if (camera.got_key && !camera.got_doll && camera.questline_progress != 5)
		{
			ImGui::Selectable("Key", false);
			*p_items = true;
		}
		else if (camera.got_key && camera.got_doll && camera.questline_progress != 5) 
		{
			ImGui::Selectable("Key", false);
			ImGui::Selectable("Doll", false);
			*p_items = true;
		}
			
		ImGui::End();
	}
}

void showMenu(bool* p_openm, bool* p_open, bool* p_openi, int* p_Idelay,bool* p_items, int* p_Jdelay,bool* p_close)
{
	if (!ImGui::Begin("Menu", p_openm))
	{
		ImGui::End();
	}
	else
	{
		if (ImGui::Selectable("Inventory", false) && !*p_openi && *p_Idelay > 100)
		{
			*p_Idelay = 0;
			*p_openi = true;
			showInventory(p_openi,p_items);
		}

		if (ImGui::Selectable("Journal", false) && !*p_open && *p_Jdelay > 100)
		{
			*p_Jdelay = 0;
			*p_open = true;
			showJournal(p_open);
		}

		if (ImGui::Selectable("Quit", false))
		{
			*p_close = true;
		}
		
		ImGui::End();
	}
}