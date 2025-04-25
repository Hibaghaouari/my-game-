#include <stdio.h>
#include <stdlib.h>
#include "minimap.h"
#include <SDL/SDL_mixer.h> // Nécessaire pour Mix_Chunk

// --- Implémentations des Fonctions ---

// --- Fonctions Minimap ---

// Identique à avant
int init_minimap(Minimap *m, SDL_Surface *initial_minimap_bg, const char *player_icon_path, int level_w, int level_h) {
    if (!initial_minimap_bg) {
        printf("Erreur: Fond de minimap initial non fourni à init_minimap.\n");
        return 0;
    }
    m->background = initial_minimap_bg;

    m->player_icon = IMG_Load(player_icon_path);
    if (!m->player_icon) {
        printf("Erreur chargement icone joueur minimap : %s\n", IMG_GetError());
        return 0;
    }
    m->position_ecran.x = MINIMAP_POS_X;
    m->position_ecran.y = MINIMAP_POS_Y;
    m->position_ecran.w = MINIMAP_WIDTH;
    m->position_ecran.h = MINIMAP_HEIGHT;
    m->level_width = level_w;
    m->level_height = level_h;
    printf("Minimap initialisée.\n");
    return 1;
}

// Modifié: Prend le bonus en paramètre pour l'afficher
void afficher_minimap(const Minimap *m, SDL_Surface *screen, SDL_Rect player_pos_jeu, SDL_Rect obstacle_pos_jeu, const Bonus *bonus) {
     if (!m || !m->background || !m->player_icon || !screen) return;
     if (m->level_width <= 0 || m->level_height <= 0) return;

    // Dessiner le fond de la minimap
    SDL_BlitSurface(m->background, NULL, screen, &m->position_ecran);

    // Calculer et dessiner le joueur sur minimap (identique)
    SDL_Rect player_pos_minimap;
    player_pos_minimap.x = m->position_ecran.x + (int)(((float)player_pos_jeu.x / m->level_width) * MINIMAP_WIDTH);
    player_pos_minimap.y = m->position_ecran.y + (int)(((float)player_pos_jeu.y / m->level_height) * MINIMAP_HEIGHT);
    player_pos_minimap.x -= m->player_icon->w / 2; // Centrer l'icône
    player_pos_minimap.y -= m->player_icon->h / 2;
    // Clipping Joueur (Assurer qu'il reste dans les limites de la minimap)
    if (player_pos_minimap.x < m->position_ecran.x) player_pos_minimap.x = m->position_ecran.x;
    if (player_pos_minimap.y < m->position_ecran.y) player_pos_minimap.y = m->position_ecran.y;
    if (player_pos_minimap.x + m->player_icon->w > m->position_ecran.x + MINIMAP_WIDTH) player_pos_minimap.x = m->position_ecran.x + MINIMAP_WIDTH - m->player_icon->w;
    if (player_pos_minimap.y + m->player_icon->h > m->position_ecran.y + MINIMAP_HEIGHT) player_pos_minimap.y = m->position_ecran.y + MINIMAP_HEIGHT - m->player_icon->h;
    SDL_BlitSurface(m->player_icon, NULL, screen, &player_pos_minimap);

    // Calculer et dessiner l'obstacle sur minimap (identique)
    SDL_Rect obstacle_rect_minimap;
    obstacle_rect_minimap.x = m->position_ecran.x + (int)(((float)obstacle_pos_jeu.x / m->level_width) * MINIMAP_WIDTH);
    obstacle_rect_minimap.y = m->position_ecran.y + (int)(((float)obstacle_pos_jeu.y / m->level_height) * MINIMAP_HEIGHT);
    obstacle_rect_minimap.w = OBSTACLE_MINIMAP_SIZE;
    obstacle_rect_minimap.h = OBSTACLE_MINIMAP_SIZE;
    obstacle_rect_minimap.x -= OBSTACLE_MINIMAP_SIZE / 2; // Centrer
    obstacle_rect_minimap.y -= OBSTACLE_MINIMAP_SIZE / 2;
    // Clipping Obstacle
    if (obstacle_rect_minimap.x < m->position_ecran.x) obstacle_rect_minimap.x = m->position_ecran.x;
    if (obstacle_rect_minimap.y < m->position_ecran.y) obstacle_rect_minimap.y = m->position_ecran.y;
    if (obstacle_rect_minimap.x + obstacle_rect_minimap.w > m->position_ecran.x + MINIMAP_WIDTH) obstacle_rect_minimap.x = m->position_ecran.x + MINIMAP_WIDTH - obstacle_rect_minimap.w;
    if (obstacle_rect_minimap.y + obstacle_rect_minimap.h > m->position_ecran.y + MINIMAP_HEIGHT) obstacle_rect_minimap.y = m->position_ecran.y + MINIMAP_HEIGHT - obstacle_rect_minimap.h;
    // Dessiner Obstacle (rectangle rouge)
    if (obstacle_rect_minimap.w > 0 && obstacle_rect_minimap.h > 0) {
         if(obstacle_rect_minimap.x + obstacle_rect_minimap.w <= m->position_ecran.x + MINIMAP_WIDTH && obstacle_rect_minimap.y + obstacle_rect_minimap.h <= m->position_ecran.y + MINIMAP_HEIGHT) {
             Uint32 red = SDL_MapRGB(screen->format, 255, 0, 0); // Couleur rouge pour l'obstacle
             SDL_FillRect(screen, &obstacle_rect_minimap, red);
         }
    }

    // NOUVEAU: Calculer et dessiner le bonus sur minimap (si actif)
    if (bonus && bonus->active) {
        SDL_Rect bonus_rect_minimap;
        bonus_rect_minimap.x = m->position_ecran.x + (int)(((float)bonus->pos.x / m->level_width) * MINIMAP_WIDTH);
        bonus_rect_minimap.y = m->position_ecran.y + (int)(((float)bonus->pos.y / m->level_height) * MINIMAP_HEIGHT);
        bonus_rect_minimap.w = BONUS_MINIMAP_SIZE; // Utilise la nouvelle constante
        bonus_rect_minimap.h = BONUS_MINIMAP_SIZE;
        bonus_rect_minimap.x -= BONUS_MINIMAP_SIZE / 2; // Centrer le point
        bonus_rect_minimap.y -= BONUS_MINIMAP_SIZE / 2;
        // Clipping Bonus
        if (bonus_rect_minimap.x < m->position_ecran.x) bonus_rect_minimap.x = m->position_ecran.x;
        if (bonus_rect_minimap.y < m->position_ecran.y) bonus_rect_minimap.y = m->position_ecran.y;
        if (bonus_rect_minimap.x + bonus_rect_minimap.w > m->position_ecran.x + MINIMAP_WIDTH) bonus_rect_minimap.x = m->position_ecran.x + MINIMAP_WIDTH - bonus_rect_minimap.w;
        if (bonus_rect_minimap.y + bonus_rect_minimap.h > m->position_ecran.y + MINIMAP_HEIGHT) bonus_rect_minimap.y = m->position_ecran.y + MINIMAP_HEIGHT - bonus_rect_minimap.h;
        // Dessiner Bonus (point noir)
        if (bonus_rect_minimap.w > 0 && bonus_rect_minimap.h > 0) {
             if(bonus_rect_minimap.x + bonus_rect_minimap.w <= m->position_ecran.x + MINIMAP_WIDTH && bonus_rect_minimap.y + bonus_rect_minimap.h <= m->position_ecran.y + MINIMAP_HEIGHT) {
                 Uint32 black = SDL_MapRGB(screen->format, 0, 0, 0); // Couleur noire pour le bonus
                 SDL_FillRect(screen, &bonus_rect_minimap, black);
             }
        }
    }
}


