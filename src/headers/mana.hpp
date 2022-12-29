#pragma once

#include <SFML/Graphics.hpp>

class ManaOrb {

    public:
        ManaOrb();
        void update();
        void setPosition(const sf::Vector2f & position);
        bool collideRect(const sf::FloatRect & rect) const;
        void display(sf::RenderTarget & target , sf::View view);
    
    private:
        sf::VertexArray m_vertices;
        sf::Shader m_shader;
        sf::FloatRect m_rect;
        sf::Clock m_timer;
};