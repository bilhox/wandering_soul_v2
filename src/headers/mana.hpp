#pragma once

#include <SFML/Graphics.hpp>
#include "asset_manager.hpp"

class ManaOrb : public sf::Transformable{

    public:
        ManaOrb(const sf::Shader & orbShader);
        void setPosition(const sf::Vector2f & position);
        sf::Vector2f getPosition() const;
        bool collideRect(const sf::FloatRect & rect) const;
        void display(sf::RenderTarget & target , sf::View view) const;
    
    private:
        sf::VertexArray m_vertices;
        const sf::Shader* m_shader;
        sf::FloatRect m_rect;
        sf::Clock m_timer;
};