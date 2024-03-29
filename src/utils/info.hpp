#ifndef SIMPLE_DB_INFO_HPP
#define SIMPLE_DB_INFO_HPP
const char * WELCOME_BANNER =  "                ___          ______                                       \n"
                               "               /__/\\     ___/_____/\\                                     \n"
                               "               \\  \\ \\   /         /\\\\                                  \n"
                               "                \\  \\ \\_/__       /  \\                                     \n"
                               "                _\\  \\ \\  /\\_____/___ \\                                    \n"
                               "               // \\__\\/ /  \\       /\\ \\                   ___                       ___           ___                       ___          _____                  \n"
                               "       _______//_______/    \\     / _\\/______            /  /\\        ___          /__/\\         /  /\\                     /  /\\        /  /  \\        _____    \n"
                               "      /      / \\       \\    /    / /        /\\          /  / /_      /  /\\        |  |  \\       /  /  \\                   /  / /_      /  / /\\ \\      /  /  \\   \n"
                               "   __/      /   \\       \\  /    / /        / _\\__      /  / / /\\    /  / /        |  | | \\     /  / /\\ \\  ___     ___    /  / / /\\    /  / /  \\ \\    /  / /\\ \\  \n"
                               "  / /      /     \\_______\\/    / /        / /   /\\    /  / / /  \\  /__/  \\      __|__| |\\ \\   /  / /~/ / /__/\\   /  /\\  /  / / / /_  /__/ / \\__\\ |  /  / /~/  \\ \n"
                               " /_/______/___________________/ /________/ /___/  \\  /__/ / / /\\ \\ \\__\\/\\ \\__  /__/    | \\ \\ /__/ / / /  \\  \\ \\ /  / / /__/ / / / /\\ \\  \\ \\ /  / / /__/ / / /\\ |\n"
                               " \\ \\      \\    ___________    \\ \\        \\ \\   \\  /  \\  \\ \\/ /~/ /    \\  \\ \\/\\ \\  \\ \\~~\\__\\/ \\  \\ \\/ /    \\  \\ \\  / /  \\  \\ \\/ / / /  \\  \\ \\  / /  \\  \\ \\/ /~/ /\n"
                               "  \\_\\      \\  /          /\\    \\ \\        \\ \\___\\/    \\  \\  / / /      \\__\\  /  \\  \\ \\        \\  \\  /      \\  \\ \\/ /    \\  \\  / / /    \\  \\ \\/ /    \\  \\  / / / \n"
                               "     \\      \\/          /  \\    \\ \\        \\  /        \\__\\/ / /       /__/ /    \\  \\ \\        \\  \\ \\       \\  \\  /      \\  \\ \\/ /      \\  \\  /      \\  \\ \\/ /  \n"
                               "      \\_____/          /    \\    \\ \\________\\/           /__/ /        \\__\\/      \\  \\ \\        \\  \\ \\       \\__\\/        \\  \\  /        \\__\\/        \\  \\  /   \n"
                               "           /__________/      \\    \\  /                   \\__\\/                     \\__\\/         \\__\\/                     \\__\\/                       \\__\\/    \n"
                               "           \\   _____  \\      /_____\\/                                     \n"
                               "            \\ /    /\\  \\    / \\  \\ \\                                                                                                        \n"
                               "             /____/  \\  \\  /   \\  \\ \\                                                                                                 Simple DB, Grp.           \n"
                               "             \\    \\  /___\\/     \\  \\ \\                                                                                                         --Presents       \n"
                               "              \\____\\/            \\__\\/                \n\n";

