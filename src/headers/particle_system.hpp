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
        sf::Vector2f position;

    public:
        Particle(sf::Vector2f direction , float speed , float duration);
        void update(float dt);
        bool isAlive() const;
        void setAnimation(Animation & animation);
        sf::IntRect getTextRect();
    
    private:

        bool m_alive;
        float m_duration;
        Animation m_anim;

};

class ParticleSystem{
    public:
        ParticleSystem(sf::Vector2f startpos);
        void update(float dt);
        void display(sf::RenderTarget & target , sf::View view);
        void setPosition(const sf::Vector2f & position);
        void setSpawnRate(float rate);
        void spawnParticles(unsigned int number);
        void setRange(std::string type , float min , float max);
        void setContinuous(bool state);
        void setAnimation(Animation & animation);
        void setColor(sf::Color color);
        void clear();
    
    private:

        float m_spawnRate;
        float m_timer;
        bool m_continuous;
        sf::Vector2f m_position;
        std::vector<sf::Vertex> m_vertices;
        std::map<std::string , urdf> m_urdfs;
        std::vector<Particle> m_particles;
        Animation m_anim;
        sf::Color m_color;
    
    private:

        void updateParticle(size_t index , sf::IntRect pRect);
    
};