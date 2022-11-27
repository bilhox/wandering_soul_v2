#ifndef ENTITY_HPP
#define ENTITY_HPP
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
        void collisions(const std::vector<sf::FloatRect> & colliders);
        virtual void postUpdate(float dt);
        void setTexture(sf::Texture & texture);
        void setSize(sf::Vector2f size);
        void setTextSize(sf::Vector2f size);
        sf::Vector2f getSize() const;
        void display(sf::RenderWindow & window , sf::View view) const;

    protected:

        sf::Vector2f m_velocity;
        std::map<std::string , bool> m_collisionSide;
        sf::FloatRect m_rect;
        sf::VertexArray m_vertices;
        sf::Texture* m_texture;
        sf::Vector2f m_textOffset;
        sf::Vector2f m_textSize;
        bool m_flip;

    private:

        void draw(sf::RenderTarget & target , sf::View view) const;
};

#endif // ENTITY_HPP