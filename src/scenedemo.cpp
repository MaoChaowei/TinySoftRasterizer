#include"render.h"


/**
 * @brief 
 * name:
 * - Boxes  : has a point light and two boxes
 * - Bunny  : has a bunny in front of a wall with a point light
 * @param shader 
 */
void Render::loadDemoScene(std::string name,ShaderType shader){
#ifdef TIME_RECORD
    timer_.start("loadDemoScene");
#endif
    if(name=="Boxes"){
        {
            addScene(std::string("assets/model/cube/cube.obj"),true);
            auto objs=scene_.getObjects();
            glm::vec3 model_position{0,0,-400};
            glm::mat4 translation = glm::translate(glm::mat4(1.0f),model_position);
            glm::mat4 rotate=glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(50));
            glm::mat4 model_matrix=translation*rotate*scale;
            objs[0]->setModel2World(model_matrix);
            objs[0]->setShader(shader);
        }
        {
            addScene(std::string("assets/model/cube/cube.obj"),true);
            auto objs=scene_.getObjects();
            glm::vec3 model_position{100,100,-300};
            glm::mat4 translation = glm::translate(glm::mat4(1.0f),model_position);
            glm::mat4 rotate=glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(30));
            glm::mat4 model_matrix=translation*rotate*scale;
            objs[1]->setModel2World(model_matrix);
            objs[1]->setShader(shader);
        }
        {
            addScene(std::string("assets/model/cube/cube.obj"),true);
            auto objs=scene_.getObjects();
            glm::vec3 lightpos{0,0,0};
            glm::mat4 translation = glm::translate(glm::mat4(1.0f),lightpos);
            glm::mat4 rotate=glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(10));
            glm::mat4 model_matrix=translation*rotate*scale;
            objs[2]->setModel2World(model_matrix);
            objs[2]->setShader(ShaderType::Light);

            PointLight pt;
            pt.pos_=lightpos;
            pt.ambient_=glm::vec3(0.5,0.5,0);
            pt.diffuse_=glm::vec3(1,1,1);
            pt.specular_=glm::vec3(0.5,0.5,0.5);
            pt.quadratic_=0.00001f;

            scene_.addLight(std::make_shared<PointLight>(pt));
        }

    }
    else if(name=="Bunny"){
        {
            addScene(std::string("assets/model/Bunny.obj"),false);
            auto objs=scene_.getObjects();
            glm::vec3 model_position{0,-100,-400};
            glm::mat4 translation = glm::translate(glm::mat4(1.0f),model_position);
            glm::mat4 rotate=glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(20));
            glm::mat4 model_matrix=translation*rotate*scale;
            objs[0]->setModel2World(model_matrix);
            objs[0]->setShader(shader);
        }
        {
            addScene(std::string("assets/model/Brickwall/brickwall.obj"),false,false);
            auto objs=scene_.getObjects();
            glm::vec3 model_position{0,-100,-400};
            glm::mat4 translation = glm::translate(glm::mat4(1.0f),model_position);
            glm::mat4 rotate=glm::rotate(glm::mat4(1.0f), glm::radians(60.f), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(120));
            glm::mat4 model_matrix=translation*rotate*scale;
            objs[1]->setModel2World(model_matrix);
            objs[1]->setShader(shader);
        }
        {
            addScene(std::string("assets/model/cube/cube.obj"),true);
            auto objs=scene_.getObjects();
            glm::vec3 lightpos{0,0,0};
            glm::mat4 translation = glm::translate(glm::mat4(1.0f),lightpos);
            glm::mat4 rotate=glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(10));
            glm::mat4 model_matrix=translation*rotate*scale;
            objs[2]->setModel2World(model_matrix);
            objs[2]->setShader(ShaderType::Light);

            PointLight pt;
            pt.pos_=lightpos;
            pt.ambient_=glm::vec3(0.1,0.1,0.1);
            pt.diffuse_=glm::vec3(1,1,1);
            pt.specular_=glm::vec3(0.4,0.4,0.4);
            pt.quadratic_=0.000001f;

            scene_.addLight(std::make_shared<PointLight>(pt));
        }

    }
    else{
        std::cerr<<"unknown demo~\n";
        exit(-1);
    }

#ifdef TIME_RECORD
    timer_.stop("loadDemoScene");
    timer_.reportElapsedTime("loadDemoScene");
    timer_.del("loadDemoScene");
#endif
}