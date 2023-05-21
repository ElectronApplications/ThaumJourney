#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>

#include "scene.h"
#include "gui.h"
#include "io.h"
#include "entity.h"
#include "effect.h"
#include "loader.h"
#include "game.h"
#include "item.h"

// Цвета
#define TMAGIC {51, 25, 56}
#define TWRED {247, 15, 15}
#define BGREEN {62, 172, 60, 255}
#define BHGREEN {78, 203, 75, 255}
#define BRED {222, 84, 97, 255}
#define BHRED {220, 103, 114, 255}
#define BHORANGE {223, 155, 48, 255}
#define BGBLUE {0, 120, 212, 255}

using namespace std;

SceneEnum current_scene = SceneEnum::MainScene;
vector<GuiElement*> gui_elements;

int selected = -1;
int escape_element = -1;

int players_page = 0;
int items_page = 0;

void update_scene(bool new_scene) {
    if(new_scene) {
        selected = -1;
        escape_element = -1;
    }

    switch(current_scene) {
        case SceneEnum::MainScene:
            main_scene(new_scene);
            break;
        case SceneEnum::NewGameScene:
            new_game_scene(new_scene);
            break;
        case SceneEnum::LoadGameScene:
            load_game_scene(new_scene);
            break;
        case SceneEnum::AboutScene:
            about_scene(new_scene);
            break;
        case SceneEnum::ReferenceScene:
            reference_scene(new_scene);
            break;
        case SceneEnum::GameScene:
            game_scene(new_scene);
            break;
        case SceneEnum::PauseScene:
            pause_scene(new_scene);
            break;
        case SceneEnum::LoginScene:
            login_scene(new_scene);
            break;
        case SceneEnum::InventoryScene:
            inventory_scene(new_scene);
            break;
        case SceneEnum::UpgradeScene:
            upgrade_scene(new_scene);
            break;
        case SceneEnum::ExitSaveScene:
            exit_save_scene(new_scene);
            break;
        case SceneEnum::DeathScene:
            death_scene(new_scene);
            break;
        case SceneEnum::ConsoleScene:
            console_scene(new_scene);
            break;
    }
}

void main_scene(bool new_scene) {
    clear_gui();
    
    gui_elements.push_back(new Label(window_width/2 - 300, 20, 600, 80, "Thaumaturge's Journey", 0, TMAGIC, TMAGIC, {0, 0, 0}, {0, 0, 0}));
    gui_elements.push_back(new Button(window_width/2 - 150, 120, 300, 100, "Новая игра", BGREEN, BHGREEN));
    gui_elements.push_back(new Button(window_width/2 - 150, 240, 300, 100, "Загрузить игру", BGREEN, BHGREEN));
    gui_elements.push_back(new Button(window_width/2 - 150, 360, 300, 100, "О программе", BGREEN, BHGREEN));
    gui_elements.push_back(new Button(window_width/2 - 150, 480, 300, 100, "Справка", BGREEN, BHGREEN));

    gui_elements[1]->on_click = [](GuiElement *element) {
        current_scene = SceneEnum::NewGameScene;
        update_scene(true);
    };
    gui_elements[2]->on_click = [](GuiElement *element) {
        current_scene = SceneEnum::LoadGameScene;
        update_scene(true);
    };
    gui_elements[3]->on_click = [](GuiElement *element) {
        current_scene = SceneEnum::AboutScene;
        update_scene(true);
    };
    gui_elements[4]->on_click = [](GuiElement *element) {
        current_scene = SceneEnum::ReferenceScene;
        update_scene(true);
    };
}

