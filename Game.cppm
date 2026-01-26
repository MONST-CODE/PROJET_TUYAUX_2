module;
#define IMGUI_DEFINE_MATH_OPERATORS
#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <algorithm>

export module game; 
import logic;
import renderer;
import theme;

export class Game {
public:
    Game() : mGrid(12, 8) { 
        if (!SDL_Init(SDL_INIT_VIDEO)) return;
        mWin = SDL_CreateWindow("PIPE MASTER ULTRA", 1280, 800, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_HIGH_PIXEL_DENSITY);
        mRenSDL = SDL_CreateRenderer(mWin, NULL);
        ImGui::CreateContext();
        
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 4.0f; // Leger arrondi classe
        style.FrameRounding = 2.0f;
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0,0,0,0);
        style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0,0,0,0.85f);
        ImGui::GetIO().FontGlobalScale = 1.2f;

        ImGui_ImplSDL3_InitForSDLRenderer(mWin, mRenSDL);
        ImGui_ImplSDLRenderer3_Init(mRenSDL);
        std::srand(std::time(nullptr));
        
        LoadProgress();
        LoadLevel(mUnlockedLevels); 
        mIsRunning = true;
    }

    ~Game() {
        SaveProgress();
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
        SDL_DestroyRenderer(mRenSDL);
        SDL_DestroyWindow(mWin);
        SDL_Quit();
    }

    void Run() {
        while (mIsRunning) {
            HandleEvents();
            ImGui_ImplSDLRenderer3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();
            float dt = ImGui::GetIO().DeltaTime;
            mTime += dt;
            
            mGrid.Update(dt); 

            ImGuiViewport* vp = ImGui::GetMainViewport();
            mRenGame.DrawPolyBackground(ImGui::GetBackgroundDrawList(), vp->Size, mThemeMgr.Get());
            float sideW = mShowSidebar ? 300.0f : 0.0f;
            mRenGame.DrawScene(mGrid, 80.0f, mTime, ImVec2(sideW, 0), dt, mThemeMgr.Get());

            RenderUI(vp);
            HandleInput(80.0f, ImVec2(sideW, 0));

            ImGui::Render();
            SDL_SetRenderDrawColor(mRenSDL, 0, 0, 0, 255);
            SDL_RenderClear(mRenSDL);
            ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), mRenSDL);
            SDL_RenderPresent(mRenSDL);
        }
    }

