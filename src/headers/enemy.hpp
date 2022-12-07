#pragma once

#include "utils.hpp"
#include "entity.hpp"
#include "asset_manager.hpp"

struct Pupil {
    sf::CircleShape red;
    sf::CircleShape black;
    sf::Vector2f offset{};
    Pupil(float radius);
    void setPosition(sf::Vector2f pos);
    void setScale(sf::Vector2f scale);
};

enum class AttackState{
    ARROW,
    SPIRAL,
    CIRCLE,
    CIRCLEV2
};

class Eye : public Entity {
    public:
        Eye(AssetManager* assets);
        void update(float dt , std::vector<EntityData> & projectiles , const sf::Vector2f & player_pos);
        void updatePupil(const sf::Vector2f & player_pos);
        void display(sf::RenderWindow & window , sf::View view);
    
    private:
        float m_timer;
        AttackState m_attack;
        AssetManager* m_assets;
        float m_spawnRate;
        float m_formDelay;
        Pupil m_pupil {8.f};
};