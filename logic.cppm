module;
#include <cstdint>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>
#include <cstdlib>

export module logic;

export enum class GameState { PLANNING, SIMULATING, VICTORY, DEFEAT };
export enum class Side : uint8_t { NONE=0, NORTH=1, EAST=2, SOUTH=4, WEST=8 };

export struct Pipe {
    uint8_t connections = 0;
    int flowDistance = -1;
    bool isLeak = false;
    float fillAmount = 0.0f;
    bool isFilling = false;
    bool isFull = false;
    float visualAngle = 0.0f; 
    void Rotate() { connections = ((connections << 1) | (connections >> 3)) & 0x0F; visualAngle -= 1.5708f; }
    void UpdateAnim(float dt) { visualAngle += (0.0f - visualAngle) * 15.0f * dt; }
};

export class PipeGrid {
public:
    PipeGrid(int w = 12, int h = 8) : mWidth(w), mHeight(h), mCells(w * h) {}
    Pipe& GetPipe(int x, int y) { return mCells[y * mWidth + x]; }
    const Pipe& GetPipe(int x, int y) const { return mCells[y * mWidth + x]; }
    int GetWidth() const { return mWidth; }
    int GetHeight() const { return mHeight; }
    GameState GetState() const { return mState; }
    float GetTimeLeft() const { return mTimeLeft; }
    float GetMaxTime() const { return mMaxTime; }
    void SetMaxTime(float t) { mMaxTime = t; mTimeLeft = t; }

    void OpenValve() { mState = GameState::SIMULATING; CalculatePathTree(); GetPipe(0, 4).isFilling = true; GetPipe(0, 4).flowDistance = 0; }
    void ResetValve() { mState = GameState::PLANNING; mTimeLeft = mMaxTime; for(auto& p : mCells) { p.flowDistance = -1; p.isLeak = false; p.fillAmount = 0.0f; p.isFilling = false; p.isFull = false; } }

    void CalculatePathTree() {
        for(auto& p : mCells) { p.flowDistance = -1; p.isLeak = false; }
        std::queue<std::pair<int, int>> q; q.push({0, 4}); GetPipe(0, 4).flowDistance = 0;
        while(!q.empty()) {
            auto [x, y] = q.front(); q.pop(); int d = GetPipe(x, y).flowDistance;
            CheckNeighbor(x, y, x, y-1, Side::NORTH, Side::SOUTH, d, q);
            CheckNeighbor(x, y, x+1, y, Side::EAST,  Side::WEST,  d, q);
            CheckNeighbor(x, y, x, y+1, Side::SOUTH, Side::NORTH, d, q);
            CheckNeighbor(x, y, x-1, y, Side::WEST,  Side::EAST,  d, q);
        }
    }

    void Update(float dt) {
        for(auto& p : mCells) p.UpdateAnim(dt);
        if (mState == GameState::PLANNING) { 
            if (mTimeLeft > 0) mTimeLeft -= dt; 
            else mState = GameState::DEFEAT; // Temps écoulé en phase planning
            return; 
        }
        if (mState != GameState::SIMULATING) return;
        
        float fillSpeed = 4.0f; bool stillFlowing = false; bool leakDetected = false;
        for (int y = 0; y < mHeight; ++y) {
            for (int x = 0; x < mWidth; ++x) {
                Pipe& p = GetPipe(x, y);
                if (p.isFilling && !p.isFull) {
                    stillFlowing = true; p.fillAmount += dt * fillSpeed;
                    if (p.fillAmount >= 1.0f) { p.fillAmount = 1.0f; p.isFull = true; TriggerNeighbors(x, y); }
                }
                if (p.isLeak && p.isFull) leakDetected = true;
            }
        }
        // Logique de fin de jeu
        if (!stillFlowing) {
            if (leakDetected) mState = GameState::DEFEAT; // Echec : Fuite
            else {
                // Victoire si on a rempli au moins 8 tuyaux connectés (pour éviter victoire immédiate sur 1 tuyau)
                int filledCount = 0;
                for(auto& p : mCells) if(p.isFull) filledCount++;
                
                if (filledCount > 8) mState = GameState::VICTORY; 
                else mState = GameState::DEFEAT; // Echec : Bloqué trop tôt
            }
        }
    }

private:
    void TriggerNeighbors(int x, int y) {
        Pipe& curr = GetPipe(x, y); int d = curr.flowDistance;
        auto Activate = [&](int nx, int ny) { if (nx >=0 && nx < mWidth && ny >= 0 && ny < mHeight) { Pipe& next = GetPipe(nx, ny); if (next.flowDistance == d + 1) next.isFilling = true; } };
        if (curr.connections & 1) Activate(x, y-1); if (curr.connections & 2) Activate(x+1, y); if (curr.connections & 4) Activate(x, y+1); if (curr.connections & 8) Activate(x-1, y);
    }
    void CheckNeighbor(int x1, int y1, int x2, int y2, Side out, Side in, int d, std::queue<std::pair<int, int>>& q) {
        Pipe& curr = GetPipe(x1, y1);
        if (x2 < 0 || x2 >= mWidth || y2 < 0 || y2 >= mHeight) { if (curr.connections & (uint8_t)out) curr.isLeak = true; return; }
        Pipe& next = GetPipe(x2, y2);
        if ((curr.connections & (uint8_t)out)) { if ((next.connections & (uint8_t)in)) { if(next.flowDistance == -1) { next.flowDistance = d + 1; q.push({x2, y2}); } } else { curr.isLeak = true; } }
    }
    int mWidth, mHeight;
    std::vector<Pipe> mCells;
    GameState mState = GameState::PLANNING;
    float mMaxTime = 60.0f;
    float mTimeLeft = 60.0f;
};