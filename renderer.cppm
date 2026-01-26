module;
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <cstdlib> 

export module renderer;
import logic;
import particles;
import theme;

export class Renderer {
public:
    ParticleSystem particles;

    // Fond triangulaire abstrait (Style Image 4)
    void DrawPolyBackground(ImDrawList* dl, ImVec2 size, const Theme& th) {
        dl->AddRectFilledMultiColor(ImVec2(0,0), size, th.bgTop, th.bgTop, th.bgBot, th.bgBot);
        
        int cols = 10; int rows = 6;
        float cw = size.x / cols; float ch = size.y / rows;

        for (int y = 0; y <= rows; y++) {
            for (int x = 0; x <= cols; x++) {
                auto GetPt = [&](int gx, int gy) {
                    float offX = (sin(gx * 0.5f + gy * 0.8f) * 0.4f) * cw;
                    float offY = (cos(gx * 0.9f + gy * 0.4f) * 0.4f) * ch;
                    return ImVec2(gx * cw + offX, gy * ch + offY);
                };
                if (x < cols && y < rows) {
                    ImVec2 p1 = GetPt(x, y); ImVec2 p2 = GetPt(x + 1, y);
                    ImVec2 p3 = GetPt(x + 1, y + 1); ImVec2 p4 = GetPt(x, y + 1);
                    
                    float noise = sin(x * 0.3f + y * 0.2f); // Variation
                    ImU32 colTri = (noise > 0) ? th.bgTop : th.bgBot;
                    // On modifie l'alpha pour faire des reflets subtils
                    int alpha = 10 + (int)(abs(noise) * 20);
                    ImU32 colOverlay = IM_COL32(255, 255, 255, alpha); 
                    
                    dl->AddTriangleFilled(p1, p2, p3, colOverlay);
                    dl->AddTriangleFilled(p1, p3, p4, colOverlay);
                    // Lignes très fines
                    dl->AddTriangle(p1, p2, p3, IM_COL32(255,255,255,5), 1.0f);
                }
            }
        }
    }

