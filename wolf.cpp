#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <climits>
#include <ctime>
#include <cstdlib>

#include <unistd.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

using namespace std;

const int SCREEN_W = 320;
const int SCREEN_H = 200;
const int SCREEN_W_CENTER = SCREEN_W / 2;
const int SCREEN_H_CENTER = SCREEN_H / 2;

const int MAP_W = 24;
const int MAP_H = 24;

const double BLOCK_SIZE = 64;

const double SPRITE_SIZE = 64;

const double FOV = 60;
const double STEP_ANGLE = FOV / SCREEN_W;
const double PLANE_DIST = ( double )SCREEN_W_CENTER / tan( ( FOV / 2 ) / 180 * 3.1415 );

class cObject {
public:
    string name;
    double x, y;
    ALLEGRO_BITMAP* bmp;
    
    cObject() {}
    
    cObject( string name_, double x_, double y_, ALLEGRO_BITMAP* bmp_ ) : name( name_ ), x( x_ ), y( y_ ), bmp( bmp_ ) {}
};

class cEnemy {
public:
    cObject* obj;
    enum state_t { STAY, SHOOT, DIE, DEAD } state;
    double currentFrame, animSpeed;
    bool shooted;
    
    cEnemy() {}

    cEnemy( cObject* obj_ ) : obj( obj_ ) {
        state = STAY;
        currentFrame = 0;
        animSpeed = 0.1;
        shooted = false;
    }
};

void RotatePoint( double angle, double x0, double y0, double* x1, double* y1 ) {
    *x1 = x0 * cos( angle / 180 * 3.1415 ) - y0 * sin( angle / 180 * 3.1415 );
    *y1 = x0 * sin( angle / 180 * 3.1415 ) + y0 * cos( angle / 180 * 3.1415 );
}

double Absf( double x ) {
    return x >= 0 ? x : -x;
}

void TextureRectangle3D( int x0, int y0, int x1, int y1, int xt0, int yt0, int xt1, int yt1, ALLEGRO_BITMAP* tex, double dist, double* zBuffer ) {
    int xstart = x0, xend = x1;
    
    if ( xstart < 0 ) { xstart = 0; }
    if ( xend >= SCREEN_W ) { xend = SCREEN_W - 1; }
    
    for ( int x = xstart; x <= xend; x++ ) {
        if ( zBuffer[ x ] < dist ) { continue; }
        
        double t = ( double )( x - x0 ) / ( double )( x1 - x0 );
        int texX = xt0 + ( xt1 - xt0 ) * t;
        al_draw_scaled_bitmap( tex, texX, 0, 1, al_get_bitmap_height( tex ), x, y0, 1, y1 - y0, 0 );
    }
}

void DrawObjects( double playerX, double playerY, double playerDir, double heightAboveFloor, double horisont, vector< cObject > objects, double* zBuffer ) {
    vector< double > xs( objects.size() ), ys( objects.size() );
    vector< double > perpDists( objects.size() );
    
    for ( int i = 0; i < objects.size(); i++ ) {
        xs[ i ] = objects[ i ].x - playerX;
        ys[ i ] = objects[ i ].y - playerY;
        RotatePoint( -playerDir, xs[ i ], ys[ i ], &xs[ i ], &ys[ i ] );
        perpDists[ i ] = xs[ i ];
    }
    
    for ( int n = objects.size(); n > 1; n-- ) {
        double perpDist = perpDists[ n - 1 ];
        
        for ( int i = 0; i < n; i++ ) {
            double currentPerpDist = perpDists[ i ];
            
            if ( perpDist > currentPerpDist ) {
                swap( perpDist, currentPerpDist );
                swap( xs[ n - 1 ], xs[ i ] );
                swap( ys[ n - 1 ], ys[ i ] );
                swap( perpDists[ n - 1 ], perpDists[ i ] );
                swap( objects[ n - 1 ], objects[ i ] );
            }
        }
    }
    
    for ( int i = 0; i < objects.size(); i++ ) {
        if ( xs[ i ] <= 0 ) { continue; }
    
        double scale = 1 / perpDists[ i ] * PLANE_DIST;
        double xProjected = ys[ i ] * scale;
        double length = SPRITE_SIZE * scale;
    
        double screenX = ( xProjected + SCREEN_W_CENTER );
        
        double startX = screenX - length / 2;
        double startY = horisont - length / heightAboveFloor;
        
        TextureRectangle3D( startX, startY,
                            startX + length, startY + length,
                            0, 0, 
                            SPRITE_SIZE - 1, SPRITE_SIZE - 1,
                            objects[ i ].bmp,
                            perpDists[ i ],
                            zBuffer );
    }
}