// Identique
void liberer_minimap(Minimap *m) {
    if (m && m->player_icon) {
        SDL_FreeSurface(m->player_icon);
        m->player_icon = NULL;
        printf("Icone joueur minimap libérée.\n");
    }
    // m->background n'est pas libéré ici
}


// --- Fonctions du Jeu ---

// Identique
int init_sdl(SDL_Surface **screen) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) { // Initialiser aussi l'audio
         printf("Erreur SDL_Init: %s\n", SDL_GetError());
         return 0;
    }
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG))) { // Init PNG et JPG
        printf("Erreur IMG_Init: %s\n", IMG_GetError());
        SDL_Quit();
        return 0;
    }
     // Initialiser SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur Mix_OpenAudio : %s\n", Mix_GetError());
        // Continuer sans son ? Ou retourner 0 ? Pour l'instant on continue.
        // IMG_Quit(); SDL_Quit(); return 0; // Décommentez pour quitter si l'audio échoue
    }

    *screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!*screen) {
        printf("Erreur SDL_SetVideoMode: %s\n", SDL_GetError());
        Mix_CloseAudio(); IMG_Quit(); SDL_Quit();
        return 0;
    }
    SDL_WM_SetCaption("Jeu SDL 1.2 - Niveaux & Bonus", NULL);
    printf("SDL, SDL_image et SDL_mixer initialisés.\n");
    return 1;
}

