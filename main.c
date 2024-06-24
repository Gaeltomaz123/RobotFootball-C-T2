#include <math.h>
#include <chipmunk.h>
#include <SOIL.h>
#include <windows.h>

// Rotinas para acesso da OpenGL
#include "opengl.h"

// Funções para movimentação de objetos

void move_ball(cpBody* body, void* data);

// Time vermelho
void move_goalkeeper_red(cpBody* body, void* data);
void move_defender_red1(cpBody* body, void* data);
void move_defender_red2(cpBody* body, void* data);
void move_defender_red3(cpBody* body, void* data);
void move_striker_red1(cpBody* body, void* data);
void move_striker_red2(cpBody* body, void* data);

// Time azul
void move_goalkeeper_blue(cpBody* body, void* data);
void move_defender_blue1(cpBody* body, void* data);
void move_defender_blue2(cpBody* body, void* data);
void move_defender_blue3(cpBody* body, void* data);
void move_striker_blue1(cpBody* body, void* data);
void move_striker_blue2(cpBody* body, void* data);

// Detecta qual time esta com a bola
boolean hasball(cpBody * body);

// Detecta se um atacante esta perto de um defensor
boolean striker_detected(cpBody * body);

// Body vai para o lugar desejado
void movement(cpBody * body, cpBody * where, cpFloat force);

// Corpo vai para o gol
void gotoposition(cpBody * body, cpVect position);

// Corpo vai para atras da bola
void gotoball(cpBody* body);

// Retorna true se passar de um limite passado em position
boolean passed_x(cpFloat body_x, cpFloat x);
boolean passed_y(cpFloat body_y, cpFloat y);

// Reseta as posicoes, o algulo e a velocidade de todos os corpos
void reset(cpBody *body, cpVect initialPosition, cpFloat initialAngle);

// Reseta "a bola esta com"
void reset_ball_players();

// Prototipos
void initCM();
void freeCM();
void restartCM();
cpShape* newLine(cpVect inicio, cpVect fim, cpFloat fric, cpFloat elast);
cpBody* newCircle(cpVect pos, cpFloat radius, cpFloat mass, char* img, bodyMotionFunc func, cpFloat fric, cpFloat elast);

// Score do jogo
int score1 = 0;
int score2 = 0;

// Flag de controle: 1 se o jogo tiver acabado
int gameOver = 0;

// cpVect e' um vetor 2D e cpv() e' uma forma rapida de inicializar ele.
cpVect gravity;

// O ambiente
cpSpace* space;

// Paredes "invisíveis" do ambiente
cpShape* leftWall, *rightWall, *topWall, *bottomWall;

// A bola
cpBody* ballBody;

// Um robô
cpBody* robotBody;

// Time vermelho
cpBody* goalkeeper_red_Body, * defender_red_1_Body, * defender_red_2_Body, * defender_red_3_Body, * striker_red_1_Body, * striker_red_2_Body;
boolean gk_r, def_r1, def_r2, def_r3, st_r1, st_r2;

// Time azul
cpBody* goalkeeper_blue_Body, * defender_blue_1_Body, * defender_blue_2_Body, * defender_blue_3_Body, * striker_blue_1_Body, * striker_blue_2_Body;
boolean gk_b, def_b1, def_b2, def_b3, st_b1, st_b2;

int team_choice, blue_striker, red_striker;

// Cada passo de simulação é 1/60 seg.
cpFloat timeStep = 1.0/60.0;

