#include <SFML/Graphics.hpp>
#include "headers/entity.hpp"
#include <iostream>
#include "headers/tilemap.hpp"
#include "headers/player.hpp"
#include <cmath>
#include <random>

struct EntityData{
    Entity projectile;
    sf::Vector2f movement;
};

EntityData instanciateProjectile(sf::Texture & texture){
    Entity projectile{};
    projectile.setTexture(texture);
    projectile.setSize({5,5});
    projectile.setTextSize({5,5});
    projectile.setTextOffset({2,2});
    projectile.resetTextCoords();
    EntityData entData;
    entData.movement = {-1.f , 0};
    entData.projectile = projectile;
    return entData;
}

int main()
{

    auto map = Tilemap();

    map.load("../assets/map_test.json");

    sf::FloatRect mapRect = {{0,0},sf::Vector2f{(float)map.getSize().x*map.getTileSize().x , (float)map.getSize().y*map.getTileSize().y}};
    auto window = sf::RenderWindow{ { 900u, 600u }, "XML parser test"};
    window.setFramerateLimit(120);
    window.setKeyRepeatEnabled(false);

    Player player {};

    auto startPos = map.getObject("player_position");
    player.setPosition(startPos.rect.getPosition());
    float death_time = 2.f;
    float proj_spawn = 0.25f;

    sf::Clock clock{};
    float dt = 0;

    std::vector<sf::FloatRect> colliders{};
    
    sf::View camera {sf::FloatRect({0,0},{300 , 200})};
    std::vector<EntityData> projectiles;

    sf::Texture proj;
    proj.loadFromFile("../assets/images/projectile.png");

    for(int i = 0;i < 14;i++){
        auto entData = instanciateProjectile(proj);
        entData.projectile.setPosition({300,((200 / 12.f)*i)});
        projectiles.push_back(entData);
    }

    float dtAvg = 0.f;
    int tick = 0;

    while (window.isOpen())
    {
        dt = clock.getElapsedTime().asSeconds();
        clock.restart();

        dtAvg += dt;
        tick ++;
        if(dtAvg >= 0.2f){
            float fps = 1/(dtAvg/(float)tick);
            tick = 0;
            dtAvg = 0.f;
            window.setTitle("FPS : " + std::to_string(fps));
        }

        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            switch(event.type){
                case sf::Event::Closed:
                    window.close();
                    break;
                
                case sf::Event::Resized:
                    
                    {   
                        auto viewSize = window.getView().getSize();
                        auto winView = window.getView();
                        winView.setViewport({
                            0.5f-(viewSize.x/2.f)/(float)event.size.width,
                            0.5f-(viewSize.y/2.f)/(float)event.size.height,
                            (viewSize.x)/(float)event.size.width,
                            (viewSize.y)/(float)event.size.height
                        });
                        window.setView(winView);
                    }
                    break;
                
                default:
                    break;
            }
            if(window.hasFocus()){
                player.events(event , window , dt);
            }
        }

        player.update(dt);
        player.collisions(map.getColliders() , camera);
        player.postUpdate(dt);

        for(int i = projectiles.size() -1 ; i >= 0 ; i--){
            projectiles[i].projectile.move(projectiles[i].movement);
            if(player.isAlive()){

                if(player.getRect().intersects(projectiles[i].projectile.getRect()))
                    player.die();
                
                if(!projectiles[i].projectile.getRect().intersects(mapRect)){
                    projectiles.erase(projectiles.begin()+i);
                }
            }
        }

        if(!player.isAlive()){
            death_time -= dt;
            if(death_time <= 0.f){
                death_time = 2.f;
                player.respawn();
            }
        }

        if(!player.isSoul() && player.isAlive())
            camera.move(((player.getPosition()+player.getSize()*.5f)-camera.getCenter())*0.1f);
        sf::Vector2f topleftCameraCorner = (camera.getCenter()-camera.getSize()/2.f);

        if(topleftCameraCorner.x < 0){
            camera.setCenter({(camera.getSize()/2.f).x , camera.getCenter().y});
        } else if ((camera.getCenter()+camera.getSize()*0.5f).x > map.getSize().x*map.getTileSize().x){
            camera.setCenter({map.getSize().x*map.getTileSize().x-(camera.getSize()/2.f).x,camera.getCenter().y});
        }
        if (topleftCameraCorner.y < 0){
            camera.setCenter({camera.getCenter().x,(camera.getSize()/2.f).y});
        } else if ((camera.getCenter()+camera.getSize()*0.5f).y > map.getSize().y*map.getTileSize().y){
            camera.setCenter({camera.getCenter().x,map.getSize().y*map.getTileSize().y-(camera.getSize()/2.f).y});
        }

        proj_spawn -= dt;
        if(proj_spawn <= 0.f){
            auto p = instanciateProjectile(proj);
            int angle = Random::randInt(urdi(60,120));
            sf::FloatRect camRect = {camera.getCenter()-camera.getSize()*0.5f,camera.getSize()};
            int randXpos = Random::randInt(urdi(0,(int)camRect.width));
            p.movement = {std::cos((float)(M_PI/180)*angle),std::sin((float)(M_PI/180)*angle)};
            p.projectile.setPosition({camRect.left+randXpos,camRect.top});
            projectiles.push_back(p);
            proj_spawn = 0.25f;
        }


        window.clear();
        map.display(window , camera , 0);
        player.display(window , camera , 0);
        for(auto & p : projectiles){
            p.projectile.display(window , camera);
        }
        map.display(window , camera , 1);
        player.display(window , camera , 1);
        window.display();
    }

    return 0;
}