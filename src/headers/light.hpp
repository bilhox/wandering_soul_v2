#pragma once

#include <SFML/Graphics.hpp>

struct Light{
    sf::Vector2f position;
    float radius;
    float intensity;
    sf::Vector3f color;
};