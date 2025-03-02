/*
 * Copyright (c) 2021, Mim Hufford <mim@hotmail.co.uk>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "Game.h"
#include <LibConfig/Client.h>
#include <LibCore/System.h>
#include <LibGUI/Application.h>
#include <LibGUI/Icon.h>
#include <LibGUI/Menu.h>
#include <LibGUI/Menubar.h>
#include <LibGUI/MessageBox.h>
#include <LibGUI/Window.h>
#include <LibMain/Main.h>

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    TRY(Core::System::pledge("stdio rpath recvfd sendfd unix", nullptr));

    auto app = TRY(GUI::Application::try_create(arguments));

    Config::pledge_domains("FlappyBug");

    TRY(Core::System::pledge("stdio rpath recvfd sendfd", nullptr));

    TRY(Core::System::unveil("/res", "r"));
    TRY(Core::System::unveil(nullptr, nullptr));

    u32 high_score = Config::read_i32("FlappyBug", "Game", "HighScore", 0);

    auto window = TRY(GUI::Window::try_create());
    window->resize(FlappyBug::Game::game_width, FlappyBug::Game::game_height);
    auto app_icon = GUI::Icon::default_icon("app-flappybug");
    window->set_icon(app_icon.bitmap_for_size(16));
    window->set_title("Flappy Bug");
    window->set_double_buffering_enabled(false);
    window->set_resizable(false);
    auto widget = TRY(window->try_set_main_widget<FlappyBug::Game>());

    widget->on_game_end = [&](u32 score) {
        if (score <= high_score)
            return high_score;

        Config::write_i32("FlappyBug", "Game", "HighScore", score);
        high_score = score;

        return high_score;
    };

    auto game_menu = TRY(window->try_add_menu("&Game"));
    TRY(game_menu->try_add_action(GUI::CommonActions::make_quit_action([](auto&) {
        GUI::Application::the()->quit();
    })));

    auto help_menu = TRY(window->try_add_menu("&Help"));
    TRY(help_menu->try_add_action(GUI::CommonActions::make_about_action("Flappy Bug", app_icon, window)));

    window->show();

    return app->exec();
}
