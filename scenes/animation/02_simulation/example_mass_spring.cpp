#include "example_mass_spring.hpp"

#ifdef SCENE_MASS_SPRING_1D

using namespace vcl;

static void set_gui(timer_basic& timer);

/** Compute spring force applied on particle pi from particle pj */
vec3 spring_force(const vec3& pi, const vec3& pj, float L0, float K)
{
    vec3 const pji = pj - pi;
    float const L = norm(pji);
    return K * (L - L0) * pji / L;
}

void scene_model::setup_data(std::map<std::string,GLuint>& , scene_structure& , gui_structure& )
{
    // Initial position and speed of particles
    // ******************************************* //

     // Initial position of particle A
    pA.p = {0,0,0};
    // Initial speed of particle A
    pA.v = {0,0,0};

     // Initial position of particle B
    pB.p = {0.5f,0,0};
    // Initial speed of particle B
    pB.v = {0,0,0};

    pC.p = {1.0f, 0, 0};
    pC.v = {0,0,0};

    // Rest length between A and B
    L0 = 0.4f;

    // Display elements
    // ******************************************* //
    segment_drawer.init();
    segment_drawer.uniform_parameter.color = {0,0,1};

    sphere = mesh_primitive_sphere();
    sphere.uniform.transform.scaling = 0.05f;


    std::vector<vec3> borders_segments = {{-1,-1,-1},{1,-1,-1}, {1,-1,-1},{1,1,-1}, {1,1,-1},{-1,1,-1}, {-1,1,-1},{-1,-1,-1},
                                          {-1,-1,1} ,{1,-1,1},  {1,-1,1}, {1,1,1},  {1,1,1}, {-1,1,1},  {-1,1,1}, {-1,-1,1},
                                          {-1,-1,-1},{-1,-1,1}, {1,-1,-1},{1,-1,1}, {1,1,-1},{1,1,1},   {-1,1,-1},{-1,1,1}};
    borders = borders_segments;
    borders.uniform.color = {0,0,0};

}

void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    timer.update();
    set_gui(timer);

    // Simulation time step (dt)
    float dt = timer.scale*0.01f;

    // Simulation parameters

    // Particle mass
    const float m  = 0.01f;
    // Spring stiffness
    const float K  = 5.0f;
    // Damping coefficient
    const float mu = 0.005f;
    
    // Gravity
    const vec3 g = { 0, -9.81f , 0 };

    // Forces
    const vec3 f_spring  = spring_force(pB.p, pA.p, L0, K);
    const vec3 f_weight =  m * g;
    const vec3 f_damping = pB.v * mu; // TO DO: correct this force value
    const vec3 F = f_spring+f_weight - f_damping;

    // Numerical Integration (Verlet)
    {
        // Only particle B should be updated

        // Position of particle
        vec3& p = pB.p;
        // Speed of particle
        vec3& v = pB.v;

        p = p + dt * v;
        v = v + dt * F / m;
    }

    // Display of the result

    // particle pa
    sphere.uniform.transform.translation = pA.p;
    sphere.uniform.color = {0,0,0};
    draw(sphere, scene.camera, shaders["mesh"]);

    // particle pb
    sphere.uniform.transform.translation = pB.p;
    sphere.uniform.color = {1,0,0};
    draw(sphere, scene.camera, shaders["mesh"]);

    // Spring pa-pb
    segment_drawer.uniform_parameter.p1 = pA.p;
    segment_drawer.uniform_parameter.p2 = pB.p;
    segment_drawer.draw(shaders["segment_im"],scene.camera);

    draw(borders, scene.camera, shaders["curve"]);
}

/** Part specific GUI drawing */
static void set_gui(timer_basic& timer)
{
    // Can set the speed of the animation
    float scale_min = 0.05f;
    float scale_max = 2.0f;
    ImGui::SliderScalar("Time scale", ImGuiDataType_Float, &timer.scale, &scale_min, &scale_max, "%.2f s");

    // Start and stop animation
    if (ImGui::Button("Stop"))
        timer.stop();
    if (ImGui::Button("Start"))
        timer.start();

}

#endif
