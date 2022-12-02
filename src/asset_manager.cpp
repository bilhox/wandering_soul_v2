#include "headers/asset_manager.hpp"
#include <iostream>

AssetManager::AssetManager(){
    m_textures = {};
    m_animations = {};
}

bool AssetManager::loadFromFile(std::string json_path){

    std::ifstream fs {json_path};
    if(!fs){
        std::cout << "Failed to load assets from json file at "+json_path << std::endl;
        return false;
    }

    auto source = std::filesystem::path(json_path);

    nlohmann::json assets = nlohmann::json::parse(fs);
    if(assets.contains("textures")){
        for(auto & text : assets["textures"]){
            auto textSource = source;
            textSource.replace_filename(text["source"]);
            load(text["name"] , textSource.string());
        }
    }

    if(assets.contains("animations")){
        for(auto & anim : assets["animations"]){
            auto animSource = source;
            animSource.replace_filename(anim["source"]);
            load(anim["name"] , animSource.string());
        }
    }

    return true;
}

bool AssetManager::load(std::string name , std::string path){

    auto source = std::filesystem::path(path);

    if(source.extension() == ".png" || source.extension() == ".jpg"){
        sf::Texture texture;
        if(!texture.loadFromFile(path))
            return false;
        
        m_textures[name] = texture;
        return true;
    } else if (source.extension() == ".json"){
        Animation animation;
    
        if(!animation.load(path))
            return false;

        animation.name = name;
        m_animations[name] = animation;
        return true;
    }
    return false;
}


sf::Texture & AssetManager::getTexture(std::string name){
    if(m_textures.find(name) != m_textures.end())
        return m_textures[name];
    
    throw std::runtime_error("With AssetManager object : Could not find a texture with the key '"+name+"'");
}


Animation & AssetManager::getAnimation(std::string name){
    if(m_animations.find(name) != m_animations.end())
        return m_animations[name];
    
    throw std::runtime_error("With AssetManager object : Could not find an animation with the key '"+name+"'");
}