// Inicializa o ambiente: é chamada por init() em opengl.c, pois necessita do contexto
// OpenGL para a leitura das imagens
void initCM()
{
    gravity = cpv(0, 100);

    // Cria o universo
    space = cpSpaceNew();

    // Seta o fator de damping, isto é, de atrito do ar
    cpSpaceSetDamping(space, 0.8);

    // Descomente a linha abaixo se quiser ver o efeito da gravidade!
    //cpSpaceSetGravity(space, gravity);

    // Adiciona 4 linhas estáticas para formarem as "paredes" do ambiente
    leftWall   = newLine(cpv(0,0), cpv(0,ALTURA_JAN), 0, 1.0);
    rightWall  = newLine(cpv(LARGURA_JAN,0), cpv(LARGURA_JAN,ALTURA_JAN), 0, 1.0);
    bottomWall = newLine(cpv(0,0), cpv(LARGURA_JAN,0), 0, 1.0);
    topWall    = newLine(cpv(0,ALTURA_JAN), cpv(LARGURA_JAN,ALTURA_JAN), 0, 1.0);

    // Agora criamos a bola...
    // Os parâmetros são:
    //   - posição: cpVect (vetor: x e y)
    //   - raio
    //   - massa
    //   - imagem a ser carregada
    //   - ponteiro para a função de movimentação (chamada a cada passo, pode ser NULL)
    //   - coeficiente de fricção
    //   - coeficiente de elasticidade
    ballBody = newCircle(cpv(512,350), 8, 1, "small_football.png", move_ball, 0.2, 1);

    // Time vermelho
    goalkeeper_red_Body = newCircle(cpv(60,350), 15, 5, "shipred.png", move_goalkeeper_red, 0.2, 0.5);
    defender_red_1_Body = newCircle(cpv(250,200), 15, 5, "shipred.png", move_defender_red1, 0.2, 0.5);
    defender_red_2_Body = newCircle(cpv(250,350), 15, 5, "shipred.png", move_defender_red2, 0.2, 0.5);
    defender_red_3_Body = newCircle(cpv(250,500), 15, 5, "shipred.png", move_defender_red3, 0.2, 0.5);
    striker_red_1_Body = newCircle(cpv(400,300), 15, 1, "shipred.png", move_striker_red1, 0.2, 0.5);
    striker_red_2_Body = newCircle(cpv(400,400), 15, 1, "shipred.png", move_striker_red2, 0.2, 0.5);
    
    // Time azul
    goalkeeper_blue_Body = newCircle(cpv(964,350), 15, 5, "shipblue.png", move_goalkeeper_blue, 0.2, 0.5);
    defender_blue_1_Body = newCircle(cpv(774,200), 15, 5, "shipblue.png", move_defender_blue1, 0.2, 0.5);
    defender_blue_2_Body = newCircle(cpv(774,350), 15, 5, "shipblue.png", move_defender_blue2, 0.2, 0.5);
    defender_blue_3_Body = newCircle(cpv(774,500), 15, 5, "shipblue.png", move_defender_blue3, 0.2, 0.5);
    striker_blue_1_Body = newCircle(cpv(624,300), 15, 1, "shipblue.png", move_striker_blue1, 0.2, 0.5);
    striker_blue_2_Body = newCircle(cpv(624,400), 15, 1, "shipblue.png", move_striker_blue2, 0.2, 0.5);

    reset_ball_players();
    restartCM();
}

void move_ball(cpBody* body, void* data) {
    cpVect pos;
    cpVect vel;
    cpFloat initialAngle = 0.0;
    // Limite time vermelho
    if (passed_x(42, cpBodyGetPosition(body).x)) {
        // Bola
        pos.x = 512;
        pos.y = 350;
        reset(ballBody, pos, initialAngle);
    }
    // Limite time azul
    if (passed_x(cpBodyGetPosition(body).x, 980)) {
        // Bola
        pos.x = 512;
        pos.y = 350;
        reset(ballBody, pos, initialAngle);
    }
    // Lateral esquerdo
    if (passed_y(43, cpBodyGetPosition(body).y)) {
        // Bola
        pos.x = 512;
        pos.y = 350;
        reset(ballBody, pos, initialAngle);
    }
    // Lateral direito
    if (passed_y(cpBodyGetPosition(body).y, 670)) {
        // Bola
        pos.x = 512;
        pos.y = 350;
        reset(ballBody, pos, initialAngle);
    }
}


// Time vermelho

void move_goalkeeper_red(cpBody* body, void* data) {
    int rand_position = (rand()%100)+300;
    cpVect position = cpv(60, rand_position);
    gotoposition(body, position);
    if(hasball(body)) {
        reset_ball_players();
        gk_r = TRUE;
    }
}

