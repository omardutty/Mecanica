#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <deque>
#include<iostream>
#include<time.h>

float posParticula[3 * 10000];//Limite a 10000 particulas
bool enableGravity = true;
int Mode = 0;
glm::vec3 p = { 1.5f,4.f,1.f };
float yVelocity = 0;
glm::vec3 v = { 4.f,-10,0.f };
glm::vec3 a = { 0.f,-9.81f,0.f };
int LifeTime = 100;
int numParticles = 0;
int particlesToSpawn = 0;
int maxParticles = 10000;
glm::vec3 ini = { -5,5,-5 }, end = { -1,5,-5 }, speed = {0,-1,-3};
glm::vec3 C = { 2.f,5.f,1.5f };
float SphereRad = 1;
float coeficienteR = 1;
float coeficienteE = 1;
glm::vec3 cascadaP1 = {-2.f,4.f,0.f};
glm::vec3 cascadaP2 = { 2.f,4.f,0.f };
glm::vec3 cascadaVel = {0.f,1.f,1.f};


namespace LilSpheres {
	extern const int maxParticles;
	extern void updateParticles(int startIdx, int count, float* array_data);
}
namespace VAR {
	
	glm::vec3 nextPoint(glm::vec3 last, glm::vec3 velocity, float frameRate,float elasticity);
	glm::vec3 nextVelocity(glm::vec3 lastVelocity, glm::vec3 acceleration, float frameRate);
	glm::vec3 rebotePared(glm::vec3 pos,glm::vec3 velocity, float grip);
	float distance(glm::vec3 p1, glm::vec3 p2);
	float calculoAlpha(glm::vec3 &P1, glm::vec3 &P2, glm::vec3 C, float r);
	glm::vec3 calculoQ(glm::vec3 &P1, glm::vec3 &P2, float alpha);
	glm::vec3 calculoN(glm::vec3 Q, glm::vec3 C);
	glm::vec3 pointReboteEsfera(glm::vec3 lastPoint, glm::vec3 point, glm::vec3 C, float radius);
	glm::vec3 velocityReboteEsfera(glm::vec3 lastVelocity, glm::vec3 lastPoint, glm::vec3 point, glm::vec3 C, float radius);
}

class Particle {
public:
	glm::vec3 pos, lastPos;
	glm::vec3 vel,lastVel;
	int tipo = Mode;
	float lifeTime;
	void Update() {
		lastPos = pos;
		lastVel = vel;
		lifeTime--;

		pos = VAR::nextPoint(pos, vel, 0.0333, coeficienteE);
		
		if (pos.x <= -5) {
			vel.x = VAR::rebotePared(pos,vel, coeficienteR).x;
		}
		else if (pos.x >= 5) {
			vel.x = VAR::rebotePared(pos, vel, coeficienteR).x;
		}
		else {
			vel.x = VAR::nextVelocity(lastVel, a, 0.0333).x;
		}

		if (pos.y <= 0) {
			vel.y = VAR::rebotePared(pos, vel, coeficienteR).y;
		}
		else if (pos.y >= 10) {
			vel.y = VAR::rebotePared(pos, vel, coeficienteR).y;
		}
		else {
			vel.y = VAR::nextVelocity(lastVel, a, 0.0333).y;
		}

		if (pos.z <= -5) {
			vel.z = VAR::rebotePared(pos, vel, coeficienteR).z;
		}
		else if (pos.z >= 5) {
			vel.z = VAR::rebotePared(pos, vel, coeficienteR).z;
		}
		else {
			vel.z = VAR::nextVelocity(lastVel, a, 0.0333).z;
		}

		if (sqrt(pow(lastPos.x * C.x, 2) + pow(lastPos.y * C.y, 2) + pow(lastPos.z * C.z, 2)) < SphereRad) {
			pos = VAR::pointReboteEsfera(lastPos,pos, C, SphereRad);
			vel = VAR::velocityReboteEsfera(lastVel, lastPos, pos, C, SphereRad);
		}
	}
	
