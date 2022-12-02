#include "headers/animation.hpp"
#include <iostream>

Animation::Animation(){

    m_duration = 0.f;
    m_timepoints = {};
    m_timer = 0.f;
    m_currentIndex = 0;
    m_slice = {0,0};
    name = "";
}

void Animation::reset(){
    m_currentIndex = 0.f;
    m_timer = 0.f;
}

bool Animation::load(std::string anim_path) {

    m_duration = 0.f;
    m_timepoints = {};
    m_timer = 0.f;
    m_currentIndex = 0;
    m_slice = {0,0};

    std::ifstream fs {anim_path};

    if(!fs){
        std::cout << "Unable to open file at "+anim_path << std::endl;
        return false;
    }

    nlohmann::json animFile = nlohmann::json::parse(fs);

    auto sourcePath = std::filesystem::path(anim_path);
    sourcePath.replace_filename(animFile["source"]);

    if(!m_sheet.loadFromFile(sourcePath.string())){
        return false;
    }

    auto slice = animFile["slice"].get<std::vector<unsigned int>>();
    m_slice = {slice[0] , slice[1]};
    
    auto durations = animFile["frames"].get<std::vector<float>>();
    float sum = 0;
    for(auto & duration : durations){
        m_timepoints.push_back(sum);
        sum += duration;
    }

    m_duration = sum;
    return true;
}

void Animation::update(float dt){
    m_timer += dt;

    if(m_timer - m_duration >= 0.f)
        m_timer = 0.f;

    int i = 0;
    for(auto & duration : m_timepoints){
        if(duration > m_timer){
            m_currentIndex = i;
            break;
        }
        i++;
    }

}

void Animation::prepareTexture(sf::VertexArray & vertices){
    
    auto offset = 0.f;
    offset += m_slice.x * m_currentIndex;
    for(int i = 0; i < vertices.getVertexCount();i++){
        sf::Vertex* vertex = &vertices[i];
        
        vertex->texCoords.x = offset+(i%2)*m_slice.x;
        vertex->texCoords.y = (i/2)*m_slice.y;
    }
}

sf::Texture & Animation::getTexture() {
    return m_sheet;
}