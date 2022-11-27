#include "headers/entity.hpp"
#include "headers/collision_utils.hpp"
#include <iostream>
#include <cmath>

Entity::Entity(){
    m_vertices.resize(4);
    m_vertices.setPrimitiveType(sf::TriangleStrip);
    m_rect = sf::FloatRect(sf::Vector2f(0,0) , sf::Vector2f(0,0));
    m_collisionSide = {{"right" , false},{"left" , false},{"top" , false},{"down" , false}};
    m_flip = false;
}

void Entity::setPosition(const sf::Vector2f & position){
    Transformable::setPosition(position);
    // {std::round(position.x) , std::round(position.y)}
    m_rect.left = position.x;
    m_rect.top = position.y;
}

void Entity::update(float dt){
}

void Entity::events(sf::Event & event , sf::Window & window , float dt){

}

void Entity::postUpdate(float dt){
}

void Entity::move(const sf::Vector2f & offset){
    Transformable::move(offset);
    m_rect.left += offset.x;
    m_rect.top += offset.y;
}

void Entity::collisions(const std::vector<sf::FloatRect> & colliders){
    m_collisionSide = {{"right" , false},{"left" , false},{"top" , false},{"down" , false}};
    move({m_velocity.x , 0});
    auto collided = collide(colliders , m_rect);
    for (auto & collider : collided){
        if(m_velocity.x < 0){
            setPosition({collider.left+collider.width,m_rect.top});
            m_collisionSide["left"] = true;
        }
        else if (m_velocity.x > 0){
            setPosition({collider.left-m_rect.width,m_rect.top});
            m_collisionSide["right"] = true;
        }
    }

    move({0,m_velocity.y});
    collided = collide(colliders , m_rect);
    for (auto & collider : collided){
        if(m_velocity.y < 0){
            setPosition({m_rect.left,collider.top+collider.height});
            m_collisionSide["top"] = true;
        }
        else if (m_velocity.y > 0){
            setPosition({m_rect.left,collider.top-m_rect.height});
            m_collisionSide["down"] = true;
        }
    }
}

void Entity::setSize(sf::Vector2f size){
    m_rect.height = size.y;
    m_rect.width = size.x;
}

void Entity::setTextSize(sf::Vector2f size){
    m_textSize = size;
    for(int i = 0; i < m_vertices.getVertexCount();i++){
        sf::Vertex* vertex = &m_vertices[i];
        vertex->position = sf::Vector2f(size.x*(i%2),size.y*(i/2));
    }
}

void Entity::setTexture(sf::Texture & texture){
    m_texture = &texture;
    setSize(sf::Vector2f(m_texture->getSize()));
}

void Entity::display(sf::RenderWindow & window , sf::View view) const {
    draw(window , view);
}

void Entity::draw(sf::RenderTarget & target , sf::View view) const {
    sf::RenderStates states;
    auto translation = getPosition()+sf::Vector2f((m_flip)?m_textSize.x:0,0)-m_textOffset-view.getCenter()+view.getSize()*0.5f;
    translation.x = std::round(translation.x*3.f);
    translation.y = std::round(translation.y*3.f);
    sf::Transform transform = sf::Transform::Identity;
    transform.translate(translation);
    transform.scale(sf::Vector2f((m_flip)?-3:3,3));
    states.transform = transform;
    states.texture = &*m_texture;

    target.draw(m_vertices , states);
}

sf::Vector2f Entity::getSize() const {
    return m_rect.getSize();
}