void new_game_scene(bool new_scene) {
    string name = "";
    string password = "";
    bool hidden = true;
    if(new_scene) {
        player = Player();
    }
    else {
        name = dynamic_cast<TextBox*>(gui_elements[1])->text;
        password = dynamic_cast<TextBox*>(gui_elements[3])->text;
        hidden = dynamic_cast<TextBox*>(gui_elements[3])->hidden_text;
    }
    clear_gui();

    gui_elements.push_back(new Label(window_width/2 - 300, 20, 130, 100, "Имя", 0, TMAGIC, TMAGIC, {0, 0, 0}, {0, 0, 0}));
    gui_elements.push_back(new TextBox(window_width/2 - 150, 20, 400, 100, name, false));

    gui_elements.push_back(new Label(window_width/2 - 300, 140, 130, 100, "Пароль", 0, TMAGIC, TMAGIC, {0, 0, 0}, {0, 0, 0}));
    gui_elements.push_back(new TextBox(window_width/2 - 150, 140, 400, 100, password, true));

    gui_elements.push_back(new Button(window_width/2 + 270, 140, 100, 100, "*", BGREEN, BHGREEN));
    gui_elements.back()->on_click = [](GuiElement *element) {
        dynamic_cast<TextBox*>(gui_elements[3])->hidden_text = !dynamic_cast<TextBox*>(gui_elements[3])->hidden_text;
        dynamic_cast<TextBox*>(gui_elements[3])->update();
    };

    gui_elements.push_back(new Label(window_width/2 - 150, 260, 400, 60, "", 0, TWRED, TWRED, {0, 0, 0}, {0, 0, 0}));

    gui_elements.push_back(new Button(window_width/2 - 370, window_height - 210, 200, 50, "Легкая сложность", BHRED, BHRED));
    gui_elements.back()->on_click = [](GuiElement *element) {
        player.difficulty = 1;
        dynamic_cast<Button*>(gui_elements[6])->background_color = BHRED;
        dynamic_cast<Button*>(gui_elements[7])->background_color = BGREEN;
        dynamic_cast<Button*>(gui_elements[8])->background_color = BGREEN;
        gui_elements[6]->update();
        gui_elements[7]->update();
        gui_elements[8]->update();
    };

    gui_elements.push_back(new Button(window_width/2 - 370, window_height - 140, 200, 50, "Средняя сложность", BGREEN, BHRED));
    gui_elements.back()->on_click = [](GuiElement *element) {
        player.difficulty = 2;
        dynamic_cast<Button*>(gui_elements[6])->background_color = BGREEN;
        dynamic_cast<Button*>(gui_elements[7])->background_color = BHRED;
        dynamic_cast<Button*>(gui_elements[8])->background_color = BGREEN;
        gui_elements[6]->update();
        gui_elements[7]->update();
        gui_elements[8]->update();
    };

    gui_elements.push_back(new Button(window_width/2 - 370, window_height - 70, 200, 50, "Сложная сложность", BGREEN, BHRED));
    gui_elements.back()->on_click = [](GuiElement *element) {
        player.difficulty = 3;
        dynamic_cast<Button*>(gui_elements[6])->background_color = BGREEN;
        dynamic_cast<Button*>(gui_elements[7])->background_color = BGREEN;
        dynamic_cast<Button*>(gui_elements[8])->background_color = BHRED;
        gui_elements[6]->update();
        gui_elements[7]->update();
        gui_elements[8]->update();
    };

    gui_elements.push_back(new Button(window_width/2 - 150, window_height - 240, 300, 100, "Начать игру", BGREEN, BHGREEN));
    gui_elements.back()->on_click = [](GuiElement *element) {
        string player_name = dynamic_cast<TextBox*>(gui_elements[1])->text;

        vector<Player> players = load_players();
        bool taken = false;

        for(auto &i : players) {
            if(i.player_name == player_name)
                taken = true;
        }

        if(player_name == "") {
            dynamic_cast<Label*>(gui_elements[5])->text = "Введите имя пользователя!";
            gui_elements[5]->update();
        }
        else if(taken) {
            dynamic_cast<Label*>(gui_elements[5])->text = "Имя пользователя занято!";
            gui_elements[5]->update();
        }
        else {
            start_new_game(player_name, dynamic_cast<TextBox*>(gui_elements[3])->text);
            block_size = 96;
            current_scene = SceneEnum::GameScene;
            update_scene(true);
        }
    };

    gui_elements.push_back(new Button(window_width/2 - 150, window_height - 120, 300, 100, "Назад", BRED, BHRED));
    gui_elements.back()->on_click = [](GuiElement *element) {
        current_scene = SceneEnum::MainScene;
        update_scene(true);
    };

    escape_element = gui_elements.size()-1;
}

