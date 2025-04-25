#ifndef MINIMAP_H_INCLUDED
#define MINIMAP_H_INCLUDED

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>

// --- Constantes Globales ---
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define BPP 32
#define LEVEL_WIDTH 1780
#define LEVEL_HEIGHT 720
#define PLAYER_SPEED 5
#define GRAVITY 0.5
#define JUMP_POWER -10.0
#define PLAYER_WIDTH 30
#define PLAYER_HEIGHT 30
#define ANIMATION_FRAMES 3
#define ANIMATION_DELAY 100

// --- Constantes Minimap ---
#define MINIMAP_WIDTH 270
#define MINIMAP_HEIGHT 150
#define MINIMAP_POS_X 10
#define MINIMAP_POS_Y 10
#define OBSTACLE_MINIMAP_SIZE 10
#define BONUS_MINIMAP_SIZE 5 // Taille du point pour le bonus sur la minimap

// --- Structures ---
typedef struct {
    SDL_Rect pos;
    float vx, vy;
    int on_ground;
    int direction;
    int current_frame;
    Uint32 last_frame_time;
    SDL_Surface* frames_right[ANIMATION_FRAMES];
    SDL_Surface* frames_left[ANIMATION_FRAMES];
    int touche_limite_droite;
    int touche_limite_gauche;
} Player;

typedef struct {
    SDL_Rect pos;
    // Ajoutez d'autres propriétés si nécessaire (ex: type d'obstacle)
} Obstacle;

// Nouvelle structure pour le bonus
typedef struct {
    SDL_Rect pos;
    SDL_Surface *image; // Image du bonus
    int active;         // 1 si le bonus est actif/visible, 0 sinon
} Bonus;

typedef struct {
    SDL_Surface *background; // Pointeur vers le fond de minimap ACTUEL
    SDL_Surface *player_icon;
    SDL_Rect position_ecran;
    int level_width;
    int level_height;
    // Pas besoin d'ajouter le bonus ici, on le passera en paramètre à afficher_minimap
} Minimap;


// --- Prototypes des Fonctions ---

// Initialisation / Chargement
int init_sdl(SDL_Surface **screen);
int load_player_assets(Player *player);
void init_player(Player *player);
void init_obstacle(Obstacle *obstacle);
int load_background(SDL_Surface **bg, const char* path, int width, int height);
int init_minimap(Minimap *m, SDL_Surface *initial_minimap_bg, const char *player_icon_path, int level_w, int level_h);
// Fonctions pour le bonus
void init_bonus(Bonus *b, int x, int y); // Initialise la position et l'état actif
int load_bonus_assets(Bonus *b, const char *image_path); // Charge l'image du bonus

// Gestion des entrées
void handle_input(Player *player, int *game_running, int *current_level, Minimap *m, SDL_Surface *mbg1, SDL_Surface *mbg2);
void switch_level(int level, int *current_level_ptr, Minimap *m, SDL_Surface *mbg1, SDL_Surface *mbg2);

// Mise à jour Logique
// Modifié: Prend maintenant aussi un pointeur vers le bonus
void update_player(Player *player, const Obstacle *obstacle, Bonus *bonus, float dt, Mix_Chunk *limite_sound);
void update_camera(SDL_Rect *camera, const Player *player);
// void update_bonus(Bonus *b); // Si le bonus doit avoir un comportement (ex: animation)

// Rendu
// Modifié: Prend maintenant aussi un pointeur vers le bonus
void render_game(SDL_Surface *screen, SDL_Surface *background_level1, SDL_Surface *background_level2, int current_level, const Player *player, const Obstacle *obstacle, const Bonus *bonus, const Minimap *minimap, const SDL_Rect *camera);
// Modifié: Prend maintenant aussi un pointeur vers le bonus pour l'afficher
void afficher_minimap(const Minimap *m, SDL_Surface *screen, SDL_Rect player_pos_jeu, SDL_Rect obstacle_pos_jeu, const Bonus *bonus);
// Fonction pour dessiner le bonus sur l'écran principal
void render_bonus(SDL_Surface *screen, const Bonus *bonus, const SDL_Rect *camera);

// Utilitaires et Nettoyage
// Renommé: collisionPP pour joueur/obstacle
int collisionPP(SDL_Rect a, SDL_Rect b);
// Nouveau: collisionBB pour joueur/bonus
int collisionBB(SDL_Rect player_rect, Bonus *bonus); // Prend un pointeur pour modifier l'état actif
void liberer_minimap(Minimap *m);
// Modifié: Prend maintenant aussi un pointeur vers le bonus pour libérer son image
void cleanup(SDL_Surface *screen, SDL_Surface *background_level1, SDL_Surface *background_level2, SDL_Surface *minimap_bg_surf1, SDL_Surface *minimap_bg_surf2, Player *player, Bonus *bonus, Minimap *minimap);


#endif // MINIMAP_H_INCLUDED
