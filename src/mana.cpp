#include "headers/mana.hpp"
#include <cmath>
#include "headers/utils.hpp"

ManaOrb::ManaOrb(){
    m_rect = sf::FloatRect();
    m_rect.width = 16.f;
    m_rect.height = 16.f;

    m_vertices.resize(6);
    m_vertices.setPrimitiveType(sf::Triangles);

    sf::Vertex* vertex = &m_vertices[0];
    vertex[0].position = {0 , 0};
    vertex[1].position = {16.f , 0.f};
    vertex[2].position = {0.f , 16.f};
    vertex[3].position = {16.f , 0.f};
    vertex[4].position = {0 , 16.f};
    vertex[5].position = {16.f , 16.f};

    vertex[0].texCoords = {0 , 0};
    vertex[1].texCoords = {16.f , 0.f};
    vertex[2].texCoords = {0.f , 16.f};
    vertex[3].texCoords = {16.f , 0.f};
    vertex[4].texCoords = {0 , 16.f};
    vertex[5].texCoords = {16.f , 16.f};

    m_shader = std::make_unique<sf::Shader>();
    
    if(!m_shader->loadFromFile("../assets/manaOrb.frag" , sf::Shader::Fragment)){
        throw std::runtime_error("Failed to shader manaOrb.");
    }

    m_shader->setUniform("u_resolution" , m_rect.getSize());

}

void ManaOrb::setPosition(const sf::Vector2f & position){
    m_rect.left = position.x;
    m_rect.top = position.y;
}

sf::Vector2f ManaOrb::getPosition() const {
    return m_rect.getPosition()+m_rect.getSize()/2.f;
}

void ManaOrb::updateShader(){
    m_shader->setUniform("u_time" , m_timer.getElapsedTime().asSeconds());
}

bool ManaOrb::collideRect(const sf::FloatRect & rect) const {
    return m_rect.intersects(rect);
}

void ManaOrb::display(sf::RenderTarget & target , sf::View view){
    sf::RenderStates states;
    sf::Vector2f tvpSize { Const::ORIGINAL_WINSIZE };
    auto zoom = sf::Vector2f(tvpSize.x / (view.getSize().x) , tvpSize.y / (view.getSize().y));
    auto translation = m_rect.getPosition()-getOrigin()-view.getCenter()+view.getSize()*0.5f;
    translation.x = std::round(translation.x*zoom.x);
    translation.y = std::round(translation.y*zoom.y);
    updateShader();
    sf::Transform transform = sf::Transform::Identity;
    transform.translate(translation);
    transform.scale(sf::Vector2f(zoom.x,zoom.y)*getScale());
    states.transform = transform;
    states.texture = nullptr;
    states.shader = m_shader.get();

    target.draw(m_vertices , states);

}