// Identique
int load_player_assets(Player *player) {
    char filename[50];
    int success = 1;
    printf("Chargement assets joueur...\n");
    for (int i = 0; i < ANIMATION_FRAMES; ++i) {
        sprintf(filename, "droite/%d.png", i); // Assurez-vous que ces chemins sont corrects
        player->frames_right[i] = IMG_Load(filename);
        if (!player->frames_right[i]) { printf("Err load %s: %s\n", filename, IMG_GetError()); success = 0; }

        sprintf(filename, "gauche/%d.png", i); // Assurez-vous que ces chemins sont corrects
        player->frames_left[i] = IMG_Load(filename);
        if (!player->frames_left[i]) { printf("Err load %s: %s\n", filename, IMG_GetError()); success = 0; }
    }
     // Fallback si certaines images manquent (utilise la première image chargée)
     if (player->frames_right[0]) { for (int i = 1; i < ANIMATION_FRAMES; ++i) if (!player->frames_right[i]) player->frames_right[i] = player->frames_right[0]; } else success = 0;
     if (player->frames_left[0]) { for (int i = 1; i < ANIMATION_FRAMES; ++i) if (!player->frames_left[i]) player->frames_left[i] = player->frames_left[0]; } else success = 0;

    if (success) printf("Assets joueur chargés.\n"); else printf("Erreur chargement assets joueur.\n");
    return success;
}

// Identique
void init_player(Player *player) {
    player->pos = (SDL_Rect){100, LEVEL_HEIGHT - PLAYER_HEIGHT - 10, PLAYER_WIDTH, PLAYER_HEIGHT};
    player->vx = 0; player->vy = 0; player->on_ground = 1; player->direction = 1; // 1=droite, 0=gauche
    player->current_frame = 0; player->last_frame_time = 0;
    player->touche_limite_droite = 0; // Initialiser les flags de limite
    player->touche_limite_gauche = 0;
    for(int i=0; i<ANIMATION_FRAMES; ++i) { player->frames_left[i] = NULL; player->frames_right[i] = NULL; }
    printf("Joueur initialisé.\n");
}

// Identique
void init_obstacle(Obstacle *obstacle) {
    // Positionnez l'obstacle où vous voulez dans le niveau
    obstacle->pos = (SDL_Rect){800, LEVEL_HEIGHT - 70, 80, 180}; // Exemple
    printf("Obstacle initialisé.\n");
}

// NOUVEAU: Initialisation du bonus
void init_bonus(Bonus *b, int x, int y) {
    b->pos = (SDL_Rect){x, y, 0, 0}; // w et h seront définis par l'image chargée
    b->image = NULL;
    b->active = 1; // Le bonus est actif au début
    printf("Bonus initialisé (position %d, %d).\n", x, y);
}

// NOUVEAU: Chargement de l'image du bonus
int load_bonus_assets(Bonus *b, const char *image_path) {
    if (!b) return 0;
    printf("Chargement asset bonus: %s\n", image_path);
    b->image = IMG_Load(image_path);
    if (!b->image) {
        printf("Erreur chargement image bonus '%s': %s\n", image_path, IMG_GetError());
        return 0;
    }
    // Mettre à jour la taille du rectangle de collision du bonus
    b->pos.w = b->image->w;
    b->pos.h = b->image->h;
    printf("Asset bonus '%s' chargé (taille %dx%d).\n", image_path, b->pos.w, b->pos.h);
    return 1;
}


// Identique (fonction utilitaire)
int load_background(SDL_Surface **bg, const char* path, int width, int height) {
    printf("Chargement background: %s\n", path);
    *bg = IMG_Load(path);
    if (!*bg) {
        printf("Erreur chargement '%s': %s. Creation fond defaut.\n", path, IMG_GetError());
        *bg = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, BPP, 0,0,0,0);
        if (*bg) { SDL_FillRect(*bg, NULL, SDL_MapRGB((*bg)->format, 100, 100, 200)); return 1; }
        else return 0;
    }
    printf("Background '%s' chargé.\n", path);
    return 1;
}

