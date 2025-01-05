#include "render.h"

/**
 * @brief
 * name:
 * - Boxes  : has a point light and several boxes;
 * - Bunny  : has a bunny in front of a wall with a point light;
 * - CornellBox
 * @param shader
 */
void Render::loadDemoScene(std::string name, ShaderType shader)
{
    if(this->is_init_){
        std::cerr<<"Currently don't support load demo scene after the pipeline's initialization!\n";
        return;
    }
#ifdef TIME_RECORD
    timer_.start("loadDemoScene");
#endif
    scene_.clearScene();
    if(name=="SingleBox"){
        {
            glm::vec3 model_position{0, 0, -200};
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), model_position);
            glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(50));
            glm::mat4 model_matrix = translation * rotate * scale;
            addObjInstance(std::string("assets/model/cube/cube.obj"),model_matrix ,ShaderType::Light,true);
        }
        {
            glm::vec3 model_position{0, 0, -400};
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), model_position);
            glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(50));
            glm::mat4 model_matrix = translation * rotate * scale;
            addObjInstance(std::string("assets/model/cube/cube.obj"),model_matrix ,shader,true);
        }
    }
    else if (name == "Boxes")
    {
        {
            glm::vec3 model_position{0, 0, -400};
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), model_position);
            glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(50));
            glm::mat4 model_matrix = translation * rotate * scale;
            addObjInstance(std::string("assets/model/cube/cube.obj"),model_matrix ,shader,true);
        }
        {
            glm::vec3 model_position{100, 100, -300};
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), model_position);
            glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(30));
            glm::mat4 model_matrix = translation * rotate * scale;
            addObjInstance(std::string("assets/model/cube/cube.obj"), model_matrix,shader,true);
        }
        {
            glm::vec3 model_position{-100, -100, -300};
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), model_position);
            glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(30));
            glm::mat4 model_matrix = translation * rotate * scale;
            addObjInstance(std::string("assets/model/cube/cube.obj"), model_matrix,shader,true);
        }
        {
            glm::vec3 lightpos{0, 0, 0};
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), lightpos);
            glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(10));
            glm::mat4 model_matrix = translation * rotate * scale;
            addObjInstance(std::string("assets/model/cube/cube.obj"),model_matrix,ShaderType::Light, true);

            PointLight pt;
            pt.pos_ = lightpos;
            pt.ambient_ = glm::vec3(0.5, 0.5, 0);
            pt.diffuse_ = glm::vec3(1, 1, 1);
            pt.specular_ = glm::vec3(0.5, 0.5, 0.5);
            pt.quadratic_ = 0.00001f;

            scene_.addLight(std::make_shared<PointLight>(pt));
        }
    }
    else if (name == "Bunny")
    {
        {
            glm::vec3 model_position{0, -100, -400};
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), model_position);
            glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(20));
            glm::mat4 model_matrix = translation * rotate * scale;
            addObjInstance(std::string("assets/model/Bunny.obj"), model_matrix,shader,false);
        }
        {
            glm::vec3 model_position{0, -100, -400};
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), model_position);
            glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(60.f), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));// 60
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(120));
            glm::mat4 model_matrix = translation * rotate * scale;
            addObjInstance(std::string("assets/model/Brickwall/brickwall.obj"), model_matrix,shader,false, false);
        }
        {
            glm::vec3 lightpos{100, 100, -200};
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), lightpos);
            glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(2));
            glm::mat4 model_matrix = translation * rotate * scale;
            addObjInstance(std::string("assets/model/cube/cube.obj"),model_matrix ,ShaderType::Light,true);

            PointLight pt;
            pt.pos_ = lightpos;
            pt.ambient_ = glm::vec3(0.1, 0.1, 0.1);
            pt.diffuse_ = glm::vec3(1, 1, 1);
            pt.specular_ = glm::vec3(0.4, 0.4, 0.4);
            pt.quadratic_ = 0.000001f;

            scene_.addLight(std::make_shared<PointLight>(pt));
        }
    }
    else if (name == "CornellBox")
    {
        {
            glm::vec3 model_position{200, -100, -200};
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), model_position);
            glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(180.f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2));
            glm::mat4 model_matrix = translation * rotate * scale;
            addObjInstance(std::string("assets/model/cornell_box/cornell_box.obj"), model_matrix,shader,false);
        }
        {
            glm::vec3 lightpos{0, 0, 0};
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), lightpos);
            glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(10));
            glm::mat4 model_matrix = translation * rotate * scale;
            addObjInstance(std::string("assets/model/cube/cube.obj"),model_matrix ,ShaderType::Light,true);

            PointLight pt;
            pt.pos_ = lightpos;
            pt.ambient_ = glm::vec3(0.1, 0.1, 0.1);
            pt.diffuse_ = glm::vec3(1, 1, 1);
            pt.specular_ = glm::vec3(0.4, 0.4, 0.4);
            pt.quadratic_ = 0.000001f;

            scene_.addLight(std::make_shared<PointLight>(pt));
        }
        {
            glm::vec3 model_position{-100, -100, -400};
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), model_position);
            glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(12));
            glm::mat4 model_matrix = translation * rotate * scale;
            addObjInstance(std::string("assets/model/ruins_building/SmallSetupRuins.obj"),model_matrix,shader,false);
        }
        {
            glm::vec3 model_position{-100, -100, -300};
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), model_position);
            glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(20));
            glm::mat4 model_matrix = translation * rotate * scale;
            addObjInstance(std::string("assets/model/Bunny.obj"), model_matrix,shader,false);
        }
    }

    else
    {
        std::cerr << "unknown demo~\n";
        exit(-1);
    }

#ifdef TIME_RECORD
    timer_.stop("loadDemoScene");
    timer_.reportElapsedTime("loadDemoScene");
    timer_.del("loadDemoScene");
#endif
}