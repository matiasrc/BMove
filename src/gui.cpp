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
    
    // -------- MENU PRINCIPAL --------
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Copnfiguración"))
        {
            if (ImGui::MenuItem("Guardar", "cmd+s")) {
                saveSettings();
                ofLogVerbose() << "Configuración guardada";
            }
            
            ImGui::Checkbox("Gráfico de movimiento", &show_plotline);
            
            ImGui::Checkbox("Imagen orignal", &viewCam);
            
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("|Entrada|"))
            {
                static const char* item_current = devicesVector[deviceID].c_str();
                if(ImGui::BeginCombo(" ", item_current)){
        
                    for(int i=0; i < devicesVector.size(); ++i){
                        const bool isSelected = (deviceID == i);
                        if(ImGui::Selectable(devicesVector[i].c_str(), isSelected)){
                            deviceID = i;
                            resetCameraSettings(deviceID);
                            item_current = devicesVector[i].c_str();
                        }
                        if(isSelected){
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                ImGui::EndCombo();
                }
                ImGui::SameLine(); HelpMarker("Elegir el dispositivo de entrada");
                
                ImGui::Separator();
                ImGui::Checkbox("ESPEJAR HORIZONTAL", &hMirror);
                ImGui::Checkbox("ESPEJAR VERTICAL", &vMirror);
                
            ImGui::EndMenu();
            }
        if (ImGui::BeginMenu("|OSC|"))
        {
            if(ImGui::InputInt("port", &puerto)) sender.setup(host, puerto);
            ImGui::SameLine(); HelpMarker("puerto de conexión");
            
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
            
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("|Acerca|"))
        {
            ImGui::Text("BFaceTracker");
            ImGui::Separator();
            ImGui::Text("Software experimental para captura de movimiento.");
            ImGui::Text("utilizando las técnicas de haar cascade ");
            ImGui::Text("para detección de objetos.");
            ImGui::Text("Esta aplicación está en desarrollo y no tiene soporte");
            ImGui::Text("..............");
            ImGui::Text("Desarrollado por Matías Romero Costas (Biopus)");
            ImGui::Text("www.biopus.ar");

            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
        
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