bool PlayerIsVisible( double playerX, double playerY, double heightAboveFloor, 
                       double enemyX, double enemyY, 
                       int worldMap[ MAP_W ][ MAP_H ], const vector< cObject >& objects ) {
    bool ret = true;
            
    double x0 = playerX, y0 = playerY;
    double x1 = enemyX, y1 = enemyY;
    bool flag = false;
            
    if ( Absf( y0 - y1 ) < Absf( x0 - x1 ) ) {
        flag = true;
        swap( x0, y0 );
        swap( x1, y1 );
    }
    if ( y0 > y1 ) { swap( x0, x1 ); swap( y0, y1 ); }
            
    for ( double y = y0; y < y1; y += 1 ) {
        double t = ( y - y0 ) / ( y1 - y0 );
        double x = x0 + ( x1 - x0 ) * t;
        int mapX, mapY;
        
        if ( flag ) {
            mapX = ( int )( y / BLOCK_SIZE );
            mapY = ( int )( x / BLOCK_SIZE );
        } else {
            mapX = ( int )( x / BLOCK_SIZE );
            mapY = ( int )( y / BLOCK_SIZE );
        }
                
        if ( worldMap[ mapX ][ mapY ] != 0 ) {
            ret = false; goto end;
        }
        for ( int i = 0; i < objects.size(); i++ ) {
            if ( objects[ i ].name == "barrel" && 
                     ( int )( objects[ i ].x / BLOCK_SIZE ) == mapX && 
                     ( int )( objects[ i ].y / BLOCK_SIZE ) == mapY && heightAboveFloor == 1.3 ) { 
                ret = false; goto end;
            }
        }
    }
    
end: 
    return ret;
}