void load_game_scene(bool new_scene) {
    players_page = 0;
    clear_gui();

    vector<Player> players = load_players();
    sort(players.begin(), players.end(), [](Player &left, Player &right){
        return left.xp > right.xp;
    });

    int height = 0;
    int i = 0;
    while(i < players.size() && height < window_height - 240) {
        gui_elements.push_back(new Button(window_width/2 - 150, 100*i + 20*(i+1), 300, 100, players[i].player_name + "\nXP: " + to_string(players[i].xp), BGREEN, BHGREEN));
        gui_elements.back()->token = i;
        gui_elements.back()->on_click = [](GuiElement *element) {
            vector<Player> players = load_players();
            sort(players.begin(), players.end(), [](Player &left, Player &right){
                return left.xp > right.xp;
            });
            player = players[dynamic_cast<Button*>(element)->token];
            current_scene = SceneEnum::LoginScene;
            update_scene(true);
        };
        i++;
        height += 120;
    }

    gui_elements.push_back(new Button(window_width/2 + 170, 20, 100, 100, "^", BGREEN, BHGREEN));
    gui_elements.back()->hidden = true;
    gui_elements.back()->on_click = [](GuiElement *element) {
        vector<Player> players = load_players();
        sort(players.begin(), players.end(), [](Player &left, Player &right){
            return left.xp > right.xp;
        });
        int total = gui_elements.size() - 4;

        players_page--;
        dynamic_cast<Label*>(gui_elements[gui_elements.size()-2])->text = to_string(players_page+1) + "/" + to_string(players.size() - total + 1);
        dynamic_cast<Label*>(gui_elements[gui_elements.size()-2])->update();
        for(int i = total-2; i >= 0; i--) {
            dynamic_cast<Button*>(gui_elements[i+1])->text = dynamic_cast<Button*>(gui_elements[i])->text;
            dynamic_cast<Button*>(gui_elements[i+1])->token = dynamic_cast<Button*>(gui_elements[i])->token;
            dynamic_cast<Button*>(gui_elements[i+1])->update();
        }
        dynamic_cast<Button*>(gui_elements[0])->text = players[players_page].player_name + "\nXP: " + to_string(players[players_page].xp);
        dynamic_cast<Button*>(gui_elements[0])->token = players_page;
        dynamic_cast<Button*>(gui_elements[0])->update();

        gui_elements[gui_elements.size()-3]->hidden = false;
        if(players_page == 0)
            element->hidden = true;
    };
    gui_elements.push_back(new Button(window_width/2 + 170, window_height - 240, 100, 100, "v", BGREEN, BHGREEN));
    if(players.size() == i)
        gui_elements.back()->hidden = true;
    gui_elements.back()->on_click = [](GuiElement *element) {
        vector<Player> players = load_players();
        sort(players.begin(), players.end(), [](Player &left, Player &right){
            return left.xp > right.xp;
        });
        int total = gui_elements.size() - 4;

        players_page++;
        dynamic_cast<Label*>(gui_elements[gui_elements.size()-2])->text = to_string(players_page+1) + "/" + to_string(players.size() - total + 1);
        dynamic_cast<Label*>(gui_elements[gui_elements.size()-2])->update();
        for(int i = 1; i < total; i++) {
            dynamic_cast<Button*>(gui_elements[i-1])->text = dynamic_cast<Button*>(gui_elements[i])->text;
            dynamic_cast<Button*>(gui_elements[i-1])->token = dynamic_cast<Button*>(gui_elements[i])->token;
            dynamic_cast<Button*>(gui_elements[i-1])->update();
        }
        dynamic_cast<Button*>(gui_elements[total-1])->text = players[players_page+total-1].player_name + "\nXP: " + to_string(players[players_page+total-1].xp);
        dynamic_cast<Button*>(gui_elements[total-1])->token = players_page+total-1;
        dynamic_cast<Button*>(gui_elements[total-1])->update();

        gui_elements[gui_elements.size()-4]->hidden = false;
        if(players_page == players.size() - total)
            element->hidden = true;
    };

    gui_elements.push_back(new Label(window_width/2 + 170, window_height/2 - 110, 100, 100, "1/" + to_string(players.size()-i+1), 0, TMAGIC, TMAGIC, {0, 0, 0}, {0, 0, 0}));
    if(players.size() == i)
        gui_elements.back()->hidden = true;

    gui_elements.push_back(new Button(window_width/2 - 150, window_height - 120, 300, 100, "Назад", BRED, BHRED));
    gui_elements.back()->on_click = [](GuiElement *element) {
        current_scene = SceneEnum::MainScene;
        update_scene(true);
    };
    escape_element = gui_elements.size()-1;
}

void login_scene(bool new_scene) {
    string password = "";
    bool hidden = true;
    if(!new_scene) {
        password = dynamic_cast<TextBox*>(gui_elements[2])->text;
        hidden = dynamic_cast<TextBox*>(gui_elements[2])->hidden_text;
    }
    clear_gui();

    gui_elements.push_back(new Label(window_width/2 - 150, 20, 300, 100, player.player_name, 0, TMAGIC, TMAGIC, {0, 0, 0}, {0, 0, 0}));

    gui_elements.push_back(new Label(window_width/2 - 300, 140, 130, 100, "Пароль", 0, TMAGIC, TMAGIC, {0, 0, 0}, {0, 0, 0}));
    gui_elements.push_back(new TextBox(window_width/2 - 150, 140, 400, 100, password, true));

    gui_elements.push_back(new Button(window_width/2 + 270, 140, 100, 100, "*", BGREEN, BHGREEN));
    gui_elements.back()->on_click = [](GuiElement *element) {
        dynamic_cast<TextBox*>(gui_elements[2])->hidden_text = !dynamic_cast<TextBox*>(gui_elements[2])->hidden_text;
        dynamic_cast<TextBox*>(gui_elements[2])->update();
    };

    gui_elements.push_back(new Label(window_width/2 - 150, 260, 400, 40, "", 0, TWRED, TWRED, {0, 0, 0}, {0, 0, 0}));

    gui_elements.push_back(new Button(window_width/2 - 150, window_height - 300, 300, 50, "Удалить аккаунт", BRED, BHRED));
    gui_elements.back()->on_click = [](GuiElement *element) {
        if(hash<string>{}(dynamic_cast<TextBox*>(gui_elements[2])->text) == player.password) {
            filesystem::remove_all("saves/" + player.player_name + "/");
            current_scene = SceneEnum::MainScene;
            update_scene(true);
        }
        else {
            dynamic_cast<Label*>(gui_elements[4])->text = "Неправильный пароль!";
            gui_elements[4]->update();
        }
    };

    gui_elements.push_back(new Button(window_width/2 - 150, window_height - 240, 300, 100, "Начать игру", BGREEN, BHGREEN));
    gui_elements.back()->on_click = [](GuiElement *element) {
        if(hash<string>{}(dynamic_cast<TextBox*>(gui_elements[2])->text) == player.password) {
            block_size = 96;
            load_game(player.player_name);
            current_scene = SceneEnum::GameScene;
            update_scene(true);
        }
        else {
            dynamic_cast<Label*>(gui_elements[4])->text = "Неправильный пароль!";
            gui_elements[4]->update();
        }
    };
    
    gui_elements.push_back(new Button(window_width/2 - 150, window_height - 120, 300, 100, "Назад", BRED, BHRED));
    gui_elements.back()->on_click = [](GuiElement *element) {
        current_scene = SceneEnum::MainScene;
        update_scene(true);
    };
    escape_element = gui_elements.size()-1;
}

