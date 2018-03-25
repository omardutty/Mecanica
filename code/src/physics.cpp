#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <deque>
#include<iostream>
#include<time.h>
/*
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
*/


float arr[252 * 3];
float x = -4;
float z = -4;


bool gravityFake = false;

namespace LilSpheres {
	extern const int maxParticles;
	extern void updateParticles(int startIdx, int count, float* array_data);
}
namespace ClothMesh {
	void updateClothMesh(float *array_data);
}

class Particle {
public:
	glm::vec3 pos, lastPos;
	glm::vec3 vel, lastVel;
	glm::vec3 greenF[4] = { { 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 } };
	glm::vec3 redF[4] = { { 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 } };
	glm::vec3 blueF[4] = { { 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 } };
	Particle();
	static glm::vec3 springForce(glm::vec3 P1, glm::vec3 P2, glm::vec3 v1, glm::vec3 v2, float lenght, const float Ke, const float Kd);
	static glm::vec3 VerletPos(glm::vec3 pos, glm::vec3 lastPos, glm::vec3 F, float time);
	static glm::vec3 Velocity(glm::vec3 pos, glm::vec3 lastPos, float time);
	~Particle();
};

std::deque<Particle*> malla;
Particle* particles[18][14];

Particle::Particle() {
	lastPos = pos;
	lastVel = vel;
}

glm::vec3 Particle::VerletPos(glm::vec3 pos, glm::vec3 lastPos, glm::vec3 F, float time) {
	return (pos + (pos - lastPos) + F * pow(time, 2));
}
glm::vec3 Particle::Velocity(glm::vec3 pos, glm::vec3 lastPos, float time) {
	return((pos - lastPos) / time);
}

