module;
#include <vector>
#include <cmath>
#include <cstdlib>
#include <imgui.h>

export module particles;

export enum class PType { WATER, STEAM };

struct Particle {
    ImVec2 pos;
    ImVec2 vel;
    float life;      
    float maxLife;
    float size;
    ImU32 color;
    PType type;
};

export class ParticleSystem {
public:
    void Spawn(ImVec2 pos, PType type, ImU32 colorOverride) {
        Particle p;
        p.pos = pos;
        p.type = type;
        p.life = 1.0f;
        
        if (type == PType::WATER) {
            float angle = (rand() % 360) * 3.14f / 180.0f;
            float speed = (rand() % 100) / 20.0f + 2.0f;
            p.vel = ImVec2(cos(angle) * speed, sin(angle) * speed);
            p.maxLife = (rand() % 50) / 100.0f + 0.5f;
            p.size = (rand() % 30) / 10.0f + 2.0f;
            p.color = colorOverride;
        } 
        else { 
            float angle = -1.57f + ((rand() % 100 - 50) / 100.0f);
            float speed = (rand() % 100) / 50.0f + 0.5f;
            p.vel = ImVec2(cos(angle) * speed * 0.5f, sin(angle) * speed);
            p.maxLife = (rand() % 100) / 50.0f + 1.0f;
            p.size = (rand() % 50) / 10.0f + 4.0f;
            p.color = IM_COL32(255, 255, 255, 30);
        }
        mParticles.push_back(p);
    }

    void UpdateAndRender(ImDrawList* dl, float dt) {
        for (size_t i = 0; i < mParticles.size(); ) {
            Particle& p = mParticles[i];
            p.life -= dt / p.maxLife; 
            p.pos.x += p.vel.x; p.pos.y += p.vel.y;
            if (p.type == PType::WATER) p.vel.y += 0.5f; else p.vel.y -= 0.1f;

            if (p.life <= 0) {
                mParticles[i] = mParticles.back(); mParticles.pop_back();
            } else {
                ImU32 col = p.color;
                int alpha = (col >> 24) & 0xFF;
                alpha = (int)(alpha * p.life);
                col = (col & 0x00FFFFFF) | (alpha << 24);
                dl->AddCircleFilled(p.pos, p.size, col);
                i++;
            }
        }
    }
private:
    std::vector<Particle> mParticles;
};