#include "sphere_collision.hpp"

#include <random>

#ifdef SCENE_SPHERE_COLLISION

using namespace vcl;

enum intersection_type { BOX = 0, SPHERE = 1};

intersection_type current_inter = BOX;
vec3 camera_down = { 0.f, -1.f, 0.f };

void scene_model::frame_draw(std::map<std::string,GLuint>& shaders , scene_structure& scene, gui_structure&)
{
    float dt = 0.02f * timer.scale;
    timer.update();

    set_gui();

    mat4 cam = scene.camera.view_matrix();
    // Extract up vector;
    vec3 up = {cam[4], cam[5], cam[6]};
    camera_down = normalize(-up);

    create_new_particle();
    compute_time_step(dt);

    display_particles(scene);
    if (current_inter == intersection_type::BOX)
        draw(borders, scene.camera);
    else
    {
        sphere.uniform.transform.translation = sphere_p;
        sphere.uniform.transform.scaling = sphere_r;
        sphere.uniform.color = {0.f, 1.f, 1.f};
        sphere.shader = shaders["wireframe"];
        draw(sphere, scene.camera);
        sphere.shader = shaders["mesh"];
    }
}

void scene_model::compute_time_step(float dt)
{
    // Set forces
    const size_t N = particles.size();
    for(size_t k = 0; k < N; ++k)
        particles[k].f = 9.81f * camera_down * 2.f;

    // Integrate position and speed of particles through time
    for (size_t k = 0; k < N; ++k)
    {
        particle_structure& particle = particles[k];
        vec3& v = particle.v;
        vec3& p = particle.p;
        vec3 const& f = particle.f;

        v = (1 - 0.91f * dt) * v + dt * f; // gravity + friction force
        p = p + dt * v;
    }

    // Collisions with cube
    float alpha = 0.5;
    float beta = 0.5;

    for (size_t i = 0; i < N; ++i)
    {
        particle_structure& p1 = particles[i];

        for (size_t j = 0; j < N; ++j)
        {
            if (j == i)
                continue;

            particle_structure& p2 = particles[j];

            float detection = norm(p1.p - p2.p);
            
            if (detection <= p1.r + p2.r)
            {
                float epsilon = 0.0001;
                vec3 u = (p1.p - p2.p) / norm(p1.p - p2.p);

                if (abs(norm(p1.v - p2.v)) > epsilon)
                {
                    float m1 = 1;
                    float m2 = 1;

                    float j = 2 * (m1 * m2) / (m1 + m2) * dot(p2.v - p1.v, u);
                    
                    p1.v = alpha * p1.v + beta * j/m1;
                    p2.v = alpha * p2.v - beta * j/m2; 
                }
                else
                {
                    float mu = 0.5;
                    p1.v = mu * p2.v;
                    p2.v = mu * p1.v;
                }

                float d = p1.r + p2.r - norm(p1.p - p2.p);
                p1.p = p1.p + d/2*u;
            }
        }
    }
    
    alpha = 0.7;
    beta = 0.7;

    // Collisions between spheres
    for (size_t i = 0; i < N; ++i)
    {
        if (current_inter == BOX)
        {
            for (size_t j = 0; j < plane_points.size(); j++)
            {
                vec3 a = plane_points[j];
                vec3 n = plane_normals[j];
                particle_structure& p = particles[i];

                float detection = dot(p.p - a, n);

                if (detection <= p.r)
                {
                    vec3 v_ortho = dot(p.v, n) * n;
                    vec3 v_parallel = p.v - dot(p.v, n) * n;
                    p.v = alpha * v_parallel - beta * v_ortho;

                    float d = p.r - dot(p.p - a, n);
                    p.p = p.p + d*n; 
                }
            }
        }
        else
        {
            particle_structure& p = particles[i];
            float detection = norm(p.p - sphere_p);
            
            if (detection >= sphere_r - p.r)
            {
                vec3 n = normalize(sphere_p - p.p);
                vec3 a = sphere_p - normalize(n) * sphere_r;
                vec3 v_ortho = dot(p.v, n) * n;
                vec3 v_parallel = p.v - dot(p.v, n) * n;
                p.v = alpha * v_parallel - beta * v_ortho;

                float d = p.r - dot(p.p - a, n);
                p.p = p.p + d*n; 
            }
        }
    }
}

void scene_model::create_new_particle()
{
    // Emission of new particle if needed
    timer.periodic_event_time_step = gui_scene.time_interval_new_sphere;
    const bool is_new_particle = timer.event;
    static const std::vector<vec3> color_lut = {{1,0,0},{0,1,0},{0,0,1},{1,1,0},{1,0,1},{0,1,1}};

    if( is_new_particle && gui_scene.add_sphere)
    {
        particle_structure new_particle;

        new_particle.r = 0.08f;
        new_particle.c = color_lut[int(rand_interval()*color_lut.size())];

        // Initial position
        new_particle.p = vec3(0,0,0);

        // Initial speed
        const float theta = rand_interval(0, 2*3.14f);
        new_particle.v = vec3( 2*std::cos(theta), 5.0f, 2*std::sin(theta));

        particles.push_back(new_particle);
    }
}

void scene_model::display_particles(scene_structure& scene)
{
    const size_t N = particles.size();
    for(size_t k=0; k<N; ++k)
    {
        const particle_structure& part = particles[k];

        sphere.uniform.transform.translation = part.p;
        sphere.uniform.transform.scaling = part.r;
        sphere.uniform.color = part.c;
        draw(sphere, scene.camera);
    }
}

void scene_model::setup_data(std::map<std::string,GLuint>& shaders, scene_structure& , gui_structure& )
{
    sphere = mesh_drawable( mesh_primitive_sphere(1.0f));
    sphere.shader = shaders["mesh"];

    std::vector<vec3> borders_segments = {{-1,-1,-1},{1,-1,-1}, {1,-1,-1},{1,1,-1}, {1,1,-1},{-1,1,-1}, {-1,1,-1},{-1,-1,-1},
                                          {-1,-1,1} ,{1,-1,1},  {1,-1,1}, {1,1,1},  {1,1,1}, {-1,1,1},  {-1,1,1}, {-1,-1,1},
                                          {-1,-1,-1},{-1,-1,1}, {1,-1,-1},{1,-1,1}, {1,1,-1},{1,1,1},   {-1,1,-1},{-1,1,1}};
    borders = segments_gpu(borders_segments);
    borders.uniform.color = {0,0,0};
    borders.shader = shaders["curve"];

   plane_points = {{0,-1,0}, {1,0,0}, {-1,0,0}, {0,0,-1}, {0,0,1}, {0,1,0}}; 
   plane_normals = {{0,1,0}, {-1,0,0}, {1,0,0}, {0,0,1}, {0,0,-1}, {0, -1, 0}};
}

void scene_model::set_gui()
{
    // Can set the speed of the animation
    ImGui::SliderFloat("Time scale", &timer.scale, 0.05f, 2.0f, "%.2f s");
    ImGui::SliderFloat("Interval create sphere", &gui_scene.time_interval_new_sphere, 0.05f, 2.0f, "%.2f s");
    ImGui::Checkbox("Add sphere", &gui_scene.add_sphere);

    bool stop_anim  = ImGui::Button("Stop"); ImGui::SameLine();
    bool start_anim = ImGui::Button("Start");

    if(stop_anim)  timer.stop();
    if(start_anim) timer.start();
}

#endif
