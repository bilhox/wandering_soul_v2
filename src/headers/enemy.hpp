#pragma once

#include "utils.hpp"
#include "entity.hpp"
#include "asset_manager.hpp"
#include "spark.hpp"
#include "player.hpp"

struct Pupil {
    sf::CircleShape red;
    sf::CircleShape black;
    sf::Vector2f offset{};
    Pupil(float radius);
    void setPosition(sf::Vector2f pos);
    void setScale(sf::Vector2f scale);
};

enum class AttackState{
    NONE,
    ARROW,
    SPIRAL,
    SPIRALV2,
    CIRCLE,
    CIRCLEV2
};

class Eye : public Entity {

    public:
        Player* player;
        std::vector<EntityData>* projectiles;
        std::vector<Light>* lights;

    public:
        Eye(AssetManager* assets);
        void update(float dt) override;
        void updatePupil(const sf::Vector2f & player_pos);
        void display(sf::RenderWindow & window , sf::View view);
        void setTargetPos(const sf::Vector2f & pos);
        void resetAttacks();
        void setVertexPositions();
        bool isDead();
    
    private:
        float m_eventTimer;
        AttackState m_attack;
        unsigned int m_attacks;
        AssetManager* m_assets;
        std::vector<Spark> m_sparks;
        float m_spawnRate;
        float m_formDelay;
        float m_stateDelay;
        bool m_spawnForm;
        unsigned int m_nProjSpawned;
        sf::Vector2f m_targetPos;
        bool m_dead;
        float m_deathTimer;
        Pupil m_pupil {8.f};
};