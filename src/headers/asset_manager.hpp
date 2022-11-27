#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP
#pragma once

#include <SFML/Graphics.hpp>
#include "json.hpp"
#include "animation.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <filesystem>


class AssetManager {

    public:
        AssetManager();
        bool loadFromFile(std::string json_path);
        template<class Asset>
        bool load(std::string name , std::string path);
        template<class Asset>
        Asset& getAsset(std::string name);
    
    private:
        std::map<std::string , sf::Texture> m_textures;
        std::map<std::string , Animation> m_animations;
};

template<>
bool AssetManager::load<sf::Texture>(std::string name , std::string path){
    sf::Texture texture;
    if(!texture.loadFromFile(path))
        return false;
    
    m_textures[name] = texture;
    return true;
}

template<>
bool AssetManager::load<Animation>(std::string name , std::string path){
    Animation animation;
    
    if(!animation.load(path))
        return false;

    m_animations[name] = animation;
    return true;
}

template<>
sf::Texture & AssetManager::getAsset<sf::Texture>(std::string name){
    if(m_textures.find(name) != m_textures.end())
        return m_textures[name];
    
    throw std::runtime_error("With AssetManager object : Could not find a texture with the key '"+name+"'");
}

template<>
Animation & AssetManager::getAsset<Animation>(std::string name){
    if(m_animations.find(name) != m_animations.end())
        return m_animations[name];
    
    throw std::runtime_error("With AssetManager object : Could not find an animation with the key '"+name+"'");
}

#endif // ASSET_MANAGER_HPP