/*
namespace VAR {

glm::vec3 nextPoint(glm::vec3 last, glm::vec3 velocity, float frameRate,float elasticity);
glm::vec3 nextVelocity(glm::vec3 lastVelocity, glm::vec3 acceleration, float frameRate);
glm::vec3 rebotePared(glm::vec3 pos,glm::vec3 velocity, float grip);
float distance(glm::vec3 p1, glm::vec3 p2);
float calculoAlpha(glm::vec3 &P1, glm::vec3 &P2, glm::vec3 C, float r);
glm::vec3 calculoQ(glm::vec3 &P1, glm::vec3 &P2, float alpha);
glm::vec3 calculoN(glm::vec3 Q, glm::vec3 C);
glm::vec3 pointReboteEsfera(glm::vec3 point, glm::vec3 n, glm::vec3 Q);
glm::vec3 velocityReboteEsfera(glm::vec3 lastVelocity, glm::vec3 n);
}

class Particle {
public:
glm::vec3 pos, lastPos;
glm::vec3 vel,lastVel;
glm::vec3 Q, n;
float alpha;
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

alpha = VAR::calculoAlpha(lastPos,pos,C,SphereRad);
std::cout << "aplha:" << alpha << std::endl;
Q = VAR::calculoQ(lastPos, pos, alpha);
std::cout << "Qx: " << Q.x << " y:" << Q.y << " z: " << Q.z << std::endl;
n = VAR::calculoN(Q, C);
std::cout << "nx: " << n.x << " y:" << n.y << " z: " << n.z << std::endl;

pos = VAR::pointReboteEsfera(pos,n,Q);
vel = VAR::velocityReboteEsfera(lastVel,n);
}
}

Particle() {
alpha = 0;
Q = { 0.f,0.f,0.f };
n = { 0.f,0.f,0.f };
if (tipo == 0) {
pos = p;
vel = glm::vec3((rand() % (30 + 30 + 1) - 30)*.1f, yVelocity, (rand() % (30 + 30 + 1) - 30)*.1f);

}
else if (tipo == 1) {

pos = cascadaP1;
vel = cascadaVel;
}

lifeTime = LifeTime;
}
~Particle() {}
};

std::deque<Particle*>particles;
Particle* p1;
int i = 0;
int j = 0;
void spawnParticles(int maxParticles) {
if(cascadaP1.x > 10){
cascadaP1.x = 10;
}
if (cascadaP2.x > 10) {
cascadaP2.x = 10;
}
if (maxParticles < LifeTime) {
if (particles.size() < maxParticles) {
p1 = new Particle();
if (Mode == 1) {
if (cascadaP1.x == cascadaP2.x) {
p1->pos.x = cascadaP1.x;
}
else if (cascadaP1.x < cascadaP2.x) {

p1->pos.x =cascadaP1.x + rand() % abs((int)cascadaP2.x - (int)cascadaP1.x);
}

else {
p1->pos.x = cascadaP2.x + rand() % abs((int)cascadaP2.x - (int)cascadaP1.x);
}
if (cascadaP1.y == cascadaP2.y) {
p1->pos.y = cascadaP1.y;
}
else if (cascadaP1.y < cascadaP2.y) {
p1->pos.y = cascadaP1.y + rand() % abs((int)cascadaP2.y - (int)cascadaP1.y);
}
else {
p1->pos.y = cascadaP2.y + rand() % abs((int)cascadaP2.y - (int)cascadaP1.y);
}
if (cascadaP1.z == cascadaP2.z) {
p1->pos.z = cascadaP1.z;
}
else if (cascadaP1.z < cascadaP2.z) {
p1->pos.z = cascadaP1.z + rand() % abs((int)cascadaP2.z - (int)cascadaP1.z);
}
else {
p1->pos.z = cascadaP2.z + rand() % abs((int)cascadaP2.z - (int)cascadaP1.z);
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
if (cascadaP1.x == cascadaP2.x) {
p1->pos.x = cascadaP1.x;
}
else if (cascadaP1.x < cascadaP2.x) {

p1->pos.x = cascadaP1.x + rand() % abs((int)cascadaP2.x - (int)cascadaP1.x);
}

else {
p1->pos.x = cascadaP2.x + rand() % abs((int)cascadaP2.x - (int)cascadaP1.x);
}
if (cascadaP1.y == cascadaP2.y) {
p1->pos.y = cascadaP1.y;
}
else if (cascadaP1.y < cascadaP2.y) {
p1->pos.y = cascadaP1.y + rand() % abs((int)cascadaP2.y - (int)cascadaP1.y);
}
else {
p1->pos.y = cascadaP2.y + rand() % abs((int)cascadaP2.y - (int)cascadaP1.y);
}
if (cascadaP1.z == cascadaP2.z) {
p1->pos.z = cascadaP1.z;
}
else if (cascadaP1.z < cascadaP2.z) {
p1->pos.z = cascadaP1.z + rand() % abs((int)cascadaP2.z - (int)cascadaP1.z);
}
else {
p1->pos.z = cascadaP2.z + rand() % abs((int)cascadaP2.z - (int)cascadaP1.z);
}
}
particles.push_back(p1);
numParticles = particles.size();
}
}
}
}
*/
bool show_test_window = false;
void GUI() {
	bool show = false;
	ImGui::Begin("Physics Parameters", &show, 0);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);//FrameRate
																															  // Do your GUI code here....
	{
		/*
		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);//FrameRate
		//ImGui::Checkbox("Gravity", &enableGravity);
		//ImGui::InputFloat("Rozamiento", &coeficienteR);
		//ImGui::RadioButton("Fuente", &Mode,0); ImGui::SameLine();
		//ImGui::RadioButton("Cascada", &Mode,1);
		//ImGui::InputInt("Num Particles", &particlesToSpawn);
		//ImGui::InputInt("LifeTime", &LifeTime);
		//ImGui::Text("Fuente: ");
		//ImGui::InputFloat3("SpawnPoint", &p.x);
		//ImGui::InputFloat("Y Velocity", &yVelocity);
		//ImGui::Text("Cascada: ");
		//ImGui::InputFloat3("P1", &cascadaP1.x);
		//ImGui::InputFloat3("P2", &cascadaP2.x);
		//ImGui::InputFloat3("Velocity", &cascadaVel.x);*/
		ImGui::Checkbox("Gravity", &gravityFake);


	}
	// .........................

	ImGui::End();

	// Example code -- ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if (show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);

		ImGui::ShowTestWindow(&show_test_window);
	}
}

glm::vec3 Particle::springForce(glm::vec3 P1, glm::vec3 P2, glm::vec3 v1, glm::vec3 v2, float lenght, const float Ke, const float Kd) {
	glm::vec3 normalized = glm::normalize(P1 - P2);
	float dotP = (v1 - v2).x*normalized.x + (v1 - v2).y*normalized.y + (v1 - v2).z*normalized.z;
	return -((Ke*(glm::length(P1 - P2) - lenght)) + Kd * dotP)*normalized;
}

bool control = false;

