
#include <window/glfw_window.hpp>
#include <renderer/renderer.hpp>
#include <misc/images_loader.hpp>

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

    window.register_mouse_click_handler([](::renderer::mouse_click_event e) {
        if (e.action == ::renderer::mouse_click_event::action_type::press &&
            e.button == ::renderer::mouse_click_event::button_type::left) {
            mouse_clicked = true;
        }


        if (e.action == ::renderer::mouse_click_event::action_type::release &&
            e.button == ::renderer::mouse_click_event::button_type::left) {
            mouse_clicked = false;
        }
    });

    auto* r = window.get_renderer();
    rubiks_cube::rubiks_cube cube(r);

    window.register_mouse_position_handler([&cube](::renderer::mouse_position_event e) {
        static float last_x;
        static float last_y;

        if (mouse_clicked) {
            auto x_offset = e.x - last_x;
            auto y_offset = e.y - last_y;
            cube.rotation.x += y_offset * 0.01;
            cube.rotation.y += x_offset * 0.01;
        }

        last_x = e.x;
        last_y = e.y;
    });

    window.register_resize_handler([](::renderer::resize_event e) {
        camera.width = e.width;
        camera.height = e.height;
    });

    window.register_mouse_scroll_callback([](::renderer::scroll_event e) {
        camera.position.z += -e.y_offset;
        camera.target_position.z += -e.y_offset;
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


    camera.position = {-1, 5, 15};
    camera.target_position = {0, 0, 0};

    float v = 0;
    while (!window.closed()) {
        camera.update();

        cube.rotation.z = v;
        cube.parent_transform = camera.get_transformation();

        r->encode_draw_command({.type = renderer::draw_command_type::pass, .pass = pass});
        cube.update();
        cube.draw();
        window.update();

        if (mouse_clicked) {
//            uint8_t pixel[4];
//            auto pos_y = window.get_size().height - y;
//            window.get_pixel_color(pixel, x, pos_y, ::renderer::texture_format::rgba, ::renderer::data_type::u8);
//            std::cout << "R: " << float(pixel[0]) / 255.f << " G: " << float(pixel[1]) / 255.f << " B: " << float(pixel[2]) / 255.f << " A: " << float(pixel[3]) / 255.f << std::endl;
        }
    }

    return 0;
}
