//
//  funcionesApp.cpp
//  sincro2Ableton
//
//  Created by Matias Romero Costas on 10/14/17.
//
//

#include "ofApp.h"

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.txt)
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

//--------------------------------------------------------------
void ofApp::drawGui(){
    //required to call this at beginning
    gui.begin();
    
    bool closeWindow = true;

    bool * p_open = &closeWindow;
    
    // Demonstrate the various window flags. Typically you would just use the default!
    static bool no_titlebar = false;
    static bool no_scrollbar = false;
    static bool no_menu = false;
    static bool no_move = false;
    static bool no_resize = false;
    static bool no_collapse = true;
    static bool no_close = true;
    static bool no_nav = false;
    static bool no_background = false;
    static bool no_bring_to_front = false;

    ImGuiWindowFlags window_flags = 0;
    if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
    if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
    if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
    if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
    if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
    if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
    if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
    if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (no_close)           p_open = NULL; // Don't pass our bool* to Begin
    
    
    // -------- MENU PRINCIPAL --------
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Copnfiguración"))
        {
            if (ImGui::MenuItem("Guardar", "cmd+s")) {
                saveSettings();
                ofLogVerbose() << "Configuración guardada";
            }
            if (ImGui::MenuItem("Config. Cam")) {
                show_cam_config_panel = true;
            }
            if (ImGui::MenuItem("Config. OSC")) {
                show_osc_config_panel = true;
            }
            
            ImGui::Checkbox("Gráfico de movimiento", &show_plotline);
            
            ImGui::Checkbox("Imagen orignal", &viewCam);
            
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Ayuda"))
        {
            show_about_window = true;
            
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
        
    }
    // Menú principal
    
    if (show_osc_config_panel)
    {
        //note: ofVec2f and ImVec2f are interchangeable
        ImGui::SetNextWindowSize(ofVec2f(290,180), ImGuiCond_FirstUseEver);
        ImGui::Begin("Configuracion OSC", &show_osc_config_panel);
    
        //-------------------------------------------------------------------------------------------
        //                         OSC - INPUT INT
        //-------------------------------------------------------------------------------------------

        //ImGui::Separator(); ImGui::Separator(); ImGui::Separator();ImGui::Separator();
        //ImGui::Text("-: OSC :----");

        if(ImGui::InputInt("port", &puerto)) sender.setup(host, puerto);
        ImGui::SameLine(); HelpMarker("puerto de conección");
        
        static char str1[128];
        strcpy(str1, host.c_str());
        //static char str1[128] = "127.0.0.1";
        //ImGui::InputTextWithHint("ip", "enter ip address here", str1, IM_ARRAYSIZE(str1));
        if( ImGui::InputTextWithHint("ip", "enter ip address here",str1, IM_ARRAYSIZE(str1))){
            host = ofVAArgsToString(str1);
            sender.setup(host, puerto);
            //ofLogVerbose() << "--------CAMBIO DE HOST: " << host;
        }
        ImGui::SameLine(); HelpMarker("dirección ip del receptor de mensajes");
        
        ImGui::Separator();
        
        static char movimientoaddress[128];
        strcpy(movimientoaddress, etiquetaMOVIMIENTO.c_str());
        if( ImGui::InputTextWithHint("address", "enter OSC address here",movimientoaddress, IM_ARRAYSIZE(movimientoaddress))){
            etiquetaMOVIMIENTO = ofVAArgsToString(movimientoaddress);
            //ofLogVerbose() << "--------CAMBIO DE ETIQUETA: " << movimientoaddress;
        }
        ImGui::SameLine(); HelpMarker("etiqueta (debe comenzar con /) ");
        ImGui::Checkbox("ENVIAR MOVIMIENTO", &enviarMOVIMIENTO);
        ImGui::SameLine(); HelpMarker("habilitar / deshabilitar el envío de datos");
        ImGui::End();
    }
    if (show_cam_config_panel)
    {
        //note: ofVec2f and ImVec2f are interchangeable
        ImGui::SetNextWindowSize(ofVec2f(50,100), ImGuiCond_FirstUseEver);
        ImGui::Begin("Configuracion Camara", &show_cam_config_panel);
        //-------------------------------------------------------------------------------------------
        //                         DISPOSITIVOS DE ENTRADA -COMBO: POP UP MENU
        //-------------------------------------------------------------------------------------------
        
         //------------------------ DISPOSITIVO DE ENTRADA - COMBO: POP UP MENU-----------------------
        //ImGui::Text("-: Fuente :----"); ImGui::SameLine(); HelpMarker("Elegir el dispositivo de entrada de sonido");
        //ImGui::Text("----: Dispositivo de entrada :----"); ImGui::SameLine(); HelpMarker("Elegir el dispositivo de entrada");
        {
            // Using the _simplified_ one-liner Combo() api here
            // See "Combo" section for examples of how to use the more complete BeginCombo()/EndCombo() api.
            //const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", //"KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
            const char* items[devicesVector.size()];
            //static int item_current = 0;
            for(int i=0; i<devicesVector.size(); i++){
                
                items[i] = devicesVector[i].c_str();
            }
            if(ImGui::Combo(" ", &deviceID, items, IM_ARRAYSIZE(items))){
                resetCameraSettings(deviceID);
                ofLogVerbose() << "--------CAMBIO DE CAMARA - DIVICE ID: " << deviceID;
            }
            ImGui::SameLine(); HelpMarker("Elegir el dispositivo de entrada");
            
            ImGui::Checkbox("ESPEJAR HORIZONTAL", &hMirror);
            ImGui::Checkbox("ESPEJAR VERTICAL", &vMirror);
        }
        
        ImGui::End();
    }
    
    if(show_about_window){
        ImGui::SetNextWindowSize(ofVec2f(300,200), ImGuiCond_FirstUseEver);
        ImGui::Begin("Another Window", &show_about_window);
        ImGui::Text("Dear ImGui");
        ImGui::Separator();
        ImGui::Text("By Omar Cornut and all Dear ImGui contributors.");
        ImGui::Text("Dear ImGui is licensed under the MIT License, see LICENSE for more information.");
        ImGui::Text("Dear ImGui is licensed under the MIT License, see LICENSE for more information.");
        ImGui::Text("Dear ImGui is licensed under the MIT License, see LICENSE for more information.");
        ImGui::Text("Dear ImGui is licensed under the MIT License, see LICENSE for more information.");
        ImGui::Text("Dear ImGui is licensed under the MIT License, see LICENSE for more information.");
        ImGui::Text("Dear ImGui is licensed under the MIT License, see LICENSE for more information.");
        ImGui::Text("Dear ImGui is licensed under the MIT License, see LICENSE for more information.");
        ImGui::Text("Dear ImGui is licensed under the MIT License, see LICENSE for more information.");
        ImGui::Text("Dear ImGui is licensed under the MIT License, see LICENSE for more information.");
        ImGui::Text("Dear ImGui is licensed under the MIT License, see LICENSE for more information.");
        ImGui::Text("Dear ImGui is licensed under the MIT License, see LICENSE for more information.");
        ImGui::Text("Dear ImGui is licensed under the MIT License, see LICENSE for more information.");
        ImGui::End();
    }
    
    
    //------------------------ Plot Line movimiento -----------------------
    
    if(show_plotline){
        
        static float MOVEMENTs[90] = {};
                  
        static int movement_offset = 0;
                  
        MOVEMENTs[movement_offset] = promedioDeMovimiento;
        movement_offset = (movement_offset+1) % IM_ARRAYSIZE(MOVEMENTs);
                  
        float actualMOVEMENT = promedioDeMovimiento;
                  
        char overlay[32];
        sprintf(overlay, "Movimiento %f", actualMOVEMENT);
        //ImGui::SetNextWindowSize(ofVec2f(290,180), ImGuiCond_FirstUseEver);
            
        ImGui::SetNextWindowPos(ImVec2(0, ofGetHeight() - 100), ImGuiCond_Always, ImVec2(0, 0));
        //ImGui::SetNextWindowSize(ImVec2(ofGetWidth(), 30), ImGuiCond_FirstUseEver );
        //
        if (ImGui::Begin("Example: Simple overlay", p_open, (ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBackground)))
        {
            //ImGui::SetNextWindowBgAlpha(0.15f); // Transparent background
            ImGui::PlotLines("", MOVEMENTs, IM_ARRAYSIZE(MOVEMENTs), movement_offset, overlay, 0.0f, 1.0f, ImVec2(180, 80));

        }
        ImGui::End();
    }
    gui.end();
}