void PhysicsInit() {
	// Do your initialization code here...
	// ...................................
	srand(time(NULL));


	//for (int i = 0; i < 252*3; i++) {
	//	Particle* p1 = new Particle();
	//	if (i % 14 == 0 && i!= 0) {
	//		x += 0.5;
	//		z = -4;
	//	}
	//	/*arr[i] = x+0.5;
	//	arr[i + 1] = 10;
	//	arr[i + 2] = z+0.5;*/

	//	p1->pos.x   = p1->lastPos.x = arr[i] = x+0.5;
	//	p1->pos.y  = p1->lastPos.y  = arr[i + 1] = 10;
	//	p1->pos.z  = p1->lastPos.z = arr[i + 2] = z+0.5;
	//	p1->vel.x = p1->vel.y = p1->vel.z = 0;
	//	p1->lastVel.x = p1->lastVel.y = p1->lastVel.z = 0;

	//	malla.push_back(p1);
	//	z += 0.5;
	//	//std::cout << arr[i] << " " << arr[i + 1] << " " << arr[i + 2] << std::endl;
	//	i += 2;
	//}

	int count = 0;
	x = -4;
	z = -4;

	for (int i = 0; i < 18; i++) {
		for (int j = 0; j < 14; j++) {
			Particle* p1 = new Particle();
			p1->pos.x = p1->lastPos.x = arr[count] = x + 0.5;
			p1->pos.y = p1->lastPos.y = arr[count + 1] = 10;
			p1->pos.z = p1->lastPos.z = arr[count + 2] = z + 0.5;
			z += 0.5;
			std::cout << arr[count] << " " << arr[count + 1] << " " << arr[count + 2] << std::endl;
			count += 3;
			p1->vel.x = p1->vel.y = p1->vel.z = 0;
			p1->lastVel.x = p1->lastVel.y = p1->lastVel.z = 0;

			particles[i][j] = p1;

		}
		x += 0.5;
		z = -4;
	}

}