    void DrawScene(const PipeGrid& grid, float cellSize, float time, ImVec2 offset, float dt, const Theme& th) {
        ImDrawList* dl = ImGui::GetBackgroundDrawList();
        ImVec2 screenSize = ImGui::GetMainViewport()->Size;
        float gridW = grid.GetWidth() * cellSize;
        float gridH = grid.GetHeight() * cellSize;
        ImVec2 startPos = offset + ImVec2((screenSize.x - offset.x - gridW) * 0.5f, (screenSize.y - gridH) * 0.5f);
        
        // Ombre portée globale de la machine
        dl->AddRectFilled(startPos + ImVec2(15,15), startPos + ImVec2(gridW+15, gridH+15), IM_COL32(0,0,0,120), 25.0f);
        // Cadre fin
        dl->AddRect(startPos - ImVec2(5,5), startPos + ImVec2(gridW+5, gridH+5), th.uiAccent, 5.0f, 0, 2.0f);

        for (int y = 0; y < grid.GetHeight(); ++y) {
            for (int x = 0; x < grid.GetWidth(); ++x) {
                ImVec2 c(startPos.x + x * cellSize + cellSize * 0.5f, startPos.y + y * cellSize + cellSize * 0.5f);
                
                // Trou sombre pour le tuyau (Socket)
                dl->AddCircleFilled(c, cellSize*0.35f, IM_COL32(5,5,5,180));
                
                const Pipe& pipe = grid.GetPipe(x, y);
                if (pipe.connections != 0) {
                    DrawRealisticPipe(dl, c, cellSize, pipe, time, th);
                    // Particules de vapeur (ambiance)
                    if (pipe.fillAmount > 0 && rand()%400 == 0) particles.Spawn(c, PType::STEAM, th.fluidGlow);
                }
            }
        }
        particles.UpdateAndRender(dl, dt);
    }

private:
    void DrawRealisticPipe(ImDrawList* dl, ImVec2 c, float size, const Pipe& pipe, float time, const Theme& th) {
        float w = size * 0.42f; 
        float armLen = size * 0.5f + 1.5f;

        // Fonction pour dessiner une ligne épaisse avec rotation
        auto DrawRotatedLine = [&](float angleOffset, float thickness, ImU32 color, float lenScale) {
            float angle = angleOffset + pipe.visualAngle - 1.5708f;
            ImVec2 dir(cos(angle), sin(angle));
            dl->AddLine(c, c + dir * (armLen * lenScale), color, thickness);
        };

        // 1. CORPS DU TUYAU (Effet Métal Cylindrique)
        // On dessine plusieurs couches pour faire le volume
        auto DrawBody = [&](float angle) {
            DrawRotatedLine(angle, w + 4.0f, IM_COL32(0,0,0,200), 1.0f); // Contour noir épais
            DrawRotatedLine(angle, w, th.pipeDark, 1.0f);                // Base sombre
            DrawRotatedLine(angle, w * 0.6f, th.pipeLight, 1.0f);        // Reflet central
            DrawRotatedLine(angle, w * 0.3f, th.pipeDark, 1.0f);         // Creux intérieur (tube verre)
        };

        if (pipe.connections & 1) DrawBody(0);
        if (pipe.connections & 2) DrawBody(1.57f);
        if (pipe.connections & 4) DrawBody(3.14f);
        if (pipe.connections & 8) DrawBody(4.71f);

        // 2. LIQUIDE (Interieur du verre)
        if (pipe.fillAmount > 0.0f) {
            float len = pipe.fillAmount;
            auto DrawFluid = [&](float angle) {
                float rot = angle + pipe.visualAngle - 1.5708f;
                ImVec2 dir(cos(rot), sin(rot));
                ImVec2 pEnd = c + dir * (armLen * len);
                
                // Halo externe (Glow)
                dl->AddLine(c, pEnd, th.fluidGlow, w * 0.8f);
                // Liquide coloré
                // On peut ajouter une variation de couleur pour faire "vivant"
                ImU32 colVar = th.fluidGlow; // Pourrait pulser
                dl->AddLine(c, pEnd, colVar, w * 0.5f);
                // Coeur blanc brillant (Plasma)
                dl->AddLine(c, pEnd, th.fluidCore, w * 0.2f);

                // Bulles / Energie qui circule
                int numBubbles = 3;
                for(int i=0; i<numBubbles; i++) {
                    float t = fmod(time * 2.0f + i * 0.3f, 1.0f);
                    if (t < len) {
                        ImVec2 bPos = c + dir * (armLen * t);
                        dl->AddCircleFilled(bPos, w*0.15f, IM_COL32(255,255,255,200));
                    }
                }
            };

            if (pipe.connections & 1) DrawFluid(0);
            if (pipe.connections & 2) DrawFluid(1.57f);
            if (pipe.connections & 4) DrawFluid(3.14f);
            if (pipe.connections & 8) DrawFluid(4.71f);
            
            // Jointure centrale fluide
            dl->AddCircleFilled(c, w * 0.4f, th.fluidGlow);
            dl->AddCircleFilled(c, w * 0.2f, th.fluidCore);
        }

        // 3. JOINTURE (Connecteur central par dessus)
        // Anneau métallique
        dl->AddCircle(c, w * 0.85f, th.pipeLight, 0, 3.0f); 
        // Boulons
        float bOff = w * 0.65f;
        dl->AddCircleFilled(c + ImVec2(-bOff, -bOff), 3.0f, th.pipeDark);
        dl->AddCircleFilled(c + ImVec2( bOff, -bOff), 3.0f, th.pipeDark);
        dl->AddCircleFilled(c + ImVec2( bOff,  bOff), 3.0f, th.pipeDark);
        dl->AddCircleFilled(c + ImVec2(-bOff,  bOff), 3.0f, th.pipeDark);

        // 4. FUITE (Spray)
        if (pipe.isLeak && pipe.isFull) {
            if ((rand() % 10) < 4) particles.Spawn(c, PType::WATER, th.fluidGlow);
            // Icone Alerte rouge qui clignote
            float flash = (sin(time * 15.0f) + 1.0f) * 0.5f;
            ImU32 colAlert = IM_COL32(255, 50, 50, (int)(flash * 255));
            dl->AddCircle(c, size*0.5f, colAlert, 0, 4.0f);
            dl->AddText(c - ImVec2(5,8), IM_COL32(255,255,255,255), "!");
        }
    }
};