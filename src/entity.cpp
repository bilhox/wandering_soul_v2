#include "headers/entity.hpp"
#include "headers/utils.hpp"
#include <iostream>
#include <cmath>

Entity::Entity(AssetManager* assets){
    m_assets = assets;
    m_vertices.resize(6);
    m_vertices.setPrimitiveType(sf::Triangles);
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
    sf::Vertex* vertex = &m_vertices[0];
    vertex[0].position = sf::Vector2f(0,0);
    vertex[1].position = sf::Vector2f(size.x,0);
    vertex[2].position = sf::Vector2f(0,size.y);
    vertex[3].position = sf::Vector2f(size.x,0);
    vertex[4].position = sf::Vector2f(0,size.y);
    vertex[5].position = size;
}

void Entity::setTexture(sf::Texture & texture){
    m_texture = &texture;
    setSize(sf::Vector2f(m_texture->getSize()));
}

void Entity::setTextPos(sf::Vector2f start , sf::Vector2f end){

    sf::Vector2f ts = end-start;

    sf::Vertex* vertex = &m_vertices[0];

    vertex[0].texCoords = start+sf::Vector2f(0,0);
    vertex[1].texCoords = start+sf::Vector2f(ts.x,0);
    vertex[2].texCoords = start+sf::Vector2f(0,ts.y);
    vertex[3].texCoords = start+sf::Vector2f(ts.x,0);
    vertex[4].texCoords = start+sf::Vector2f(0,ts.y);
    vertex[5].texCoords = start+ts;

    m_textSize = ts;
}

void Entity::resetTextCoords(){
    sf::Vertex* vertex = &m_vertices[0];
    vertex[0].texCoords = sf::Vector2f(0,0);
    vertex[1].texCoords = sf::Vector2f(m_textSize.x,0);
    vertex[2].texCoords = sf::Vector2f(0,m_textSize.y);
    vertex[3].texCoords = sf::Vector2f(m_textSize.x,0);
    vertex[4].texCoords = sf::Vector2f(0,m_textSize.y);
    vertex[5].texCoords = m_textSize;
}

void Entity::display(sf::RenderWindow & window , sf::View view)  {
    draw(window , view);
}

const sf::Vector2f & Entity::getTextOffset() const {
    return m_textOffset;
}

Light & Entity::getLight(){
    return m_light;
}

void Entity::draw(sf::RenderTarget & target , sf::View view) {
    sf::RenderStates states;
    sf::Vector2f tvpSize { Const::ORIGINAL_WINSIZE };
    auto zoom = sf::Vector2f(tvpSize.x / (view.getSize().x) , tvpSize.y / (view.getSize().y));
    auto translation = getPosition()+sf::Vector2f((m_flip)?m_textSize.x:0,0)-m_textOffset-view.getCenter()+view.getSize()*0.5f;
    translation.x = std::round(translation.x*zoom.x);
    translation.y = std::round(translation.y*zoom.y);
    sf::Transform transform = sf::Transform::Identity;
    transform.translate(translation);
    transform.scale(sf::Vector2f(((m_flip)?-1:1)*zoom.x,zoom.y));
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