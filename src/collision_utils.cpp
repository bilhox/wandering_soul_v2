#include "headers/collision_utils.hpp"
#include <iostream>

std::vector<sf::FloatRect> collide(const std::vector<sf::FloatRect> & colliders , sf::FloatRect & rect){

    std::vector<sf::FloatRect> collided;

    for (auto & collider : colliders){
        if(collider.intersects(rect)){
            collided.push_back(collider);
        }
    }

    return collided;
}

Line::Line(sf::Vector2f startPoint , bool direction , bool sens , float length){
    this->startPoint = startPoint;
    this->direction = direction;
    this->length = length;
    this->sens = sens;
}

sf::Vector2f Line::getEndPos(){
    return startPoint+sf::Vector2f((!direction)?((sens)?length:-length):0,(direction)?((sens)?-length:length):0);
}

std::vector<sf::FloatRect> collideLine(const std::vector<sf::FloatRect> & colliders , sf::FloatRect rect , sf::Vector2f movement){
    std::vector<sf::FloatRect> collided;
    sf::FloatRect spreadRect{rect.getPosition() , rect.getSize()+movement};

    for(auto & collider : colliders){
        if(collider.intersects(spreadRect)){
            collided.push_back(collider);
        }
    }

    return collided;
}