void move_defender_red1(cpBody* body, void* data) {
    int rand_y = (rand()%200)+100;
    int rand_x = (rand()%200)+180;
    cpVect position = cpv(rand_x, rand_y);
    gotoposition(body, position);

    if(hasball(body)) {
        reset_ball_players();
        def_r1 = TRUE;
    }

    if(striker_detected(body) && (st_b1 || st_b2)) {
        movement(body, ballBody, 1000);
    }
    if(def_r1) {
        movement(ballBody, body, 200);
        cpVect position = cpv(964, rand()%450+250);
        gotoposition(body, position);
        if(cpBodyGetPosition(body).x >= 380 ) {
            int striker = rand()%2+1;
            if(striker == 1) {
                movement(ballBody, striker_red_1_Body, 1000);
            } else {
                movement(ballBody, striker_red_2_Body, 1000);
            }
        }
    }
}

void move_defender_red2(cpBody* body, void* data) {
    int rand_y = (rand()%200)+250;
    int rand_x = (rand()%200)+180;
    cpVect position = cpv(rand_x, rand_y);
    gotoposition(body, position);

    if(hasball(body)) {
        reset_ball_players();
        def_r2 = TRUE;
    }

    if(striker_detected(body) && (st_b1 || st_b2)) {
        movement(body, ballBody, 1000);
    }
    if(def_r2) {
        movement(ballBody, body, 200);
        cpVect position = cpv(964, rand()%450+250);
        gotoposition(body, position);
        if(cpBodyGetPosition(body).x >= 380 ) {
            int striker = rand()%2+1;
            if(striker == 1) {
                movement(ballBody, striker_red_1_Body, 1000);
            } else {
                movement(ballBody, striker_red_2_Body, 1000);
            }
        }
    }
}

void move_defender_red3(cpBody* body, void* data) {
    int rand_y = (rand()%200)+400;
    int rand_x = (rand()%200)+180;
    cpVect position = cpv(rand_x, rand_y);
    gotoposition(body, position);

    if(hasball(body)) {
        reset_ball_players();
        def_r3 = TRUE;
    }

    if(striker_detected(body) && (st_b1 || st_b2)) {
        movement(body, ballBody, 1000);
    }
    if(def_r3) {
        movement(ballBody, body, 200);
        cpVect position = cpv(964, rand()%450+250);
        gotoposition(body, position);
        if(cpBodyGetPosition(body).x >= 380 ) {
            int striker = rand()%2+1;
            if(striker == 1) {
                movement(ballBody, striker_red_1_Body, 1000);
            } else {
                movement(ballBody, striker_red_2_Body, 1000);
            }
        }
    }
}

void move_striker_red1(cpBody* body, void* data) {
    if(hasball(body)) {
        reset_ball_players();
        st_r1 = TRUE;
    }
    // Passe inicial
    if(red_striker == 1 && team_choice == 1) {
        if(st_r1) {
            movement(ballBody, striker_red_2_Body, 500);
        }
    }
    if(team_choice == 1 && red_striker == 2) {
        if(st_r1) {
            movement(ballBody, body, 500);
            int rand_y = (rand()%270)+80;
            int rand_x = (rand()%400)+512;
            cpVect position = cpv(rand_x, rand_y);
            gotoposition(body, position);
        }
    } else {
        int rand_y = (rand()%270)+80;
        int rand_x = (rand()%400)+512;
        cpVect position = cpv(rand_x, rand_y);
        gotoposition(body, position);
    }
    if(cpBodyGetPosition(ballBody).x >= 512 && !st_r2 && !st_r1) {
        gotoball(body);
    }
}

void move_striker_red2(cpBody* body, void* data) {
    if(hasball(body)) {
        reset_ball_players();
        st_r2 = TRUE;
    }
    // Passe inicial
    if(red_striker == 2 && team_choice == 1) {
        if(st_r2) {
            movement(ballBody, striker_red_1_Body, 500);
        }
    } 
    if(team_choice == 1 && red_striker == 1) {
        if(st_r2) {
            movement(ballBody, body, 500);
            int rand_y = (rand()%270)+350;
            int rand_x = (rand()%400)+512;
            cpVect position = cpv(rand_x, rand_y);
            gotoposition(body, position);
        }
    } else {
        int rand_y = (rand()%270)+350;
        int rand_x = (rand()%400)+512;
        cpVect position = cpv(rand_x, rand_y);
        gotoposition(body, position);
    }
    if(cpBodyGetPosition(ballBody).x >= 512 & !st_r1 && !st_r2) {
        gotoball(body);
    }
}

