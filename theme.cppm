module;
#include <imgui.h>
#include <vector>

export module theme;

export struct Theme {
    const char* name;
    ImU32 bgTop;      // Haut du degradé fond
    ImU32 bgBot;      // Bas du degradé fond
    ImU32 pipeDark;   // Ombre du métal
    ImU32 pipeLight;  // Reflet du métal
    ImU32 fluidCore;  // Coeur du liquide (brillant)
    ImU32 fluidGlow;  // Halo autour du liquide
    ImU32 uiAccent;   // Texte et bordures UI
};

export class ThemeManager {
public:
    std::vector<Theme> themes;
    int currentIdx = 0;

    ThemeManager() {
        // 0. CYBERPUNK (Néon Cyan/Rose sur fond sombre)
        themes.push_back({ "CYBERPUNK", IM_COL32(10,10,18,255), IM_COL32(5,5,10,255), IM_COL32(30,30,40,255), IM_COL32(80,80,90,255), IM_COL32(200,255,255,255), IM_COL32(0,200,255,200), IM_COL32(255,0,100,255) });
        
        // 1. OBSIDIAN (Noir, Gris, Blanc - Très classe)
        themes.push_back({ "OBSIDIAN", IM_COL32(20,20,22,255), IM_COL32(5,5,5,255), IM_COL32(10,10,10,255), IM_COL32(60,60,65,255), IM_COL32(255,255,255,255), IM_COL32(150,150,150,100), IM_COL32(255,255,255,255) });
        
        // 2. MAGMA (Lave en fusion)
        themes.push_back({ "MAGMA", IM_COL32(30,5,5,255), IM_COL32(10,0,0,255), IM_COL32(40,10,10,255), IM_COL32(80,40,40,255), IM_COL32(255,255,0,255), IM_COL32(255,50,0,220), IM_COL32(255,150,0,255) });
        
        // 3. TOXIC (Acide Vert fluo)
        themes.push_back({ "TOXIC", IM_COL32(10,20,10,255), IM_COL32(0,10,0,255), IM_COL32(20,40,20,255), IM_COL32(50,80,50,255), IM_COL32(220,255,200,255), IM_COL32(50,255,0,180), IM_COL32(100,255,50,255) });
        
        // 4. OCEAN (Abysses bleues)
        themes.push_back({ "OCEAN", IM_COL32(5,15,30,255), IM_COL32(0,5,10,255), IM_COL32(20,40,60,255), IM_COL32(50,80,110,255), IM_COL32(200,240,255,255), IM_COL32(0,100,255,180), IM_COL32(0,200,255,255) });
        
        // 5. GOLDEN (Luxe, Or)
        themes.push_back({ "GOLDEN", IM_COL32(40,30,10,255), IM_COL32(10,5,0,255), IM_COL32(60,50,20,255), IM_COL32(120,100,40,255), IM_COL32(255,255,200,255), IM_COL32(255,200,0,180), IM_COL32(255,215,0,255) });
        
        // 6. VAPORWAVE (Violet/Rose rétro)
        themes.push_back({ "VAPORWAVE", IM_COL32(30,10,40,255), IM_COL32(10,0,20,255), IM_COL32(50,30,60,255), IM_COL32(90,60,110,255), IM_COL32(255,100,255,255), IM_COL32(180,0,255,180), IM_COL32(0,255,255,255) });
        
        // 7. MILITARY (Vert Camo / Orange Radar)
        themes.push_back({ "MILITARY", IM_COL32(20,25,20,255), IM_COL32(10,12,10,255), IM_COL32(30,40,30,255), IM_COL32(60,70,60,255), IM_COL32(255,200,100,255), IM_COL32(255,100,0,180), IM_COL32(255,160,0,255) });
        
        // 8. SNOW (Blanc arctique)
        themes.push_back({ "ARCTIC", IM_COL32(180,190,200,255), IM_COL32(150,160,170,255), IM_COL32(200,210,220,255), IM_COL32(255,255,255,255), IM_COL32(200,240,255,255), IM_COL32(0,150,255,150), IM_COL32(50,100,150,255) });
        
        // 9. BLOODMOON (Rouge Sang sombre)
        themes.push_back({ "BLOODMOON", IM_COL32(20,0,0,255), IM_COL32(5,0,0,255), IM_COL32(40,0,0,255), IM_COL32(80,20,20,255), IM_COL32(255,200,200,255), IM_COL32(200,0,0,200), IM_COL32(255,50,50,255) });
    }

    const Theme& Get() const { return themes[currentIdx]; }
    void Set(int idx) { if (idx >= 0 && idx < (int)themes.size()) currentIdx = idx; }
};