void PhysicsUpdate(float dt) {
	//srand(time(NULL));
	// Do your update code here...
	// ...........................
	/*
	if (enableGravity) {
	a = { 0,-9.81,0 };
	}
	else {
	a = { 0,0,0 };
	}

	spawnParticles(particlesToSpawn);

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
	*/
	/*if (gravityFake) {
	for (int i = 0; i < 252 * 3; i++) {
	if (arr[i + 1] >= 0) {
	if (i != 13*3 && i!=0) {
	arr[i + 1] -= 0.1;
	}
	}
	i += 2;
	}
	}*/

	//int i = 0;

	/*for (Particle* p1 : malla) {

	glm::vec3 newpos = Particle::VerletPos(p1->pos, p1->lastPos, { 0,-9.81,0 }, dt);
	p1->lastPos = p1->pos;
	p1->pos = newpos;
	arr[i] = p1->pos.x;
	arr[i + 1] = p1->pos.y;
	arr[i + 2] = p1->pos.z;
	i += 3;
	}*/

	int count = 0;
	for (int i = 0; i < 18; i++) {
		for (int j = 0; j < 14; j++) {

			//VELOCIDAD
			glm::vec3 newVel = Particle::Velocity(particles[i][j]->pos, particles[i][j]->lastPos, dt);
			particles[i][j]->lastVel = particles[i][j]->vel;
			particles[i][j]->vel = newVel;

			//FUERZA
			//horizontal y vertical
			glm::vec3 vert1;
			glm::vec3 vert2;
			glm::vec3 vert3;
			glm::vec3 vert4;

			//diagonales
			glm::vec3 vert5;
			glm::vec3 vert6;
			glm::vec3 vert7;
			glm::vec3 vert8;

			//horizontal y vertical largas
			glm::vec3 vert9;
			glm::vec3 vert10;
			glm::vec3 vert11;
			glm::vec3 vert12;

			//VERTICALES Y HORIZONTALES
			if (i > 0) {
				vert1 = Particle::springForce(particles[i][j]->pos, particles[i - 1][j]->pos, particles[i][j]->vel, particles[i - 1][j]->vel, 0.5, 20, 0);
			}
			else {
				vert1 = { 0, 0, 0 };
			}
			if (i < 17) {
				vert3 = Particle::springForce(particles[i][j]->pos, particles[i + 1][j]->pos, particles[i][j]->vel, particles[i + 1][j]->vel, 0.5, 20, 0);
			}
			else {
				vert3 = { 0, 0, 0 };
			}
			if (j > 0) {
				vert4 = Particle::springForce(particles[i][j]->pos, particles[i][j - 1]->pos, particles[i][j]->vel, particles[i][j - 1]->vel, 0.5, 20, 0);
			}
			else {
				vert4 = { 0, 0, 0 };
			}
			if (j < 13) {
				vert2 = Particle::springForce(particles[i][j]->pos, particles[i][j + 1]->pos, particles[i][j]->vel, particles[i][j + 1]->vel, 0.5, 20, 0);
			}
			else {
				vert2 = { 0, 0, 0 };
			}

			//DIAGONALES
			if (i > 0 && j < 13) {
				vert5 = Particle::springForce(particles[i][j]->pos, particles[i - 1][j + 1]->pos, particles[i][j]->vel, particles[i - 1][j + 1]->vel, 0.5, 20, 0);
			}
			else {
				vert5 = { 0, 0, 0 };
			}
			if (i < 17 && j < 13) {
				vert6 = Particle::springForce(particles[i][j]->pos, particles[i + 1][j + 1]->pos, particles[i][j]->vel, particles[i + 1][j + 1]->vel, 0.5, 20, 0);
			}
			else {
				vert6 = { 0, 0, 0 };
			}
			if (i < 17 && j > 0) {
				vert7 = Particle::springForce(particles[i][j]->pos, particles[i + 1][j - 1]->pos, particles[i][j]->vel, particles[i + 1][j - 1]->vel, 0.5, 20, 0);
			}
			else {
				vert7 = { 0, 0, 0 };
			}
			if (i > 0 && j > 0) {
				vert8 = Particle::springForce(particles[i][j]->pos, particles[i - 1][j - 1]->pos, particles[i][j]->vel, particles[i - 1][j - 1]->vel, 0.5, 20, 0);
			}
			else {
				vert8 = { 0, 0, 0 };
			}

			//VERTICALES Y HORIZONTALES LARGAS
			if (i > 2) {
				vert9 = Particle::springForce(particles[i][j]->pos, particles[i - 2][j]->pos, particles[i][j]->vel, particles[i - 2][j]->vel, 0.5, 20, 0);
			}
			else {
				vert9 = { 0, 0, 0 };
			}
			if (j < 12) {
				vert10 = Particle::springForce(particles[i][j]->pos, particles[i][j + 2]->pos, particles[i][j]->vel, particles[i][j + 2]->vel, 0.5, 20, 0);
			}
			else {
				vert10 = { 0, 0, 0 };
			}
			if (i < 16) {
				vert11 = Particle::springForce(particles[i][j]->pos, particles[i + 2][j]->pos, particles[i][j]->vel, particles[i + 2][j]->vel, 0.5, 20, 0);
			}
			else {
				vert11 = { 0, 0, 0 };
			}
			if (j > 2) {
				vert12 = Particle::springForce(particles[i][j]->pos, particles[i][j - 2]->pos, particles[i][j]->vel, particles[i][j - 2]->vel, 0.5, 20, 0);
			}
			else {
				vert12 = { 0, 0, 0 };
			}

		
			particles[i][j]->greenF[0] = vert1;
			particles[i][j]->greenF[1] = vert2;
			particles[i][j]->greenF[2] = vert3;
			particles[i][j]->greenF[3] = vert4;

			particles[i][j]->redF[0] = vert5;
			particles[i][j]->redF[1] = vert6;
			particles[i][j]->redF[2] = vert7;
			particles[i][j]->redF[3] = vert8;

			particles[i][j]->blueF[0] = vert9;
			particles[i][j]->blueF[1] = vert10;
			particles[i][j]->blueF[2] = vert11;
			particles[i][j]->blueF[3] = vert12;
		}
	}


	for (int i = 0; i < 18; i++) {
		for (int j = 0; j < 14; j++) {

			//POSICION
			if (i != 0 && j != 14) {
				glm::vec3 sumF = (particles[i][j]->greenF[0] + particles[i][j]->greenF[1] + particles[i][j]->greenF[2] + particles[i][j]->greenF[3]) +
								 (particles[i][j]->redF[0] + particles[i][j]->redF[1] + particles[i][j]->redF[2] + particles[i][j]->redF[3]) +
								 (particles[i][j]->blueF[0] + particles[i][j]->blueF[1] + particles[i][j]->blueF[2] + particles[i][j]->blueF[3]) +
								 glm::vec3(0, -9.81, 0);
				glm::vec3 newpos = Particle::VerletPos(particles[i][j]->pos, particles[i][j]->lastPos, sumF, dt);
				particles[i][j]->lastPos = particles[i][j]->pos;
				particles[i][j]->pos = newpos;
			}


			arr[count] = particles[i][j]->pos.x;
			arr[count + 1] = particles[i][j]->pos.y;
			arr[count + 2] = particles[i][j]->pos.z;
			count += 3;
		}
	}
	ClothMesh::updateClothMesh(arr);

}

void PhysicsCleanup() {
	// Do your cleanup code here...
	// ............................
}