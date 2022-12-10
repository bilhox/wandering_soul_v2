#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>

class Entity : public sf::Transformable {
    public:
        Entity();
        virtual void update(float dt);
        virtual void events(sf::Event & event , sf::Window & window , float dt);
        void setPosition(const sf::Vector2f & position);
        void move(const sf::Vector2f & offset);
        void setTexture(sf::Texture & texture);
        void setSize(sf::Vector2f size);
        void setTextSize(sf::Vector2f size);
        void setTextOffset(sf::Vector2f offset);
        const sf::Vector2f & getTextOffset() const;
        const sf::FloatRect & getRect() const;
        void resetTextCoords();
        sf::Vector2f getSize() const;
        void setLightRadius(float radius);
        void setLightColor(sf::Color color);
        void display(sf::RenderWindow & window , sf::View view) ;

    protected:

        sf::Vector2f m_velocity;
        std::map<std::string , bool> m_collisionSide;
        sf::FloatRect m_rect;
        sf::VertexArray m_vertices;
        sf::Texture* m_texture = nullptr;
        sf::Vector2f m_textOffset;
        sf::Vector2f m_textSize;
        sf::CircleShape m_light;
        float m_rotation;
        bool m_flip;

    private:

        void draw(sf::RenderTarget & target , sf::View view) ;
};