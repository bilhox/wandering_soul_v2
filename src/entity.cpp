#include "headers/entity.hpp"
#include <iostream>
#include <cmath>

Entity::Entity(){
    m_vertices.resize(4);
    m_vertices.setPrimitiveType(sf::TriangleStrip);
    m_rect = sf::FloatRect(sf::Vector2f(0,0) , sf::Vector2f(0,0));
    m_collisionSide = {{"right" , false},{"left" , false},{"top" , false},{"down" , false}};
    m_flip = false;
    m_rotation = 0.f;
    setOrigin({0,0});
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

const sf::FloatRect & Entity::getRect() const {
    return m_rect;
}

void Entity::move(const sf::Vector2f & offset){
    Transformable::move(offset);
    m_rect.left += offset.x;
    m_rect.top += offset.y;
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

void Entity::resetTextCoords(){
    for(int i = 0; i < m_vertices.getVertexCount();i++){
        sf::Vertex* vertex = &m_vertices[i];
        vertex->texCoords = sf::Vector2f(m_textSize.x*(i%2),m_textSize.y*(i/2));
    }
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
    auto origin = getOrigin();
    transform.rotate(m_rotation , origin.x , origin.y);
    states.transform = transform;
    states.texture = &*m_texture;

    target.draw(m_vertices , states);
}

sf::Vector2f Entity::getSize() const {
    return m_rect.getSize();
}

void Entity::setTextOffset(sf::Vector2f offset){
    m_textOffset = offset;
}