// Time azul

void move_goalkeeper_blue(cpBody* body, void* data) {
    int rand_position = (rand()%100)+300;
    cpVect position = cpv(964, rand_position);
    gotoposition(body, position);
    if(hasball(body)) {
        reset_ball_players();
        gk_b = TRUE;
    }
}

void move_defender_blue1(cpBody* body, void* data) {
    int rand_y = (rand()%200)+100;
    int rand_x = (rand()%200)+644;
    cpVect position = cpv(rand_x, rand_y);
    gotoposition(body, position);

    if(hasball(body)) {
        reset_ball_players();
        def_b1 = TRUE;
    }

    if(striker_detected(body) && (st_r1 || st_r2)) {
        movement(body, ballBody, 1000);
    }
    if(def_b1) {
        movement(ballBody, body, 200);
        cpVect position = cpv(60, rand()%450+250);
        gotoposition(body, position);
        if(cpBodyGetPosition(body).x <= 644) {
            int striker = rand()%2+1;
            if(striker == 1) {
                movement(ballBody, striker_blue_1_Body, 1000);
            } else {
                movement(ballBody, striker_blue_2_Body, 1000);
            }
        }
    }
}

void move_defender_blue2(cpBody* body, void* data) {
    int rand_y = (rand()%200)+250;
    int rand_x = (rand()%200)+644;
    cpVect position = cpv(rand_x, rand_y);
    gotoposition(body, position);

    if(hasball(body)) {
        reset_ball_players();
        def_b2 = TRUE;
    }

    if(striker_detected(body) && (st_r1 || st_r2)) {
        movement(body, ballBody, 1000);
    }
    if(def_b2) {
        movement(ballBody, body, 200);
        cpVect position = cpv(60, rand()%450+250);
        gotoposition(body, position);;
        if(cpBodyGetPosition(body).x <= 644) {
            int striker = rand()%2+1;
            if(striker == 1) {
                movement(ballBody, striker_blue_1_Body, 1000);
            } else {
                movement(ballBody, striker_blue_2_Body, 1000);
            }
        }
    }
}

void move_defender_blue3(cpBody* body, void* data) {
    int rand_y = (rand()%200)+400;
    int rand_x = (rand()%200)+644;
    cpVect position = cpv(rand_x, rand_y);
    gotoposition(body, position);

    if(hasball(body)) {
        reset_ball_players();
        def_b3 = TRUE;
    }

    if(striker_detected(body) && (st_r1 || st_r2)) {
        movement(body, ballBody, 1000);
    }
    if(def_b3) {
        movement(ballBody, body, 200);
        cpVect position = cpv(60, rand()%450+250);
        gotoposition(body, position);
        if(cpBodyGetPosition(body).x <= 644) {
            int striker = rand()%2+1;
            if(striker == 1) {
                movement(ballBody, striker_blue_1_Body, 1000);
            } else {
                movement(ballBody, striker_blue_2_Body, 1000);
            }
        }
    }
}

void move_striker_blue1(cpBody* body, void* data) {
    if(hasball(body)) {
        reset_ball_players();
        st_b1 = TRUE;
    }
    // Passe inicial
    if(blue_striker == 1 && team_choice == 2) {
        if(st_b1) {
            movement(ballBody, striker_blue_2_Body, 500);
        }
    }
    if(team_choice == 2 && blue_striker == 2) {
        if(st_b1) {
            movement(ballBody, body, 500);
            int rand_y = (rand()%270)+80;
            int rand_x = (rand()%400)+112;
            cpVect position = cpv(rand_x, rand_y);
            gotoposition(body, position);
        }
    } else {
        int rand_y = (rand()%270)+80;
        int rand_x = (rand()%400)+112;
        cpVect position = cpv(rand_x, rand_y);
        gotoposition(body, position);
    }
    if(cpBodyGetPosition(ballBody).x <= 512 & !st_b2 && !st_b1) {
        gotoball(body);
    }
}

