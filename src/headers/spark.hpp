#pragma once

#include <SFML/Graphics.hpp>

class Spark{
    public:

        float speed;
        float rotation;
        float scale;
        float speedScale;

        Spark(const sf::Vector2f & position , float rotation = 0.f , float speed = 1.f , float scale = 1.f , float speedScale = 2.f);
        void update(float dt);
        void setPosition(const sf::Vector2f & position);
        const sf::Vector2f & getPosition() const;
        void move(const sf::Vector2f & movement);
        void setColor(const sf::Color & color);
        const sf::Color & getColor() const;
        void draw(sf::RenderTarget & target , sf::View view) const;

    private:
        sf::VertexArray m_vertices;
        sf::Color m_color;
        sf::Vector2f m_position;
};