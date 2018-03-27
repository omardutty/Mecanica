#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <deque>
#include<iostream>
#include<time.h>



float arr[252 * 3];
float x = -4;
float z = -4;
glm::vec2  ke_G = { 201,1 }, ke_R = { 201,1 }, ke_B = { 250,1 };
float e = 0.0f;
float f = 0;
bool simulate = false;
float simTime = 5.0f;
glm::vec3 gravity = { 0,-9.81,0 };
bool collisions = true;
bool sphereCollisions = false;
bool reset = false;
double timeManager = 0;
bool click = false;
float dist = 0.5;


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
	glm::vec3 ElasticF = { 0,0,0 };
	Particle();
	static glm::vec3 springForce(glm::vec3 P1, glm::vec3 P2, glm::vec3 v1, glm::vec3 v2, float lenght, const float Ke, const float Kd);
	static glm::vec3 VerletPos(glm::vec3 pos, glm::vec3 lastPos, glm::vec3 F, float time);
	static glm::vec3 Velocity(glm::vec3 pos, glm::vec3 lastPos, float time);
	static glm::vec3 ElasticForce(glm::vec3 pos, float e, float d, glm::vec3 n);
	static glm::vec3 ElasticVel(glm::vec3 vel,float e, glm::vec3 n);
	static glm::vec3 Friction(glm::vec3 vel, glm::vec3 lastVel, glm::vec3 n,float f);
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
glm::vec3 Particle::ElasticForce(glm::vec3 pos, float e, float d, glm::vec3 n) {
	glm::vec3 nN = glm::normalize(n);
	float dotP = nN.x*pos.x + nN.y*pos.y + nN.z*pos.z;
	return(pos - (1 + e)*(dotP + d)*nN);
}
glm::vec3 Particle::ElasticVel(glm::vec3 vel,float e, glm::vec3 n) {
	glm::vec3 nN = glm::normalize(n);
	float dotP = nN.x*vel.x + nN.y*vel.y + nN.z*vel.z;
	return(vel - (1 + e)*(dotP*nN));
}
glm::vec3 Particle::springForce(glm::vec3 P1, glm::vec3 P2, glm::vec3 v1, glm::vec3 v2, float lenght, const float Ke, const float Kd) {
	glm::vec3 normalized = glm::normalize(P1 - P2);
	float dotP = (v1 - v2).x*normalized.x + (v1 - v2).y*normalized.y + (v1 - v2).z*normalized.z;
	return -((Ke*(glm::length(P1 - P2) - lenght)) + Kd * dotP)*normalized;
}
glm::vec3 Particle::Friction(glm::vec3 vel, glm::vec3 lastVel, glm::vec3 n, float f) {
	glm::vec3 nN = glm::normalize(n);
	float dotP = nN.x*lastVel.x + nN.y*lastVel.y + nN.z*lastVel.z;
	glm::vec3 VN = dotP * nN;
	return vel - f * (lastVel*VN);
}



bool show_test_window = false;
void GUI() {
	bool show = false;
	ImGui::Begin("Physics Parameters", &show, 0);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);//FrameRate
																															  // Do your GUI code here....
	{
		ImGui::Checkbox("Play Simulation", &simulate);
		if (ImGui::Button("Reset")) {
			click = true;
		}
		ImGui::InputFloat("Reset Time", &simTime);
		ImGui::InputFloat3("Gravity Accel", &gravity.x);
		if (ImGui::TreeNode("Spring parameters")) {
			ImGui::InputFloat2("k_stretch", &ke_G.x);
			ImGui::InputFloat2("k_shear", &ke_R.x);
			ImGui::InputFloat2("k_bend", &ke_B.x);
			ImGui::InputFloat("Particle Dist", &dist);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Collisions")) {
			ImGui::Checkbox("Use Collisions", &collisions);
			ImGui::Checkbox("Use Sphere Collider", &sphereCollisions);
			ImGui::InputFloat("Elastic Coef", &e);
			ImGui::InputFloat("Friction Coef", &f);
			ImGui::TreePop();
		}
		



	}
	// .........................

	ImGui::End();

	// Example code -- ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if (show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);

		ImGui::ShowTestWindow(&show_test_window);
	}
}