void move_striker_blue2(cpBody* body, void* data) {
    if(hasball(body)) {
        reset_ball_players();
        st_b2 = TRUE;
    }
    // Passe inicial
    if(blue_striker == 2 && team_choice == 2) {
        if(st_b2) {
            movement(ballBody, striker_blue_1_Body, 500);
        }
    }
    if(team_choice == 2 && blue_striker == 1) {
        if(st_b2) {
            movement(ballBody, body, 500);
            int rand_y = (rand()%270)+350;
            int rand_x = (rand()%400)+112;
            cpVect position = cpv(rand_x, rand_y);
            gotoposition(body, position);
        }
    } else {
        int rand_y = (rand()%270)+350;
        int rand_x = (rand()%400)+112;
        cpVect position = cpv(rand_x, rand_y);
        gotoposition(body, position);
    }
    if(cpBodyGetPosition(ballBody).x <= 512 & !st_b1 & !st_b2) {
        gotoball(body);
    }
}

void reset_ball_players() {
    // Bola esta com:
    gk_r = FALSE;
    def_r1 = FALSE;
    def_r2 = FALSE;
    def_r3 = FALSE; 
    st_r1 = FALSE;
    st_r2 = FALSE;
    gk_b = FALSE;
    def_b1 = FALSE;
    def_b2 = FALSE;
    def_b3 = FALSE; 
    st_b1 = FALSE;
    st_b2 = FALSE;
}

boolean hasball(cpBody * body) {
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos  = cpBodyGetPosition(ballBody);
    boolean x = robotPos.x <= ballPos.x && robotPos.x >= ballPos.x-25 || robotPos.x >= ballPos.x && robotPos.x <= ballPos.x+25;
    boolean y = robotPos.y <= ballPos.y && robotPos.y >= ballPos.y-25 || robotPos.y >= ballPos.y && robotPos.y <= ballPos.y+25;
    if(x && y) {
        return TRUE;
    }
    return FALSE;
}

boolean striker_detected(cpBody * body) {
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos  = cpBodyGetPosition(ballBody);
    boolean x = robotPos.x <= ballPos.x && robotPos.x >= ballPos.x-100 || robotPos.x >= ballPos.x && robotPos.x <= ballPos.x+100;
    boolean y = robotPos.y <= ballPos.y && robotPos.y >= ballPos.y-100 || robotPos.y >= ballPos.y && robotPos.y <= ballPos.y+100;
    if(x && y) {
        return TRUE;
    }
    return FALSE;
}

boolean passed_x(cpFloat body_x, cpFloat x) {
    if(body_x >= x) {
        return TRUE;
    }
    return FALSE;
}

boolean passed_y(cpFloat body_y, cpFloat y) {
    if(body_y >= y) {
        return TRUE;
    }
    return FALSE;
}

void movement(cpBody * body, cpBody * where, cpFloat force) {
    cpVect direction = cpvsub(cpBodyGetPosition(where), cpBodyGetPosition(body));
    direction = cpvnormalize(direction);
    cpVect impulse = cpvmult(direction, force);
    cpBodyApplyForceAtWorldPoint(body, impulse, cpBodyGetPosition(body));
}

void gotoposition(cpBody * body, cpVect position) {
        cpVect vel = cpBodyGetVelocity(body);

        vel = cpvclamp(vel, 50);
        cpBodySetVelocity(body, vel);

        cpVect robotPos = cpBodyGetPosition(body);

        cpVect pos = robotPos;
        pos.x = -robotPos.x;
        pos.y = -robotPos.y;
        cpVect delta = cpvadd(position, pos);

        int imp = rand()%40+20;
        delta = cpvmult(cpvnormalize(delta),imp);
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos); 
}

void gotoball(cpBody* body)
{
    cpVect vel = cpBodyGetVelocity(body);
    vel = cpvclamp(vel, 50);
    cpBodySetVelocity(body, vel);
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos  = cpBodyGetPosition(ballBody);
    cpVect pos = robotPos;
    pos.x = -robotPos.x;
    pos.y = -robotPos.y;
    cpVect delta = cpvadd(ballPos,pos);

    int imp = rand()%40+20;
    delta = cpvmult(cpvnormalize(delta),imp);
    cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
}




