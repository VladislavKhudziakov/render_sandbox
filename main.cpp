
#include <window/glfw_window.hpp>
#include <renderer/renderer.hpp>

#include <camera.hpp>
#include <rubiks_cube.hpp>

renderer::camera camera{};
bool mouse_clicked = false;

int main()
{
    renderer::mesh_handler mesh;
    renderer::shader_handler shader;

    renderer::glfw_window window("my window", {800, 600});
    camera.width = 800;
    camera.height = 600;

    auto* r = window.get_renderer();
    rubiks_cube::rubiks_cube cube(r);

    window.register_mouse_click_handler([&cube](::renderer::mouse_click_event e) {
        if (e.action == ::renderer::mouse_click_event::action_type::press && e.button == ::renderer::mouse_click_event::button_type::left) {
            mouse_clicked = true;
        }


        if (e.action == ::renderer::mouse_click_event::action_type::release && e.button == ::renderer::mouse_click_event::button_type::left) {
            mouse_clicked = false;
            if (cube.rotation_manager.is_any_row_acquired()) {
                cube.rotation_manager.release_row();
            }
        }
    });


    window.register_mouse_position_handler([&cube, &window](::renderer::mouse_position_event e) {
        static float last_x;
        static float last_y;
        static bool x_axis;

        if (mouse_clicked) {
            auto x_offset = e.x - last_x;
            auto y_offset = e.y - last_y;

            auto ray = math::mouse_to_world_space_ray(
                {float(e.x), float(e.y)},
                {float(window.get_size().width), float(window.get_size().height)},
                camera.position,
                camera.target_position,
                {0, 1, 0},
                camera.fov,
                camera.near,
                camera.far);

            rubiks_cube::rubiks_cube::face face;
            math::vec3 hit_point;

            if (cube.rotation_manager.is_any_row_acquired()) {
                cube.rotation_manager.rotate((x_axis ? x_offset : y_offset) * 0.01); // in reverse order
            } else {
                if (!cube.hit({camera.position, ray * (camera.far - camera.near)}, face, hit_point)) {
                    cube.rotation.y += x_offset * 0.01;
                    cube.rotation.x += y_offset * 0.01;
                } else {
                    auto row_col = cube.get_row_col_by_hit_pos(face, hit_point);
                    auto axis = cube.get_face_rotation_axis(face);
                    x_axis = abs(x_offset) >= abs(y_offset);
                    auto row_index = x_axis ? row_col.y : row_col.x; // in reverse order
                    auto axis_index = x_axis ? axis.x : axis.y;

                    cube.rotation_manager.acquire_row(rubiks_cube::rotation_manager::axis(axis_index), row_index);
                    std::cout << "axis " << axis_index << std::endl;
                    std::cout << "row_col x: " << row_col.x << " y: " << row_col.y << std::endl;
                }
            }
        }

        last_x = e.x;
        last_y = e.y;
    });

    window.register_resize_handler([](::renderer::resize_event e) {
        camera.width = e.width;
        camera.height = e.height;
    });

    window.register_mouse_scroll_callback([](::renderer::scroll_event e) {
        auto dir = math::normalize(camera.target_position - camera.position);
        camera.position = camera.position + dir * e.y_offset;
    });

    renderer::texture_descriptor color_attachment_tex_descriptor{
        .pixels_data_type = renderer::data_type::u8,
        .format = renderer::texture_format::rgba,
        .type = renderer::texture_type::attachment,
        .size = {
            1, 1, 0, 1}};

    renderer::texture_descriptor depth_attachment_tex_descriptor{
        .pixels_data_type = renderer::data_type::d24,
        .format = renderer::texture_format::rgba,
        .type = renderer::texture_type::attachment,
        .size = {
            1, 1, 0, 1}};

    auto color_attachment_tex = r->create_texture(color_attachment_tex_descriptor);
    auto depth_attachment_tex = r->create_texture(depth_attachment_tex_descriptor);

    renderer::pass_descriptor pass_descriptor{
        .width = 1600,
        .height = 1200,
        .attachments = {
            {.type = renderer::attachment_type::color, .render_texture = color_attachment_tex},
            {.type = renderer::attachment_type::depth, .render_texture = depth_attachment_tex},
        }};

    auto pass = r->create_pass(pass_descriptor);

    camera.position = {-10, 10, -10};
    camera.target_position = {0, 0, 0};

    while (!window.closed()) {
        camera.update();

        cube.parent_transform = camera.get_transformation();

        r->encode_draw_command({.type = renderer::draw_command_type::pass, .pass = pass});
        cube.update();

        cube.draw();
        window.update();
    }

    return 0;
}
