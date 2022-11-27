#ifndef COLLISION_UTILS_HPP
#define COLLISION_UTILS_HPP
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

std::vector<sf::FloatRect> collide(const std::vector<sf::FloatRect> & colliders , sf::FloatRect & rect);

struct Line {
    bool direction;
    bool sens;
    float length;
    sf::Vector2f startPoint;

    Line(sf::Vector2f startPoint , bool direction , bool sens , float length);
    sf::Vector2f getEndPos();
};

std::vector<sf::FloatRect> collideLine(const std::vector<sf::FloatRect> & colliders , sf::FloatRect rect , sf::Vector2f movement);

#endif