// Libera memória ocupada por cada corpo, forma e ambiente
// Acrescente mais linhas caso necessário

void freeCM()
{
    printf("Cleaning up!\n");
    UserData* ud = cpBodyGetUserData(ballBody);
    cpShapeFree(ud->shape);
    cpBodyFree(ballBody);

    // Time vermelho

    ud = cpBodyGetUserData(goalkeeper_red_Body);
    cpShapeFree(ud->shape);
    cpBodyFree(goalkeeper_red_Body);

    ud = cpBodyGetUserData(defender_red_1_Body);
    cpShapeFree(ud->shape);
    cpBodyFree(defender_red_1_Body);

    ud = cpBodyGetUserData(defender_red_2_Body);
    cpShapeFree(ud->shape);
    cpBodyFree(defender_red_2_Body);

    ud = cpBodyGetUserData(defender_red_3_Body);
    cpShapeFree(ud->shape);
    cpBodyFree(defender_red_3_Body);

    ud = cpBodyGetUserData(striker_red_1_Body);
    cpShapeFree(ud->shape);
    cpBodyFree(striker_red_1_Body);

    ud = cpBodyGetUserData(striker_red_2_Body);
    cpShapeFree(ud->shape);
    cpBodyFree(striker_red_2_Body);

    // Time azul

    ud = cpBodyGetUserData(goalkeeper_blue_Body);
    cpShapeFree(ud->shape);
    cpBodyFree(goalkeeper_blue_Body);

    ud = cpBodyGetUserData(defender_blue_1_Body);
    cpShapeFree(ud->shape);
    cpBodyFree(defender_blue_1_Body);

    ud = cpBodyGetUserData(defender_blue_2_Body);
    cpShapeFree(ud->shape);
    cpBodyFree(defender_blue_2_Body);

    ud = cpBodyGetUserData(defender_blue_3_Body);
    cpShapeFree(ud->shape);
    cpBodyFree(defender_blue_3_Body);

    ud = cpBodyGetUserData(striker_blue_1_Body);
    cpShapeFree(ud->shape);
    cpBodyFree(striker_blue_1_Body);

    ud = cpBodyGetUserData(striker_blue_2_Body);
    cpShapeFree(ud->shape);
    cpBodyFree(striker_blue_2_Body);

    cpShapeFree(leftWall);
    cpShapeFree(rightWall);
    cpShapeFree(bottomWall);
    cpShapeFree(topWall);

    cpSpaceFree(space);
}

// Função chamada para reiniciar a simulação
void restartCM()
{
    // Escreva o código para reposicionar os jogadores, ressetar o score, etc.
    team_choice = (rand()%2)+1;
    blue_striker = (rand()%2)+1;
    red_striker = (rand()%2)+1;
    cpVect pos;
    
    cpVect vel;
    cpFloat initialAngle = 0.0;
    
    // Bola
    pos.x = 512;
    pos.y = 350;
    reset(ballBody, pos, initialAngle);

    // Time vermelho
    pos.x = 60;
    pos.y = 350;
    reset(goalkeeper_red_Body, pos, initialAngle);

    pos.x = 250;
    pos.y = 200;
    reset(defender_red_1_Body, pos, initialAngle);

    pos.x = 250;
    pos.y = 350;
    reset(defender_red_2_Body, pos, initialAngle);

    pos.x = 250;
    pos.y = 500;
    reset(defender_red_3_Body, pos, initialAngle);

    pos.x = 400;
    pos.y = 300;
    reset(striker_red_1_Body, pos, initialAngle);

    pos.x = 400;
    pos.y = 400;
    reset(striker_red_2_Body, pos, initialAngle);
    

    // Time azul
    pos.x = 964;
    pos.y = 350;
    reset(goalkeeper_blue_Body, pos, initialAngle);

    pos.x = 774;
    pos.y = 200;
    reset(defender_blue_1_Body, pos, initialAngle);

    pos.x = 774;
    pos.y = 350;
    reset(defender_blue_2_Body, pos, initialAngle);

    pos.x = 774;
    pos.y = 500;
    reset(defender_blue_3_Body, pos, initialAngle);

    pos.x = 624;
    pos.y = 300;
    reset(striker_blue_1_Body, pos, initialAngle);

    pos.x = 624;
    pos.y = 400;
    reset(striker_blue_2_Body, pos, initialAngle);




    // Se a bola começar com o time vermelho
    if(team_choice == 1) {
        if(red_striker == 1) {
            st_r1 = TRUE;
            pos.x = 532;
            pos.y = 350;
            reset(striker_red_1_Body, pos, initialAngle);
        } else {
            st_r2 = TRUE;
            pos.x = 532;
            pos.y = 350;
            reset(striker_red_2_Body, pos, initialAngle);
        } 
    // Se a bola começar com o time azul
    } else {
        if(blue_striker == 1) {
            st_b1 = TRUE;
            pos.x = 492;
            pos.y = 350;
            reset(striker_blue_1_Body, pos, initialAngle);
        } else {
            st_b2 = TRUE;
            pos.x = 492;
            pos.y = 350;
            reset(striker_blue_2_Body, pos, initialAngle);
        } 
    }

    


    // Não esqueça de ressetar a variável gameOver!
    gameOver = 0;
}