bool control = false;

void PhysicsInit() {
	// Do your initialization code here...
	// ...................................
	srand(time(NULL));
	int count = 0;
	x = -dist*8;
	z = -dist*8;

	for (int i = 0; i < 18; i++) {
		for (int j = 0; j < 14; j++) {
			Particle* p1 = new Particle();
			p1->pos.x = p1->lastPos.x = arr[count] = x + dist;
			p1->pos.y = p1->lastPos.y = arr[count + 1] = 9;
			p1->pos.z = p1->lastPos.z = arr[count + 2] = z + dist;
			z += dist;
			std::cout << arr[count] << " " << arr[count + 1] << " " << arr[count + 2] << std::endl;
			count += 3;
			p1->vel.x = p1->vel.y = p1->vel.z = 0;
			p1->lastVel.x = p1->lastVel.y = p1->lastVel.z = 0;

			particles[i][j] = p1;

		}
		x += dist;
		z = -dist*8;
	}

}

void PhysicsUpdate(float dt) {

	if (click) {
		click = false;
		timeManager = 0;
		PhysicsInit();
	}
	
	if (timeManager > simTime) {
		timeManager = 0;
		PhysicsInit();
	}
	std::cout << timeManager << std::endl;
	if (simulate) {
		timeManager += dt;
		glm::vec3 newPosE = { 0,0,0 };
		int count = 0;
		for (int i = 0; i < 18; i++) {
			for (int j = 0; j < 14; j++) {
				//VELOCIDAD
				glm::vec3 newVel = Particle::Velocity(particles[i][j]->pos, particles[i][j]->lastPos, dt);
				particles[i][j]->lastVel = particles[i][j]->vel;
				particles[i][j]->vel = newVel;

				//Pared en X=-5
				if (particles[i][j]->pos.x <= -5 && collisions) {
					glm::vec3 n = { 1,0,0 };
					glm::vec3 P = { -5,0,0 };
					float d = -(n.x*P.x + n.y*P.y + n.z*P.z);
					newPosE = Particle::ElasticForce(particles[i][j]->pos, e, d, n);
					glm::vec3 newVelE = Particle::ElasticVel(particles[i][j]->vel, e, n);
					particles[i][j]->lastVel = particles[i][j]->vel;
					particles[i][j]->vel = Particle::Friction(newVelE,particles[i][j]->lastVel,n,f);
				}

				//Pared en X=5
				else if (particles[i][j]->pos.x >= 5 && collisions) {
					glm::vec3 n = { -1,0,0 };
					glm::vec3 P = { 5,0,0 };
					float d = -(n.x*P.x + n.y*P.y + n.z*P.z);
					newPosE = Particle::ElasticForce(particles[i][j]->pos, e, d, n);
					glm::vec3 newVelE = Particle::ElasticVel(particles[i][j]->vel, e, n);
					particles[i][j]->lastVel = particles[i][j]->vel;
					particles[i][j]->vel = Particle::Friction(newVelE, particles[i][j]->lastVel, n, f);
				}

				//Pared en Y=-5
				else if (particles[i][j]->pos.y <= 0 && collisions) {
					glm::vec3 n = { 0,1,0 };
					glm::vec3 P = { 0,0,0 };
					float d = -(n.x*P.x + n.y*P.y + n.z*P.z);
					newPosE = Particle::ElasticForce(particles[i][j]->pos, e, d, n);
					glm::vec3 newVelE = Particle::ElasticVel(particles[i][j]->vel, e, n);
					particles[i][j]->lastVel = particles[i][j]->vel;
					particles[i][j]->vel = Particle::Friction(newVelE, particles[i][j]->lastVel, n, f);
				}

				//Pared en Y=5
				else if (particles[i][j]->pos.y >= 10 && collisions) {
					glm::vec3 n = { 0,1,0 };
					glm::vec3 P = { 0,10,0 };
					float d = -(n.x*P.x + n.y*P.y + n.z*P.z);
					newPosE = Particle::ElasticForce(particles[i][j]->pos, e, d, n);
					glm::vec3 newVelE = Particle::ElasticVel(particles[i][j]->vel, e, n);
					particles[i][j]->lastVel = particles[i][j]->vel;
					particles[i][j]->vel = Particle::Friction(newVelE, particles[i][j]->lastVel, n, f);
				}
				//Pared en Z=5
				else if (particles[i][j]->pos.z >= 5 && collisions) {
					glm::vec3 n = { 0,0,-1 };
					glm::vec3 P = { 0,0,5 };
					float d = -(n.x*P.x + n.y*P.y + n.z*P.z);
					newPosE = Particle::ElasticForce(particles[i][j]->pos, e, d, n);
					glm::vec3 newVelE = Particle::ElasticVel(particles[i][j]->vel, e, n);
					particles[i][j]->lastVel = particles[i][j]->vel;
					particles[i][j]->vel = Particle::Friction(newVelE, particles[i][j]->lastVel, n, f);
				}
				//Pared en Z=-5
				else if (particles[i][j]->pos.z <= -5 && collisions) {
					glm::vec3 n = { 0,0,1 };
					glm::vec3 P = { 0,0,-5 };
					float d = -(n.x*P.x + n.y*P.y + n.z*P.z);
					newPosE = Particle::ElasticForce(particles[i][j]->pos, e, d, n);
					glm::vec3 newVelE = Particle::ElasticVel(particles[i][j]->vel, e, n);
					particles[i][j]->lastVel = particles[i][j]->vel;
					particles[i][j]->vel = Particle::Friction(newVelE, particles[i][j]->lastVel, n, f);
				}


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
					vert1 = Particle::springForce(particles[i][j]->pos, particles[i - 1][j]->pos, particles[i][j]->vel, particles[i - 1][j]->vel,dist, ke_G.x, ke_G.y);
				}
				else {
					vert1 = { 0, 0, 0 };
				}
				if (i < 17) {
					vert3 = Particle::springForce(particles[i][j]->pos, particles[i + 1][j]->pos, particles[i][j]->vel, particles[i + 1][j]->vel, dist, ke_G.x, ke_G.y);
				}
				else {
					vert3 = { 0, 0, 0 };
				}
				if (j > 0) {
					vert4 = Particle::springForce(particles[i][j]->pos, particles[i][j - 1]->pos, particles[i][j]->vel, particles[i][j - 1]->vel, dist, ke_G.x, ke_G.y);
				}
				else {
					vert4 = { 0, 0, 0 };
				}
				if (j < 13) {
					vert2 = Particle::springForce(particles[i][j]->pos, particles[i][j + 1]->pos, particles[i][j]->vel, particles[i][j + 1]->vel, dist, ke_G.x, ke_G.y);
				}
				else {
					vert2 = { 0, 0, 0 };
				}

				//DIAGONALES
				if (i > 0 && j < 13) {
					vert5 = Particle::springForce(particles[i][j]->pos, particles[i - 1][j + 1]->pos, particles[i][j]->vel, particles[i - 1][j + 1]->vel, sqrt(pow(dist,2)*2), ke_R.x, ke_R.y);
				}
				else {
					vert5 = { 0, 0, 0 };
				}
				if (i < 17 && j < 13) {
					vert6 = Particle::springForce(particles[i][j]->pos, particles[i + 1][j + 1]->pos, particles[i][j]->vel, particles[i + 1][j + 1]->vel, sqrt(pow(dist, 2) * 2), ke_R.x, ke_R.y);
				}
				else {
					vert6 = { 0, 0, 0 };
				}
				if (i < 17 && j > 0) {
					vert7 = Particle::springForce(particles[i][j]->pos, particles[i + 1][j - 1]->pos, particles[i][j]->vel, particles[i + 1][j - 1]->vel, sqrt(pow(dist, 2) * 2), ke_R.x, ke_R.y);
				}
				else {
					vert7 = { 0, 0, 0 };
				}
				if (i > 0 && j > 0) {
					vert8 = Particle::springForce(particles[i][j]->pos, particles[i - 1][j - 1]->pos, particles[i][j]->vel, particles[i - 1][j - 1]->vel, sqrt(pow(dist, 2) * 2), ke_R.x, ke_R.y);
				}
				else {
					vert8 = { 0, 0, 0 };
				}

				//VERTICALES Y HORIZONTALES LARGAS
				if (i > 2) {
					vert9 = Particle::springForce(particles[i][j]->pos, particles[i - 2][j]->pos, particles[i][j]->vel, particles[i - 2][j]->vel, dist*2, ke_B.x, ke_B.y);
				}
				else {
					vert9 = { 0, 0, 0 };
				}
				if (j < 12) {
					vert10 = Particle::springForce(particles[i][j]->pos, particles[i][j + 2]->pos, particles[i][j]->vel, particles[i][j + 2]->vel, dist * 2, ke_B.x, ke_B.y);
				}
				else {
					vert10 = { 0, 0, 0 };
				}
				if (i < 16) {
					vert11 = Particle::springForce(particles[i][j]->pos, particles[i + 2][j]->pos, particles[i][j]->vel, particles[i + 2][j]->vel, dist * 2, ke_B.x, ke_B.y);
				}
				else {
					vert11 = { 0, 0, 0 };
				}
				if (j > 2) {
					vert12 = Particle::springForce(particles[i][j]->pos, particles[i][j - 2]->pos, particles[i][j]->vel, particles[i][j - 2]->vel, dist * 2, ke_B.x, ke_B.y);
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
					if (particles[i][j]->pos.x < -5 && collisions) {
						glm::vec3 sumF = (particles[i][j]->greenF[0] + particles[i][j]->greenF[1] + particles[i][j]->greenF[2] + particles[i][j]->greenF[3]) +
							(particles[i][j]->redF[0] + particles[i][j]->redF[1] + particles[i][j]->redF[2] + particles[i][j]->redF[3]) +
							(particles[i][j]->blueF[0] + particles[i][j]->blueF[1] + particles[i][j]->blueF[2] + particles[i][j]->blueF[3]) +
							gravity - newPosE;
						glm::vec3 newpos = Particle::VerletPos(particles[i][j]->pos, particles[i][j]->lastPos, sumF, dt);
						particles[i][j]->lastPos = particles[i][j]->pos;
						particles[i][j]->pos = newpos;
						//Limitador eje X
						particles[i][j]->pos.x = -5.f;
					}
					else if (particles[i][j]->pos.x > 5 && collisions) {
						glm::vec3 sumF = (particles[i][j]->greenF[0] + particles[i][j]->greenF[1] + particles[i][j]->greenF[2] + particles[i][j]->greenF[3]) +
							(particles[i][j]->redF[0] + particles[i][j]->redF[1] + particles[i][j]->redF[2] + particles[i][j]->redF[3]) +
							(particles[i][j]->blueF[0] + particles[i][j]->blueF[1] + particles[i][j]->blueF[2] + particles[i][j]->blueF[3]) +
							gravity - newPosE;
						glm::vec3 newpos = Particle::VerletPos(particles[i][j]->pos, particles[i][j]->lastPos, sumF, dt);
						particles[i][j]->lastPos = particles[i][j]->pos;
						particles[i][j]->pos = newpos;
						//Limitador eje X
						particles[i][j]->pos.x = 5.f;
					}
					else if (particles[i][j]->pos.y < 0 && collisions) {
						glm::vec3 sumF = (particles[i][j]->greenF[0] + particles[i][j]->greenF[1] + particles[i][j]->greenF[2] + particles[i][j]->greenF[3]) +
							(particles[i][j]->redF[0] + particles[i][j]->redF[1] + particles[i][j]->redF[2] + particles[i][j]->redF[3]) +
							(particles[i][j]->blueF[0] + particles[i][j]->blueF[1] + particles[i][j]->blueF[2] + particles[i][j]->blueF[3]) +
							gravity - newPosE;
						glm::vec3 newpos = Particle::VerletPos(particles[i][j]->pos, particles[i][j]->lastPos, sumF, dt);
						particles[i][j]->lastPos = particles[i][j]->pos;
						particles[i][j]->pos = newpos;
						//Limitador eje Y
						particles[i][j]->pos.y = 0.f;
					}
					else if (particles[i][j]->pos.y > 10 && collisions) {
						glm::vec3 sumF = (particles[i][j]->greenF[0] + particles[i][j]->greenF[1] + particles[i][j]->greenF[2] + particles[i][j]->greenF[3]) +
							(particles[i][j]->redF[0] + particles[i][j]->redF[1] + particles[i][j]->redF[2] + particles[i][j]->redF[3]) +
							(particles[i][j]->blueF[0] + particles[i][j]->blueF[1] + particles[i][j]->blueF[2] + particles[i][j]->blueF[3]) +
							gravity - newPosE;
						glm::vec3 newpos = Particle::VerletPos(particles[i][j]->pos, particles[i][j]->lastPos, sumF, dt);
						particles[i][j]->lastPos = particles[i][j]->pos;
						particles[i][j]->pos = newpos;
						//Limitador eje Y
						particles[i][j]->pos.y = 10.f;
					}
					else if (particles[i][j]->pos.z < -5 && collisions) {
						glm::vec3 sumF = (particles[i][j]->greenF[0] + particles[i][j]->greenF[1] + particles[i][j]->greenF[2] + particles[i][j]->greenF[3]) +
							(particles[i][j]->redF[0] + particles[i][j]->redF[1] + particles[i][j]->redF[2] + particles[i][j]->redF[3]) +
							(particles[i][j]->blueF[0] + particles[i][j]->blueF[1] + particles[i][j]->blueF[2] + particles[i][j]->blueF[3]) +
							gravity - newPosE;
						glm::vec3 newpos = Particle::VerletPos(particles[i][j]->pos, particles[i][j]->lastPos, sumF, dt);
						particles[i][j]->lastPos = particles[i][j]->pos;
						particles[i][j]->pos = newpos;
						//Limitador eje Z
						particles[i][j]->pos.z = -5.f;
					}
					else if (particles[i][j]->pos.z > 5 && collisions) {
						glm::vec3 sumF = (particles[i][j]->greenF[0] + particles[i][j]->greenF[1] + particles[i][j]->greenF[2] + particles[i][j]->greenF[3]) +
							(particles[i][j]->redF[0] + particles[i][j]->redF[1] + particles[i][j]->redF[2] + particles[i][j]->redF[3]) +
							(particles[i][j]->blueF[0] + particles[i][j]->blueF[1] + particles[i][j]->blueF[2] + particles[i][j]->blueF[3]) +
							gravity - newPosE;
						glm::vec3 newpos = Particle::VerletPos(particles[i][j]->pos, particles[i][j]->lastPos, sumF, dt);
						particles[i][j]->lastPos = particles[i][j]->pos;
						particles[i][j]->pos = newpos;
						//Limitador eje Z
						particles[i][j]->pos.x = 5.f;
					}
					else {
						if (i == 0 && j == 0) {}
						else if (i == 0 && j == 13) {}
						else {
							glm::vec3 sumF = (particles[i][j]->greenF[0] + particles[i][j]->greenF[1] + particles[i][j]->greenF[2] + particles[i][j]->greenF[3]) +
								(particles[i][j]->redF[0] + particles[i][j]->redF[1] + particles[i][j]->redF[2] + particles[i][j]->redF[3]) +
								(particles[i][j]->blueF[0] + particles[i][j]->blueF[1] + particles[i][j]->blueF[2] + particles[i][j]->blueF[3]) +
								gravity;
							glm::vec3 newpos = Particle::VerletPos(particles[i][j]->pos, particles[i][j]->lastPos, sumF, dt);
							particles[i][j]->lastPos = particles[i][j]->pos;
							particles[i][j]->pos = newpos;
						}

					}

				arr[count] = particles[i][j]->pos.x;
				arr[count + 1] = particles[i][j]->pos.y;
				arr[count + 2] = particles[i][j]->pos.z;
				count += 3;
			}
		}
	}
	ClothMesh::updateClothMesh(arr);

}

void PhysicsCleanup() {
	// Do your cleanup code here...
	// ............................
}