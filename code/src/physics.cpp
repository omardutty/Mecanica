#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
namespace LilSpheres {
	extern const int maxParticles;
	extern void updateParticles(int startIdx, int count, float* array_data);
}

float posParticula[3];
float arr[30];

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
	if(show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

void PhysicsInit() {
	// Do your initialization code here...
	// ...................................
	posParticula[0] = 4;
	posParticula[1] = 2;
	posParticula[2] = 3;
	for (int i);

}

void PhysicsUpdate(float dt) {
	// Do your update code here...
	// ...........................

	posParticula[1] += 0.01;



	LilSpheres::updateParticles(0, 3, posParticula);
}

void PhysicsCleanup() {
	// Do your cleanup code here...
	// ............................
}