private:
    SDL_Window* mWin;
    SDL_Renderer* mRenSDL;
    PipeGrid mGrid;
    Renderer mRenGame;
    ThemeManager mThemeMgr;
    float mTime = 0.0f;
    bool mIsRunning = false;
    bool mShowSidebar = false;
    bool mShowLevelList = false;
    int mCurrentLevel = 1;
    int mUnlockedLevels = 1;

    void SaveProgress() { std::ofstream f("savegame.txt"); if(f.is_open()){f<<mUnlockedLevels;f.close();} }
    void LoadProgress() { std::ifstream f("savegame.txt"); if(f.is_open()){f>>mUnlockedLevels;f.close();} mUnlockedLevels=std::max(1,std::min(mUnlockedLevels,9)); }

    void LoadLevel(int levelIdx) {
        mGrid.ResetValve();
        mCurrentLevel = levelIdx;
        mGrid.SetMaxTime(60.0f + (levelIdx * 15.0f)); // Plus de temps pour les niveaux durs
        
        int w = mGrid.GetWidth(); int h = mGrid.GetHeight();
        for(int i=0; i<w*h; i++) mGrid.GetPipe(i%w, i/w).connections = 0;
        mGrid.GetPipe(0, 4).connections = (uint8_t)Side::EAST;

        if (levelIdx == 1) SetPath({ {1,4}, {2,4}, {3,4}, {4,4}, {5,4}, {6,4}, {7,4}, {8,4} });
        else if (levelIdx == 2) SetPath({ {1,4}, {2,4}, {2,3}, {2,2}, {3,2}, {4,2}, {5,2}, {5,3}, {5,4}, {6,4}, {7,4} });
        else if (levelIdx == 3) SetPath({ {1,4}, {1,6}, {3,6}, {3,2}, {5,2}, {5,6}, {7,6}, {7,4}, {8,4} });
        else if (levelIdx == 4) SetPath({ {1,4}, {2,4}, {3,4}, {3,3}, {3,2}, {2,2}, {1,2}, {1,1}, {4,1}, {4,5}, {6,5}, {6,4} });
        else if (levelIdx == 5) { SetPath({ {1,4}, {2,4}, {3,4}, {4,4}, {5,4}, {6,4}, {7,4}, {8,4} }); SetPipe(2,3, 6); SetPipe(2,5, 3); SetPipe(5,3, 12); SetPipe(5,5, 9); }
        else if (levelIdx >= 6 && levelIdx <= 8) { 
            for(int y=1; y<7; y++) for(int x=1; x<9; x++) if((x+y)%2!=0) SetPipe(x,y, 10); else SetPipe(x,y, 5);
            SetPath({{1,4}, {2,4}, {3,4}, {4,4}}); 
        }
        else if (levelIdx == 9) { 
             for(int x=1; x<11; x++) { SetPipe(x,1, 10); SetPipe(x,7, 10); } 
             for(int y=1; y<8; y++) { SetPipe(11,y, 5); SetPipe(1,y, 5); } 
             SetPipe(1,4, 15);
             for(int x=3; x<9; x+=2) for(int y=3; y<6; y++) SetPipe(x,y, 15);
        }
        else GenerateRandomLevel();

        for(int y=0; y<h; y++) for(int x=0; x<w; x++) {
            if(x==0 && y==4) continue;
            if(mGrid.GetPipe(x,y).connections != 0) {
                int rots = std::rand() % 4;
                for(int r=0; r<rots; r++) mGrid.GetPipe(x,y).Rotate();
            }
        }
    }

    void SetPath(std::vector<std::pair<int,int>> path) {
        for(size_t i=0; i<path.size(); i++) {
            int x = path[i].first; int y = path[i].second;
            int cx = (i==0) ? 0 : path[i-1].first; int cy = (i==0) ? 4 : path[i-1].second;
            uint8_t conn = 0;
            if (cx < x) conn |= 8; if (cx > x) conn |= 2; if (cy < y) conn |= 1; if (cy > y) conn |= 4;
            if (i < path.size()-1) {
                int nx = path[i+1].first; int ny = path[i+1].second;
                if (nx > x) conn |= 2; if (nx < x) conn |= 8; if (ny > y) conn |= 4; if (ny < y) conn |= 1;
            }
            SetPipe(x, y, conn);
        }
    }
    void SetPipe(int x, int y, int type) { if(x>=0 && x<mGrid.GetWidth() && y>=0 && y<mGrid.GetHeight()) mGrid.GetPipe(x,y).connections = (uint8_t)type; }
    
    // Generation aleatoire plus intelligente (essaye de faire un chemin continu)
    void GenerateRandomLevel() {
        mCurrentLevel = 0; mGrid.SetMaxTime(90.0f);
        int w = mGrid.GetWidth(); int h = mGrid.GetHeight();
        for(int i=0; i<w*h; i++) mGrid.GetPipe(i%w, i/w).connections = 0;
        mGrid.GetPipe(0, 4).connections = (uint8_t)Side::EAST;
        
        // Algorithme de "ver de terre" simple
        int cx = 1, cy = 4;
        int length = 0;
        while(length < 20 && cx < w-1) {
            SetPipe(cx, cy, 15); // Croix temporaire
            // Deplacement random
            int dir = rand() % 3; // 0: droit, 1: haut, 2: bas
            if(dir==0) cx++;
            else if(dir==1 && cy > 1) cy--;
            else if(dir==2 && cy < h-2) cy++;
            length++;
        }
        // Remplissage de bruit autour
        for (int y = 0; y < h; ++y) for (int x = 0; x < w; ++x) {
            if (x == 0 && y == 4) continue;
            if (mGrid.GetPipe(x,y).connections == 0 && (rand() % 100) > 60) {
                int types[] = { 5, 10, 3, 6, 12, 9, 7, 11, 13, 14 };
                mGrid.GetPipe(x, y).connections = (uint8_t)types[std::rand() % 10];
            }
        }
    }
    void HandleEvents() { SDL_Event event; while (SDL_PollEvent(&event)) { ImGui_ImplSDL3_ProcessEvent(&event); if (event.type == SDL_EVENT_QUIT) mIsRunning = false; } }
    
    void HandleInput(float cellSize, ImVec2 offset) {
        if (!ImGui::GetIO().WantCaptureMouse && ImGui::IsMouseClicked(0) && mGrid.GetState() == GameState::PLANNING) {
            ImVec2 m = ImGui::GetMousePos();
            ImVec2 screenSize = ImGui::GetMainViewport()->Size;
            float gridW = mGrid.GetWidth() * cellSize; float gridH = mGrid.GetHeight() * cellSize;
            ImVec2 startPos = offset + ImVec2((screenSize.x - offset.x - gridW) * 0.5f, (screenSize.y - gridH) * 0.5f);
            if (m.x >= startPos.x && m.x < startPos.x + gridW && m.y >= startPos.y && m.y < startPos.y + gridH) {
                int gx = (int)((m.x - startPos.x) / cellSize);
                int gy = (int)((m.y - startPos.y) / cellSize);
                if (gx >= 0 && gx < mGrid.GetWidth() && gy >= 0 && gy < mGrid.GetHeight()) mGrid.GetPipe(gx, gy).Rotate();
            }
        }
    }

    void RenderUI(ImGuiViewport* vp) {
        const Theme& th = mThemeMgr.Get();
        float hH = 60.0f;
        ImGui::SetNextWindowPos(vp->Pos);
        ImGui::SetNextWindowSize(ImVec2(vp->Size.x, hH));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0,0,0,0.6f));
        ImGui::Begin("Header", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
        if (ImGui::Button(mShowSidebar ? " X " : " MENU ", ImVec2(80, 40))) mShowSidebar = !mShowSidebar;
        ImGui::SameLine(); ImGui::SetCursorPosY(15);
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorConvertU32ToFloat4(th.uiAccent));
        ImGui::Text("PIPE MASTER // TERMINAL"); 
        ImGui::PopStyleColor();
        ImGui::SameLine(vp->Size.x * 0.5f - 80);
        
        // Timer Styling
        float timeP = mGrid.GetTimeLeft() / mGrid.GetMaxTime();
        char timeBuf[32]; sprintf(timeBuf, "TEMPS: %.0f s", mGrid.GetTimeLeft());
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImGui::ColorConvertU32ToFloat4(th.fluidGlow));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f,0.1f,0.1f,0.8f));
        ImGui::ProgressBar(timeP, ImVec2(160, 30), timeBuf);
        ImGui::PopStyleColor(2);
        ImGui::End(); ImGui::PopStyleColor();

        if (mShowSidebar) {
            ImGui::SetNextWindowPos(vp->Pos + ImVec2(0, hH));
            ImGui::SetNextWindowSize(ImVec2(320, vp->Size.y - hH));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.08f, 0.1f, 0.98f));
            ImGui::Begin("Sidebar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(th.uiAccent), ":: NAVIGATION SYSTEM"); 
            ImGui::Separator(); ImGui::Dummy(ImVec2(0, 10));
            if (ImGui::Button("SELECTION NIVEAU", ImVec2(-1, 40))) mShowLevelList = !mShowLevelList;
            if (mShowLevelList) {
                for (int i = 1; i <= 9; i++) {
                    char buf[32]; sprintf(buf, " NIVEAU %d %s", i, i<=mUnlockedLevels ? "" : "[VERROUILLÉ]");
                    ImGui::PushID(i);
                    if (i > mUnlockedLevels) ImGui::BeginDisabled();
                    if (ImGui::Button(buf, ImVec2(-1, 30))) { LoadLevel(i); mShowSidebar = false; }
                    if (i > mUnlockedLevels) ImGui::EndDisabled();
                    ImGui::PopID();
                }
                if (ImGui::Button(" MODE INFINI", ImVec2(-1, 30))) { GenerateRandomLevel(); mShowSidebar = false; }
            }
            ImGui::Dummy(ImVec2(0, 15));
            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(th.uiAccent), ":: PERSONNALISATION");
            if (ImGui::BeginCombo("THEME", th.name)) {
                for (int i = 0; i < mThemeMgr.themes.size(); i++) {
                    bool isSelected = (mThemeMgr.currentIdx == i);
                    if (ImGui::Selectable(mThemeMgr.themes[i].name, isSelected)) mThemeMgr.Set(i);
                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::Dummy(ImVec2(0, 30)); ImGui::Separator();
            if (ImGui::Button("OUVRIR VANNE (START)", ImVec2(-1, 50))) { mGrid.OpenValve(); mShowSidebar = false; }
            ImGui::Dummy(ImVec2(0, 5));
            if (ImGui::Button("REINITIALISER", ImVec2(-1, 40))) { mGrid.ResetValve(); }
            float avail = ImGui::GetContentRegionAvail().y; ImGui::Dummy(ImVec2(0, avail - 50));
            if (ImGui::Button("QUITTER LE JEU", ImVec2(-1, 45))) ImGui::OpenPopup("QuitConfirm");
            
            // Modal centré
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal("QuitConfirm", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Quitter l'application ?\nVotre progression sera sauvegardee."); ImGui::Separator();
                if (ImGui::Button("OUI, QUITTER", ImVec2(120, 0))) { SaveProgress(); mIsRunning = false; }
                ImGui::SetItemDefaultFocus(); ImGui::SameLine();
                if (ImGui::Button("ANNULER", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }
            ImGui::End(); ImGui::PopStyleColor();
        }

        GameState st = mGrid.GetState();
        if (st == GameState::VICTORY || st == GameState::DEFEAT) {
            ImGui::SetNextWindowPos(vp->Pos + vp->Size * 0.5f, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(450, 300));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f,0.05f,0.08f,0.95f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, st==GameState::VICTORY ? ImVec4(0,1,0,1) : ImVec4(1,0,0,1));
            
            ImGui::Begin("EndGame", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
            float scale = 2.5f; ImGui::SetWindowFontScale(scale);
            
            float winW = ImGui::GetWindowSize().x;
            if (st == GameState::VICTORY) {
                const char* txt = "SUCCES !";
                float tw = ImGui::CalcTextSize(txt).x;
                ImGui::SetCursorPosX((winW - tw) * 0.5f);
                ImGui::TextColored(ImVec4(0.2f,1.0f,0.2f,1), txt);
                
                ImGui::SetWindowFontScale(1.2f);
                const char* sub = "Flux retabli. Pression nominale.";
                tw = ImGui::CalcTextSize(sub).x;
                ImGui::SetCursorPosX((winW - tw) * 0.5f);
                ImGui::Text("%s", sub);
                
                ImGui::Dummy(ImVec2(0,40));
                if (ImGui::Button("NIVEAU SUIVANT >>", ImVec2(-1, 60))) {
                    if (mCurrentLevel == mUnlockedLevels && mUnlockedLevels < 9) mUnlockedLevels++;
                    if (mCurrentLevel < 9) LoadLevel(mCurrentLevel + 1); else GenerateRandomLevel();
                }
            } else {
                const char* txt = "ECHEC CRITIQUE";
                float tw = ImGui::CalcTextSize(txt).x;
                ImGui::SetCursorPosX((winW - tw) * 0.5f);
                ImGui::TextColored(ImVec4(1.0f,0.2f,0.2f,1), txt);
                
                ImGui::SetWindowFontScale(1.2f);
                // On pourrait etre plus precis si on passait la raison de l'echec
                const char* sub = (mGrid.GetTimeLeft() <= 0) ? "Temps ecoule." : "Fuite ou Blocage detecte.";
                tw = ImGui::CalcTextSize(sub).x;
                ImGui::SetCursorPosX((winW - tw) * 0.5f);
                ImGui::Text("%s", sub);
                
                ImGui::Dummy(ImVec2(0,40));
                if (ImGui::Button("REESSAYER LA MISSION", ImVec2(-1, 60))) { mGrid.ResetValve(); }
            }
            ImGui::End();
            ImGui::PopStyleColor(2); ImGui::PopStyleVar();
        }
    }
};