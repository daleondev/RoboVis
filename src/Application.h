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

    void update(const Timestep dt);  

    static Application& get() { return *s_instance; }

private: 
    void onEvent(Event& e);      
    bool onWindowClose(WindowCloseEvent& e);
    
    static void handleSignal(int signal);
    
    bool m_running;
    float m_lastFrameTime;

    const aiScene* m_scene;
    std::vector<std::shared_ptr<EntityOld>> m_entities;

    static Application* s_instance;
};