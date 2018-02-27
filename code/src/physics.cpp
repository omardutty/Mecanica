#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <deque>
#include<iostream>
#include<time.h>

float posParticula[3 * 10000];
float arr[30];
glm::vec3 p = { 0.f,8.f,0.f };
glm::vec3 v = { 4.f,10.f,0.f };
glm::vec3 a = { 0.f,-9.81f,0.f };


namespace LilSpheres {
	extern const int maxParticles;
	extern void updateParticles(int startIdx, int count, float* array_data);
}
namespace VAR {
	glm::vec3 nextPoint(glm::vec3 last, glm::vec3 velocity, float frameRate);
	glm::vec3 nextVelocity(glm::vec3 lastVelocity, glm::vec3 acceleration, float frameRate);
	glm::vec3 rebotePared(glm::vec3 lastPoint, glm::vec3 velocity, float grip);
	float distance(glm::vec3 p1, glm::vec3 p2);
	float calculoN(glm::vec3 P1, glm::vec3 P2, glm::vec3 C, float r);
}

class Particle {
public:
	glm::vec3 pos;
	glm::vec3 vel;
	float lifeTime;
	void Update() {
		pos = VAR::nextPoint(pos, vel, 0.0333);
		vel = VAR::nextVelocity(vel, glm::vec3(0.f, -9.81f, 0.f), 0.0333);
		lifeTime--;
		if (pos.y <= 0 || pos.y >= 10) {
			vel.y -= 1.6*vel.y;

		}
		if (pos.x <= -5 || pos.x >= 5) {
			vel.x = -vel.x;
		}
		if (pos.z <= -5 || pos.z >= 5) {
			vel.z = -vel.z;
		}
	}
	Particle() {
		pos = glm::vec3(0, 8, 0);
		//srand(time(NULL));
		vel = glm::vec3((rand() % (30 + 30 + 1) - 30)*.1f, 10, (rand() % (30 + 30 + 1) - 30)*.1f);
		lifeTime = 100;
	}
	~Particle() {}
};

std::deque<Particle*>particles;
Particle* p1;

bool show_test_window = false;
void GUI() {
	bool show = true;
	ImGui::Begin("Physics Parameters", &show, 0);

	// Do your GUI code here....
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);//FrameRate

	}
	// .........................

	ImGui::End();

	// Example code -- ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if (show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

void PhysicsInit() {
	// Do your initialization code here...
	// ...................................
	/*for (int i = 0; i < 10; i++) {
	p1 = new Particle();
	particles.push_back(p1);
	}*/
}

void PhysicsUpdate(float dt) {
	// Do your update code here...
	// ...........................
	p1 = new Particle();
	particles.push_back(p1);

	if (particles[0]->lifeTime <= 0 && particles.size()>0) {
		particles.pop_front();
	}
	
	//Llenar array de la gpu
	int i = 0;
	for (Particle* p1 : particles) {
		p1->Update();
		posParticula[i] = p1->pos.x;
		posParticula[i + 1] = p1->pos.y;
		posParticula[i + 2] = p1->pos.z;
		i += 3;
	}
	std::cout << posParticula[0] << " " << posParticula[1] << " " << posParticula[2] << std::endl;



	LilSpheres::updateParticles(0, 10000, posParticula);
}

void PhysicsCleanup() {
	// Do your cleanup code here...
	// ............................
}