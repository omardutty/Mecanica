#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <deque>
#include<iostream>
#include<time.h>


glm::mat4 objMat;
glm::vec3 gravity = { 0,9.81,0 };
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

	static glm::vec3 nextP(glm::vec3 P, float dt, glm::vec3 F);
	static glm::vec3 calcV(glm::vec3 P, float M);
	static glm::vec3 nextX(glm::vec3 x, float dt, glm::vec3 V);

	RigidBody();
	~RigidBody();
};

RigidBody::RigidBody() {
	P = { 0.0 , 0.0, 0.0 };
	x = { 0.0, 5.0, 0.0 };
	Mass = 0.5;
}

glm::vec3 RigidBody::nextP(glm::vec3 P, float dt, glm::vec3 F) {
	return P + dt*F;
}

glm::vec3 RigidBody::calcV(glm::vec3 P, float M) {
	return P / M;
}

glm::vec3 RigidBody::nextX(glm::vec3 x, float dt, glm::vec3 V) {
	return x + dt*V;
}

bool show_test_window = false;
void GUI() {
	//bool show = false;
	//ImGui::Begin("Physics Parameters", &show, 0);
	//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);//FrameRate
	//																														  // Do your GUI code here....
	//{
	//	ImGui::Checkbox("Play Simulation", &simulate);
	//	if (ImGui::Button("Reset")) {
	//		click = true;
	//	}
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
	//}
	// .........................
	//ImGui::End();
	//// Example code -- ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	//if (show_test_window) {
	//	ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
	//	ImGui::ShowTestWindow(&show_test_window);
	//}
}



//bool control = false;

void PhysicsInit() {
	RigidBody


}

float i = 0;
void PhysicsUpdate(float dt) {
	RigidBody::nextP();
	RigidBody::calcV();
	RigidBody::nextX();

	objMat = glm::mat4(1.0f, 0.f, 0.f, 0.f,
						0.0f, 1.f, 0.f, 0.f,
						0.0f, 0.f, 1.f, 0.f,
						0.0f, i, 0.f, 1.f);
	Cube::updateCube(objMat);
	i += 0.015f;
}

void PhysicsCleanup() {
	// Do your cleanup code here...
	// ............................
}