void about_scene(bool new_scene) {
    clear_gui();
    
    gui_elements.push_back(new Label(20, 20, window_width - 40, window_height - 160, about_text, 0, {255, 255, 255}, {255, 255, 255}, {0, 0, 0, 64}, {0, 0, 0, 64}));

    gui_elements.push_back(new Button(window_width/2 - 150, window_height - 120, 300, 100, "Назад", BRED, BHRED));
    gui_elements.back()->on_click = [](GuiElement *element) {
        current_scene = SceneEnum::MainScene;
        update_scene(true);
    };
    escape_element = 1;
}

void reference_scene(bool new_scene) {
    clear_gui();
    
    gui_elements.push_back(new Label(20, 20, window_width - 40, window_height - 160, reference_text, 30, {255, 255, 255}, {255, 255, 255}, {0, 0, 0, 64}, {0, 0, 0, 64}));

    gui_elements.push_back(new Button(window_width/2 - 150, window_height - 120, 300, 100, "Назад", BRED, BHRED));
    gui_elements.back()->on_click = [](GuiElement *element) {
        current_scene = SceneEnum::MainScene;
        update_scene(true);
    };
    escape_element = 1;
}

void game_scene(bool new_scene) {
    string text = "";
    int token = 0;
    if(!new_scene) {
        text = dynamic_cast<Label*>(gui_elements[3])->text;
        token = gui_elements[3]->token;
    }
    clear_gui();

    gui_elements.push_back(new Button(20, 20, 50, 50, "||", {0, 0, 0}, {0, 0, 0}));
    gui_elements.back()->on_click = [](GuiElement *element) {
        current_scene = SceneEnum::PauseScene;
        update_scene(true);
    };
    escape_element = gui_elements.size()-1;

    gui_elements.push_back(new Button(90, 20, 200, 50, "Инвентарь (E)", {0, 0, 0}, {0, 0, 0}));
    gui_elements.back()->on_click = [](GuiElement *element) {
        current_scene = SceneEnum::InventoryScene;
        update_scene(true);
    };

    gui_elements.push_back(new Button(300, 20, 200, 50, "Прокачка (U)", {0, 0, 0}, {0, 0, 0}));
    gui_elements.back()->on_click = [](GuiElement *element) {
        current_scene = SceneEnum::UpgradeScene;
        update_scene(true);
    };

    gui_elements.push_back(new Label(window_width - 310, window_height/2 - 150, 300, 300, text, 25, {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, {0, 0, 0}));
    gui_elements.back()->token = token;

    gui_elements.push_back(new ProgressBar(window_width/2 - 310, window_height - 60, 300, 50, player.health, player.max_health, {24, 24, 24, 255}, {255, 0, 0, 255}));
    gui_elements.push_back(new ProgressBar(window_width/2 + 10, window_height - 60, 300, 50, player.mana, player.max_mana, {24, 24, 24, 255}, {0, 0, 255, 255}));
    gui_elements.push_back(new Label(window_width/2 - 100, window_height - 120, 200, 50, "XP: " + to_string(player.xp) + " / " + to_string(player.level*player.level), 0, TMAGIC, TMAGIC, {0, 0, 0}, {0, 0, 0}));
}

void pause_scene(bool new_scene) {
    clear_gui();

    gui_elements.push_back(new Solid(0, 0, window_width, window_height, {0, 0, 0, 96}));
    
    gui_elements.push_back(new Button(window_width/2 - 150, 20, 300, 100, "Назад", BGREEN, BHGREEN));
    gui_elements.back()->on_click = [](GuiElement *element) {
        current_scene = SceneEnum::GameScene;
        update_scene(true);
    };
    escape_element = gui_elements.size()-1;

    gui_elements.push_back(new Button(window_width/2 - 150, 140, 300, 100, "Сохранить игру", BGREEN, BHGREEN));
    gui_elements.back()->on_click = [](GuiElement *element) {
        save_game();
        current_scene = SceneEnum::GameScene;
        update_scene(true);
    };

    gui_elements.push_back(new Button(window_width/2 - 150, 260, 300, 100, "Выйти из игры", BRED, BHRED));
    gui_elements.back()->on_click = [](GuiElement *element) {
        quitting = false;
        current_scene = SceneEnum::ExitSaveScene;
        update_scene(true);
    };
}

void inventory_scene(bool new_scene) {
    items_page = 0;
    clear_gui();

    gui_elements.push_back(new Solid(0, 0, window_width/4, window_height, {24, 24, 24, 255}));
    gui_elements.push_back(new Solid(3*window_width/4, 0, window_width/4, window_height, {24, 24, 24, 255}));

    gui_elements.push_back(new Label(window_width/4 + 10, 10, window_width/2 - 10, 50, "", 0, {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, {0, 0, 0}));
    gui_elements.push_back(new Label(window_width/4 + 10, 70, window_width/2 - 10, 100, "", window_width/50, {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, {0, 0, 0}));
    gui_elements.push_back(new Label(window_width/4 + 10, 180, window_width/2 - 10, 100, "", window_width/50, {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, {0, 0, 0}));

    gui_elements.push_back(new Label(window_width/4, window_height/2 - 25, 50, 50, "<", 0, {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, BHORANGE));
    gui_elements.back()->hidden = true;
    gui_elements.back()->on_click = [](GuiElement *element) {
        inventory.items.push_back(inventory.equipped_foci[element->token]);
        inventory.equipped_foci[element->token] = nullptr;
        update_scene(false);
    };

    gui_elements.push_back(new Label(3*window_width/4 - 50, window_height/2 - 25, 50, 50, ">", 0, {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, BHORANGE));
    gui_elements.back()->hidden = true;
    gui_elements.back()->on_click = [](GuiElement *element) {
        for(int i = 0; i < 3; i++) {
            if(inventory.equipped_foci[i] == nullptr) {
                inventory.equipped_foci[i] = dynamic_cast<WandFocus*>(inventory.items[element->token]);
                inventory.items.erase(inventory.items.begin() + element->token);
                break;
            }
        }
        update_scene(false);
    };

    gui_elements.push_back(new Button(window_width/2 - 150, 300, 300, 100, "Использовать", BGREEN, BHGREEN));
    gui_elements.back()->hidden = true;
    gui_elements.back()->on_click = [](GuiElement *element) {
        if(dynamic_cast<UsableItem*>(inventory.items[element->token])->use())
            inventory.items.erase(inventory.items.begin() + element->token);
        update_scene(false);
    };

    gui_elements.push_back(new Button(window_width/2 - 150, 410, 300, 50, "Удалить", BRED, BHRED));
    gui_elements.back()->hidden = true;
    gui_elements.back()->on_click = [](GuiElement *element) {
        if(element->token < 0) {
            delete inventory.equipped_foci[-element->token - 1];
            inventory.equipped_foci[-element->token - 1] = nullptr;
        }
        else {
            delete inventory.items[element->token];
            inventory.items.erase(inventory.items.begin() + element->token);
        }
        update_scene(false);
    };

    gui_elements.push_back(new Label(0, 0, window_width/4, 50, "Предметы", 0, {255, 255, 255}, {255, 255, 255}, BGBLUE, BGBLUE));
    gui_elements.push_back(new Label(3*window_width/4, 0, window_width/4, 50, "Линзы", 0, {255, 255, 255}, {255, 255, 255}, BGBLUE, BGBLUE));

    for(int i = 0; i < 3; i++) {
        string name = default_focus.name;
        if(inventory.equipped_foci[i] != nullptr)
            name = inventory.equipped_foci[i]->name;
        gui_elements.push_back(new Label(3*window_width/4, 100 + i*50, window_width/4, 50, name, 0, {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, BHORANGE));
        gui_elements.back()->token = i;
        gui_elements.back()->on_click = [](GuiElement *element) {
            gui_elements[5]->hidden = true;
            gui_elements[6]->hidden = true;
            gui_elements[7]->hidden = true;
            gui_elements[8]->hidden = true;

            WandFocus *focus = (inventory.equipped_foci[element->token] == nullptr) ? &default_focus : inventory.equipped_foci[element->token];
            dynamic_cast<Label*>(gui_elements[2])->text = focus->name;
            dynamic_cast<Label*>(gui_elements[3])->text = focus->description;
            dynamic_cast<Label*>(gui_elements[4])->text = "Потребление маны: " + to_string(focus->mana_usage) + "\nТребуемый интеллект: " + to_string(focus->intelligence_requirement);
            gui_elements[2]->update();
            gui_elements[3]->update();
            gui_elements[4]->update();

            if(focus != &default_focus) {
                gui_elements[5]->hidden = false;
                gui_elements[5]->token = element->token;
                gui_elements[8]->hidden = false;
                gui_elements[8]->token = -element->token - 1;
            }
        };
    }

    int height = 100;
    int i = 0;
    while(i < inventory.items.size() && height <= window_height - 100) {
        gui_elements.push_back(new Label(0, 100 + i*50, window_width/4, 50, inventory.items[i]->name, 0, {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, BHORANGE));
        gui_elements.back()->token = i;
        gui_elements.back()->on_click = [](GuiElement *element) {
            dynamic_cast<Label*>(gui_elements[4])->text = "";
            gui_elements[4]->update();
            gui_elements[5]->hidden = true;
            gui_elements[6]->hidden = true;
            gui_elements[7]->hidden = true;

            gui_elements[8]->hidden = false;
            gui_elements[8]->token = element->token;

            Item *item = inventory.items[element->token];
            dynamic_cast<Label*>(gui_elements[2])->text = item->name;
            dynamic_cast<Label*>(gui_elements[3])->text = item->description;
            gui_elements[2]->update();
            gui_elements[3]->update();

            if(dynamic_cast<WandFocus*>(item)) {
                dynamic_cast<Label*>(gui_elements[4])->text = "Потребление маны: " + to_string(dynamic_cast<WandFocus*>(item)->mana_usage) + "\nТребуемый интеллект: " + to_string(dynamic_cast<WandFocus*>(item)->intelligence_requirement);
                gui_elements[4]->update();
                if(inventory.equipped_foci[0] == nullptr || inventory.equipped_foci[1] == nullptr || inventory.equipped_foci[2] == nullptr) {
                    gui_elements[6]->hidden = false;
                    gui_elements[6]->token = element->token;
                }
            }
            else if(dynamic_cast<UsableItem*>(item)) {
                gui_elements[7]->hidden = false;
                gui_elements[7]->token = element->token;
            }
        };

        i++;
        height += 50;
    }

    gui_elements.push_back(new Label(0, 50, window_width/4, 50, "^", 0, {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, BHORANGE));
    gui_elements.back()->hidden = true;
    gui_elements.back()->on_click = [](GuiElement *element) {
        int total = gui_elements.size() - 17;

        items_page--;
        for(int i = total-2; i >= 0; i--) {
            dynamic_cast<Label*>(gui_elements[14+i+1])->text = dynamic_cast<Label*>(gui_elements[14+i])->text;
            dynamic_cast<Label*>(gui_elements[14+i+1])->token = dynamic_cast<Label*>(gui_elements[14+i])->token;
            dynamic_cast<Label*>(gui_elements[14+i+1])->update();
        }
        dynamic_cast<Label*>(gui_elements[14])->text = inventory.items[items_page]->name;
        dynamic_cast<Label*>(gui_elements[14])->token = items_page;
        dynamic_cast<Label*>(gui_elements[14])->update();

        gui_elements[gui_elements.size()-2]->hidden = false;
        if(items_page == 0)
            element->hidden = true;
    };

    gui_elements.push_back(new Label(0, window_height - 50, window_width/4, 50, "v", 0, {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, BHORANGE));
    if(i == inventory.items.size())
        gui_elements.back()->hidden = true;
    gui_elements.back()->on_click = [](GuiElement *element) {
        int total = gui_elements.size() - 17;

        items_page++;
        for(int i = 1; i < total; i++) {
            dynamic_cast<Label*>(gui_elements[14+i-1])->text = dynamic_cast<Label*>(gui_elements[14+i])->text;
            dynamic_cast<Label*>(gui_elements[14+i-1])->token = dynamic_cast<Label*>(gui_elements[14+i])->token;
            dynamic_cast<Label*>(gui_elements[14+i-1])->update();
        }
        dynamic_cast<Label*>(gui_elements[14+total-1])->text = inventory.items[items_page+total-1]->name;
        dynamic_cast<Label*>(gui_elements[14+total-1])->token = items_page+total-1;
        dynamic_cast<Label*>(gui_elements[14+total-1])->update();

        gui_elements[gui_elements.size()-3]->hidden = false;
        if(items_page == inventory.items.size() - total)
            element->hidden = true;
    };

    gui_elements.push_back(new Button(window_width/2 - 150, window_height - 120, 300, 100, "Назад", BRED, BHRED));
    gui_elements.back()->on_click = [](GuiElement *element) {
        current_scene = SceneEnum::GameScene;
        update_scene(true);
    };
    escape_element = gui_elements.size()-1;
}

void upgrade_scene(bool new_scene) {
    clear_gui();

    gui_elements.push_back(new Label(0, 0, window_width, 50, "Доступно очков прокачки: " + to_string(player.level - player.used_points), 0, {255, 255, 255}, {255, 255, 255}, BGBLUE, BGBLUE));

    gui_elements.push_back(new Label(window_width/2 - 320, 70, 250, 50, "Здоровье: " + to_string(player.max_health), 0, {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, {0, 0, 0}));
    gui_elements.push_back(new Button(window_width/2 - 60, 70, 50, 50, "+", BGREEN, BHGREEN));
    if(player.used_points < player.level) {
        gui_elements.back()->on_click = [](GuiElement *element) {
            player.max_health++;
            player.used_points++;
            update_scene(false);
        };
    }
    else
        gui_elements.back()->hidden = true;
    
    gui_elements.push_back(new Label(window_width/2 + 10, 70, 250, 50, "Мана: " + to_string(player.max_mana), 0, {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, {0, 0, 0}));
    gui_elements.push_back(new Button(window_width/2 + 260, 70, 50, 50, "+", BGREEN, BHGREEN));
    if(player.used_points < player.level) {
        gui_elements.back()->on_click = [](GuiElement *element) {
            player.max_mana++;
            player.used_points++;
            update_scene(false);
        };
    }
    else
        gui_elements.back()->hidden = true;
    
    gui_elements.push_back(new Label(window_width/2 - 320, 130, 250, 50, "Восстановление здоровья: " + to_string(player.health_regen), 0, {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, {0, 0, 0}));
    gui_elements.push_back(new Button(window_width/2 - 60, 130, 50, 50, "+", BGREEN, BHGREEN));
    if(player.used_points < player.level) {
        gui_elements.back()->on_click = [](GuiElement *element) {
            player.health_regen++;
            player.used_points++;
            update_scene(false);
        };
    }
    else
        gui_elements.back()->hidden = true;
    
    gui_elements.push_back(new Label(window_width/2 + 10, 130, 250, 50, "Восстановление маны: " + to_string(player.mana_regen), 0, {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, {0, 0, 0}));
    gui_elements.push_back(new Button(window_width/2 + 260, 130, 50, 50, "+", BGREEN, BHGREEN));
    if(player.used_points < player.level) {
        gui_elements.back()->on_click = [](GuiElement *element) {
            player.mana_regen++;
            player.used_points++;
            update_scene(false);
        };
    }
    else
        gui_elements.back()->hidden = true;

    gui_elements.push_back(new Label(window_width/2 - 320, 190, 250, 50, "Сопротивление: " + to_string(player.resistance), 0, {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, {0, 0, 0}));
    gui_elements.push_back(new Button(window_width/2 - 60, 190, 50, 50, "+", BGREEN, BHGREEN));
    if(player.used_points < player.level) {
        gui_elements.back()->on_click = [](GuiElement *element) {
            player.resistance++;
            player.used_points++;
            update_scene(false);
        };
    }
    else
        gui_elements.back()->hidden = true;
    
    gui_elements.push_back(new Label(window_width/2 + 10, 190, 250, 50, "Интеллект: " + to_string(player.intelligence), 0, {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, {0, 0, 0}));
    gui_elements.push_back(new Button(window_width/2 + 260, 190, 50, 50, "+", BGREEN, BHGREEN));
    if(player.used_points < player.level) {
        gui_elements.back()->on_click = [](GuiElement *element) {
            player.intelligence++;
            player.used_points++;
            update_scene(false);
        };
    }
    else
        gui_elements.back()->hidden = true;
    
    gui_elements.push_back(new Label(window_width/2 - 155, 250, 250, 50, "Ловкость: " + to_string(player.dexterity), 0, {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, {0, 0, 0}));
    gui_elements.push_back(new Button(window_width/2 + 105, 250, 50, 50, "+", BGREEN, BHGREEN));
    if(player.used_points < player.level) {
        gui_elements.back()->on_click = [](GuiElement *element) {
            player.dexterity++;
            player.used_points++;
            update_scene(false);
        };
    }
    else
        gui_elements.back()->hidden = true;

    gui_elements.push_back(new Button(window_width/2 - 150, window_height - 120, 300, 100, "Назад", BRED, BHRED));
    gui_elements.back()->on_click = [](GuiElement *element) {
        current_scene = SceneEnum::GameScene;
        update_scene(true);
    };
    escape_element = gui_elements.size()-1;
}

void exit_save_scene(bool new_scene) {
    clear_gui();

    string last_save = "Последнее сохранение: ";

    if(player.save_time != 0) {
        int64_t time_difference_ns = (chrono::system_clock().now() - chrono::system_clock::from_time_t(player.save_time)).count();
        int64_t time_difference_s = time_difference_ns / 1000000000LL;
        int64_t time_difference_m = time_difference_s / 60;
        int64_t time_difference_h = time_difference_m / 60;
        int64_t time_difference_d = time_difference_h / 24;

        last_save += to_string(time_difference_d) + " дней " + to_string(time_difference_h - time_difference_d*24) +
                " часов " + to_string(time_difference_m - time_difference_h*60) + " минут " + to_string(time_difference_s - time_difference_m*60) + " секунд назад";
    }
    else
        last_save += "никогда";

    gui_elements.push_back(new Solid(0, 0, window_width, window_height, {0, 0, 0, 96}));

    gui_elements.push_back(new Label(window_width/2 - 310, window_height/2 - 270, 620, 100, "Сохранить игру?", 0, {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, {0, 0, 0}));
    gui_elements.push_back(new Label(window_width/2 - 310, window_height/2 - 160, 620, 100, last_save, 25, {255, 255, 255}, {255, 255, 255}, {0, 0, 0}, {0, 0, 0}));
    
    gui_elements.push_back(new Button(window_width/2 - 310, window_height/2 - 50, 300, 100, "Не сохранять игру", BRED, BHRED));
    gui_elements.back()->on_click = [](GuiElement *element) {
        loaded_chunks.clear();
        clear_entities();
        clear_effects();
        inventory.clear_inventory();

        if(!quitting) {
            current_scene = SceneEnum::MainScene;
            update_scene(true);
        }
        else
            is_running = false;
    };

    gui_elements.push_back(new Button(window_width/2 + 10, window_height/2 - 50, 300, 100, "Сохранить игру", BGREEN, BHGREEN));
    gui_elements.back()->on_click = [](GuiElement *element) {
        save_game();
        loaded_chunks.clear();
        clear_entities();
        clear_effects();
        inventory.clear_inventory();

        if(!quitting) {
            current_scene = SceneEnum::MainScene;
            update_scene(true);
        }
        else
            is_running = false;
    };
}

void death_scene(bool new_scene) {
    clear_gui();

    gui_elements.push_back(new Solid(0, 0, window_width, window_height, {0, 0, 0, 96}));

    gui_elements.push_back(new Label(window_width/2 - 310, window_height/2 - 270, 620, 100, "Вы умерли", 0, TWRED, TWRED, {0, 0, 0}, {0, 0, 0}));

    gui_elements.push_back(new Button(window_width/2 - 310, window_height/2 - 50, 300, 100, "Возродиться", BGREEN, BHGREEN));
    gui_elements.back()->on_click = [](GuiElement *element) {
        player.x = 0;
        player.y = 0;
        player.health = player.max_health;
        player.mana = player.max_mana;
        player.damaged = 0;
        player.vel_x = 0;
        player.vel_y = 0;

        player.xp -= player.xp / (9/player.difficulty);

        current_scene = SceneEnum::GameScene;
        update_scene(true);
    };

    gui_elements.push_back(new Button(window_width/2 + 10, window_height/2 - 50, 300, 100, "Выйти из игры", BRED, BHRED));
    gui_elements.back()->on_click = [](GuiElement *element) {
        quitting = false;
        current_scene = SceneEnum::ExitSaveScene;
        update_scene(true);
    };
}

void console_scene(bool new_scene) {
    string console_text;
    string input;
    if(!new_scene) {
        console_text = dynamic_cast<Label*>(gui_elements[0])->text;
        input = dynamic_cast<TextBox*>(gui_elements[1])->text;
    }
    clear_gui();

    gui_elements.push_back(new Label(10, 10, window_width/2 - 20, window_height - 260, console_text, 0, BHGREEN, BHGREEN, {0, 0, 0}, {0, 0, 0}));
    
    gui_elements.push_back(new TextBox(10, window_height - 240, window_width - 20, 100, input, false));
    selected = gui_elements.size()-1;
    gui_elements.back()->on_click = [](GuiElement *element) {
        if(selected == 1) {
            string command = dynamic_cast<TextBox*>(gui_elements[1])->text;
            vector<string> split;
            stringstream stream(command);
            
            string item;
            while(getline(stream, item, ' '))
                split.push_back(item);
            
            bool error = false;
            if(split.size() == 0)
                error = true;
            else {
                if(split[0] == "setparam") {
                    if(split.size() != 3)
                        error = true;
                    else if(split[1] == "spawnrate") {
                        try {
                            int value = stoi(split[2]);
                            if(value > 0)
                                spawn_rate = value;
                            else
                                error = true;
                        }
                        catch(const exception&) {
                            error = true;
                        }
                    }
                    else if(split[1] == "dexterity") {
                        try {
                            int value = stoi(split[2]);
                            if(value > 0)
                                player.dexterity = value;
                            else
                                error = true;
                        }
                        catch(const exception&) {
                            error = true;
                        }
                    }
                    else
                        error = true;
                }
                else if(split.size() == 2 && split[0] == "give" && split[1] == "opfocus") {
                    BasicFocus *focus = new BasicFocus("Супер линза", "Линза, наносящая 99999 урона.", 0, 0, 99999);
                    inventory.items.push_back(focus);
                }
                else
                    error = true;
            }

            if(error)
                dynamic_cast<Label*>(gui_elements[0])->text += "Неправильно введена команда!\n";
            else
                dynamic_cast<Label*>(gui_elements[0])->text += "Успешно\n";
            gui_elements[0]->update();
            dynamic_cast<TextBox*>(gui_elements[1])->text = "";
            gui_elements[1]->update();
        }
    };

    gui_elements.push_back(new Button(window_width/2 - 150, window_height - 120, 300, 100, "Назад", BRED, BHRED));
    gui_elements.back()->on_click = [](GuiElement *element) {
        current_scene = SceneEnum::GameScene;
        update_scene(true);
    };
    escape_element = gui_elements.size()-1;
}

void clear_gui() {
    while(!gui_elements.empty()) {
        if(gui_elements.back() != nullptr)
            delete gui_elements.back();
        gui_elements.pop_back();
    }
}

void draw_gui() {
    for(auto i : gui_elements) {
        if(i != nullptr && !i->hidden) {
            i->draw();
        }
    }
}

void hover_gui(int x, int y) {
    for(auto i : gui_elements) {
        if(i != nullptr && !i->hidden && x >= i->x && x <= i->x + i->w && y >= i->y && y <= i->y + i->h) {
            i->hover_handler();
        }
    }
}

void click_gui(int x, int y) {
    selected = -1;
    for(int i = 0; i < gui_elements.size(); i++) {
        if(gui_elements[i] != nullptr && !gui_elements[i]->hidden && x >= gui_elements[i]->x && x <= gui_elements[i]->x + gui_elements[i]->w && y >= gui_elements[i]->y && y <= gui_elements[i]->y + gui_elements[i]->h) {
            SceneEnum temp = current_scene;
            gui_elements[i]->click_handler();
            selected = i;
            if(temp != current_scene)
                break;
        }
    }
}

void key_pressed_gui(SDL_Keycode key) {
    if(key == SDLK_DOWN && selected < ((int) gui_elements.size())-1) {
        selected++;
    }
    else if(key == SDLK_UP && selected > 0) {
        selected--;
    }
    else if(key == SDLK_RETURN && selected >= 0 && selected < gui_elements.size() && !gui_elements[selected]->hidden) {
        gui_elements[selected]->click_handler();
    }
    else if(key == SDLK_ESCAPE && escape_element != -1) {
        if(selected == -1)
            gui_elements[escape_element]->click_handler();
        else
            selected = -1;
    }
    else if(key == SDLK_e && current_scene == SceneEnum::GameScene) {
        gui_elements[1]->click_handler();
    }
    else if(key == SDLK_e && current_scene == SceneEnum::InventoryScene) {
        gui_elements[escape_element]->click_handler();
    }
    else if(key == SDLK_u && current_scene == SceneEnum::GameScene) {
        gui_elements[2]->click_handler();
    }
    else if(key == SDLK_u && current_scene == SceneEnum::UpgradeScene) {
        gui_elements[escape_element]->click_handler();
    }
    else if(key == SDLK_BACKQUOTE && current_scene == SceneEnum::GameScene) {
        current_scene = SceneEnum::ConsoleScene;
        update_scene(true);
    }
    else if(key == SDLK_BACKQUOTE && current_scene == SceneEnum::ConsoleScene) {
        gui_elements[escape_element]->click_handler();
    }
    else if(selected < gui_elements.size()) {
        gui_elements[selected]->key_pressed_handler(key);
    }
}