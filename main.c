#include "minimap.h" // Contient toutes les définitions et prototypes nécessaires
#include <SDL/SDL_mixer.h>

int main(int argc, char *argv[]) {
    SDL_Surface *screen = NULL;
    // Fonds principaux
    SDL_Surface *background_level1 = NULL;
    SDL_Surface *background_level2 = NULL;
    // Fonds minimap
    SDL_Surface *minimap_bg_surf1 = NULL;
    SDL_Surface *minimap_bg_surf2 = NULL;

    // Structures de jeu
    Player player;
    Obstacle obstacle;
    Bonus bonus; // Nouvelle structure pour le bonus
    Minimap minimap;

    SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}; // Caméra pour le scrolling
    int game_running = 1; // Flag pour la boucle de jeu
    int current_level = 1; // Niveau actuel (1 ou 2)

    // Timing
    Uint32 last_time = 0, current_time;
    float dt = 0; // Delta time en secondes

    // Son (exemple pour les limites)
    Mix_Chunk *limite_sound = NULL;

    // 1. Initialisation SDL (Vidéo, Audio, Image)
    printf("--- Initialisation SDL ---\n");
    if (!init_sdl(&screen)) { // init_sdl gère maintenant SDL, IMG et Mixer
        return 1; // Quitter si l'initialisation échoue
    }

    // Charger le son (après init_sdl qui initialise Mixer)
    limite_sound = Mix_LoadWAV("click.wav"); // Assurez-vous que ce fichier existe
    if (!limite_sound) {
        printf("Avertissement: Son 'click.wav' non chargé : %s\n", Mix_GetError());
        // Le jeu peut continuer sans ce son
    }

    // 2. Chargement et Initialisation des Ressources
    printf("--- Chargement Ressources ---\n");
    // Charger les fonds principaux
    if (!load_background(&background_level1, "background.png", LEVEL_WIDTH, LEVEL_HEIGHT)) { // Niveau 1
        fprintf(stderr, "Avertissement: background.png non chargé!\n");
    }
    if (!load_background(&background_level2, "background2.jpeg", LEVEL_WIDTH, LEVEL_HEIGHT)) { // Niveau 2
         fprintf(stderr, "Avertissement: background2.jpeg non chargé!\n");
    }
    // Charger les fonds de minimap
    if (!load_background(&minimap_bg_surf1, "minimap.png", MINIMAP_WIDTH, MINIMAP_HEIGHT)) { // Minimap N1
         fprintf(stderr, "Erreur critique: minimap.png non chargé!\n");
         cleanup(screen, background_level1, background_level2, minimap_bg_surf1, minimap_bg_surf2, &player, &bonus, &minimap); // Nettoyer ce qui a été chargé
         return 1;
    }
     if (!load_background(&minimap_bg_surf2, "minimap_bg2.jpg", MINIMAP_WIDTH, MINIMAP_HEIGHT)) { // Minimap N2
         fprintf(stderr, "Avertissement: minimap_bg2.jpg non chargé!\n");
    }

    // Initialiser joueur
    init_player(&player);
    if (!load_player_assets(&player)) {
        fprintf(stderr, "Erreur critique: Assets joueur.\n");
        cleanup(screen, background_level1, background_level2, minimap_bg_surf1, minimap_bg_surf2, &player, &bonus, &minimap);
        return 1;
    }

    // Initialiser obstacle
    init_obstacle(&obstacle);

    // Initialiser le bonus
    init_bonus(&bonus, 500, LEVEL_HEIGHT - 50); // Positionnez le bonus (x, y)
    if (!load_bonus_assets(&bonus, "bonus.png")) { // Charger l'image du bonus
        fprintf(stderr, "Avertissement: Asset bonus 'bonus.png' non chargé.\n");
        // Le jeu peut continuer sans le bonus, ou vous pouvez quitter
        // cleanup(...); return 1; // Décommentez pour quitter si le bonus est essentiel
    }


    // Initialiser la minimap (avec le fond du niveau 1 par défaut)
    if (!init_minimap(&minimap, minimap_bg_surf1, "mini.png", LEVEL_WIDTH, LEVEL_HEIGHT)) { // mini.png = icone joueur
        fprintf(stderr, "Erreur critique: Initialisation Minimap.\n");
         cleanup(screen, background_level1, background_level2, minimap_bg_surf1, minimap_bg_surf2, &player, &bonus, &minimap);
        return 1;
    }

    // 3. Boucle de Jeu
    printf("--- Lancement Boucle de Jeu ---\n");
    last_time = SDL_GetTicks(); // Temps initial
    while (game_running) {
        // Calculer Delta Time (dt)
        current_time = SDL_GetTicks();
        dt = (current_time - last_time) / 1000.0f; // Temps écoulé en secondes
        last_time = current_time;

        // Gestion Entrées (Clavier/Souris/Events)
        handle_input(&player, &game_running, &current_level, &minimap, minimap_bg_surf1, minimap_bg_surf2);

        // Mise à Jour Logique (Mouvement, Collisions, IA, etc.)
        // Passe maintenant le pointeur vers le bonus pour la détection de collision
        update_player(&player, &obstacle, &bonus, dt, limite_sound);
        // update_bonus(&bonus, dt); // Si le bonus a besoin d'être mis à jour (ex: animation)
        update_camera(&camera, &player); // Mettre à jour la caméra après le joueur

        // Rendu Graphique (Dessin à l'écran)
        // Passe maintenant le pointeur vers le bonus pour l'affichage
        render_game(screen, background_level1, background_level2, current_level, &player, &obstacle, &bonus, &minimap, &camera);

        // Affichage Final (Met à jour l'écran)
        if (SDL_Flip(screen) < 0) {
             printf("Erreur SDL_Flip: %s\n", SDL_GetError());
             game_running = 0; // Arrêter le jeu en cas d'erreur d'affichage
        }

        // Petite pause pour éviter de consommer 100% CPU
        SDL_Delay(16); // Vise environ 60 FPS (1000ms / 60fps ≈ 16ms)
    }

    // 4. Nettoyage (Libération des ressources)
    printf("--- Fin du jeu, Nettoyage ---\n");
    if (limite_sound) { // Libérer le son s'il a été chargé
        Mix_FreeChunk(limite_sound);
        printf("Son limite libéré.\n");
    }
    // Passe maintenant le pointeur vers le bonus pour libérer son image
    cleanup(screen, background_level1, background_level2, minimap_bg_surf1, minimap_bg_surf2, &player, &bonus, &minimap);

    printf("--- Programme terminé ---\n");
    return 0; // Succès
}