const char * WELCOME_BANNER_V1="                     ___          ______                                       \n"
                               "                    /__/\\     ___/_____/\\                                     \n"
                               "                    \\  \\ \\   /         /\\\\                                  \n"
                               "                     \\  \\ \\_/__       /  \\                                     \n"
                               "                     _\\  \\ \\  /\\_____/___ \\                                    \n"
                               "                    // \\__\\/ /  \\       /\\ \\                                    ___                       ___           ___                       ___          _____                  \n"
                               "            _______//_______/    \\     / _\\/______                             /  /\\        ___          /__/\\         /  /\\                     /  /\\        /  /  \\        _____    \n"
                               "           /      / \\       \\    /    / /        /\\                           /  / /_      /  /\\        |  |  \\       /  /  \\                   /  / /_      /  / /\\ \\      /  /  \\   \n"
                               "        __/      /   \\       \\  /    / /        / _\\__                       /  / / /\\    /  / /        |  | | \\     /  / /\\ \\  ___     ___    /  / / /\\    /  / /  \\ \\    /  / /\\ \\  \n"
                               "       / /      /     \\_______\\/    / /        / /   /\\                     /  / / /  \\  /__/  \\      __|__| |\\ \\   /  / /~/ / /__/\\   /  /\\  /  / / / /_  /__/ / \\__\\ |  /  / /~/  \\ \n"
                               "      /_/______/___________________/ /________/ /___/  \\                   /__/ / / /\\ \\ \\__\\/\\ \\__  /__/    | \\ \\ /__/ / / /  \\  \\ \\ /  / / /__/ / / / /\\ \\  \\ \\ /  / / /__/ / / /\\ |\n"
                               "      \\ \\      \\    ___________    \\ \\        \\ \\   \\  /                   \\  \\ \\/ /~/ /    \\  \\ \\/\\ \\  \\ \\~~\\__\\/ \\  \\ \\/ /    \\  \\ \\  / /  \\  \\ \\/ / / /  \\  \\ \\  / /  \\  \\ \\/ /~/ /\n"
                               "       \\_\\      \\  /          /\\    \\ \\        \\ \\___\\/                     \\  \\  / / /      \\__\\  /  \\  \\ \\        \\  \\  /      \\  \\ \\/ /    \\  \\  / / /    \\  \\ \\/ /    \\  \\  / / / \n"
                               "          \\      \\/          /  \\    \\ \\        \\  /                         \\__\\/ / /       /__/ /    \\  \\ \\        \\  \\ \\       \\  \\  /      \\  \\ \\/ /      \\  \\  /      \\  \\ \\/ /  \n"
                               "           \\_____/          /    \\    \\ \\________\\/                            /__/ /        \\__\\/      \\  \\ \\        \\  \\ \\       \\__\\/        \\  \\  /        \\__\\/        \\  \\  /   \n"
                               "                /__________/      \\    \\  /                                    \\__\\/                     \\__\\/         \\__\\/                     \\__\\/                       \\__\\/    \n"
                               "                \\   _____  \\      /_____\\/                                     \n"
                               "                 \\ /    /\\  \\    / \\  \\ \\                 Simple DB, Grp.              \n"
                               "                  /____/  \\  \\  /   \\  \\ \\                           --Presents      \n"
                               "                  \\    \\  /___\\/     \\  \\ \\                                    \n"
                               "                   \\____\\/            \\__\\/                \n\n";

const char * PRODUCT_BANNER = "      ___                       ___           ___                       ___          _____                  \n"
                              "     /  /\\        ___          /__/\\         /  /\\                     /  /\\        /  /  \\        _____    \n"
                              "    /  / /_      /  /\\        |  |  \\       /  /  \\                   /  / /_      /  / /\\ \\      /  /  \\   \n"
                              "   /  / / /\\    /  / /        |  | | \\     /  / /\\ \\  ___     ___    /  / / /\\    /  / /  \\ \\    /  / /\\ \\  \n"
                              "  /  / / /  \\  /__/  \\      __|__| |\\ \\   /  / /~/ / /__/\\   /  /\\  /  / / / /_  /__/ / \\__\\ |  /  / /~/  \\ \n"
                              " /__/ / / /\\ \\ \\__\\/\\ \\__  /__/    | \\ \\ /__/ / / /  \\  \\ \\ /  / / /__/ / / / /\\ \\  \\ \\ /  / / /__/ / / /\\ |\n"
                              " \\  \\ \\/ /~/ /    \\  \\ \\/\\ \\  \\ \\~~\\__\\/ \\  \\ \\/ /    \\  \\ \\  / /  \\  \\ \\/ / / /  \\  \\ \\  / /  \\  \\ \\/ /~/ /\n"
                              "  \\  \\  / / /      \\__\\  /  \\  \\ \\        \\  \\  /      \\  \\ \\/ /    \\  \\  / / /    \\  \\ \\/ /    \\  \\  / / / \n"
                              "   \\__\\/ / /       /__/ /    \\  \\ \\        \\  \\ \\       \\  \\  /      \\  \\ \\/ /      \\  \\  /      \\  \\ \\/ /  \n"
                              "     /__/ /        \\__\\/      \\  \\ \\        \\  \\ \\       \\__\\/        \\  \\  /        \\__\\/        \\  \\  /   \n"
                              "     \\__\\/                     \\__\\/         \\__\\/                     \\__\\/                       \\__\\/    \n\n";
#endif