// Identique
void switch_level(int level, int *current_level_ptr, Minimap *m, SDL_Surface *mbg1, SDL_Surface *mbg2) {
    if (level == *current_level_ptr) return;

    printf("Changement vers Niveau %d\n", level);
    *current_level_ptr = level;

    if (level == 1 && mbg1) {
        m->background = mbg1;
    } else if (level == 2 && mbg2) {
        m->background = mbg2;
    } else {
        printf("Avertissement: Fond minimap pour niveau %d non disponible.\n", level);
        if(level == 1 && !mbg1 && mbg2) m->background = mbg2; // Fallback
        else if(level == 2 && !mbg2 && mbg1) m->background = mbg1; // Fallback
        // Si les deux manquent, on garde l'ancien fond
    }
    // Optionnel: Réinitialiser position joueur, bonus, etc. ici si nécessaire
}

// Identique (gestion clavier/souris)
void handle_input(Player *player, int *game_running, int *current_level, Minimap *m, SDL_Surface *mbg1, SDL_Surface *mbg2) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                *game_running = 0;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE: *game_running = 0; break;
                    case SDLK_UP: // Saut
                        if (player->on_ground) { player->vy = JUMP_POWER; player->on_ground = 0; }
                        break;
                    case SDLK_1: // Changer au niveau 1
                        switch_level(1, current_level, m, mbg1, mbg2);
                        break;
                    case SDLK_2: // Changer au niveau 2
                         switch_level(2, current_level, m, mbg1, mbg2);
                        break;
                    // Ajouter d'autres touches si nécessaire
                    default: break;
                }
                break;
            // Ajouter gestion souris si besoin (ex: clic sur minimap)
        }
    }
    // Mouvement continu (gauche/droite)
    Uint8 *ks = SDL_GetKeyState(NULL);
    player->vx = 0; // Réinitialiser la vitesse horizontale à chaque frame
    if (ks[SDLK_LEFT]) {
        player->vx = -PLAYER_SPEED;
        player->direction = 0; // Gauche
    }
    if (ks[SDLK_RIGHT]) {
        player->vx = PLAYER_SPEED;
        player->direction = 1; // Droite
    }
}

