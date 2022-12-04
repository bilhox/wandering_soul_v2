#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include "animation.hpp"
#include "utils.hpp"

enum class TextState{
    ANIMATION,
    TEXTURE
};

class Particle{

    public:
        sf::Vector2f movement;
        float speed;

    public:
        Particle(sf::Vector2f startpos , sf::Vector2f direction , float speed , float duration);
        void update(float dt);
        void move(const sf::Vector2f & movement);
        void display(sf::RenderTarget & target , sf::View view);
        void setTexture(sf::Texture & texture);
        void setAnimation(Animation & aninmation);
        sf::VertexArray & getVertices();
        bool isAlive() const;
        void setTextPos(sf::Vector2f start , sf::Vector2f end);
        const sf::Vector2f & getPosition() const;
    
    private:
        sf::Texture* m_texture = nullptr;
        sf::VertexArray m_vertices;
        sf::Vector2f m_position;
        bool m_alive;
        float m_duration;
        sf::Vector2f m_textSize;
        Animation m_anim;
        TextState m_textState;

};

class ParticleSystem{
    public:
        ParticleSystem(sf::Vector2f startpos);
        void update(float dt);
        void display(sf::RenderTarget & target , sf::View view);
        void setPosition(const sf::Vector2f & position);
        void setSpawnRate(float rate);
        void spawnParticles(unsigned int number);
        void setTexture(sf::Texture & texture);
        void setRange(std::string type , float min , float max);
        void setContinuous(bool state);
        void setAnimation(Animation & animation);
        void setLightRadius(float radius);
        void setLightColor(sf::Color color);
        void clear();
    
    private:

        float m_spawnRate;
        float m_timer;
        bool m_continuous;
        sf::Vector2f m_position;
        std::map<std::string , urdf> m_urdfs;
        std::vector<Particle> m_particles;
        TextState m_textState;
        Animation m_anim;
        sf::Texture* m_texture = nullptr;
        sf::CircleShape m_light;
};