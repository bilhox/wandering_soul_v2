#include <SFML/Graphics.hpp>
#include "headers/entity.hpp"
#include <iostream>
#include "headers/tilemap.hpp"
#include "headers/player.hpp"
#include "headers/utils.hpp"
#include <cmath>
#include <random>

struct EntityData{
    Entity projectile;
    sf::Vector2f movement;
};

struct Door{
    Entity door;
    std::string destination;
    bool visible;
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

Door instanciateDoor(sf::Texture & texture){
    Entity door{};
    door.setTexture(texture);
    door.setSize({16,32});
    door.setTextSize({16,32});
    door.resetTextCoords();
    Door d;
    d.door = door;
    return d;
}

int main()
{

    auto map = Tilemap();

    map.load("../assets/levels/level-1.json");

    sf::FloatRect mapRect = {{0,0},sf::Vector2f{(float)map.getSize().x*map.getTileSize().x , (float)map.getSize().y*map.getTileSize().y}};
    auto window = sf::RenderWindow{ {(unsigned int) Const::ORIGINAL_WINSIZE.x , (unsigned int) Const::ORIGINAL_WINSIZE.y} , "XML parser test"};
    window.setFramerateLimit(120);
    window.setKeyRepeatEnabled(false);

    AssetManager assets{};
    assets.loadFromFile("../assets/assets.json");

    Player player {&assets};

    auto startPos = map.getObject("player_position");
    player.setPosition(startPos.rect.getPosition());
    float death_time = 1.f;
    float proj_spawn = 0.5f;

    sf::Clock clock{};
    float dt = 0;

    std::vector<sf::FloatRect> colliders{};
    
    sf::View camera {sf::FloatRect({0,0},{300 , 200})};
    std::vector<EntityData> projectiles;

    auto dObj = map.getObject("door");

    auto door = instanciateDoor(assets.getTexture("door"));
    door.door.setPosition(dObj.rect.getPosition());
    door.destination = "../assets/levels/"+dObj.properties[0]["value"].get<std::string>();
    door.destination += ".json";
    door.visible = dObj.properties[1]["value"].get<bool>();

    int level = 1;

    std::vector<Spark> sparks;

    sf::Texture proj;
    proj.loadFromFile("../assets/images/projectile.png");

    float dtAvg = 0.f;
    int tick = 0;

    bool projSpawning = false;
    float slowTime = 0.f;
    float zoomTime = 1.2f;
    float game_timer = 0.f;
    float transition_time = 0.f;
    float lspt = 0.f;
    bool transition = false;

    sf::VertexArray black_filter;
    black_filter.resize(4);
    black_filter.setPrimitiveType(sf::TriangleStrip);

    ParticleSystem pSys {{0,0}};

    pSys.setAnimation(assets.getAnimation("player_particle"));
    pSys.setContinuous(false);
    pSys.setRange("speed" , 5 , 20);
    pSys.setRange("angle" , 0 , 360);
    pSys.setRange("duration" , 0.5f , 0.8f);
    pSys.setRange("offsetX" , 0 , 0);

    for(int i = 0;i < black_filter.getVertexCount();i++){
        sf::Vertex & vertex = black_filter[i];
        vertex.position = sf::Vector2f((i%2)*900.f , (i/2)*600.f);
        vertex.color = sf::Color(0,0,0,0);
    }

    while (window.isOpen())
    {
        dt = clock.getElapsedTime().asSeconds();
        clock.restart();

        dtAvg += dt;
        tick ++;
        if(dtAvg >= 1.f){
            float fps = 1/(dtAvg/(float)tick);
            tick = 0;
            dtAvg = 0.f;
            window.setTitle("FPS : " + std::to_string(fps));
        }

        if(slowTime > 0.f){
            slowTime -= dt;
            dt *= 0.5f;
        }

        game_timer += dt;

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
            if(window.hasFocus() && !transition){
                player.events(event , window , dt);
            }
        }

        pSys.update(dt);

        if(level == 2){

            if(game_timer > 30.f){
                if(game_timer < 40.f){
                    if(lspt == 0.f){
                        int dir = Random::randInt(urdi(0 , 1));
                        for(int i = 0;i < 22;i++){
                            sf::FloatRect camRect = {camera.getCenter()-camera.getSize()*0.5f,camera.getSize()};
                            auto entData = instanciateProjectile(proj);
                            auto pp = sf::Vector2f{(!dir)?camRect.left+camRect.width:camRect.left,(!dir)?0:20+(((float)(map.getSize().y*map.getTileSize().y) / 20.f)*i)};
                            entData.projectile.setPosition(pp);
                            entData.movement = sf::Vector2f((!dir)?-1:1 , 0)*50.f;
                            projectiles.push_back(entData);
                            for(int i = 0;i < 6 ;i++){
                                float sp = Random::randFloat(urdf(200,250));
                                float sc = Random::randFloat(urdf(8.f,10.5f));
                                float ang = Random::randFloat(urdf(-30,30));
                                Spark s {{pp.x,pp.y},(float)(M_PI/180.f)*(ang+(!dir)?180:0),sp,sc,sp/100.f};
                                s.setColor(sf::Color::Black);
                                sparks.push_back(s);
                            }
                        }
                    }
                    lspt += dt;
                    if(lspt > 2.f)
                        lspt = 0.f;
                } else {
                    door.visible = true;
                    pSys.spawnParticles(25);
                    pSys.setPosition(door.door.getPosition()+door.door.getSize()*0.5f);
                    game_timer = 0.f;
                }
            }
        }


        if(transition){
            transition_time += dt;
            for(int i = 0;i < black_filter.getVertexCount();i++){
                sf::Vertex & vertex = black_filter[i];
                vertex.color = sf::Color(0,0,0,(int)(std::min(transition_time , 1.f)*255));
            }
            if(transition_time > 1.f){
                map.load(door.destination);
                player.respawn(map.getObject("player_position").rect.getPosition());
                projectiles.clear();
                auto dObj = map.getObject("door");

                door.door.setPosition(dObj.rect.getPosition());
                door.destination = "../assets/levels/"+dObj.properties[0]["value"].get<std::string>();
                door.destination += ".json";
                door.visible = dObj.properties[1]["value"].get<bool>();
                transition = false;
                projSpawning = true;
            }
        } else if (transition_time > 0.f){
            transition_time -= dt;
            for(int i = 0;i < black_filter.getVertexCount();i++){
                sf::Vertex & vertex = black_filter[i];
                vertex.color = sf::Color(0,0,0,(int)(std::max(transition_time , 0.f)*255));
            }
        } else {
            transition_time = 0.f;
        }

        player.update(dt);
        if(!transition)
            player.collisions(map.getColliders() , camera);
        player.postUpdate(dt);

        if(level == 1 && player.isAlive() && !projSpawning && map.getObject("ps1").rect.getPosition().x < player.getPosition().x+player.getSize().x){
            for(int i = 0;i < 22;i++){
                sf::FloatRect camRect = {camera.getCenter()-camera.getSize()*0.5f,camera.getSize()};
                auto entData = instanciateProjectile(proj);
                auto pp = sf::Vector2f{camRect.left+camRect.width,(((float)(map.getSize().y*map.getTileSize().y) / 20.f)*i)};
                entData.projectile.setPosition(pp);
                entData.movement = sf::Vector2f(-1 , 0)*50.f;
                projectiles.push_back(entData);
                for(int i = 0;i < 6 ;i++){
                    float sp = Random::randFloat(urdf(200,250));
                    float sc = Random::randFloat(urdf(8.f,10.5f));
                    float ang = Random::randFloat(urdf(-20,20));
                    Spark s {{pp.x,pp.y},(float)(M_PI/180.f)*(ang+180),sp,sc,sp/100.f};
                    s.setColor(sf::Color::Black);
                    sparks.push_back(s);
                }
                slowTime = 2.f;
            }
            projSpawning = true;
            player.changeState();
        }

        for(int i = projectiles.size() -1 ; i >= 0 ; i--){
            projectiles[i].projectile.move(projectiles[i].movement*dt);
            if(projSpawning && !transition){
                if(player.isAlive() && player.getRect().intersects(projectiles[i].projectile.getRect())){
                    player.die();
                    projectiles.clear();
                    game_timer = 0.f;
                    if(level == 1){
                        projSpawning = false;
                    } else {
                        door.visible = false;
                    }
                    zoomTime = 0.f;
                    break;
                }
                
                if(!projectiles[i].projectile.getRect().intersects(mapRect)){
                    projectiles.erase(projectiles.begin()+i);
                }
            }
        }

        for(int i = sparks.size()-1;i >= 0;i--){
            
            sparks[i].speedScale -= 3.5f * dt;
            sparks[i].speed -= 17.5f * dt;
            sparks[i].update(dt);
            if(sparks[i].speedScale <= 0){
                sparks.erase(sparks.begin()+i);
            }
        }

        if(!player.isAlive()){
            death_time -= dt;
            if(death_time <= 0.f){
                death_time = 2.f;
                player.respawn(map.getObject("player_position").rect.getPosition());
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

        if(projSpawning && !transition && player.isAlive()){
            proj_spawn -= dt;
            if(proj_spawn <= 0.f){
                auto p = instanciateProjectile(proj);
                int angle = Random::randInt(urdi(60,120));
                sf::FloatRect camRect = {camera.getCenter()-camera.getSize()*0.5f,camera.getSize()};
                int randXpos = Random::randInt(urdi(0,(int)camRect.width));
                p.movement = sf::Vector2f{std::cos((float)(M_PI/180)*angle),std::sin((float)(M_PI/180)*angle)}*50.f;
                p.projectile.setPosition({camRect.left+randXpos,camRect.top});
                projectiles.push_back(p);
                if(level == 1){
                    proj_spawn = 0.5f;
                }
                else if (level == 2) {
                    proj_spawn = 0.3f;
                }
                else if (level == 3){
                    proj_spawn = 0.35f;
                }
                for(int i = 0;i < 6 ;i++){
                    float sp = Random::randFloat(urdf(150,200));
                    float sc = Random::randFloat(urdf(6.f,9.5f));
                    float ang = Random::randFloat(urdf(-30,30));
                    Spark s {{camRect.left+randXpos,camRect.top},(float)(M_PI/180.f)*(ang+angle),sp,sc,sp/100.f};
                    s.setColor(sf::Color::Black);
                    sparks.push_back(s);
                }
            }
        }

        if(player.isAlive() && !transition && door.visible && !player.isSoul() && player.getRect().intersects(door.door.getRect())){
            transition = true;
            level += 1;
            game_timer = 0.f;
        }


        window.clear({20, 19, 39});
        map.display(window , camera , 0);
        if(door.visible)
            door.door.display(window , camera);
        if(!player.isSoul())
            player.display(window , camera , 0);
        map.display(window , camera , 1);
        player.display(window , camera , 1);
        if(player.isSoul())
            player.display(window , camera , 0);
        pSys.display(window , camera);
        for(auto & p : projectiles){
            p.projectile.display(window , camera);
        }
        for(auto & spark : sparks){
            spark.draw(window , camera);
        }
        window.draw(black_filter);
        window.display();
    }

    return 0;
}