// Modifié: Ajout de la vérification de collision avec le bonus
void update_player(Player *player, const Obstacle *obstacle, Bonus *bonus, float dt, Mix_Chunk *limite_sound) {
    Uint32 current_time = SDL_GetTicks();

    // Appliquer la gravité si le joueur n'est pas au sol
    if (!player->on_ground) {
        player->vy += GRAVITY; // Augmenter la vitesse verticale (chute)
        // Limiter la vitesse de chute maximale (optionnel)
        // if (player->vy > MAX_FALL_SPEED) player->vy = MAX_FALL_SPEED;
    }

    // Calculer la position potentielle pour la frame suivante
    SDL_Rect next_pos = player->pos;
    next_pos.x += (int)(player->vx); // Déplacement horizontal basé sur la vitesse
    next_pos.y += (int)(player->vy); // Déplacement vertical basé sur la vitesse

    // --- Gestion des Collisions et Limites ---

    // 1. Limite inférieure (Sol)
    if (next_pos.y + player->pos.h >= LEVEL_HEIGHT) {
        next_pos.y = LEVEL_HEIGHT - player->pos.h; // Ajuster pour être exactement sur le sol
        player->vy = 0; // Arrêter le mouvement vertical
        player->on_ground = 1; // Le joueur est sur le sol
    } else {
        player->on_ground = 0; // Le joueur est en l'air
    }

    // 2. Collision avec l'Obstacle (Utilise collisionPP)
    // Vérifier collision en X et Y séparément pour un meilleur comportement
    SDL_Rect p_next_x = {next_pos.x, player->pos.y, player->pos.w, player->pos.h}; // Mouvement X seul
    SDL_Rect p_next_y = {player->pos.x, next_pos.y, player->pos.w, player->pos.h}; // Mouvement Y seul

    if (collisionPP(p_next_x, obstacle->pos)) { // Collision horizontale avec obstacle
        next_pos.x = player->pos.x; // Annuler le mouvement horizontal
        player->vx = 0;             // Arrêter la vitesse horizontale
    }

    if (collisionPP(p_next_y, obstacle->pos)) { // Collision verticale avec obstacle
        if (player->vy > 0) { // Collision par le dessous (atterrissage sur l'obstacle)
            next_pos.y = obstacle->pos.y - player->pos.h; // Placer le joueur au-dessus
            player->on_ground = 1; // Le joueur est sur l'obstacle (considéré comme sol)
        } else if (player->vy < 0) { // Collision par le dessus (tête contre l'obstacle)
            next_pos.y = obstacle->pos.y + obstacle->pos.h; // Placer le joueur en dessous
        }
        player->vy = 0; // Arrêter le mouvement vertical
    }

    // Appliquer la position calculée (après corrections de collision)
    player->pos = next_pos;

    // 3. Collision avec le Bonus (Utilise collisionBB)
    // Vérifier après que la position finale du joueur est déterminée pour cette frame
    if (bonus && bonus->active) { // Vérifier si le bonus existe et est actif
        if (collisionBB(player->pos, bonus)) {
            // La fonction collisionBB met bonus->active à 0 si collision
            printf("Bonus collecté!\n");
            // Ici, vous pouvez ajouter d'autres effets:
            // - Jouer un son de collecte: Mix_PlayChannel(-1, son_bonus, 0);
            // - Augmenter un score: score += 100;
            // - Afficher un message temporaire
        }
    }


    // 4. Limites horizontales du niveau (avec son)
    if (player->pos.x < 0) {
        player->pos.x = 0; // Bloquer à gauche
        if (!player->touche_limite_gauche && limite_sound) { // Jouer le son une seule fois
            Mix_PlayChannel(-1, limite_sound, 0);
            player->touche_limite_gauche = 1;
        }
    } else {
        player->touche_limite_gauche = 0; // Réinitialiser le flag si on s'éloigne
    }

    if (player->pos.x + player->pos.w > LEVEL_WIDTH) {
        player->pos.x = LEVEL_WIDTH - player->pos.w; // Bloquer à droite
        if (!player->touche_limite_droite && limite_sound) { // Jouer le son une seule fois
            Mix_PlayChannel(-1, limite_sound, 0);
            player->touche_limite_droite = 1;
        }
    } else {
        player->touche_limite_droite = 0; // Réinitialiser le flag si on s'éloigne
    }


    // --- Animation ---
    if (player->vx != 0 && player->on_ground) { // Animer seulement si on bouge au sol
        if (current_time > player->last_frame_time + ANIMATION_DELAY) {
            player->current_frame = (player->current_frame + 1) % ANIMATION_FRAMES;
            player->last_frame_time = current_time;
        }
    } else if (player->vx == 0 && player->on_ground) { // Si immobile au sol
        player->current_frame = 0; // Image par défaut
    }
    // Optionnel: Ajouter une frame de saut si vy != 0 et !on_ground
}


// Identique
void update_camera(SDL_Rect *camera, const Player *player) {
    // Centrer la caméra sur le joueur horizontalement
    camera->x = player->pos.x + player->pos.w / 2 - SCREEN_WIDTH / 2;

    // Bloquer la caméra aux bords du niveau
    if (camera->x < 0) {
        camera->x = 0;
    }
    if (camera->x > LEVEL_WIDTH - camera->w) {
        camera->x = LEVEL_WIDTH - camera->w;
    }

    // La caméra verticale est fixe dans cet exemple
    camera->y = 0; // ou ajustez si vous voulez un scrolling vertical
}

