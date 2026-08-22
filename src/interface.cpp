
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "glm/gtc/type_ptr.hpp"

#include "interface.h"

void initUI( GLFWwindow* window ){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL( window, true );
    ImGui_ImplOpenGL3_Init( "#version 460" );
}

void startUIFrame(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void renderUI( SpotLight& light, float& occlusionStrength, float& absorption, float& scattering, float& lpvStrength ){
    ImGui::Begin( "Configure" );
    ImGui::SliderFloat3( "Light Position", glm::value_ptr( light.position ), -15.0f, 15.0f );
    ImGui::ColorEdit3( "Light Color", glm::value_ptr( light.color ) );
    ImGui::SliderFloat( "Light Strength", &light.strength, 0.0f, 5.0f );
    ImGui::SliderFloat( "Occlusion Strength", &occlusionStrength, 0.0f, 5.0f );
    ImGui::SliderFloat( "Absorption", &absorption, 0.0f, 1.0f );
    ImGui::SliderFloat( "Scattering", &scattering, 0.0f, 1.0f );
    ImGui::SliderFloat( "LPV Strength", &lpvStrength, 0.0f, 2.0f );
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
}