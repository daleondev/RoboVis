#include "pch.h"

#include "ImGuiLayer.h"
#include "Scene.h"

#include "Window/Window.h"

#include "Util/Log.h"
#include "Util/geometry.h"

std::vector<std::tuple<float, float, float>> ImGuiLayer::s_sliders;
bool ImGuiLayer::s_bbActive;
bool ImGuiLayer::s_framesActive = true;
std::pair<uint16_t, uint16_t> ImGuiLayer::s_viewportSize;

void ImGuiLayer::init()
{
	s_viewportSize = {Window::getWidth(), Window::getHeight()};

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui::StyleColorsLight();

	ImGui_ImplGlfw_InitForOpenGL(Window::getNativeWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 300 es");

    io.Fonts->AddFontDefault();
    io.FontDefault = io.Fonts->AddFontFromFileTTF("/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf", 18.0f);

    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_CreateFontsTexture();
}

void ImGuiLayer::shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiLayer::render(const Timestep dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

    dockSpace([](const ImGuiID dockspaceId) {
		settings(dockspaceId);
		viewport(dockspaceId);
	});

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)Window::getWidth(), (float)Window::getHeight());

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		auto window = Window::getNativeWindow();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(window);
	}
}

void ImGuiLayer::dockSpace(const std::function<void(const ImGuiID)>& dockspaceContent)
{
    static constexpr ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None; //ImGuiDockNodeFlags_NoUndocking;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		windowFlags |= ImGuiWindowFlags_NoBackground;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Window", nullptr, windowFlags);
	ImGui::PopStyleVar(3);

	const ImGuiID dockspaceId = ImGui::GetID("Dockspace");
	ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);
	dockspaceContent(dockspaceId);

	ImGui::End();
}

void ImGuiLayer::settings(const ImGuiID dockspaceId)
{
	// ImGui::SetNextWindowDockID(dockspaceId, ImGuiCond_Always);

    ImGui::Begin("Settings");

	ImGui::Text("Joint values:", "");
	for (size_t i = 0; i < s_sliders.size(); ++i) {
		auto&[val, min, max] = s_sliders[i];
		ImGui::SliderAngle(("Joint " + std::to_string(i+1)).c_str(), &val, rad2deg(min), rad2deg(max));
	}
	ImGui::Separator();
	ImGui::Checkbox("Frames", &s_framesActive);
	ImGui::Checkbox("Bounding Boxes", &s_bbActive);

	ImGui::End();
}

void ImGuiLayer::viewport(const ImGuiID dockspaceId)
{
	const auto convSz = [](const ImVec2& vec) -> std::pair<uint16_t, uint16_t> {
		return { static_cast<uint16_t>(vec.x), static_cast<uint16_t>(vec.y) };
	};

	// ImGui::SetNextWindowDockID(dockspaceId, ImGuiCond_Always);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport");
	ImGui::PopStyleVar(3);

	const auto size = convSz(ImGui::GetContentRegionAvail());
	if (size != s_viewportSize && size.first > 0 && size.second > 0) {
		Scene::getFrameBuffer()->resize(size.first, size.second);
		s_viewportSize = size;
		CameraController::onResize();
	}
	ImGui::Image(Scene::getFrameBuffer()->getColorAttachment(), ImVec2(s_viewportSize.first, s_viewportSize.second), ImVec2(0, 1), ImVec2(1, 0));

	ImGui::End();
}