	Particle() {

		if (tipo == 0) {
			pos = p;
			vel = glm::vec3((rand() % (30 + 30 + 1) - 30)*.1f, yVelocity, (rand() % (30 + 30 + 1) - 30)*.1f);
			//vel = v;
		}
		else if (tipo == 1) {
			//	pos = p;
			//	vel = glm::vec3(0,yVelocity,1);
			//	glm::vec3 spawnVector(end.x - ini.x, end.y - ini.y, end.z - ini.z);
			//	float vectorModule = glm::distance(ini, end);
			//	float distBTparicles = vectorModule / 100;
			//	pos = glm::vec3(distBTparicles*spawnVector.x + ini, distBTparicles*spawnVector.y + ini, distBTparicles*spawnVector.z + ini);
			//	if (distBTparicles >= vectorModule * 100) {
			//		distBTparicles = vectorModule / 100;
			//	}
			//	else {
			//		distBTparicles *= 2;
			//	}
			//	vel = speed;
			//}
			
			pos = cascadaP1;
			vel = cascadaVel;
		}
		//vel = speed;
		lifeTime = LifeTime;
	}
	~Particle() {}
};

std::deque<Particle*>particles;
Particle* p1;
int i = 0;
int j = 0;
void spawnParticles(int maxParticles) {
	if(cascadaP1.x > 5){
		cascadaP1.x = 5;
	}
	if (cascadaP2.x > 5) {
		cascadaP2.x = 5;
	}
	if (maxParticles < LifeTime) {
		if (particles.size() < maxParticles) {
			p1 = new Particle();
			if (Mode == 1) {
				p1->pos.x += i;
				i += 0.1f;
				if (p1->pos.x + i > cascadaP2.x) {
					i = 0;
				}
			}
			particles.push_back(p1);
			numParticles = particles.size();
		}
	}
	else {
		if (particles.size() < maxParticles) {
			for (int i = 0; i < maxParticles / LifeTime; i++) {
				p1 = new Particle();
				if (Mode == 1) {
					p1->pos.x += j ;
					j ++;
					if (p1->pos.x + j > cascadaP2.x) {
						j = 0;
					}
				}
				particles.push_back(p1);
				numParticles = particles.size();
			}
		}
	}
}

bool show_test_window = true;
void GUI() {
	bool show = true;
	ImGui::Begin("Physics Parameters", &show, 0);

	// Do your GUI code here....
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);//FrameRate
		ImGui::Checkbox("Gravity", &enableGravity);
		ImGui::InputFloat("Rozamiento", &coeficienteR);
		ImGui::RadioButton("Fuente", &Mode,0); ImGui::SameLine();
		ImGui::RadioButton("Cascada", &Mode,1);
		ImGui::InputInt("Num Particles", &particlesToSpawn);
		ImGui::InputInt("LifeTime", &LifeTime);
		ImGui::Text("Fuente: ");
		ImGui::InputFloat3("SpawnPoint", &p.x);
		ImGui::InputFloat("Y Velocity", &yVelocity);
		ImGui::Text("Cascada: ");
		ImGui::InputFloat3("P1", &cascadaP1.x);
		ImGui::InputFloat3("P2", &cascadaP2.x);
		ImGui::InputFloat3("Velocity", &cascadaVel.x);


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
	srand(time(NULL));
}

void PhysicsUpdate(float dt) {
	//srand(time(NULL));
	// Do your update code here...
	// ...........................
	//p1 = new Particle();
	//particles.push_back(p1);
	//std::cout << numParticles << std::endl;
	if (enableGravity) {
		a = { 0,-9.81,0 };
	}
	else {
		a = { 0,0,0 };
	}

	spawnParticles(particlesToSpawn);
	//std::cout << numParticles << std::endl;
	
	//Llenar array de la gpu
	int i = 0;
	if (particles.size() > 0) {
		for (Particle* p1 : particles) {
			if (p1->lifeTime <= 0) {
				particles.pop_front();
				numParticles = particles.size();
			}
			p1->Update();
			posParticula[i] = p1->pos.x;
			posParticula[i + 1] = p1->pos.y;
			posParticula[i + 2] = p1->pos.z;
			i += 3;
		}

	}

	LilSpheres::updateParticles(0, particles.size(), posParticula);
}

void PhysicsCleanup() {
	// Do your cleanup code here...
	// ............................
}