#pragma once

#include "Timestep.h"

#include "Events/WindowEvent.h"

#include "Entities/Entity.h"

class Application {
public:
    Application();
    virtual ~Application();

    int run(int argc, char **argv);
    void close();

    void onUpdate(const Timestep dt);
    void onEvent(Event& e);

    static Application& get() { return *s_instance; }

private:       
    bool onWindowClose(WindowCloseEvent& e);
    static void handleSignal(int signal);
    
    bool m_running;
    float m_lastFrameTime;

    const aiScene* m_scene;
    std::vector<std::shared_ptr<Entity>> m_entities;

    static Application* s_instance;
};