void reset(cpBody *body, cpVect initialPosition, cpFloat initialAngle) {
    // Resetar a posição
    cpBodySetPosition(body, initialPosition);
    
    // Resetar o ângulo
    cpBodySetAngle(body, initialAngle);
    
    // Resetar a velocidade linear e angular para zero
    cpBodySetVelocity(body, cpvzero);
    cpBodySetAngularVelocity(body, 0.0);
}

// ************************************************************
//
// A PARTIR DESTE PONTO, O PROGRAMA NÃO DEVE SER ALTERADO
//
// A NÃO SER QUE VOCÊ SAIBA ***EXATAMENTE*** O QUE ESTÁ FAZENDO
//
// ************************************************************

int main(int argc, char** argv)
{
    // Inicialização da janela gráfica
    init(argc,argv);

    // Não retorna... a partir daqui, interação via teclado e mouse apenas, na janela gráfica
    glutMainLoop();
    return 0;
}

// Cria e adiciona uma nova linha estática (segmento) ao ambiente
cpShape* newLine(cpVect inicio, cpVect fim, cpFloat fric, cpFloat elast)
{
   cpShape* aux = cpSegmentShapeNew(cpSpaceGetStaticBody(space), inicio, fim, 0);
   cpShapeSetFriction(aux, fric);
   cpShapeSetElasticity(aux, elast);
   cpSpaceAddShape(space, aux);
   return aux;
}

// Cria e adiciona um novo corpo dinâmico, com formato circular
cpBody* newCircle(cpVect pos, cpFloat radius, cpFloat mass, char* img, bodyMotionFunc func, cpFloat fric, cpFloat elast)
{
    // Primeiro criamos um cpBody para armazenar as propriedades fisicas do objeto
    // Estas incluem: massa, posicao, velocidade, angulo, etc do objeto
    // A seguir, adicionamos formas de colisao ao cpBody para informar o seu formato e tamanho

    // O momento de inercia e' como a massa, mas para rotacao
    // Use as funcoes cpMomentFor*() para calcular a aproximacao dele
    cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);

    // As funcoes cpSpaceAdd*() retornam o que voce esta' adicionando
    // E' conveniente criar e adicionar um objeto na mesma linha
    cpBody* newBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));

    // Por fim, ajustamos a posicao inicial do objeto
    cpBodySetPosition(newBody, pos);

    // Agora criamos a forma de colisao do objeto
    // Voce pode criar multiplas formas de colisao, que apontam ao mesmo objeto (mas nao e' necessario para o trabalho)
    // Todas serao conectadas a ele, e se moverao juntamente com ele
    cpShape* newShape = cpSpaceAddShape(space, cpCircleShapeNew(newBody, radius, cpvzero));
    cpShapeSetFriction(newShape, fric);
    cpShapeSetElasticity(newShape, elast);

    UserData* newUserData = malloc(sizeof(UserData));
    newUserData->tex = loadImage(img);
    newUserData->radius = radius;
    newUserData->shape= newShape;
    newUserData->func = func;
    cpBodySetUserData(newBody, newUserData);
    printf("newCircle: loaded img %s\n", img);
    return newBody;
}