// Modifié: Ajout du rendu du bonus
void render_game(SDL_Surface *screen, SDL_Surface *background_level1, SDL_Surface *background_level2, int current_level, const Player *player, const Obstacle *obstacle, const Bonus *bonus, const Minimap *minimap, const SDL_Rect *camera) {
    // 1. Effacer l'écran (ou dessiner un fond de couleur si le background manque)
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0)); // Fond noir par défaut

    // 2. Choisir et dessiner le fond principal actif en fonction du niveau
    SDL_Surface* current_main_bg = NULL;
    if (current_level == 1) {
        current_main_bg = background_level1;
    } else if (current_level == 2) {
        current_main_bg = background_level2;
    } // Ajouter d'autres 'else if' pour plus de niveaux

    if (current_main_bg) {
        SDL_Rect bg_src = {camera->x, camera->y, camera->w, camera->h}; // Portion du fond à afficher
        SDL_Rect bg_dest = {0, 0, camera->w, camera->h}; // Où l'afficher sur l'écran
        SDL_BlitSurface(current_main_bg, &bg_src, screen, &bg_dest);
    } else {
        // Optionnel: Dessiner une couleur si aucun fond n'est dispo pour le niveau actuel
         SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 50, 50, 50)); // Gris foncé
         printf("Avertissement: Fond principal pour niveau %d non disponible.\n", current_level);
    }

    // 3. Dessiner l'obstacle
    SDL_Rect obs_ecran = obstacle->pos;
    obs_ecran.x -= camera->x; // Ajuster la position selon la caméra
    obs_ecran.y -= camera->y;
    // Vérifier si l'obstacle est visible à l'écran avant de le dessiner (optimisation)
    if (obs_ecran.x < SCREEN_WIDTH && obs_ecran.x + obs_ecran.w > 0 &&
        obs_ecran.y < SCREEN_HEIGHT && obs_ecran.y + obs_ecran.h > 0)
    {
        // Utiliser FillRect pour un obstacle simple, ou BlitSurface si vous avez une image
        SDL_FillRect(screen, &obs_ecran, SDL_MapRGB(screen->format, 150, 75, 0)); // Couleur marron pour l'obstacle
    }


    // 4. NOUVEAU: Dessiner le bonus (s'il est actif)
    if (bonus && bonus->active) {
        render_bonus(screen, bonus, camera); // Appel de la nouvelle fonction de rendu
    }


    // 5. Dessiner le joueur
    SDL_Rect player_ecran = player->pos;
    player_ecran.x -= camera->x; // Ajuster la position selon la caméra
    player_ecran.y -= camera->y;

    // Choisir la bonne frame d'animation
    SDL_Surface* frame_to_draw = NULL;
    if (player->direction == 1) { // Droite
        frame_to_draw = player->frames_right[player->current_frame];
    } else { // Gauche
        frame_to_draw = player->frames_left[player->current_frame];
    }
    // Fallback si la frame spécifique manque (utilise la première frame)
    if (!frame_to_draw) {
        frame_to_draw = (player->direction == 1) ? player->frames_right[0] : player->frames_left[0];
    }

    // Dessiner la frame si elle existe
    if (frame_to_draw) {
         // Vérifier si le joueur est visible à l'écran
         if (player_ecran.x < SCREEN_WIDTH && player_ecran.x + frame_to_draw->w > 0 &&
             player_ecran.y < SCREEN_HEIGHT && player_ecran.y + frame_to_draw->h > 0)
         {
            SDL_BlitSurface(frame_to_draw, NULL, screen, &player_ecran);
         }
    } else {
        // Fallback: Dessiner un rectangle bleu si aucune image n'est chargée
        player_ecran.w = player->pos.w; player_ecran.h = player->pos.h;
         if (player_ecran.x < SCREEN_WIDTH && player_ecran.x + player_ecran.w > 0 &&
             player_ecran.y < SCREEN_HEIGHT && player_ecran.y + player_ecran.h > 0)
         {
            SDL_FillRect(screen, &player_ecran, SDL_MapRGB(screen->format, 0, 0, 255));
         }
    }

    // 6. Dessiner la Minimap (par-dessus tout le reste)
    // Passe maintenant le bonus pour qu'il puisse être affiché sur la minimap
    afficher_minimap(minimap, screen, player->pos, obstacle->pos, bonus);

    // 7. Afficher les FPS ou autres infos de debug (optionnel)
    // ... code pour afficher du texte ...
}

// NOUVEAU: Fonction pour dessiner le bonus sur l'écran principal
void render_bonus(SDL_Surface *screen, const Bonus *bonus, const SDL_Rect *camera) {
    if (!bonus || !bonus->image || !bonus->active) return; // Ne rien faire si inactif ou image non chargée

    SDL_Rect bonus_ecran = bonus->pos;
    bonus_ecran.x -= camera->x; // Ajuster selon la caméra
    bonus_ecran.y -= camera->y;

    // Vérifier si le bonus est visible à l'écran
    if (bonus_ecran.x < SCREEN_WIDTH && bonus_ecran.x + bonus_ecran.w > 0 &&
        bonus_ecran.y < SCREEN_HEIGHT && bonus_ecran.y + bonus_ecran.h > 0)
    {
        SDL_BlitSurface(bonus->image, NULL, screen, &bonus_ecran);
    }
}


