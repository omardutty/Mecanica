#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <deque>
#include<iostream>
#include<time.h>
#include<glm\gtc\quaternion.hpp>



//VerticeActual
glm::vec4 vertice;
glm::vec4 vertice2;
glm::vec4 vertice3;
glm::vec4 vertice4;
glm::vec4 vertice5;
glm::vec4 vertice6;
glm::vec4 vertice7;
glm::vec4 vertice8;
//VerticeAnterior
glm::vec4 Lastvertice ;
glm::vec4 Lastvertice2;
glm::vec4 Lastvertice3;
glm::vec4 Lastvertice4;
glm::vec4 Lastvertice5;
glm::vec4 Lastvertice6;
glm::vec4 Lastvertice7;
glm::vec4 Lastvertice8;


glm::vec4 vertices[8];
glm::vec4 Lastvertices[8];
glm::mat3 I;
glm::mat3 lastI{	1,0,0,
					0,1,0,
					0,0,1 };
glm::mat3 LastR  { 1,0,0,
					0,1,0,
					0,0,1};
glm::quat LastRQ;
glm::vec3 tor,AngMoment;
glm::vec3 LastAngMoment = { 0,0,0 };
glm::mat4 objMat, rotXMat, rotYMat, rotZMat,rotMat;
glm::vec3 gravity = { 0,9.81,0 };
glm::vec4 position = { (rand() % 9) - 4, (rand() % 9) + 1, (rand() % 9) - 4, 1.f };
glm::vec4 lastX = { 0,0,0,1 };
glm::vec4 llMoment = { 0,0,0,1 };
float click = false;
float mass, w, h, d;
float Dplane,LastDplane;
glm::vec4 F;


glm::quat q;

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
	static glm::quat lastR(glm::mat3 lastR, glm::vec3 w, float dt);
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

glm::quat RigidBody::lastR(glm::mat3 R, glm::vec3 w, float dt) {
	glm::mat3 wMat = { 0,w.z,-w.y,-w.z,0,w.x,w.y,-w.x,0 };
	//return R + dt*(wMat*R);
	return glm::normalize(q + dt*(0.5f*(glm::quat(0, w)*q)));
}

glm::mat3 RigidBody::calcI(glm::mat3 R, float mass, float w, float h, float d) {
	glm::mat3 Ibody = { (1.f / 12.f)*mass*(pow(d,2) + pow(h,2)),0,0,0,(1.f / 12.f)*mass*(pow(w,2) + pow(d,2)),0,0,0,(1.f / 12.f)*mass*(pow(w,2) + pow(h,2)) };
	return R*glm::inverse(Ibody)*glm::transpose(R);
}

glm::vec3 RigidBody::angularMoment(glm::vec3 L,float dt, glm::vec3 t) {
	return L + dt*t;
}
glm::vec3 RigidBody::w(glm::vec3 L, glm::mat3 I) {
	return I*L;
}

