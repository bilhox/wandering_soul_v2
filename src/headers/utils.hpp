#pragma once

#include <vector>
#include <string>
#include <random>
#include "entity.hpp"
#include <SFML/Graphics.hpp>

struct EntityData{
    Entity projectile{nullptr};
    sf::Vector2f movement;
};

struct Door{
    Entity door{nullptr};
    std::string destination;
    bool visible;
};

using urdf = std::uniform_real_distribution<float>;
using urdi = std::uniform_int_distribution<int>;

std::vector<std::string> split(std::string str , std::string delimiter);
std::string strip(std::string str , bool before = true , bool after = true);
bool overlaps(const sf::FloatRect & rect1 , const sf::FloatRect & rect2);
sf::Vector2f operator/(const sf::Vector2f& vec1 , const float& x);
sf::Vector2f operator*(const sf::Vector2f& vec1 , const sf::Vector2f& vec2);
EntityData instanciateProjectile(AssetManager* assets);
Door instanciateDoor(AssetManager* assets);


namespace Random{
    static std::random_device dev;
    static std::mt19937 rng = std::mt19937(dev());

    static float randFloat(urdf dist){
        return dist(rng);
    }

    static int randInt(urdi dist){
        return dist(rng);
    }
}

namespace Const{
    static const sf::Vector2f ORIGINAL_WINSIZE {900 , 600};
}