int main( int argc, char** argv ) {
    srand( time( nullptr ) );
    
    al_init();
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_keyboard();
    ALLEGRO_DISPLAY* display = al_create_display( SCREEN_W, SCREEN_H );
    ALLEGRO_EVENT_QUEUE* eventQueue = al_create_event_queue();
    al_register_event_source( eventQueue, al_get_display_event_source( display ) );
    al_register_event_source( eventQueue, al_get_keyboard_event_source() );
    ALLEGRO_KEYBOARD_STATE keyboard;
    
    ALLEGRO_BITMAP* weapons = al_load_bitmap( "weapons/wolf3d_weapons.png" );
    al_convert_mask_to_alpha( weapons, al_map_rgb( 152, 0, 136 ) );
    
    vector< ALLEGRO_BITMAP* > pistoleAnimation( 5, nullptr );
    pistoleAnimation[ 0 ] = al_create_sub_bitmap( weapons, 65 * 0, 65, 64, 64 );
    pistoleAnimation[ 1 ] = al_create_sub_bitmap( weapons, 65 * 1, 65, 64, 64 );
    pistoleAnimation[ 2 ] = al_create_sub_bitmap( weapons, 65 * 2, 65, 64, 64 );
    pistoleAnimation[ 3 ] = al_create_sub_bitmap( weapons, 65 * 3, 65, 64, 64 );
    pistoleAnimation[ 4 ] = al_create_sub_bitmap( weapons, 65 * 4, 65, 64, 64 );
    
    vector< ALLEGRO_BITMAP* > textures( 5, nullptr );
    textures[ 0 ] = al_load_bitmap( "textures/texture.png" );
    textures[ 1 ] = al_load_bitmap( "textures/NZ01.PNG" );
    textures[ 2 ] = al_load_bitmap( "textures/NZ0C.PNG" );
    textures[ 3 ] = al_load_bitmap( "textures/NZ1A.PNG" );
    textures[ 4 ] = al_load_bitmap( "textures/NZ2E.PNG" );
    
    int worldMap[ MAP_W ][ MAP_H ] = {
     //        0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 2 2 2 2
     //    y   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 
  //x
  /*00*/      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  /*01*/      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /*02*/      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /*03*/      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /*04*/      {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  /*05*/      {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /*06*/      {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  /*07*/      {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /*08*/      {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  /*09*/      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /*10*/      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /*11*/      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /*12*/      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /*13*/      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /*14*/      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /*15*/      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /*16*/      {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /*17*/      {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /*18*/      {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /*19*/      {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /*20*/      {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /*21*/      {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /*22*/      {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  /*23*/      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };
    
    ALLEGRO_BITMAP* barrel = al_load_bitmap( "sprites/barrel.bmp" );
    al_convert_mask_to_alpha( barrel, al_map_rgb( 0, 255, 255 ) );
    
    ALLEGRO_BITMAP* enemy = al_load_bitmap( "sprites/enemy.png" );
    al_convert_mask_to_alpha( enemy, al_map_rgb( 152, 0, 136 ) );
    
    ALLEGRO_BITMAP* enemyStay = al_create_sub_bitmap( enemy, 0, 0, 64, 64 );
    vector< ALLEGRO_BITMAP* > enemyDie( 5, nullptr );
    enemyDie[ 0 ] = al_create_sub_bitmap( enemy, 65 * 0, 65 * 5, 64, 64 );
    enemyDie[ 1 ] = al_create_sub_bitmap( enemy, 65 * 1, 65 * 5, 64, 64 );
    enemyDie[ 2 ] = al_create_sub_bitmap( enemy, 65 * 2, 65 * 5, 64, 64 );
    enemyDie[ 3 ] = al_create_sub_bitmap( enemy, 65 * 3, 65 * 5, 64, 64 );
    enemyDie[ 4 ] = al_create_sub_bitmap( enemy, 65 * 4, 65 * 5, 64, 64 );
    ALLEGRO_BITMAP* enemyDead = al_create_sub_bitmap( enemy, 65 * 4, 65 * 5, 64, 64 );
    vector< ALLEGRO_BITMAP* > enemyShoot( 3, nullptr );
    enemyShoot[ 0 ] = al_create_sub_bitmap( enemy, 65 * 0, 65 * 6, 64, 64 );
    enemyShoot[ 1 ] = al_create_sub_bitmap( enemy, 65 * 1, 65 * 6, 64, 64 );
    enemyShoot[ 2 ] = al_create_sub_bitmap( enemy, 65 * 2, 65 * 6, 64, 64 );
    
    vector< cObject > objects( 11 );
    objects[ 0 ] = cObject( "barrel", 2 * BLOCK_SIZE + BLOCK_SIZE / 2, 2 * BLOCK_SIZE + BLOCK_SIZE / 2, barrel );
    objects[ 1 ] = cObject( "enemy", 6 * BLOCK_SIZE + BLOCK_SIZE / 2, 8 * BLOCK_SIZE + BLOCK_SIZE / 2, enemyStay );
    objects[ 2 ] = cObject( "enemy", 21 * BLOCK_SIZE + BLOCK_SIZE / 2, 6 * BLOCK_SIZE + BLOCK_SIZE / 2, enemyStay );
    objects[ 3 ] = cObject( "enemy", 6 * BLOCK_SIZE + BLOCK_SIZE / 2, 17 * BLOCK_SIZE + BLOCK_SIZE / 2, enemyStay );
    objects[ 4 ] = cObject( "enemy", 7 * BLOCK_SIZE + BLOCK_SIZE / 2, 17 * BLOCK_SIZE + BLOCK_SIZE / 2, enemyStay );
    objects[ 5 ] = cObject( "enemy", 17 * BLOCK_SIZE + BLOCK_SIZE / 2, 2 * BLOCK_SIZE + BLOCK_SIZE / 2, enemyStay );
    objects[ 6 ] = cObject( "enemy", 19 * BLOCK_SIZE + BLOCK_SIZE / 2, 9 * BLOCK_SIZE + BLOCK_SIZE / 2, enemyStay );
    objects[ 7 ] = cObject( "enemy", 19 * BLOCK_SIZE + BLOCK_SIZE / 2, 4 * BLOCK_SIZE + BLOCK_SIZE / 2, enemyStay );
    objects[ 8 ] = cObject( "enemy", 9 * BLOCK_SIZE + BLOCK_SIZE / 2, 7 * BLOCK_SIZE + BLOCK_SIZE / 2, enemyStay );
    objects[ 9 ] = cObject( "barrel", 9 * BLOCK_SIZE + BLOCK_SIZE / 2, 5 * BLOCK_SIZE + BLOCK_SIZE / 2, barrel );
    objects[ 10 ] = cObject( "barrel", 10 * BLOCK_SIZE + BLOCK_SIZE / 2, 9 * BLOCK_SIZE + BLOCK_SIZE / 2, barrel );
    
    vector< cEnemy > enemies;
    
    for ( int i = 0; i < 9; i++ ) {
        if ( objects[ i ].name == "enemy" ) {
            enemies.push_back( cEnemy( &objects[ i ] ) );
        }
    }
    
    double playerX = BLOCK_SIZE + BLOCK_SIZE / 2;
    double playerY = BLOCK_SIZE + BLOCK_SIZE / 2;
    double playerXVel = 0, playerYVel = 0;
    double playerDir = 45;
    bool shoot = false, shooted = false, life = true;
    double currentFrame = 0, animSpeed = 0.2;
    
    double heightAboveFloor = 2, horisont = SCREEN_H_CENTER;
    
    double zBuffer[ SCREEN_W ];
    
    bool run = true;
    
    while ( run ) {
        ALLEGRO_EVENT event;
        
        while ( al_get_next_event( eventQueue, &event ) ) {
            if ( event.type == ALLEGRO_EVENT_DISPLAY_CLOSE ) {
                run = false;
            } else if ( event.type == ALLEGRO_EVENT_KEY_DOWN ) {
                if ( event.keyboard.keycode == ALLEGRO_KEY_LCTRL && !shoot ) {
                    shoot = true;
                    shooted = false;
                }
            }
        }
        
        al_get_keyboard_state( &keyboard );
        
        playerXVel *= 0.95;
        playerYVel *= 0.95;
        heightAboveFloor = 2;
        
        if ( al_key_down( &keyboard, ALLEGRO_KEY_UP ) ) {
            playerXVel += cos( playerDir / 180 * 3.1415 ) * 0.2;
            playerYVel += sin( playerDir / 180 * 3.1415 ) * 0.2;
        }
        if ( al_key_down( &keyboard, ALLEGRO_KEY_DOWN ) ) {
            playerXVel += -cos( playerDir / 180 * 3.1415 ) * 0.2;
            playerYVel += -sin( playerDir / 180 * 3.1415 ) * 0.2;
        }
        if ( al_key_down( &keyboard, ALLEGRO_KEY_LEFT ) && al_key_down( &keyboard, ALLEGRO_KEY_ALT ) ) {
            playerXVel += cos( ( playerDir - 90 ) / 180 * 3.1415 ) * 0.2;
            playerYVel += sin( ( playerDir - 90 ) / 180 * 3.1415 ) * 0.2;
        }
        if ( al_key_down( &keyboard, ALLEGRO_KEY_RIGHT ) && al_key_down( &keyboard, ALLEGRO_KEY_ALT ) ) {
            playerXVel += cos( ( playerDir + 90 ) / 180 * 3.1415 ) * 0.2;
            playerYVel += sin( ( playerDir + 90 ) / 180 * 3.1415 ) * 0.2;
        }
        if ( al_key_down( &keyboard, ALLEGRO_KEY_LEFT ) && !al_key_down( &keyboard, ALLEGRO_KEY_ALT ) ) {
            playerDir -= 2;
        }
        if ( al_key_down( &keyboard, ALLEGRO_KEY_RIGHT ) && !al_key_down( &keyboard, ALLEGRO_KEY_ALT ) ) {
            playerDir += 2;
        }
        if ( al_key_down( &keyboard, ALLEGRO_KEY_W ) ) {
            horisont += 2;
        }
        if ( al_key_down( &keyboard, ALLEGRO_KEY_S ) ) {
            horisont -= 2;
        }
        if ( al_key_down( &keyboard, ALLEGRO_KEY_C ) ) {
            heightAboveFloor = 1.3;
        }
        
        playerX += playerXVel;
        
        if ( worldMap[ ( int )( playerX / BLOCK_SIZE ) ][ ( int )( playerY / BLOCK_SIZE ) ] != 0 ) { playerX -= playerXVel; }
        else {
            for ( int i = 0; i < objects.size(); i++ ) {
                if ( objects[ i ].name == "barrel" ) {
                    int mapX = ( int )( objects[ i ].x / BLOCK_SIZE  );
                    int mapY = ( int )( objects[ i ].y / BLOCK_SIZE  );
                    
                    if ( ( int )( playerX / BLOCK_SIZE ) == mapX && ( int )( playerY / BLOCK_SIZE ) == mapY ) {
                        playerX -= playerXVel;
                        break;
                    }
                }
            }
        }
        
        playerY += playerYVel;
        
        if ( worldMap[ ( int )( playerX / BLOCK_SIZE ) ][ ( int )( playerY / BLOCK_SIZE ) ] != 0 ) { playerY -= playerYVel; }
        else {
            for ( int i = 0; i < objects.size(); i++ ) {
                if ( objects[ i ].name == "barrel" ) {
                    int mapX = ( int )( objects[ i ].x / BLOCK_SIZE  );
                    int mapY = ( int )( objects[ i ].y / BLOCK_SIZE  );
                    
                    if ( ( int )( playerX / BLOCK_SIZE ) == mapX && ( int )( playerY / BLOCK_SIZE ) == mapY ) {
                        playerY -= playerYVel;
                        break;
                    }
                }
            }
        }
        
        
        for ( int i = 0; i < enemies.size(); i++ ) {
            if ( enemies[ i ].state == cEnemy::STAY ) { enemies[ i ].obj->bmp = enemyStay; }
            else if ( enemies[ i ].state == cEnemy::DEAD ) { enemies[ i ].obj->bmp = enemyDead; }
            else if ( enemies[ i ].state == cEnemy::DIE ) {
                cEnemy& e = enemies[ i ];
                e.currentFrame += e.animSpeed;
                
                if ( ( int )e.currentFrame > 4 ) { e.state = cEnemy::DEAD; e.currentFrame = 0; e.obj->bmp = enemyDead; }
                else { e.obj->bmp = enemyDie[ ( int )e.currentFrame ]; }
            } else if ( enemies[ i ].state == cEnemy::SHOOT ) {
                cEnemy& e = enemies[ i ];
                e.currentFrame += e.animSpeed;
                
                if ( ( int )e.currentFrame > 2 ) { e.state = cEnemy::STAY; e.currentFrame = 0; e.obj->bmp = enemyStay; }
                else { e.obj->bmp = enemyShoot[ ( int )e.currentFrame ]; }
            
                if ( ( int )e.currentFrame == 2 && !e.shooted ) {
                    e.shooted = true;
                    
                    bool playerIsVisible = PlayerIsVisible( playerX, playerY, heightAboveFloor, e.obj->x, e.obj->y, worldMap, objects );
                    
                    if ( playerIsVisible ) { 
                        double dist = sqrt( ( playerX - e.obj->x ) * ( playerX - e.obj->x ) + ( playerY - e.obj->y ) * ( playerY - e.obj->y ) );
                    
                        if ( dist < BLOCK_SIZE * 2 ) {
                            life = false;
                        } else if ( rand() % ( int )( dist / 120 ) == 0 ) {
                            life = false;
                        }
                    }
                } 
            }
        }
        
        for ( int i = 0; i < enemies.size(); i++ ) {
            if ( enemies[ i ].state == cEnemy::DIE || enemies[ i ].state == cEnemy::DEAD ) {
                continue;
            }
        
            bool playerIsVisible = PlayerIsVisible( playerX, playerY, heightAboveFloor, enemies[ i ].obj->x, enemies[ i ].obj->y, worldMap, objects );
            
            if ( playerIsVisible && enemies[ i ].state != cEnemy::SHOOT ) {
                enemies[ i ].state = cEnemy::SHOOT;
                enemies[ i ].currentFrame = 0;
                enemies[ i ].shooted = false;
            }
        }
        
        
        for ( int i = 0; i < SCREEN_W; i++ ) { zBuffer[ i ] = 1000000000; }
        al_clear_to_color( al_map_rgb( 200, 200, 200 ) );
        
        
        if ( !life ) { al_clear_to_color( al_map_rgb( 255, 0, 0 ) ); al_flip_display(); usleep( 10000 ); continue; }
        
        
        /////////////////ray casting////////////////////////////
        
        double currAngle = playerDir - FOV / 2;
        
        for ( int x = 0; x < SCREEN_W; x++ ) {
            double dx = cos( currAngle / 180 * 3.1415 );
            double dy = sin( currAngle / 180 * 3.1415 );
            
            double ax, ay, bx, by;
            double xa, ya;
            double distH, distV;
            int hMapX, hMapY, vMapX, vMapY;
            
            ////////////////////////horisontal intersection/////////////////////////
    
            if ( dy != 0 ) {
                if ( dy < 0 ) { ay = ( int )( playerY / BLOCK_SIZE ) * BLOCK_SIZE; }
                if ( dy > 0 ) { ay = ( int )( playerY / BLOCK_SIZE ) * BLOCK_SIZE + BLOCK_SIZE; }
    
                double dist = Absf( ay - playerY ) / Absf( dy );
                ax = playerX + dist * dx;
    
                ya = dy > 0 ? BLOCK_SIZE : -BLOCK_SIZE;
                dist = BLOCK_SIZE / Absf( dy );
                xa = dist * dx;
    
                while ( true ) {
                    int mapX = ( int )( ax / BLOCK_SIZE );
                    int mapY = ( int )( ay / BLOCK_SIZE );
      
                    if ( dy < 0 ) { mapY--; }
                    
                    if ( mapX < 0 || mapX >= MAP_W || mapY < 0 || mapY >= MAP_H ) {
                        distH = -1;
                        break;
                    } else if ( worldMap[ mapX ][ mapY ] != 0 ) {
                        distH = sqrt( ( ax - playerX ) * ( ax - playerX ) + ( ay - playerY ) * ( ay - playerY ) );
                        hMapX = mapX;
                        hMapY = mapY;
                        break;
                    }
      
                    ax += xa;
                    ay += ya;
                }
            } else {
                distH = -1;
            }
    
            ////////////////////////vertical intersection/////////////////////////
    
            if ( dx != 0 ) {
                if ( dx < 0 ) { bx = ( int )( playerX / BLOCK_SIZE ) * BLOCK_SIZE; }
                if ( dx > 0 ) { bx = ( int )( playerX / BLOCK_SIZE ) * BLOCK_SIZE + BLOCK_SIZE; }
    
                double dist = Absf( bx - playerX ) / Absf( dx );
                by = playerY + dist * dy;
    
                xa = dx > 0 ? BLOCK_SIZE : -BLOCK_SIZE;
                dist = BLOCK_SIZE / Absf( dx );
                ya = dist * dy;
    
                while ( true ) {
                    int mapX = ( int )( bx / BLOCK_SIZE );
                    int mapY = ( int )( by / BLOCK_SIZE );
      
                    if ( dx < 0 ) { mapX--; }
      
                    if ( mapX < 0 || mapX >= MAP_W || mapY < 0 || mapY >= MAP_H ) {
                        distV = -1;
                        break;
                    } else if ( worldMap[ mapX ][ mapY ] != 0 ) {
                        distV = sqrt( ( bx - playerX ) * ( bx - playerX ) + ( by - playerY ) * ( by - playerY ) );
                        vMapX = mapX;
                        vMapY = mapY;
                        break;
                    }  
      
                    bx += xa;
                    by += ya;
                }
            } else {
                distV = -1;
            }
            
            /////////////////////////////////////////////////////////
            
            double ix, iy, idist;
            int iMapX, iMapY;
            bool isH;
    
            if ( distH != -1 && distV != -1 ) {
                if ( distH < distV ) {
                    ix = ax;
                    iy = ay;
                    idist = distH;
                    iMapX = hMapX;
                    iMapY = hMapY;
                    isH = true;
                } else if ( distV < distH ) {
                    ix = bx;
                    iy = by;
                    iMapX = vMapX;
                    iMapY = vMapY;
                    idist = distV;
                    isH = false;
                }
            } else if ( distH == -1 ) {
                ix = bx;
                iy = by;
                idist = distV;
                iMapX = vMapX;
                iMapY = vMapY;
                isH = false;
            } else if ( distV == -1 ) {
                ix = ax;
                iy = ay;
                idist = distH;
                iMapX = hMapX;
                iMapY = hMapY;
                isH = true;
            }
            
            double projectionDist = idist * cos( Absf( playerDir - currAngle ) / 180 * 3.1415 );
            
            double lineLength = BLOCK_SIZE / projectionDist * PLANE_DIST;
            double drawStart = horisont - lineLength / heightAboveFloor;
            
            int blockIdent = worldMap[ iMapX ][ iMapY ];
            
            ALLEGRO_BITMAP* tex = textures[ blockIdent - 1 ];
            ALLEGRO_COLOR tint = al_map_rgb_f( 1, 1, 1 );
            
            if ( isH ) { tint = al_map_rgb_f( 0.5, 0.5, 0.5 ); }
            
            int columnNumber;
    
            if (isH) {
                columnNumber = ix - ( ( int )( ix / BLOCK_SIZE ) * BLOCK_SIZE );
            } else {
                columnNumber = iy - ( ( int )( iy / BLOCK_SIZE ) * BLOCK_SIZE );
            }
            
            int texX = columnNumber;
            al_draw_tinted_scaled_bitmap( tex, tint, texX, 0, 1, BLOCK_SIZE, x, drawStart, 1, lineLength, 0 );
            zBuffer[ x ] = projectionDist;
            
            currAngle += STEP_ANGLE;
        }
        
        /////////////////////////////////////////////////////////
        DrawObjects( playerX, playerY, playerDir, heightAboveFloor, horisont, objects, zBuffer );
        
        if ( shoot == true ) {
            if ( ( int )currentFrame == 2 && !shooted ) {
                shooted = true;
                
                double xv = cos( playerDir / 180 * 3.1415 );
                double yv = sin( playerDir / 180 * 3.1415 );
                double xbullet = playerX, ybullet = playerY;
                
                while ( true ) {
                    int mapX = ( int )( xbullet / BLOCK_SIZE );
                    int mapY = ( int )( ybullet / BLOCK_SIZE );
                    
                    if ( worldMap[ mapX ][ mapY ] != 0 ) { break; }
                    for ( int i = 0; i < objects.size(); i++ ) {
                        if ( objects[ i ].name == "barrel" && 
                             ( int )( objects[ i ].x / BLOCK_SIZE ) == mapX && 
                             ( int )( objects[ i ].y / BLOCK_SIZE ) == mapY && heightAboveFloor == 1.3 ) { goto end; }
                    }
                    
                    for ( int i = 0; i < enemies.size(); i++ ) {
                        int eMapX = ( int )( enemies[ i ].obj->x / BLOCK_SIZE );
                        int eMapY = ( int )( enemies[ i ].obj->y / BLOCK_SIZE );
                        
                        if ( eMapX == mapX && eMapY == mapY && enemies[ i ].state != cEnemy::DIE && enemies[ i ].state != cEnemy::DEAD ) {
                            enemies[ i ].state = cEnemy::DIE;
                            enemies[ i ].currentFrame = 0;
                            goto end;
                        }
                    }
                    
                    xbullet += xv;
                    ybullet += yv;
                }
            end: 
                ;
            }
            
            currentFrame += animSpeed;
            
            if ( ( int )currentFrame > 4 ) { currentFrame = 0; shoot = false; }
        } else {
            currentFrame = 0;
        }
        
        al_draw_scaled_bitmap( pistoleAnimation[ ( int )currentFrame ], 0, 0, 64, 64, SCREEN_W_CENTER - 75, SCREEN_H - 150, 150, 150, 0 );
        
        al_flip_display();
        usleep( 10000 );
    }


    for ( int i = 0; i < 5; i++ ) {
        al_destroy_bitmap( textures[ i ] );
        al_destroy_bitmap( pistoleAnimation[ i ] );
        al_destroy_bitmap( enemyDie[ i ] );
    }
    for ( int i = 0; i < 3; i++ ) {
        al_destroy_bitmap( enemyShoot[ i ] );
    }
    
    al_destroy_bitmap( weapons );
    al_destroy_bitmap( enemy );
    al_destroy_bitmap( enemyStay );
    al_destroy_bitmap( enemyDead );
    al_destroy_bitmap( barrel );
    
    al_unregister_event_source( eventQueue, al_get_keyboard_event_source() );
    al_unregister_event_source( eventQueue, al_get_display_event_source( display ) );
    al_destroy_event_queue( eventQueue );
    al_destroy_display( display );
    al_uninstall_keyboard();
    al_shutdown_image_addon();
    al_shutdown_primitives_addon();
    al_uninstall_system();
    return 0;
}
