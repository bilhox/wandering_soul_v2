#pragma once

#include "entity.hpp"
#include "animation.hpp"
#include "asset_manager.hpp"
#include "particle_system.hpp"
#include <map>
#include <unordered_map>
#include <array>
#include "spark.hpp"
#include "utils.hpp"
#include <SFML/Audio.hpp>

class Player : public Entity {

    public:

        Player(AssetManager* assets);
        void update(float dt);
        void events(sf::Event & event , sf::Window & window , float dt);
        void collisions(const std::vector<sf::FloatRect> & colliders , sf::View & view);
        const sf::FloatRect getRect() const;
        bool isSoul() const;
        void respawn(sf::Vector2f pos);
        void die();
        bool canEnterTheDoor(Door & door) const;
        bool isAlive() const;
        bool isAbleToReleaseSoul() const;
        void postUpdate(float dt);
        void display(sf::RenderWindow & window , sf::View view , unsigned int to_draw = 0) ;
        void changeState();
        void setMovementAbility(bool state);
        void setSoulReleasingAbility(bool state);
    
    private:

        enum class TextState{
            TEXTURE,
            ANIMATION
        };

        enum class State{
            SOUL,
            NORMAL
        };

        std::map<std::string , bool> m_keys;
        float m_gravityDt;
        float m_jumpAmount;
        int m_airtime;
        TextState m_textState;
        bool m_jumping;
        Animation* m_anim;
        ParticleSystem m_pSys {{0,0}};
        std::vector<Spark> m_sparks;
        State m_state;
        bool m_alive;
        bool m_ableToMove;
        bool m_ableToReleaseSoul;
        std::unordered_map<std::string , sf::Sound> m_sounds;
        std::array<float , 2> m_soulTimer {10.f , 0.f};

};
