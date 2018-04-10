#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <deque>
#include<iostream>
#include<time.h>


glm::mat4 objMat, rotXMat,rotYMat,rotZMat;
glm::vec3 gravity = { 0,9.81,0 };
float click = false;
namespace Cube{
	void updateCube(const glm::mat4& transform);
}
class RigidBody {
public:
	//position
	glm::vec3 x;

	//orientation
	glm::mat4 R;

	//linear momentum
	glm::vec3 P;

	//angular momentum
	glm::vec3 L;

	float Mass;

	static glm::vec4 nextP(glm::vec4 P, float dt, glm::vec4 F);
	static glm::vec4 calcV(glm::vec4 P, float M);
	static glm::vec4 nextX(glm::vec4 x, float dt, glm::vec4 V);

	RigidBody();
	~RigidBody();
private:
};

RigidBody::RigidBody() {
	P = { 0.0 , 0.0, 0.0 };
	x = { 0.0, 5.0, 0.0 };
	Mass = 0.5;
}

glm::vec4 RigidBody::nextP(glm::vec4 P, float dt, glm::vec4 F) {
	return P + dt*F;
}

glm::vec4 RigidBody::calcV(glm::vec4 P, float M) {
	return P / M;
}

glm::vec4 RigidBody::nextX(glm::vec4 x, float dt, glm::vec4 V) {
	return x + dt*V;
}

bool show_test_window = false;
void GUI() {
	bool show = false;
	ImGui::Begin("Parameters", &show, 0);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);//FrameRate
	//																														  // Do your GUI code here....
	{
		//ImGui::Checkbox("Play Simulation", &simulate);
		if (ImGui::Button("Reset")) {
			click = true;
		}
	//	ImGui::InputFloat("Reset Time", &simTime);
	//	ImGui::InputFloat3("Gravity Accel", &gravity.x);
	//	if (ImGui::TreeNode("Spring parameters")) {
	//		ImGui::InputFloat2("k_stretch", &ke_G.x);
	//		ImGui::InputFloat2("k_shear", &ke_R.x);
	//		ImGui::InputFloat2("k_bend", &ke_B.x);
	//		ImGui::InputFloat("Particle Dist", &dist);
	//		ImGui::TreePop();
	//	}
	//	if (ImGui::TreeNode("Collisions")) {
	//		ImGui::Checkbox("Use Collisions", &collisions);
	//		ImGui::Checkbox("Use Sphere Collider", &sphereCollisions);
	//		ImGui::InputFloat("Elastic Coef", &e);
	//		ImGui::InputFloat("Friction Coef", &f);
	//		ImGui::TreePop();
	//	}*/
	//	
	}
	// .........................
	ImGui::End();
	//// Example code -- ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	//if (show_test_window) {
	//	ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
	//	ImGui::ShowTestWindow(&show_test_window);
	//}
}

float t = 0;

//bool control = false;

void PhysicsInit() {
	//RigidBody
	srand(time(NULL));
	objMat = glm::mat4(1, 0, 0, 0.f,
		0, 1, 0, 0.f,
		0, 0, 1, 0.f,
		(rand() % 9) - 4, (rand() % 9) + 1, (rand() % 9) - 4, 1.f);
	float randValX = rand();
	float randValY = rand();
	float randValZ = rand();
	rotXMat = glm::mat4(1, 0, 0, 0, 0, cos(randValX), sin(randValX), 0, 0, -sin(randValX), cos(randValX), 0, 0, 0, 0, 1);
	rotYMat = glm::mat4(cos(randValY), 0, -sin(randValY), 0, 0, 1, 0, 0, sin(randValY), 0, cos(randValY), 0, 0, 0, 0, 1);
	rotZMat = glm::mat4(cos(randValZ), sin(randValZ), 0, 0, -sin(randValZ), cos(randValZ), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

void PhysicsUpdate(float dt) {
	//glm::vec3 linealMoment = RigidBody::nextP(glm::vec4(objMat[12],objMat[13],objMat[14],1),dt,glm::vec4(0,0,0,1));
	glm::vec4 velocity = RigidBody::calcV(glm::vec4((rand() % 9) - 4, (rand() % 9) + 1, (rand() % 9) - 4,1), 0.5);
	glm::vec4 nextPos = RigidBody::nextX(glm::vec4(0,0.5,0,1),dt,velocity);
	/*glm::vec3 nextPos = { 0,0,0 };*/
	objMat = glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, nextPos.x,nextPos.y,nextPos.z, 1);
	/*if (t > 1) {
		click = true;
		t = 0;
	}
	t += 0.1;*/
	if (click) {
		PhysicsInit();
		click = false;
	}
	Cube::updateCube(objMat*rotXMat*rotYMat*rotZMat);
}

	void PhysicsCleanup() {
		// Do your cleanup code here...
		// ............................
	}