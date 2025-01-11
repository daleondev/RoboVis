#include "pch.h"

#include "ImGuiLayer.h"
#include "Panels/ScenePanel.h"

#include "Scene/Scene.h"
#include "Scene/Camera.h"

#include "Window/Window.h"

#include "Entities/Robot.h"

#include "Util/Log.h"
#include "Util/geometry.h"

std::pair<uint16_t, uint16_t> ImGuiLayer::s_viewportSize;
glm::vec2 ImGuiLayer::s_viewportPos;
bool ImGuiLayer::s_viewportHovered;
bool ImGuiLayer::s_viewportFocused;
EdgeDetector<float> ImGuiLayer::m_sliderTime;
EdgeDetector<bool> ImGuiLayer::m_buttonPlay;

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

	TextureLibrary::load("/home/david/Downloads/pause-play.png", "image");
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
		viewport(dockspaceId);
		robotControls(dockspaceId);
		ScenePanel::render(dockspaceId);
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

void ImGuiLayer::onEvent(Event& e)
{
	ImGuiIO& io = ImGui::GetIO();
	if (!isViewportHovered())
		e.setHandled(e.isHandled() | e.isInCategory(EventCategory::Mouse) & io.WantCaptureMouse);
	if (!isViewportFocused())
		e.setHandled(e.isHandled() | e.isInCategory(EventCategory::Keyboard) & io.WantCaptureKeyboard);
}

glm::vec2 ImGuiLayer::screenToViewport(const glm::vec2& screenPos)
{
	return screenPos - s_viewportPos;
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

void ImGuiLayer::viewport(const ImGuiID dockspaceId)
{
	const auto convertSize = [](const ImVec2& vec) -> std::pair<uint16_t, uint16_t> {
		return { static_cast<uint16_t>(vec.x), static_cast<uint16_t>(vec.y) };
	};

	// --------------------------------------

	ImGui::SetNextWindowDockID(dockspaceId, ImGuiCond_Always);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoMove);
	ImGui::PopStyleVar(3);

	auto dockNode = ImGui::GetWindowDockNode();
	dockNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingOverMe;

	const auto mainPos = ImGui::GetMainViewport()->Pos;
	const auto winPos = ImGui::GetWindowPos();
	s_viewportPos.x = winPos.x - mainPos.x;
	s_viewportPos.y = winPos.y - mainPos.y;
	s_viewportHovered = ImGui::IsWindowHovered();
	s_viewportFocused = ImGui::IsWindowFocused();

	const auto size = convertSize(ImGui::GetContentRegionAvail());
	if (size != s_viewportSize && size.first > 0 && size.second > 0) {
		Scene::getFrameBuffer()->resize(size.first, size.second);
		s_viewportSize = size;
		CameraController::onResize();
	}
	ImGui::Image(Scene::getFrameBuffer()->getColorAttachment(), ImVec2(s_viewportSize.first, s_viewportSize.second), ImVec2(0, 1), ImVec2(1, 0));

	ImGui::End();
}

void ImGuiLayer::robotControls(const ImGuiID dockspaceId)
{
	// for (auto&[name, entity] : Scene::getEntities()) {
	// 	if (auto robot = dynamic_cast<Robot*>(entity.get()); robot != nullptr) {
	// 		auto& controlData = robot->getControlData();
	// 		const auto& joints = robot->getJoints();

	// 		// ImGui::SetNextWindowDockID(dockspaceId);
	// 		ImGui::Begin(name.c_str());

	// 		ImGui::Text("%s", robot->getName().c_str());

	// 		ImGui::Separator();

	// 		ImGui::Text("%s", "Joint values:");
	// 		for (size_t i = 0; i < controlData.jointValues.size(); ++i) {
	// 			ImGui::SliderAngle(joints[i]->name.c_str(), &controlData.jointValues[i], rad2deg(joints[i]->limits.first), rad2deg(joints[i]->limits.second));
	// 		}
	// 		ImGui::Separator();

	// 		ImGui::Checkbox("Frames", &controlData.drawFrames);
	// 		ImGui::Checkbox("Bounding Boxes", &controlData.drawBoundingBoxes);

	// 		ImGui::Separator();

	// 		if (controlData.trajectory) {
	// 			ImGui::SetNextItemWidth(0.94 * ImGui::GetCurrentWindow()->Size.x);
	// 			ImGui::SliderFloat("##Traj", &controlData.trajectory->currentTime, controlData.trajectory->times.front(), controlData.trajectory->times.back());				
	// 			m_sliderTime.val() = controlData.trajectory->currentTime;
	// 			if (m_sliderTime().edge() && !controlData.trajectory->active && controlData.trajectory->currentIndex > 0 && controlData.trajectory->currentIndex < controlData.trajectory->jointValues.size()) {
	// 				// controlData.jointValues = controlData.trajectory->jointValues[controlData.trajectory->currentIndex];
	// 				for (size_t i = 0; i < robot->numJoints(); ++i) {           
	// 					controlData.jointValues[i] = map(
	// 						controlData.trajectory->currentTime, 
	// 						controlData.trajectory->times[controlData.trajectory->currentIndex-1], 			controlData.trajectory->times[controlData.trajectory->currentIndex], 
	// 						controlData.trajectory->jointValues[controlData.trajectory->currentIndex-1][i], controlData.trajectory->jointValues[controlData.trajectory->currentIndex][i]); 
	// 				} 
	// 			}

	// 			m_buttonPlay.val() = ImGui::ImageButton("play", TextureLibrary::get("image")->getId(), ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0));
	// 			if (m_buttonPlay().rising()) {
	// 				controlData.trajectory->active = !controlData.trajectory->active;
	// 			}
	// 		}

	// 		ImGui::End();
	// 	}
	// }

}