// Renommé: Anciennement check_collision
int collisionPP(SDL_Rect a, SDL_Rect b) {
    // Vérifie s'il n'y a PAS de collision
    if (a.y + a.h <= b.y || // A est au-dessus de B
        a.y >= b.y + b.h || // A est en dessous de B
        a.x + a.w <= b.x || // A est à gauche de B
        a.x >= b.x + b.w)   // A est à droite de B
    {
        return 0; // Pas de collision
    }
    // S'il y a chevauchement sur les deux axes, il y a collision
    return 1; // Collision détectée
}

// NOUVEAU: Fonction de collision spécifique pour le joueur et le bonus
int collisionBB(SDL_Rect player_rect, Bonus *bonus) {
    // 1. Vérifier si le bonus est valide et actif
    if (!bonus || !bonus->active) {
        return 0; // Pas de collision si le bonus n'existe pas ou est inactif
    }

    // 2. Utiliser la logique de collision standard (AABB)
    if (collisionPP(player_rect, bonus->pos)) {
        // 3. Collision détectée! Désactiver le bonus
        bonus->active = 0;
        return 1; // Retourner 1 pour indiquer qu'une collision a eu lieu
    }

    // 4. Pas de collision
    return 0;
}


// Modifié: Ajout de la libération du bonus
void cleanup(SDL_Surface *screen, SDL_Surface *background_level1, SDL_Surface *background_level2, SDL_Surface *minimap_bg_surf1, SDL_Surface *minimap_bg_surf2, Player *player, Bonus *bonus, Minimap *minimap) {
     printf("Nettoyage des ressources...\n");

     // Libérer assets joueur (logique complexe pour éviter double free)
     SDL_Surface* freed_pointers[ANIMATION_FRAMES * 2] = {NULL}; // Tableau pour suivre les pointeurs déjà libérés
     int freed_count = 0;
     for (int i = 0; i < ANIMATION_FRAMES; ++i) {
         // Libérer frame droite si elle existe et n'a pas déjà été libérée
         if (player->frames_right[i] != NULL) {
             int already_freed = 0;
             for (int j = 0; j < freed_count; ++j) {
                 if (player->frames_right[i] == freed_pointers[j]) {
                     already_freed = 1;
                     break;
                 }
             }
             if (!already_freed) {
                 SDL_FreeSurface(player->frames_right[i]);
                 if (freed_count < ANIMATION_FRAMES * 2) { // Éviter dépassement buffer
                    freed_pointers[freed_count++] = player->frames_right[i];
                 }
             }
             player->frames_right[i] = NULL; // Mettre le pointeur à NULL après libération
         }
         // Libérer frame gauche si elle existe et n'a pas déjà été libérée
         if (player->frames_left[i] != NULL) {
             int already_freed = 0;
             for (int j = 0; j < freed_count; ++j) {
                 if (player->frames_left[i] == freed_pointers[j]) {
                     already_freed = 1;
                     break;
                 }
             }
             if (!already_freed) {
                 SDL_FreeSurface(player->frames_left[i]);
                  if (freed_count < ANIMATION_FRAMES * 2) { // Éviter dépassement buffer
                    freed_pointers[freed_count++] = player->frames_left[i];
                 }
             }
             player->frames_left[i] = NULL; // Mettre le pointeur à NULL
         }
     }
    printf("Assets joueur libérés.\n");

     // Libérer les fonds principaux
     if (background_level1) { SDL_FreeSurface(background_level1); printf("Fond principal 1 libéré.\n"); }
     if (background_level2) { SDL_FreeSurface(background_level2); printf("Fond principal 2 libéré.\n"); }

     // Libérer les fonds de minimap
     if (minimap_bg_surf1) { SDL_FreeSurface(minimap_bg_surf1); printf("Fond minimap 1 libéré.\n"); }
     if (minimap_bg_surf2) { SDL_FreeSurface(minimap_bg_surf2); printf("Fond minimap 2 libéré.\n"); }

     // NOUVEAU: Libérer l'image du bonus
     if (bonus && bonus->image) {
         SDL_FreeSurface(bonus->image);
         bonus->image = NULL;
         printf("Image bonus libérée.\n");
     }

     // Libérer la minimap (libère seulement l'icône joueur maintenant)
     liberer_minimap(minimap);

     // Quitter SDL et ses sous-systèmes
     Mix_CloseAudio(); // Fermer SDL_mixer
     IMG_Quit();       // Fermer SDL_image
     SDL_Quit();       // Fermer SDL
     printf("SDL, SDL_image et SDL_mixer quittés. Nettoyage terminé.\n");
}
