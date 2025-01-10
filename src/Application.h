#pragma once

#include "Events/WindowEvent.h"

#include "Entities/Entity.h"

#include "Util/Timestep.h"

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

    static Application* s_instance;
};