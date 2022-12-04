#include "headers/spark.hpp"
#include "./headers/utils.hpp"
#include <cmath>

const sf::Vector2f & Spark::getPosition() const {
    return m_position;
}

void Spark::setPosition(const sf::Vector2f & position){
    m_position = position;
}

Spark::Spark(const sf::Vector2f & position , float rotation , float speed , float scale , float speedScale){
    setPosition(position);
    this->speed = speed;
    this->rotation = rotation;
    m_vertices.resize(4);
    m_vertices.setPrimitiveType(sf::TriangleStrip);
    this->scale = scale;
    this->speedScale = speedScale;

}

void Spark::setColor(const sf::Color & color) {
    m_color = color;
    for(int i = 0;i < m_vertices.getVertexCount();i++){
        sf::Vertex* vertex = &m_vertices[i];
        vertex->color = color;
    }
}

const sf::Color & Spark::getColor() const {
    return m_color;
}

void Spark::move(const sf::Vector2f & movement){
    m_position += movement;
}

void Spark::update(float dt){

    move(sf::Vector2f(std::cos(rotation),std::sin(rotation))*speed*dt);
    m_vertices[0].position = -sf::Vector2f(std::cos(rotation),std::sin(rotation))*speedScale*scale*2.5f;
    m_vertices[1].position = sf::Vector2f(std::cos(rotation+M_PI/2.f),std::sin(rotation+M_PI/2.f))*speedScale*scale*0.3f;
    m_vertices[2].position = sf::Vector2f(std::cos(rotation-M_PI/2.f),std::sin(rotation-M_PI/2.f))*speedScale*scale*0.3f;
    m_vertices[3].position = sf::Vector2f(std::cos(rotation),std::sin(rotation))*speedScale*scale;

}

void Spark::draw(sf::RenderTarget & target , sf::View view) const {
    sf::RenderStates states;
    sf::Vector2f tvpSize { Const::ORIGINAL_WINSIZE };
    auto zoom = sf::Vector2f(tvpSize.x / (view.getSize().x) , tvpSize.y / (view.getSize().y));
    auto translation = getPosition()-view.getCenter()+view.getSize()*0.5f;
    translation.x = std::round(translation.x*zoom.x);
    translation.y = std::round(translation.y*zoom.y);
    sf::Transform transform = sf::Transform::Identity;
    transform.translate(translation);
    transform.scale(sf::Vector2f(zoom.x,zoom.y));
    states.transform = transform;
    states.texture = nullptr;

    target.draw(m_vertices , states);
}