glm::vec3 RigidBody::Torque(glm::vec3 Punto, glm::vec3 Pos, glm::vec3 F) {
	glm::vec3 vector = Punto - Pos;
	return glm::cross(vector, F);
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
	rotMat = glm::mat4(1.f);
	mass = 0.5;
	F = { 0,-9.81*mass,0,1 };
	lastX = position;
	llMoment = { 0, 0, 0, 1 };
	float randValX = rand();
	float randValY = rand();
	float randValZ = rand();
	rotXMat = glm::mat4(1, 0, 0, 0, 0, cos(randValX), sin(randValX), 0, 0, -sin(randValX), cos(randValX), 0, 0, 0, 0, 1);
	rotYMat = glm::mat4(cos(randValY), 0, -sin(randValY), 0, 0, 1, 0, 0, sin(randValY), 0, cos(randValY), 0, 0, 0, 0, 1);
	rotZMat = glm::mat4(cos(randValZ), sin(randValZ), 0, 0, -sin(randValZ), cos(randValZ), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	objMat = objMat*rotXMat*rotYMat*rotZMat;
	tor = RigidBody::Torque(glm::vec3((rand() % 10) / 10.f, (rand() % 10) / 10.f, (rand() % 10) / 10.f), position, glm::vec3(1,0,0));
	AngMoment = RigidBody::angularMoment(glm::vec3(0.f, 0.f, 0.f), 0.f, tor);
	LastAngMoment = AngMoment;
	LastR = rotXMat*rotYMat*rotZMat;
	LastRQ = glm::quat_cast(LastR);
	lastI = { 1,0,0,
		0,1,0,
		0,0,1 };
	q = glm::quat_cast(LastR);
	//VerticeActual
	vertice = (glm::vec4(position.x - 1, position.y + 1, position.z + 1, 1)*rotMat);
	vertice2 = (glm::vec4(position.x - 1, position.y + 1, position.z - 1, 1)*rotMat);
	vertice3 = (glm::vec4(position.x + 1, position.y + 1, position.z + 1, 1)*rotMat);
	vertice4 = (glm::vec4(position.x + 1, position.y + 1, position.z - 1, 1)*rotMat);
	vertice5 = (glm::vec4(position.x - 1, position.y - 1, position.z + 1, 1)*rotMat);
	vertice6 = (glm::vec4(position.x - 1, position.y + 1, position.z - 1, 1)*rotMat);
	vertice7 = (glm::vec4(position.x + 1, position.y - 1, position.z + 1, 1)*rotMat);
	vertice8 = (glm::vec4(position.x + 1, position.y + 1, position.z - 1, 1)*rotMat);
	//VerticeAnterior
	Lastvertice = (glm::vec4(position.x - 1, position.y + 1, position.z + 1, 1)*rotMat);
	Lastvertice2 = (glm::vec4(position.x - 1, position.y + 1, position.z - 1, 1)*rotMat);
	Lastvertice3 = (glm::vec4(position.x + 1, position.y + 1, position.z + 1, 1)*rotMat);
	Lastvertice4 = (glm::vec4(position.x + 1, position.y + 1, position.z - 1, 1)*rotMat);
	Lastvertice5 = (glm::vec4(position.x - 1, position.y - 1, position.z + 1, 1)*rotMat);
	Lastvertice6 = (glm::vec4(position.x - 1, position.y + 1, position.z - 1, 1)*rotMat);
	Lastvertice7 = (glm::vec4(position.x + 1, position.y - 1, position.z + 1, 1)*rotMat);
	Lastvertice8 = (glm::vec4(position.x + 1, position.y + 1, position.z - 1, 1)*rotMat);
	//distance
	Dplane = abs(position.y - 0);
	std::cout << "D" << Dplane << std::endl;
	std::cout << "vertice : x" << vertice.x << " y" << vertice.y << " z" << vertice.z << std::endl;

	std::cout << "Position: x" << position.x << " y" << position.y << " z" << position.z << std::endl;
		//vertices[8] = /*Lastvertices[8] =*/ (	glm::vec4(position.x - 1,position.y + 1,position.z + 1,1)/**rotMat*/,glm::vec4(position.x - 1,position.y + 1,position.z - 1,1)/**rotMat*/,
		//									glm::vec4(position.x + 1,position.y + 1,position.z + 1,1)/**rotMat*/,glm::vec4(position.x + 1,position.y + 1,position.z - 1,1)/**rotMat*/,
		//									glm::vec4(position.x - 1,position.y - 1,position.z + 1,1)/**rotMat*/,glm::vec4(position.x - 1,position.y + 1,position.z - 1,1)/**rotMat*/,
		//									glm::vec4(position.x + 1,position.y - 1,position.z + 1,1)/**rotMat*/,glm::vec4(position.x + 1,position.y + 1,position.z - 1,1)/**rotMat*/ );	
		//for (int i = 0; i < 8; i++) {
		//	std::cout << i << ": x" << vertices[i].x << " y" << vertices[i].y << " z" << vertices[i].z << std::endl;
		//}
}

void PhysicsUpdate(float dt) {

	//Momento Lineal
	glm::vec4 Mtmp = RigidBody::nextP(llMoment, dt,F);
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

	//velocidad angular
	glm::vec3 omega = RigidBody::w(LastAngMoment, lastI);

	//Rotation Matrix
	glm::quat tmpMat = RigidBody::lastR(LastR, omega, dt);
	q = LastRQ;
	LastRQ = tmpMat;

	LastR = glm::mat4_cast(LastRQ);
	glm::mat4 rotationMatrix = glm::mat4_cast(q);


	/*for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			std::cout << rotationMatrix[i][j] << std::endl;
		}
	}
	std::cout << "endMatrix" << std::endl;*/


	objMat = glm::mat4(objMat[0][0], objMat[0][1], objMat[0][2], objMat[0][3],
		objMat[1][0], objMat[1][1], objMat[1][2], objMat[1][3],
		objMat[2][0], objMat[2][1], objMat[2][2], objMat[2][3],
		position.x, position.y, position.z, objMat[3][3]);

	rotMat = glm::mat4(rotationMatrix[0][0], rotationMatrix[0][1], rotationMatrix[0][2], objMat[0][3],
		rotationMatrix[1][0], rotationMatrix[1][1], rotationMatrix[1][2], objMat[1][3],
		rotationMatrix[2][0], rotationMatrix[2][1], rotationMatrix[2][2], objMat[2][3],
		0,0,0,1);
	LastDplane = Dplane;
	Dplane = abs(position.y - 0);
	//VerticeAnterior
	Lastvertice = vertice;
	Lastvertice2 = vertice2;
	Lastvertice3 = vertice3;
	Lastvertice4 = vertice4;
	Lastvertice5 = vertice5;
	Lastvertice6 = vertice6;
	Lastvertice7 = vertice7;
	Lastvertice8 = vertice8;
	//VerticeActual
	vertice = (glm::vec4(position.x - 0.5f, position.y - 0.5f, position.z - 0.5f, 1)/**rotMat*/);
	vertice2 = (glm::vec4(position.x - 0.5f, position.y - 0.5f, position.z + 0.5f, 1)/**rotMat*/);
	vertice3 = (glm::vec4(position.x + 0.5f, position.y - 0.5f, position.z + 0.5f, 1)/**rotMat*/);
	vertice4 = (glm::vec4(position.x + 0.5f, position.y - 0.5f, position.z - 0.5f, 1)/**rotMat*/);
	vertice5 = (glm::vec4(position.x - 0.5f, position.y + 0.5f, position.z - 0.5f, 1)/**rotMat*/);
	vertice6 = (glm::vec4(position.x - 0.5f, position.y + 0.5f, position.z + 0.5f, 1)/**rotMat*/);
	vertice7 = (glm::vec4(position.x + 0.5f, position.y + 0.5f, position.z + 0.5f, 1)/**rotMat*/);
	vertice8 = (glm::vec4(position.x + 0.5f, position.y + 0.5f, position.z - 0.5f, 1)/**rotMat*/);
	

	float timeAux = dt;
	//Comprobación de colision de los vertices

	/*for (int i = 0; i <= 8; ++i) {
*/
		float epsilon = 0.1;
		std::cout << vertice.y << std::endl;
		//SUELO
		//(n·p+d)*(n·pA+d)
		/*float dot = (glm::dot(glm::vec3(0, 1, 0), (glm::vec3)vertice) + Dplane) * (glm::dot(glm::vec3(0, 1, 0), (glm::vec3)Lastvertice) + Dplane);

		if (dot <= 0) {*/
		if(vertice.y <=0){
			glm::vec4 posAux;
			while (vertice.y > epsilon && vertice.y < -epsilon) {
				timeAux /= 2;
				posAux = RigidBody::nextX(lastX, timeAux, velocity);
			}
			//vrel calc
			glm::vec4 mlAux = RigidBody::nextP(llMoment, timeAux, F);
			glm::vec4 Va = RigidBody::calcV(mlAux, mass);
			glm::vec3 amAux = RigidBody::angularMoment(LastAngMoment, timeAux, tor);
			glm::vec3 omegaAux = RigidBody::w(amAux, lastI);
			glm::vec3 pad = (glm::vec3)Va + glm::cross(omegaAux, ((glm::vec3)vertice - (glm::vec3)posAux));
			float Vrel = glm::dot(glm::vec3(0, 1, 0), pad);
			float j = (-(1.f + 0.5f)*Vrel) / ((1 / mass) + glm::dot(glm::vec3(0, 1, 0), glm::cross((glm::vec3)vertice, lastI * (glm::cross((glm::vec3)vertice, glm::vec3(0, 1, 0))))));
			glm::vec3 J = j * glm::vec3(0, 1, 0);
			glm::vec3 torqueImpulse = glm::cross((glm::vec3)vertice, J);

			(glm::vec3)vertice = (glm::vec3)Lastvertice + J;
			LastAngMoment = AngMoment + torqueImpulse;
		}
	/*}*/

	//for (int i = 0; i <= 8; ++i) {

	//	float epsilon = 0.1;

	//	//SUELO
	//	//(n·p+d)*(n·pA+d)
	//	float dot = (glm::dot(glm::vec3(0, 1, 0), (glm::vec3)vertices[i]) + Dplane) * (glm::dot(glm::vec3(0, 1, 0), (glm::vec3)Lastvertices[i]) + Dplane);

	//	if (dot <= 0) {
	//		glm::vec4 posAux;
	//		while (vertices[i].y <= epsilon && vertices[i].y > -epsilon) {
	//			timeAux /= 2;
	//			 posAux = RigidBody::nextX(lastX, timeAux, velocity);
	//		}
	//		//vrel calc
	//		glm::vec4 mlAux = RigidBody::nextP(llMoment, timeAux, F);
	//		glm::vec4 Va = RigidBody::calcV(mlAux, mass);
	//		glm::vec3 amAux = RigidBody::angularMoment(LastAngMoment, timeAux, tor);
	//		glm::vec3 omegaAux = RigidBody::w(amAux, lastI);
	//		glm::vec3 pad = (glm::vec3)Va + glm::cross(omegaAux, ((glm::vec3)vertices[i] - (glm::vec3)posAux));
	//		float Vrel = glm::dot(glm::vec3(0, 1, 0), pad);
	//		float j = (-(1.f + 0.5f)*Vrel) / ((1 / mass) + glm::dot(glm::vec3(0, 1, 0), glm::cross((glm::vec3)vertices[i], lastI * (glm::cross((glm::vec3)vertices[i], glm::vec3(0, 1, 0))))));
	//		glm::vec3 J = j*glm::vec3(0, 1, 0);
	//		glm::vec3 torqueImpulse = glm::cross((glm::vec3)vertices[i], J);

	//		(glm::vec3)vertices[i] = (glm::vec3)Lastvertices[i] + J;
	//		LastAngMoment = AngMoment + torqueImpulse;
	//	}
	#pragma region
			//TECHO
			//(n·p+d)*(n·pA+d)
			//float dot = (glm::dot(glm::vec3(0, -1, 0), glm::vec3(vertices[i])) + Dplane) * (glm::dot(glm::vec3(0, -1, 0), glm::vec3(Lastvertices[i])) + Dplane);

			//if (dot <= 0) {
			//	while (vertices[i].y >= epsilon && vertices[i].y > -epsilon) {
			//		timeAux /= 2;
			//		glm::vec4 posAux = RigidBody::nextX(lastX, timeAux, velocity);
			//	}
			//	//vrel calc
			//	glm::vec4 mlAux = RigidBody::nextP(llMoment, timeAux, F);
			//	glm::vec4 Va = RigidBody::calcV(mlAux, mass);
			//	glm::vec3 amAux = RigidBody::angularMoment(LastAngMoment, timeAux, tor);
			//	glm::vec3 omegaAux = RigidBody::w(amAux, lastI);
			//	glm::vec3 pad = glm::vec3(Va) + glm::cross(omegaAux, (glm::vec3(vertices[i] - position)));
			//	float Vrel = glm::dot(glm::vec3(0, -1, 0), pad);
			//	float j = -(1.f + 0.5f)*Vrel / 1 / mass + glm::dot(glm::vec3(0, -1, 0), glm::cross(glm::vec3(vertices[i]), lastI * (glm::cross(glm::vec3(vertices[i]), glm::vec3(0, -1, 0)))));
			//	glm::vec3 J = j*glm::vec3(0, -1, 0);
			//	glm::vec3 torqueImpulse = glm::cross(glm::vec3(vertices[i]), J);

			//	glm::vec3(vertices[i].y) = glm::vec3(Lastvertices[i].y) + J.y;
			//	LastAngMoment = AngMoment + torqueImpulse;

			//}
			//PARED IZQ
			//(n·p+d)*(n·pA+d)
			//float dot = (glm::dot(glm::vec3(0, 0, 1), glm::vec3(vertices[i])) + Dplane) * (glm::dot(glm::vec3(0, 0, 1), glm::vec3(Lastvertices[i])) + Dplane);

			//if (dot <= 0) {
			//	while (vertices[i].y >= epsilon && vertices[i].y > -epsilon) {
			//		timeAux /= 2;
			//		glm::vec4 posAux = RigidBody::nextX(lastX, timeAux, velocity);
			//	}
			//	//vrel calc
			//	glm::vec4 mlAux = RigidBody::nextP(llMoment, timeAux, F);
			//	glm::vec4 Va = RigidBody::calcV(mlAux, mass);
			//	glm::vec3 amAux = RigidBody::angularMoment(LastAngMoment, timeAux, tor);
			//	glm::vec3 omegaAux = RigidBody::w(amAux, lastI);
			//	glm::vec3 pad = glm::vec3(Va) + glm::cross(omegaAux, (glm::vec3(vertices[i] - position)));
			//	float Vrel = glm::dot(glm::vec3(0, 0, 1), pad);
			//	float j = -(1.f + 0.5f)*Vrel / 1 / mass + glm::dot(glm::vec3(0, 0, 1), glm::cross(glm::vec3(vertices[i]), lastI * (glm::cross(glm::vec3(vertices[i]), glm::vec3(0, 0, 1)))));
			//	glm::vec3 J = j*glm::vec3(0, 0, 1);
			//	glm::vec3 torqueImpulse = glm::cross(glm::vec3(vertices[i]), J);

			//	glm::vec3(vertices[i].y) = glm::vec3(Lastvertices[i].y) + J.y;
			//	LastAngMoment = AngMoment + torqueImpulse;
			//}

			//LADO DERECHO
			//(n·p+d)*(n·pA+d)
			//float dot = (glm::dot(glm::vec3(0, 0, -1), glm::vec3(vertices[i])) + Dplane) * (glm::dot(glm::vec3(0, 0, -1), glm::vec3(Lastvertices[i])) + Dplane);

			//if (dot <= 0) {
			//	while (vertices[i].y >= epsilon && vertices[i].y > -epsilon) {
			//		timeAux /= 2;
			//		glm::vec4 posAux = RigidBody::nextX(lastX, timeAux, velocity);
			//	}
			//	//vrel calc
			//	glm::vec4 mlAux = RigidBody::nextP(llMoment, timeAux, F);
			//	glm::vec4 Va = RigidBody::calcV(mlAux, mass);
			//	glm::vec3 amAux = RigidBody::angularMoment(LastAngMoment, timeAux, tor);
			//	glm::vec3 omegaAux = RigidBody::w(amAux, lastI);
			//	glm::vec3 pad = glm::vec3(Va) + glm::cross(omegaAux, (glm::vec3(vertices[i] - position)));
			//	float Vrel = glm::dot(glm::vec3(0, 0, -1), pad);
			//	float j = -(1.f + 0.5f)*Vrel / 1 / mass + glm::dot(glm::vec3(0, 0, -1), glm::cross(glm::vec3(vertices[i]), lastI * (glm::cross(glm::vec3(vertices[i]), glm::vec3(0, 0, -1)))));
			//	glm::vec3 J = j*glm::vec3(0, 0, -1);
			//	glm::vec3 torqueImpulse = glm::cross(glm::vec3(vertices[i]), J);

			//	glm::vec3(vertices[i].y) = glm::vec3(Lastvertices[i].y) + J.y;
			//	LastAngMoment = AngMoment + torqueImpulse;
			//}

			//LADO FRENTE
			//(n·p+d)*(n·pA+d)
			//float dot = (glm::dot(glm::vec3(1, 0, 0), glm::vec3(vertices[i])) + Dplane) * (glm::dot(glm::vec3(1, 0, 0), glm::vec3(Lastvertices[i])) + Dplane);

			//if (dot <= 0) {
			//	while (vertices[i].y >= epsilon && vertices[i].y > -epsilon) {
			//		timeAux /= 2;
			//		glm::vec4 posAux = RigidBody::nextX(lastX, timeAux, velocity);
			//	}
			//	//vrel calc
			//	glm::vec4 mlAux = RigidBody::nextP(llMoment, timeAux, F);
			//	glm::vec4 Va = RigidBody::calcV(mlAux, mass);
			//	glm::vec3 amAux = RigidBody::angularMoment(LastAngMoment, timeAux, tor);
			//	glm::vec3 omegaAux = RigidBody::w(amAux, lastI);
			//	glm::vec3 pad = glm::vec3(Va) + glm::cross(omegaAux, (glm::vec3(vertices[i] - position)));
			//	float Vrel = glm::dot(glm::vec3(1, 0, 0), pad);
			//	float j = -(1.f + 0.5f)*Vrel / 1 / mass + glm::dot(glm::vec3(1, 0, 0), glm::cross(glm::vec3(vertices[i]), lastI * (glm::cross(glm::vec3(vertices[i]), glm::vec3(1, 0, 0)))));
			//	glm::vec3 J = j*glm::vec3(1, 0, 0);
			//	glm::vec3 torqueImpulse = glm::cross(glm::vec3(vertices[i]), J);

			//	glm::vec3(vertices[i].y) = glm::vec3(Lastvertices[i].y) + J.y;
			//	LastAngMoment = AngMoment + torqueImpulse;
			//}

			//LADO ESPALDA
			//(n·p+d)*(n·pA+d)
			//float dot = (glm::dot(glm::vec3(-1, 0, 0), glm::vec3(vertices[i])) + Dplane) * (glm::dot(glm::vec3(-1, 0, 0), glm::vec3(Lastvertices[i])) + Dplane);

			//if (dot <= 0) {
			//	while (vertices[i].y >= epsilon && vertices[i].y > -epsilon) {
			//		timeAux /= 2;
			//		glm::vec4 posAux = RigidBody::nextX(lastX, timeAux, velocity);
			//	}
			//	//vrel calc
			//	glm::vec4 mlAux = RigidBody::nextP(llMoment, timeAux, F);
			//	glm::vec4 Va = RigidBody::calcV(mlAux, mass);
			//	glm::vec3 amAux = RigidBody::angularMoment(LastAngMoment, timeAux, tor);
			//	glm::vec3 omegaAux = RigidBody::w(amAux, lastI);
			//	glm::vec3 pad = glm::vec3(Va) + glm::cross(omegaAux, (glm::vec3(vertices[i] - position)));
			//	float Vrel = glm::dot(glm::vec3(-1, 0, 0), pad);
			//	float j = -(1.f + 0.5f)*Vrel / 1 / mass + glm::dot(glm::vec3(1, 0, 0), glm::cross(glm::vec3(vertices[i]), lastI * (glm::cross(glm::vec3(vertices[i]), glm::vec3(-1, 0, 0)))));
			//	glm::vec3 J = j*glm::vec3(-1, 0, 0);
			//	glm::vec3 torqueImpulse = glm::cross(glm::vec3(vertices[i]), J);

			//	glm::vec3(vertices[i].y) = glm::vec3(Lastvertices[i].y) + J.y;
			//	LastAngMoment = AngMoment + torqueImpulse;
			//}
#pragma endregion
	/*}*/

	if (click) {
		PhysicsInit();
		click = false;
	}
	Cube::updateCube(objMat*rotMat);
}

	void PhysicsCleanup() {
		// Do your cleanup code here...
		// ............................
	}