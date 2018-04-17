#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <deque>
#include<iostream>
#include<time.h>

glm::mat3 I;
glm::mat3 lastI{	1,0,0,
					0,1,0,
					0,0,1 };
glm::mat3 LastR = { 1,0,0,
					0,1,0,
					0,0,1};
glm::vec3 tor,AngMoment;
glm::vec3 LastAngMoment = { 0,0,0 };
glm::mat4 objMat, rotXMat,rotYMat,rotZMat;
glm::vec3 gravity = { 0,9.81,0 };
glm::vec4 position = { (rand() % 9) - 4, (rand() % 9) + 1, (rand() % 9) - 4, 1.f };
glm::vec4 lastX = { 0,0,0,1 };
glm::vec4 llMoment = { 0,0,0,1 };
float click = false;
float mass, w, h, d;
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
	glm::vec4 P;
	//angular momentum
	glm::vec3 L;
	float Mass;

	static glm::vec4 nextP(glm::vec4 P, float dt, glm::vec4 F);
	static glm::vec4 calcV(glm::vec4 P, float M);
	static glm::vec4 nextX(glm::vec4 x, float dt, glm::vec4 V);
	static glm::mat3 lastR(glm::mat3 lastR, glm::vec3 w, float dt);
	static glm::mat3 calcI(glm::mat3 R, float mass, float w, float h, float d);
	static glm::vec3 angularMoment(glm::vec3 L,float dt, glm::vec3 t);
	static glm::vec3 w(glm::vec3 L,glm::mat3 I);
	static glm::vec3 Torque(glm::vec3 Punto, glm::vec3 Pos, glm::vec3 F);

	RigidBody();
	~RigidBody();
private:
};

RigidBody::RigidBody() {
	P = { 0.0 , 0.0, 0.0,1 };
	x = { 0.0, 5.0, 0.0 };
	Mass = 5;
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

glm::mat3 RigidBody::lastR(glm::mat3 R, glm::vec3 w, float dt) {
	glm::mat3 wMat = { 0,-w.z,w.y,w.z,0,-w.x,-w.y,w.x,0 };
	return R + dt*(wMat*R);
}

glm::mat3 RigidBody::calcI(glm::mat3 R, float mass, float w, float h, float d) {
	glm::mat3 Ibody = { (1.f / 12.f)*mass*(pow(d,2) + pow(h,2)),0,0,0,(1.f / 12.f)*mass*(pow(w,2) + pow(d,2)),0,0,0,(1.f / 12.f)*mass*(pow(w,2) + pow(h,2)) };
	return R*glm::inverse(Ibody)*R;
}

glm::vec3 RigidBody::angularMoment(glm::vec3 L,float dt, glm::vec3 t) {
	return L + dt*t;
}
glm::vec3 RigidBody::w(glm::vec3 L, glm::mat3 I) {
	return glm::inverse(I)*L;
}

glm::vec3 RigidBody::Torque(glm::vec3 Punto, glm::vec3 Pos, glm::vec3 F) {
	glm::vec3 vector = Punto - Pos;
	return glm::normalize(vector * F);

}

float a = pow(2, 2);

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
	position = { (rand() % 9) - 4, (rand() % 9) + 1, (rand() % 9) - 4, 1.f };
	objMat = glm::mat4(1, 0, 0, 0.f,
		0, 1, 0, 0.f,
		0, 0, 1, 0.f,
		position.x, position.y, position.z, 1);
	lastX = position;
	mass = 0.5;
	llMoment = { 0, 0, 0, 1 };
	float randValX = rand();
	float randValY = rand();
	float randValZ = rand();
	rotXMat = glm::mat4(1, 0, 0, 0, 0, cos(randValX), sin(randValX), 0, 0, -sin(randValX), cos(randValX), 0, 0, 0, 0, 1);
	rotYMat = glm::mat4(cos(randValY), 0, -sin(randValY), 0, 0, 1, 0, 0, sin(randValY), 0, cos(randValY), 0, 0, 0, 0, 1);
	rotZMat = glm::mat4(cos(randValZ), sin(randValZ), 0, 0, -sin(randValZ), cos(randValZ), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	tor = RigidBody::Torque(glm::vec3((rand() % 10) / 10.f, (rand() % 10) / 10.f, (rand() % 10) / 10.f), position, glm::vec3(0, -9.81*mass, 0));
	AngMoment = RigidBody::angularMoment(glm::vec3(0.f, 0.f, 0.f), 0.f, tor);
	LastAngMoment = AngMoment;
	LastR = { 1,0,0,
		0,1,0,
		0,0,1 };
	lastI = { 1,0,0,
		0,1,0,
		0,0,1 };
}

void PhysicsUpdate(float dt) {
	//Momento Lineal
	glm::vec4 Mtmp = RigidBody::nextP(llMoment, dt, glm::vec4(0, -9.81*mass, 0, 0));
	glm::vec4 linealMoment = llMoment;
	llMoment = Mtmp;
	//Velocidad
	glm::vec4 velocity = RigidBody::calcV(linealMoment, mass);
	//Posicion
	glm::vec4 tmp = RigidBody::nextX(lastX, dt, velocity);
	position = lastX;
	lastX = tmp;
	//Momento Angular
	glm::vec3 AnMomnentTmp = RigidBody::angularMoment(LastAngMoment, dt, tor);
	AngMoment = LastAngMoment;
	LastAngMoment = AnMomnentTmp;
	//Inercia
	glm::mat3 Itmp = RigidBody::calcI(LastR, mass, 1.f, 1.f, 1.f);
	I = lastI;
	lastI = Itmp;
	

	objMat = glm::mat4(1, 0, 0, 0.f,
		0, 1, 0, 0.f,
		0, 0, 1, 0.f,
		position.x, position.y, position.z, 1);
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