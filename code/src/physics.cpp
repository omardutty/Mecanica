#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <deque>
#include<iostream>
#include<time.h>
#include<glm\gtc\quaternion.hpp>

float mass = 300.f;

namespace ClothMesh {
	void updateClothMesh(float*array_data);
}

namespace Sphere {
	extern void setupSphere(glm::vec3 pos = glm::vec3(0.f, 1.f, 0.f), float radius = 1.f);
	extern void cleanupSphere();
	extern void updateSphere(glm::vec3 pos, float radius = 1.f);
	extern void drawSphere();
}

class Particle {
public:
	glm::vec3 pos, lastPos;
	Particle();
	~Particle();
};

Particle::Particle() {

}

std::deque<Particle*> malla;
Particle* particles[18][18];


bool show_test_window = false;
void GUI() {
	bool show = false;
	ImGui::Begin("Parameters", &show, 0);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);//FrameRate
																															  //																														  // Do your GUI code here....
	{
		ImGui::InputFloat("Sphere Mass", &mass);
	}
	// .........................
	ImGui::End();
}

float x = -4;
float z = -4;
float dist = 0.5;
float arr[18 * 14 * 3];

//bool control = false;

void PhysicsInit() {

	srand(time(NULL));
	int count = 0;
	x = -4.5;
	z = -4;

	for (int i = 0; i < 18; i++) {
		for (int j = 0; j < 14; j++) {
			Particle* p1 = new Particle();
			p1->pos.x = p1->lastPos.x = arr[count] = x + dist;
			p1->pos.y = p1->lastPos.y = arr[count + 1] = 8;
			p1->pos.z = p1->lastPos.z = arr[count + 2] = z + dist;
			z += dist;
			std::cout << arr[count] << " " << arr[count + 1] << " " << arr[count + 2] << std::endl;
			count += 3;

			particles[i][j] = p1;

		}
		x += dist;
		z = -4;
	}



}

glm::vec3 K = { 1.f,0.2f,0 };
glm::vec3 K2 = { 0.f,0.2f,2.f };
float w = 5.f;
float w2 = 1.f;
float A = 0.4f;
float A2 = 0.3f;
float simTime = 0.f;
float phi = 5.f;
float phi2 = 3.f;

void PhysicsUpdate(float dt) {

	Sphere::drawSphere();
	int count = 0;
	simTime += dt;
	for (int i = 0; i < 18; i++) {
		for (int j = 0; j < 14; j++) {


			particles[i][j]->pos = particles[i][j]->lastPos - (glm::normalize(K)*A*sin(glm::dot(K, particles[i][j]->lastPos) - w*simTime + phi) + glm::normalize(K2)*A2*sin(glm::dot(K2, particles[i][j]->lastPos) - w2*simTime + phi2));
			particles[i][j]->pos.y = A*cos(glm::dot(K, particles[i][j]->lastPos) - w*simTime + phi) + A2*cos(glm::dot(K2, particles[i][j]->lastPos) - w2*simTime + phi2);

			arr[count] = particles[i][j]->pos.x;
			arr[count + 1] = particles[i][j]->pos.y + 3;

			count += 3;
		}
	}



	ClothMesh::updateClothMesh(arr);
}

void PhysicsCleanup() {
	// Do your cleanup code here...
	// ............................
}