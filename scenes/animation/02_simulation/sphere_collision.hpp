#pragma once

#include "main/scene_base/base.hpp"

#ifdef SCENE_SPHERE_COLLISION

// Structure of a particle
struct particle_structure
{
    // Position
    vcl::vec3 p;
    // Speed
    vcl::vec3 v;
    // Forces
    vcl::vec3 f;

    // Color
    vcl::vec3 c;
    // Radius
    float r;
};

struct gui_scene_structure
{
    bool add_sphere = true;
    float time_interval_new_sphere = 0.5f;
};

struct scene_model : scene_base
{
    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);

    void set_gui();

    void compute_time_step(float dt);
    void create_new_particle();
    void display_particles(scene_structure& scene);


    std::vector<particle_structure> particles;

     // Visual display of particles
    vcl::mesh_drawable sphere;
    // Visual display of borders
    vcl::segments_drawable borders;

    vcl::timer_event timer;
    gui_scene_structure gui_scene;
 
   std::vector<vcl::vec3> plane_points; 
   std::vector<vcl::vec3> plane_normals;

   vcl::vec3 sphere_p = {0.f, 0.f, 0.f};
   float sphere_r = 1.f;
};

#endif
