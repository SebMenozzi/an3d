#pragma once

#include <cmath>
#include "main/scene_base/base.hpp"

#ifdef SCENE_CLOTH

struct user_parameters_structure
{
    // Global mass (to be divided by the number of particles)
    float m;
    // Global stiffness (to be divided by the number of particles)
    float K;
    // Damping
    float mu;
    // Wind magnitude;
    float wind;
};

struct simulation_parameters_structure
{
    // Mass
    float m;
    // Spring rest length
    float L0;
};

// Sphere and ground used for collision
struct collision_shapes_structure
{
    // Position of the colliding sphere
    vcl::vec3 sphere_p;
    // Radius of the colliding sphere
    float sphere_r;
    // Height of the ground (in y-coordinate)
    float ground_height;
};

struct scene_model : scene_base
{
    // Particles parameters
    vcl::buffer2D<vcl::vec3> position;
    vcl::buffer2D<vcl::vec3> speed;
    vcl::buffer2D<vcl::vec3> force;

    // Simulation parameters

    // Parameters that user can control directly
    simulation_parameters_structure simulation_parameters;
    // Parameters adjusted with respect to mesh size (not controled directly by the user)
    user_parameters_structure user_parameters;

    // Cloth mesh elements

     // Visual model for the cloth
    vcl::mesh_drawable cloth;
    // Normal of the cloth used for rendering and wind force computation
    vcl::buffer<vcl::vec3> normals;
     // Connectivity of the triangular model
    vcl::buffer<vcl::uint3> connectivity;

    // Parameters of the shape used for collision
    collision_shapes_structure collision_shapes;

    // Store index and position of vertices constrained to have a fixed 3D position
    std::map<int,vcl::vec3> positional_constraints;

    // Textures
    GLuint texture_cloth;
    GLuint texture_wood;

    // Visual elements of the scene
    vcl::mesh_drawable sphere;
    vcl::mesh_drawable ground;

    // Gui parameters
    bool gui_display_wireframe;
    bool gui_display_texture;

    // Parameters used to control if the simulation runs when a numerical divergence is detected

    // Active when divergence is detected
    bool simulation_diverged;
    // Force to run simulation even if divergence is detected
    bool force_simulation;
    GLuint shader_mesh;

    vcl::timer_event timer;

    void initialize();
    void collision_constraints();
    void compute_forces();
    void numerical_integration(float h);
    void detect_simulation_divergence();
    void hard_constraints();
    void set_gui();

    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void display_elements(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
};

#endif
