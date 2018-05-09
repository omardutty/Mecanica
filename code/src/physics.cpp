#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <deque>
#include<iostream>
#include<time.h>
#include<glm\gtc\quaternion.hpp>

float sphereMass = 1.f;
float sphereForce;

namespace ClothMesh {
	void updateClothMesh(float*array_data);
}
bool reset = false;

namespace Sphere {
	extern void setupSphere(glm::vec3 pos = glm::vec3(0.f, 1.f, 0.f), float radius = 1.f);
	extern void cleanupSphere();
	extern void updateSphere(glm::vec3 pos, float radius = 1.f);
	extern void drawSphere();
}

namespace SphereVars {
	glm::vec3 pos, lastPos;
	glm::vec3 vel, lastVel;
	glm::vec3 F = {0,-9.81*sphereMass,0};
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
		ImGui::InputFloat("Sphere Mass", &sphereMass);
		if (ImGui::Button("RESET SIM")) {
			reset = !reset;
		}
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

	std::cout << "reset vars" << std::endl;

	float SposX = (rand() % 7) -3;
	float SposY = (rand() % 4) + 6;
	float SposZ = (rand() % 5) - 2;

	SphereVars::lastPos = { SposX,SposY,SposZ };
	SphereVars::vel = { 0,0,0 };

	for (int i = 0; i < 18; i++) {
		for (int j = 0; j < 14; j++) {
			Particle* p1 = new Particle();
			p1->pos.x = p1->lastPos.x = arr[count] = x + dist;
			p1->pos.y = p1->lastPos.y = arr[count + 1] = 8;
			p1->pos.z = p1->lastPos.z = arr[count + 2] = z + dist;
			z += dist;
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
float Vol;

int i = 1;
void PhysicsUpdate(float dt) {

	if (simTime > 15 || reset) {
		PhysicsInit();
		simTime = 0;
		reset = false;
	}

	//Sphere Euler Solver
	glm::vec3 BuoyancyF = { 0,9.81,0 };
	glm::vec3 FTot = BuoyancyF - SphereVars::F;


	//if (SphereVars::lastPos.y <= 4) {
	//	glm::vec3 temp = { 0, A*cos(glm::dot(K, SphereVars::lastPos) - w*simTime + phi) + A2*cos(glm::dot(K2, SphereVars::lastPos) - w2*simTime + phi2),0 };
	//	SphereVars::lastPos.y = A*cos(glm::dot(K, SphereVars::lastPos) - w*simTime + phi) + A2*cos(glm::dot(K2, SphereVars::lastPos) - w2*simTime + phi2)+4;
	//	SphereVars::pos.y = temp.y;
	//}
	//else {

	//glm::vec3 temp = SphereVars::lastPos + dt*FTot;
	//SphereVars::lastPos = SphereVars::lastPos + dt*FTot;
	//SphereVars::pos = temp;
	//}

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

	
	SphereVars::pos = SphereVars::lastPos - (glm::normalize(K)*A*sin(glm::dot(K, SphereVars::lastPos) - w*simTime + phi) + glm::normalize(K2)*A2*sin(glm::dot(K2, SphereVars::lastPos) - w2*simTime + phi2));
	SphereVars::pos.y = A*cos(glm::dot(K, SphereVars::lastPos) - w*simTime + phi) + A2*cos(glm::dot(K2, SphereVars::lastPos) - w2*simTime + phi2) + 4;

	if (SphereVars::pos.y < 4) {
			/*SphereVars::pos = SphereVars::lastPos - (glm::normalize(K)*A*sin(glm::dot(K, SphereVars::lastPos) - w*simTime + phi) + glm::normalize(K2)*A2*sin(glm::dot(K2, SphereVars::lastPos) - w2*simTime + phi2));
			SphereVars::pos.y += BuoyancyF.y;*/
	}

	//SphereVars::pos = SphereVars::lastPos - (glm::normalize(K)*A*sin(glm::dot(K, SphereVars::lastPos) - w*simTime + phi) + glm::normalize(K2)*A2*sin(glm::dot(K2, SphereVars::lastPos) - w2*simTime + phi2));
	
	ClothMesh::updateClothMesh(arr);
}

void PhysicsCleanup() {
	// Do your cleanup code here...
	// ............................
}