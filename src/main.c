//////////////////////////////////////////////////////////////////////////////
// HAMOOPI(G) by GameDevBoss (Daniel Moura) 2015-2022                       //
// www.youtube.com/c/GameDevBoss                                            //
// HAMOOPIG É GRATUITO E SEMPRE SERÁ - HAMOOPIG IS FREE AND ALWAYS WILL BE  //
// O CONHECIMENTO DEVE SER COMPARTILHADO - KNOWLEDGE MUST BE SHARED         //
//////////////////////////////////////////////////////////////////////////////

#include <genesis.h>
#include "sprite.h"
#include "gfx.h"
#include "sound.h"

//--- DEFINICOES ---//
#define WEAK   1
#define MEDIUM 2
#define FIERCE 3
#define LP     1
#define MP     2
#define HP     3
#define LK     4
#define MK     5
#define HK     6
#define INDEX_MAR 0xe3 // Usado na animacao do cenario (endereco de memoria)
#define CIGD 3         // (C)iclo(I)nteracoes(G)ravidade(D)efault (usado na engine de Fisica)
#define BODYSPACE 20   // Distancia minima entre os jogadores (x2)
#define RAGETIMER 600  // Tempo de Fúria no samsho2, antes de voltar ao normal
//SOUND DEFs
#define INGAME_SFX 64
#define P1_SFX     65
#define P2_SFX     66

//--- FUNCOES ---//
void FUNCAO_INICIALIZACAO();
void FUNCAO_ROUND_INIT();
void FUNCAO_ROUND_RESTART();
void FUNCAO_RELOGIO();
void FUNCAO_BARRAS_DE_ENERGIA();
void PLAYER_STATE(u8 Player, u16 State);
void FUNCAO_PLAY_SND(u8 Player, u16 State);
void FUNCAO_INPUT_SYSTEM();
void FUNCAO_FSM();
void FUNCAO_FSM_HITBOXES(u8 Player);
void FUNCAO_PHYSICS();
void FUNCAO_SPR_POSITION();
void FUNCAO_ANIMACAO();
void FUNCAO_CAMERA_BGANIM();
void FUNCAO_DEPTH(u8 Player);
void FUNCAO_SAMSHOFX();
void FUNCAO_DEBUG();
bool FUNCAO_COLISAO(s16 R1x1, s16 R1y1, s16 R1x2, s16 R1y2, s16 R2x1, s16 R2y1, s16 R2x2, s16 R2y2);
void FUNCAO_UPDATE_LIFESP(u8 Player, u8 EnergyType, s8 Value);
void CLEAR_VDP();

//--- VARIAVEIS ---//

u8   i;                   //Variavel inteira de uso geral
u32  gFrames = 0;         //Contador de frames
bool gPodeMover = 1;      //Desabilita os inputs dos players em momentos especificos (inicio do Round por exemplo)
u8   gWinnerID;           //Personagem Vencedor da Luta
u8   gLoseID;             //Personagem Perdedor da Luta
bool gContinueOption;     //Reinicia, ou nao, a luta
u8   gRoom=1;             //'Room / Sala' do jogo (Tela de apresentacao, Menu, In game, etc)
u8   gTimerExit;          //Timer para ir para a proxima tela, usada em algumas rooms
u8   gDescompressionExit; //Usado na Room de Descompressao
s8   gClockTimer=60;      //Relogio, timer de 1 seg
s8   gClockLTimer=9;      //Digito esquerdo do Relogio
s8   gClockRTimer=9;      //Digito direito do Relogio
u8   gRound=1;            //Round Number
u8   gSombraStyle=1;      //Estilo de Sombra, carregamento de 1 ou 2 sombras, por padrao, carregamos 1 para liberar mais tiles na VDP
bool gDebug = 0;          //Exibe o Debug
char gStr[64];            //Variavel para armazenar textos, usada no debug
u16  gInd_tileset;        //Variavel utilizada para carregar dados de background
bool gASG_system;         //variavel usada pelo "Anti Sprite Glitch System" system (A.S.G.S.)
u8   gPing2 = 0;          //Variavel que alterna entre 0 e 1 constantemente
u8   gPing4 = 0;          //Variavel que alterna entre 0 ; 1 ; 2 ; 3 constantemente
u8   gPing10 = 9;         //Variavel que alterna entre 0 ; 1 ; 2 ... 7 ; 8 ; 9 constantemente
u8   gInputTimerCountDown=20; //Utilizada para os comandos de magias
u16  gDistancia;          //Distancia entre os Players
u8   gAlturaPiso;         //Altura do Piso
u8   gBgAnim;             //Timer para animacao do cenario
u16  gLimiteCenarioE= 30; //Limite Esquerdo para controle de movimentacao
u16  gLimiteCenarioD=290; //Limite Direito  para controle de movimentacao
u8   gravidadePadrao = 3; //Utilizado nos controles de Fisica
u8   impulsoPadrao = -14; //Utilizado nos controles de Fisica
u16  gBG_Width;           //Largura do Cenario, em pixels
s16  gScrollValues[28] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }; //Scrolling do Cenario
s16  gScrollValue;        //Scrolling de Cenario
u8   gBG_Choice=1;        //Cenario Escolhido
u8   Spark1_countDown;    //Desativa Efeito Spark1
u8   Spark2_countDown;    //Desativa Efeito Spark2
s16  PetalaPX[40];        //Petalas da tela de Apresentacao, X
s16  PetalaPY[40];        //Petalas da tela de Apresentacao, Y

//Char Selection Menu
u8   cursorP1linha  = 1;  //Posicao inicial do P1 na tela de Char Select
u8   cursorP1coluna = 1;  //Posicao inicial do P1 na tela de Char Select
u8   cursorP1_ID;         //Atribui ID ao player de acordo com o cursor de selecao
u8   cursorP1ConfirmFX;   //timer auxiliar, ver 'cursorConfirmTimer'
u8   cursorP1ColorChoice; //Escolha de cor
u8   cursorP2linha  = 1;  //Posicao inicial do P2 na tela de Char Select
u8   cursorP2coluna = 4;  //Posicao inicial do P2 na tela de Char Select
u8   cursorP2_ID;         //Atribui ID ao player de acordo com o cursor de selecao
u8   cursorP2ConfirmFX;   //timer auxiliar, ver 'cursorConfirmTimer'
u8   cursorP2ColorChoice; //Escolha de cor
u8   cursorConfirmTimer;  //Timer que confirma a selecao dos Chars, usado pelo efeito do cursor, FX 
bool endP1Selection;      //Encerra as selecoes do Char Select
bool endP2Selection;      //Encerra as selecoes do Char Select

//Sprites
Sprite* Rect1BB1_Q1; Sprite* Rect1BB1_Q2; Sprite* Rect1BB1_Q3; Sprite* Rect1BB1_Q4; 
Sprite* Rect1HB1_Q1; Sprite* Rect1HB1_Q2; Sprite* Rect1HB1_Q3; Sprite* Rect1HB1_Q4;
Sprite* Rect2BB1_Q1; Sprite* Rect2BB1_Q2; Sprite* Rect2BB1_Q3; Sprite* Rect2BB1_Q4;
Sprite* Rect2HB1_Q1; Sprite* Rect2HB1_Q2; Sprite* Rect2HB1_Q3; Sprite* Rect2HB1_Q4;

Sprite* ClockL;      //Relogio, digito esquedo, N_
Sprite* ClockR;      //Relogio, digito direito, _N
Sprite* HUD_Lethers; //Round N, Duel N, Fight, Begin, etc...
Sprite* Spark[3];    //Efeitos de Hits, Blocks, Magias, etc

//Inputs, Joystick
bool JOY1_UP, JOY1_DOWN, JOY1_LEFT, JOY1_RIGHT, JOY1_A, JOY1_B, JOY1_C, JOY1_X, JOY1_Y, JOY1_Z, JOY1_START, JOY1_MODE;
bool key_JOY1_UP_pressed, key_JOY1_UP_hold, key_JOY1_UP_released;
bool key_JOY1_DOWN_pressed, key_JOY1_DOWN_hold, key_JOY1_DOWN_released;
bool key_JOY1_LEFT_pressed, key_JOY1_LEFT_hold, key_JOY1_LEFT_released;
bool key_JOY1_RIGHT_pressed, key_JOY1_RIGHT_hold, key_JOY1_RIGHT_released;
bool key_JOY1_A_pressed, key_JOY1_A_hold, key_JOY1_A_released;
bool key_JOY1_B_pressed, key_JOY1_B_hold, key_JOY1_B_released;
bool key_JOY1_C_pressed, key_JOY1_C_hold, key_JOY1_C_released;
bool key_JOY1_X_pressed, key_JOY1_X_hold, key_JOY1_X_released;
bool key_JOY1_Y_pressed, key_JOY1_Y_hold, key_JOY1_Y_released;
bool key_JOY1_Z_pressed, key_JOY1_Z_hold, key_JOY1_Z_released;
bool key_JOY1_START_pressed, key_JOY1_START_hold, key_JOY1_START_released;
bool key_JOY1_MODE_pressed, key_JOY1_MODE_hold, key_JOY1_MODE_released;

bool JOY2_UP, JOY2_DOWN, JOY2_LEFT, JOY2_RIGHT, JOY2_A, JOY2_B, JOY2_C, JOY2_X, JOY2_Y, JOY2_Z, JOY2_START, JOY2_MODE;
bool key_JOY2_UP_pressed, key_JOY2_UP_hold, key_JOY2_UP_released;
bool key_JOY2_DOWN_pressed, key_JOY2_DOWN_hold, key_JOY2_DOWN_released;
bool key_JOY2_LEFT_pressed, key_JOY2_LEFT_hold, key_JOY2_LEFT_released;
bool key_JOY2_RIGHT_pressed, key_JOY2_RIGHT_hold, key_JOY2_RIGHT_released;
bool key_JOY2_A_pressed, key_JOY2_A_hold, key_JOY2_A_released;
bool key_JOY2_B_pressed, key_JOY2_B_hold, key_JOY2_B_released;
bool key_JOY2_C_pressed, key_JOY2_C_hold, key_JOY2_C_released;
bool key_JOY2_X_pressed, key_JOY2_X_hold, key_JOY2_X_released;
bool key_JOY2_Y_pressed, key_JOY2_Y_hold, key_JOY2_Y_released;
bool key_JOY2_Z_pressed, key_JOY2_Z_hold, key_JOY2_Z_released;
bool key_JOY2_START_pressed, key_JOY2_START_hold, key_JOY2_START_released;
bool key_JOY2_MODE_pressed, key_JOY2_MODE_hold, key_JOY2_MODE_released;

struct PlayerDEF {
	Sprite* sprite;      //Sprite do Player 
	Sprite* sombra;      //Sprite (sombra) do Player 
	Sprite* fBall;       //Fireball do Player
	s8 fBallDirecao;     //Direcao de propagacao da Fireball (eixo X)
	u8 fBallSpeedX;      //Velocidade da Magia (eixo X)
	bool fBallActive;    //Fireball está ativada ou nao
	s16 fBallX;          //Posicao X da Magia
	s16 fBallY;          //Posicao Y da Magia
	int paleta;          //Paleta do personagem
	u8  palID;           //ID da paleta ativa no momento
	u8  wins;            //Contador de vitorias; Rounds vencidos
	u8  energia;         //Energia do Player, usada para fins de HUD, graficos das barras
	u8  energiaBase;     //Energia Base do Player, essa é a energia real, usada para definir o round
	u8  energiaSP;       //Energia / Barra de Especial
	u8  rageLvl;         //Nivel de Raiva, usado no jogo Samurai Shodown 2 /*samsho2*/
	u16 rageTimerCountdown; //Tempo em que fica furioso no samsho2
	u16 state;           //Controla o estado (animacao) do Player
	u8  joyDirTimer[10]; //Utilizado para verificar o comando das magias -> Direcionais Timers
	u8  inputArray[5];   //Utilizado para verificar o comando das magias -> Direcionais Ordenados
	u8  attackPower;     //Utilizado para verificar o comando das magias -> Weak; Medium; Fierce
	u8  attackButton;    //Utilizado para verificar o comando das magias -> LP, MP, HK, LK, MK, HK
	u8  id;              //Identificacao numerica do personagem selecionado. Exemplo: Ryu=1; Ken=2; etc...
	s16 x;               //Posicao X do Player
	s16 y;               //Posicao Y do Player
	u8  w;               //Largura do Sprite
	u8  h;               //Altura do Sprite
	u8  axisX;           //Posicao X do Ponto Pivot
	u8  axisY;           //Posicao Y do Ponto Pivot
	s8  direcao;         //Direcao para qual o Player esta olhando (1 ; -1)
	u8  hSpeed;          //Velocidade Horizontal
	u8  puloTimer;       //Tempo do Pulo
	u8  cicloInteracoesGravidade;     //Controles de Gravidade
	u8  cicloInteracoesGravidadeCont; //Controles de Gravidade
	s8  impulsoY;                     //Controles de Gravidade
	s8  gravidadeY;                   //Controles de Gravidade
	u16 frameTimeAtual;  //Tempo atual do frame de animacao corrente
	u16 frameTimeTotal;  //Tempo total do frame de animacao corrente
	u16 animFrame;       //Frame de animacao atual
	u16 animFrameTotal;  //Qtde total de frames deste estado (animacao)
	u8  hitPause;        //Tempo de congelamento apos o Hit
	u16 dataAnim[60];    //Total de frames disponiveis para cada estado (animacao)
	u16 dataHBox[4];     //Posiconamento das Hit Boxes (caixas vermelhas de ataque)
	u16 dataBBox[4];     //Posiconamento das Body Boxes (caixas azuis, area vulneravel a ataques)
	u16 dataMBox[4];     //Posiconamento das Mass Boxes (caixas de massa, corpo fisico do personagem, usado para empurrar)
	//JOYSTICK
	u8 key_JOY_UP_status; u8 key_JOY_DOWN_status; u8 key_JOY_LEFT_status; u8 key_JOY_RIGHT_status;
	u8 key_JOY_A_status; u8 key_JOY_B_status; u8 key_JOY_C_status;
	u8 key_JOY_X_status; u8 key_JOY_Y_status; u8 key_JOY_Z_status;
	u8 key_JOY_START_status; u8 key_JOY_MODE_status;
	u8 key_JOY_countdown[10]; //timer regressivo, ativado apos pressionar um botao direcional (usado para correr, esquivar, etc)
}; struct PlayerDEF P[3];

struct GraphicElementDEF {
	Sprite* sprite;      //Sprite do Graphic Element
}; struct GraphicElementDEF GE[25];


//mapa
TileMap *Cenario_Tilemap;

int main(u16 hard) /************** MAIN **************/
{
    //Inicializacao da VDP (Video Display Processor)
	SYS_disableInts();
	 VDP_init();                    //Inicializa a VDP (Video Display Processor)
	 VDP_setScreenWidth320();       //Resolucao padrao de 320x224 (Largura)
	 VDP_setScreenHeight224();      //Resolucao padrao de 320x224 (Altura)
	 VDP_setPlaneSize(64,32,TRUE);  //Recomendado para BGs grandes
	 VDP_setTextPlane(BG_A);        //Textos serao desenhados no BG_A
	 VDP_setTextPalette(PAL1);      //Textos serao desenhados com a ultima cor da PAL0
     SPR_init(127, 384, 256);       //SPR_init(u16 maxSprite, u16 vramSize, u16 unpackBufferSize)
	 VDP_setBackgroundColor(0);     //Range 0-63 //4 Paletas de 16 cores = 64 cores
	SYS_enableInts();
	
    while(TRUE) /// LOOP PRINCIPAL ///
    {
        gFrames++; 
		if(gPing2  == 1){ gPing2 = -1; } gPing2++;  //var 'gPing2'  (50%) variacao: 0 ; 1
		if(gPing4  == 3){ gPing4 = -1; } gPing4++;  //var 'gPing4'  (25%) variacao: 0 ; 1 ; 2 ; 3
		if(gPing10 == 9){ gPing10= -1; } gPing10++; //var 'gPing10' (10%) variacao: 0 ; 1 ; 2 ; 3 ; 4 ; 5 ; 6 ; 7 ; 8 ; 9
		
		if(gRoom==1) //TELA HAMOOPIG --------------------------------------------------------------
		{
			FUNCAO_INPUT_SYSTEM(); //Verifica os joysticks 
			
			//inicializacao
			if(gFrames==1)
			{
				//bg_A
				VDP_drawImageEx(BG_A, &room_0_bga, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset), 0, 0, FALSE, TRUE);
				gInd_tileset += room_0_bga.tileset->numTile;
				//bg_B
				VDP_drawImageEx(BG_B, &room_0_bgb, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset), 0, 0, FALSE, TRUE);
				gInd_tileset += room_0_bgb.tileset->numTile;
				
				VDP_setPalette(PAL0, room_0_bga.palette->data);
				VDP_setPalette(PAL1, room_0_bgb.palette->data);
				
				VDP_setBackgroundColor(0);
			}
			
			if(gFrames==10)
			{
				XGM_setPCM(P1_SFX, snd_future_now, sizeof(snd_future_now)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3);
			}
			
			if( (gFrames>=60*5) || (P[1].key_JOY_START_status==1 && gFrames>10) ){
				CLEAR_VDP();
				gRoom=2;
				gFrames=1;
				//if(key_JOY1_A_status>0){gRoom=111111;}//atalho para outra Room...
				//if(key_JOY1_B_status>0){gRoom=222222;}//atalho para outra Room...
				//if(key_JOY1_C_status>0){gRoom=333333;}//atalho para outra Room...
			};
		}
		
		if(gRoom==2) //TELA DE APRESENTACAO -------------------------------------------------------
		{
			FUNCAO_INPUT_SYSTEM(); //Verifica os joysticks 
			
			if(gFrames==1)
			{
				gInd_tileset=1;
			
				VDP_loadTileSet(bg_title_bgb.tileset,gInd_tileset,DMA); 
				VDP_setTileMapEx(BG_B,bg_title_bgb.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,gInd_tileset),0,0,0,0,40,28,DMA_QUEUE);
				VDP_setPalette(PAL0, bg_title_bgb.palette->data);
				gInd_tileset += bg_title_bgb.tileset->numTile;
				
				VDP_loadTileSet(bg_title_bga.tileset,gInd_tileset,DMA); 
				VDP_setTileMapEx(BG_A,bg_title_bga.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,gInd_tileset),0,0,0,0,40,28,DMA_QUEUE);
				VDP_setPalette(PAL1, bg_title_bga.palette->data);
				gInd_tileset += bg_title_bga.tileset->numTile;
			}
			
			if(
			(
				P[1].key_JOY_START_status==1  ||
				P[1].key_JOY_A_status==1  ||
				P[1].key_JOY_B_status==1  ||
				P[1].key_JOY_C_status==1  ||
				P[1].key_JOY_X_status==1  ||
				P[1].key_JOY_Y_status==1  ||
				P[1].key_JOY_Z_status==1  
			)
			&& gFrames>5)
			{
				if (GE[1].sprite){ SPR_releaseSprite(GE[1].sprite); GE[1].sprite = NULL; }
				CLEAR_VDP();
				gFrames=0;
				gRoom=3;
			};
			
		}
		
		if(gRoom==3) //CHAR SELECT ----------------------------------------------------------------
		{
			FUNCAO_INPUT_SYSTEM(); //Verifica os joysticks 
			
			//"cursorConfirmTimer" eh usado para o efeito FX, ativado quando se escolhe um personagem
			//cursorConfirmTimer = Velocidade da animacao (sprite.res) * Total de quadros :)
			cursorConfirmTimer = 2*9; //18
			
			if(gFrames==1)
			{
				//comeca a tocar a musica
				XGM_startPlay(music_char_select); XGM_isPlaying(); //FIX
				
				cursorP1ConfirmFX = cursorConfirmTimer;
				cursorP2ConfirmFX = cursorConfirmTimer;
				
				//apos a escolha de ambos os players, este eh o tempo (em frames) para avancar para a proxima tela
				gTimerExit=60*1;
				
				//Fique atento para mudar aqui, ALTERE caso tenha outros personagens no jogo
				P[1].id = 1; cursorP1_ID = 1; //terry selecao inicial
				P[2].id = 2; cursorP2_ID = 2; //mai selecao inicial
				
				cursorP1ColorChoice = 1;
				cursorP2ColorChoice = 1;
				
				endP1Selection = 0;
				endP2Selection = 0;
				
				gInd_tileset=1;
			
				VDP_loadTileSet(bg_char_select_bgb.tileset,gInd_tileset,DMA); 
				VDP_setTileMapEx(BG_B,bg_char_select_bgb.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,gInd_tileset),0,0,0,0,40,28,DMA_QUEUE);
				VDP_setPalette(PAL0, bg_char_select_bgb.palette->data);
				gInd_tileset += bg_char_select_bgb.tileset->numTile;
				
				VDP_loadTileSet(bg_char_select_bga.tileset,gInd_tileset,DMA); 
				VDP_setTileMapEx(BG_A,bg_char_select_bga.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,gInd_tileset),0,0,0,0,40,28,DMA_QUEUE);
				VDP_setPalette(PAL1, bg_char_select_bga.palette->data);
				gInd_tileset += bg_char_select_bga.tileset->numTile;
				
				//Cursores
				GE[2].sprite = SPR_addSpriteExSafe(&spr_char_sel_cursor_p2, 192, 28, TILE_ATTR(PAL0, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
				SPR_setVRAMTileIndex(GE[2].sprite, 1461); //define uma posicao especifica para o GFX na VRAM
				GE[1].sprite = SPR_addSpriteExSafe(&spr_char_sel_cursor_p1,  96, 28, TILE_ATTR(PAL0, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
				SPR_setVRAMTileIndex(GE[1].sprite, 1441); //define uma posicao especifica para o GFX na VRAM
				
				//Retratos grandes
				/*
				GE[3].sprite = SPR_addSpriteExSafe(&spr_terry_001,  16, 16, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
				VDP_setPalette(PAL2, spr_terry_001.palette->data);
				GE[4].sprite = SPR_addSpriteExSafe(&spr_mai_001,  232, 16, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
				VDP_setPalette(PAL3, spr_mai_001.palette->data);
				SPR_setHFlip(GE[4].sprite, TRUE);
				*/
				
				VDP_setPalette(PAL2, spr_terry_100.palette->data);
				VDP_setPalette(PAL3, spr_mai_100.palette->data);
				
				//Lutadores
				GE[5].sprite = SPR_addSpriteExSafe(&spr_terry_100CS, 16, 72, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
				GE[6].sprite = SPR_addSpriteExSafe(&spr_mai_100CS,  240, 82, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
				SPR_setHFlip(GE[6].sprite, TRUE);
			}
			
			//Tamanho do seletor, quantidade total de personagens (Colunas * Linhas)
			u8 QtdeColunas = 4;
			u8 QtdeLinhas  = 5;
			u8 moverCursorP1 = 0;
			u8 moverCursorP2 = 0;
			u8 tamanhoDoIcone = 32; //tamanho do icone de selecao dos personagens, em pixels
			
			//Movimenta os cursores
			if(cursorP1ConfirmFX==cursorConfirmTimer)
			{
				if(P[1].key_JOY_RIGHT_status==1){ moverCursorP1=1; if(cursorP1coluna<QtdeColunas){cursorP1coluna++;}else{cursorP1coluna=1;} }
				if(P[1].key_JOY_LEFT_status ==1){ moverCursorP1=1; if(cursorP1coluna>1){cursorP1coluna--;}else{cursorP1coluna=QtdeColunas;} }
				if(P[1].key_JOY_DOWN_status ==1){ moverCursorP1=1; if(cursorP1linha<QtdeLinhas){cursorP1linha++;}else{cursorP1linha=1;} }
				if(P[1].key_JOY_UP_status   ==1){ moverCursorP1=1; if(cursorP1linha>1){cursorP1linha--;}else{cursorP1linha=QtdeLinhas;} }
			}
			
			if(cursorP2ConfirmFX==cursorConfirmTimer)
			{
				if(P[2].key_JOY_RIGHT_status==1){ moverCursorP2=1; if(cursorP2coluna<QtdeColunas){cursorP2coluna++;}else{cursorP2coluna=1;} }
				if(P[2].key_JOY_LEFT_status ==1){ moverCursorP2=1; if(cursorP2coluna>1){cursorP2coluna--;}else{cursorP2coluna=QtdeColunas;} }
				if(P[2].key_JOY_DOWN_status ==1){ moverCursorP2=1; if(cursorP2linha<QtdeLinhas){cursorP2linha++;}else{cursorP2linha=1;} }
				if(P[2].key_JOY_UP_status   ==1){ moverCursorP2=1; if(cursorP2linha>1){cursorP2linha--;}else{cursorP2linha=QtdeLinhas;} }
			}			
			
			//Mapeamento dos personagens
			/*
			            1       2       3       4     coluna
			        +--------+--------+--------+--------+
			  1     | char1  | char2  | char3  | char4  |
			        +--------+--------+--------+--------+
			  2     | char5  | char6  | char7  | char8  |
			        +--------+--------+--------+--------+
			  3     | char9  |     char10      | char11 |
			        +--------+--------+--------+--------+
			  4     | char12 | char13 | char14 | char15 |
			        +--------+--------+--------+--------+
			  5     | char16 | char17 | char18 | char19 |
			        +--------+--------+--------+--------+
			linha
			*/
			
			if(moverCursorP1==1) //P1
			{
				XGM_setPCM(P1_SFX, snd_cursor, sizeof(snd_cursor)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3);
				
				//atribuicao de ID //RHEO, mudar aqui, conforme adicionar chars!
				if(cursorP1coluna==1 && cursorP1linha==1){ cursorP1_ID=1; }
				if(cursorP1coluna==2 && cursorP1linha==1){ cursorP1_ID=1; }
				if(cursorP1coluna==3 && cursorP1linha==1){ cursorP1_ID=4; } 
				if(cursorP1coluna==4 && cursorP1linha==1){ cursorP1_ID=2; }
				
				if(cursorP1coluna==1 && cursorP1linha==2){ cursorP1_ID=1; }
				if(cursorP1coluna==2 && cursorP1linha==2){ cursorP1_ID=1; }
				if(cursorP1coluna==3 && cursorP1linha==2){ cursorP1_ID=1; }
				if(cursorP1coluna==4 && cursorP1linha==2){ cursorP1_ID=1; }
				
				if(cursorP1coluna==1 && cursorP1linha==3){ cursorP1_ID=1; }
				if(cursorP1coluna==2 && cursorP1linha==3){ cursorP1_ID=1; } //mesmo char
				if(cursorP1coluna==3 && cursorP1linha==3){ cursorP1_ID=1; } //mesmo char
				if(cursorP1coluna==4 && cursorP1linha==3){ cursorP1_ID=1; }
				
				if(cursorP1coluna==1 && cursorP1linha==4){ cursorP1_ID=1; }
				if(cursorP1coluna==2 && cursorP1linha==4){ cursorP1_ID=1; }
				if(cursorP1coluna==3 && cursorP1linha==4){ cursorP1_ID=1; }
				if(cursorP1coluna==4 && cursorP1linha==4){ cursorP1_ID=1; }
				
				if(cursorP1coluna==1 && cursorP1linha==5){ cursorP1_ID=3; }
				if(cursorP1coluna==2 && cursorP1linha==5){ cursorP1_ID=1; }
				if(cursorP1coluna==3 && cursorP1linha==5){ cursorP1_ID=1; }
				if(cursorP1coluna==4 && cursorP1linha==5){ cursorP1_ID=1; }
				P[1].id = cursorP1_ID;
				
				//Movimentacao de Sprite do cursor
				SPR_setPosition(GE[1].sprite, 96+((cursorP1coluna-1)*tamanhoDoIcone), 28+((cursorP1linha-1)*tamanhoDoIcone));
				
				//Update Visual Char
				if (GE[3].sprite){ SPR_releaseSprite(GE[3].sprite); GE[3].sprite = NULL; }
				if (GE[5].sprite){ SPR_releaseSprite(GE[5].sprite); GE[5].sprite = NULL; }
				
				if(cursorP1_ID==1)
				{
					//GE[3].sprite = SPR_addSpriteExSafe(&spr_terry_001,  16, 16, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					//VDP_setPalette(PAL2, spr_terry_001.palette->data);
					GE[5].sprite = SPR_addSpriteExSafe(&spr_terry_100CS, 16, 80-8, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					VDP_setPalette(PAL2, spr_terry_100CS.palette->data);
				}
				if(cursorP1_ID==2)
				{
					//GE[3].sprite = SPR_addSpriteExSafe(&spr_mai_001,  16,  16, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					//VDP_setPalette(PAL2, spr_mai_001.palette->data);
					GE[5].sprite = SPR_addSpriteExSafe(&spr_mai_100CS, 16, 90-8, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					VDP_setPalette(PAL2, spr_mai_100CS.palette->data);
				}
				if(cursorP1_ID==3)
				{
					//GE[3].sprite = SPR_addSpriteExSafe(&spr_mai_001,  16,  16, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					//VDP_setPalette(PAL2, spr_mai_001.palette->data);
					GE[5].sprite = SPR_addSpriteExSafe(&spr_bluemary_100CS, 16, 90-8, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					VDP_setPalette(PAL2, spr_bluemary_100CS.palette->data);
				}
				if(cursorP1_ID==4)
				{
					//GE[3].sprite = SPR_addSpriteExSafe(&spr_mai_001,  16,  16, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					//VDP_setPalette(PAL2, spr_mai_001.palette->data);
					GE[5].sprite = SPR_addSpriteExSafe(&spr_joe_100CS, 16, 80-8, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					VDP_setPalette(PAL2, spr_joe_100CS.palette->data);
				}
			}//---end P1
			
			if(moverCursorP2==1) //P2
			{
				XGM_setPCM(P2_SFX, snd_cursor, sizeof(snd_cursor)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4);
				
				//atribuicao de ID //RHEO, mudar aqui, conforme adicionar chars!
				if(cursorP2coluna==1 && cursorP2linha==1){ cursorP2_ID=1; }
				if(cursorP2coluna==2 && cursorP2linha==1){ cursorP2_ID=1; }
				if(cursorP2coluna==3 && cursorP2linha==1){ cursorP2_ID=4; }
				if(cursorP2coluna==4 && cursorP2linha==1){ cursorP2_ID=2; }
				
				if(cursorP2coluna==1 && cursorP2linha==2){ cursorP2_ID=1; }
				if(cursorP2coluna==2 && cursorP2linha==2){ cursorP2_ID=1; }
				if(cursorP2coluna==3 && cursorP2linha==2){ cursorP2_ID=1; }
				if(cursorP2coluna==4 && cursorP2linha==2){ cursorP2_ID=1; }
				
				if(cursorP2coluna==1 && cursorP2linha==3){ cursorP2_ID=1; }
				if(cursorP2coluna==2 && cursorP2linha==3){ cursorP2_ID=1; } //mesmo char
				if(cursorP2coluna==3 && cursorP2linha==3){ cursorP2_ID=1; } //mesmo char
				if(cursorP2coluna==4 && cursorP2linha==3){ cursorP2_ID=1; }
				
				if(cursorP2coluna==1 && cursorP2linha==4){ cursorP2_ID=1; }
				if(cursorP2coluna==2 && cursorP2linha==4){ cursorP2_ID=1; }
				if(cursorP2coluna==3 && cursorP2linha==4){ cursorP2_ID=1; }
				if(cursorP2coluna==4 && cursorP2linha==4){ cursorP2_ID=1; }
				
				if(cursorP2coluna==1 && cursorP2linha==5){ cursorP2_ID=3; }
				if(cursorP2coluna==2 && cursorP2linha==5){ cursorP2_ID=1; }
				if(cursorP2coluna==3 && cursorP2linha==5){ cursorP2_ID=1; }
				if(cursorP2coluna==4 && cursorP2linha==5){ cursorP2_ID=1; }
				
				P[2].id = cursorP2_ID;
				
				//Movimentacao de Sprite do cursor
				SPR_setPosition(GE[2].sprite, 96+((cursorP2coluna-1)*tamanhoDoIcone), 28+((cursorP2linha-1)*tamanhoDoIcone));
				
				//Update Visual Char
				if (GE[4].sprite){ SPR_releaseSprite(GE[4].sprite); GE[4].sprite = NULL; }
				if (GE[6].sprite){ SPR_releaseSprite(GE[6].sprite); GE[6].sprite = NULL; }
				
				if(cursorP2_ID==1)
				{
					//GE[4].sprite = SPR_addSpriteExSafe(&spr_terry_001,  232, 16, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					//VDP_setPalette(PAL3, spr_terry_001.palette->data);
					GE[6].sprite = SPR_addSpriteExSafe(&spr_terry_100CS, 230, 80-8, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					VDP_setPalette(PAL3, spr_terry_100CS.palette->data);
				}
				if(cursorP2_ID==2)
				{
					//GE[4].sprite = SPR_addSpriteExSafe(&spr_mai_001,   232,  16, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					//VDP_setPalette(PAL3, spr_mai_001.palette->data);
					GE[6].sprite = SPR_addSpriteExSafe(&spr_mai_100CS, 228, 90-8, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					VDP_setPalette(PAL3, spr_mai_100CS.palette->data);
				}
				if(cursorP2_ID==3)
				{
					//GE[4].sprite = SPR_addSpriteExSafe(&spr_mai_001,   232,  16, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					//VDP_setPalette(PAL3, spr_mai_001.palette->data);
					GE[6].sprite = SPR_addSpriteExSafe(&spr_bluemary_100CS, 228, 90-8, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					VDP_setPalette(PAL3, spr_bluemary_100CS.palette->data);
				}
				if(cursorP2_ID==4)
				{
					//GE[4].sprite = SPR_addSpriteExSafe(&spr_mai_001,   232,  16, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					//VDP_setPalette(PAL3, spr_mai_001.palette->data);
					GE[6].sprite = SPR_addSpriteExSafe(&spr_joe_100CS, 228, 80-8, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					VDP_setPalette(PAL3, spr_joe_100CS.palette->data);
				}
				SPR_setHFlip(GE[4].sprite, TRUE);
				SPR_setHFlip(GE[6].sprite, TRUE);
			}//---end P2
			
			//Seleciona o Char, P1
			if(
				cursorP1ConfirmFX==cursorConfirmTimer && (
				P[1].key_JOY_A_status==1 || P[1].key_JOY_B_status==1 || P[1].key_JOY_C_status==1 || 
				P[1].key_JOY_X_status==1 || P[1].key_JOY_Y_status==1 || P[1].key_JOY_Z_status==1 || 
				P[1].key_JOY_START_status==1 || P[1].key_JOY_MODE_status==1 )
			)
			{
				cursorP1ConfirmFX--;
				//if (GE[1].sprite){ SPR_releaseSprite(GE[1].sprite); GE[1].sprite = NULL; }
				//GE[1].sprite = SPR_addSpriteExSafe(&spr_cursor_confirm,  96+((cursorP1coluna-1)*tamanhoDoIcone), 148+((cursorP1linha-1)*tamanhoDoIcone)+4, TILE_ATTR(PAL0, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
				//SPR_setVRAMTileIndex(GE[1].sprite, 1441); //define uma posicao especifica para o GFX na VRAM
			}
			if(cursorP1ConfirmFX<cursorConfirmTimer)
			{
				if(cursorP1ConfirmFX>0)
				{
					cursorP1ConfirmFX--;
					if(cursorP1ConfirmFX==1)
					{
						XGM_setPCM(P1_SFX, snd_confirm, sizeof(snd_confirm)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3);
						
						//if (GE[1].sprite){ SPR_releaseSprite(GE[1].sprite); GE[1].sprite = NULL; }
						//GE[1].sprite = SPR_addSpriteExSafe(&spr_color_cursor,  20, 89, TILE_ATTR(PAL0, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					
						endP1Selection=1; //encerra a selecao do P1
					
					}
				}else{
					//SELECAO DE COR!
					u8 mudarcor=0;
					if(P[1].key_JOY_RIGHT_status==1 && endP1Selection==0){ mudarcor=1; if(cursorP1ColorChoice<8){cursorP1ColorChoice++; }else{cursorP1ColorChoice=1;} }
					if(P[1].key_JOY_LEFT_status ==1 && endP1Selection==0){ mudarcor=1; if(cursorP1ColorChoice>1){cursorP1ColorChoice--; }else{cursorP1ColorChoice=8;} }
					if(mudarcor==1)
					{
						XGM_setPCM(P1_SFX, snd_cursor, sizeof(snd_cursor)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3);
						
						if(cursorP1_ID==1) //TERRY
						{
							if(cursorP1ColorChoice==1){ VDP_setPalette(PAL2, spr_terry_pal1.palette->data); }
							if(cursorP1ColorChoice==2){ VDP_setPalette(PAL2, spr_terry_pal2.palette->data); }
							if(cursorP1ColorChoice==3){ VDP_setPalette(PAL2, spr_terry_pal3.palette->data); }
							if(cursorP1ColorChoice==4){ VDP_setPalette(PAL2, spr_terry_pal4.palette->data); }
							if(cursorP1ColorChoice==5){ VDP_setPalette(PAL2, spr_terry_pal5.palette->data); }
							if(cursorP1ColorChoice==6){ VDP_setPalette(PAL2, spr_terry_pal6.palette->data); }
							if(cursorP1ColorChoice==7){ VDP_setPalette(PAL2, spr_terry_pal7.palette->data); }
							if(cursorP1ColorChoice==8){ VDP_setPalette(PAL2, spr_terry_pal8.palette->data); }
						}
						if(cursorP1_ID==2) //MAI
						{
							if(cursorP1ColorChoice==1){ VDP_setPalette(PAL2, spr_mai_pal1.palette->data); }
							if(cursorP1ColorChoice==2){ VDP_setPalette(PAL2, spr_mai_pal2.palette->data); }
							if(cursorP1ColorChoice==3){ VDP_setPalette(PAL2, spr_mai_pal3.palette->data); }
							if(cursorP1ColorChoice==4){ VDP_setPalette(PAL2, spr_mai_pal4.palette->data); }
							if(cursorP1ColorChoice==5){ VDP_setPalette(PAL2, spr_mai_pal5.palette->data); }
							if(cursorP1ColorChoice==6){ VDP_setPalette(PAL2, spr_mai_pal6.palette->data); }
							if(cursorP1ColorChoice==7){ VDP_setPalette(PAL2, spr_mai_pal7.palette->data); }
							if(cursorP1ColorChoice==8){ VDP_setPalette(PAL2, spr_mai_pal8.palette->data); }
						}
						if(cursorP1_ID==3) //BLUEMARY
						{
							if(cursorP1ColorChoice==1){ VDP_setPalette(PAL2, spr_bluemary_pal1.palette->data); }
							if(cursorP1ColorChoice==2){ VDP_setPalette(PAL2, spr_bluemary_pal2.palette->data); }
							if(cursorP1ColorChoice==3){ VDP_setPalette(PAL2, spr_bluemary_pal3.palette->data); }
							if(cursorP1ColorChoice==4){ VDP_setPalette(PAL2, spr_bluemary_pal4.palette->data); }
							if(cursorP1ColorChoice==5){ VDP_setPalette(PAL2, spr_bluemary_pal5.palette->data); }
							if(cursorP1ColorChoice==6){ VDP_setPalette(PAL2, spr_bluemary_pal6.palette->data); }
							if(cursorP1ColorChoice==7){ VDP_setPalette(PAL2, spr_bluemary_pal7.palette->data); }
							if(cursorP1ColorChoice==8){ VDP_setPalette(PAL2, spr_bluemary_pal8.palette->data); }
						}
						//Movimentacao de Sprite do cursor de cor
						//SPR_setPosition(GE[1].sprite, 20+((cursorP1ColorChoice-1)*8), 89);
					}
					if(
						endP1Selection==0 && (
						P[1].key_JOY_A_status==1 || P[1].key_JOY_B_status==1 || P[1].key_JOY_C_status==1 || 
						P[1].key_JOY_X_status==1 || P[1].key_JOY_Y_status==1 || P[1].key_JOY_Z_status==1 || 
						P[1].key_JOY_START_status==1 || P[1].key_JOY_MODE_status==1 )
					)
					{
						endP1Selection=1;
						
						XGM_setPCM(P1_SFX, snd_confirm, sizeof(snd_confirm)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3);
						
						//if (GE[1].sprite){ SPR_releaseSprite(GE[1].sprite); GE[1].sprite = NULL; }
						//GE[1].sprite = SPR_addSpriteExSafe(&spr_color_cursor_static,  20+((cursorP1ColorChoice-1)*8), 89, TILE_ATTR(PAL0, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					}
				}
			}
			
			//Seleciona o Char, P2
			if(
				cursorP2ConfirmFX==cursorConfirmTimer && (
				P[2].key_JOY_A_status==1 || P[2].key_JOY_B_status==1 || P[2].key_JOY_C_status==1 || 
				P[2].key_JOY_X_status==1 || P[2].key_JOY_Y_status==1 || P[2].key_JOY_Z_status==1 || 
				P[2].key_JOY_START_status==1 || P[2].key_JOY_MODE_status==1 )
			)
			{
				cursorP2ConfirmFX--;
				//if (GE[2].sprite){ SPR_releaseSprite(GE[2].sprite); GE[2].sprite = NULL; }
				//GE[2].sprite = SPR_addSpriteExSafe(&spr_cursor_confirm,  96+((cursorP2coluna-1)*tamanhoDoIcone), 148+((cursorP2linha-1)*tamanhoDoIcone)+4, TILE_ATTR(PAL0, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
				//SPR_setVRAMTileIndex(GE[2].sprite, 1461); //define uma posicao especifica para o GFX na VRAM
			}
			if(cursorP2ConfirmFX<cursorConfirmTimer)
			{
				if(cursorP2ConfirmFX>0)
				{
					cursorP2ConfirmFX--;
					if(cursorP2ConfirmFX==1)
					{
						XGM_setPCM(P2_SFX, snd_confirm, sizeof(snd_confirm)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4);
						
						//if (GE[2].sprite){ SPR_releaseSprite(GE[2].sprite); GE[2].sprite = NULL; }
						//GE[2].sprite = SPR_addSpriteExSafe(&spr_color_cursor,  237, 89, TILE_ATTR(PAL0, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					
						endP2Selection=1; //encerra a selecao do P2
					
					}
				}else{
					//SELECAO DE COR!
					u8 mudarcor=0;
					if(P[2].key_JOY_RIGHT_status==1 && endP2Selection==0){ mudarcor=1; if(cursorP2ColorChoice<8){cursorP2ColorChoice++; }else{cursorP2ColorChoice=1;} }
					if(P[2].key_JOY_LEFT_status ==1 && endP2Selection==0){ mudarcor=1; if(cursorP2ColorChoice>1){cursorP2ColorChoice--; }else{cursorP2ColorChoice=8;} }
					if(mudarcor==1)
					{
						XGM_setPCM(P2_SFX, snd_cursor, sizeof(snd_cursor)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4);
						
						if(cursorP2_ID==1) //terry
						{
							if(cursorP2ColorChoice==1){ VDP_setPalette(PAL3, spr_terry_pal1.palette->data); }
							if(cursorP2ColorChoice==2){ VDP_setPalette(PAL3, spr_terry_pal2.palette->data); }
							if(cursorP2ColorChoice==3){ VDP_setPalette(PAL3, spr_terry_pal3.palette->data); }
							if(cursorP2ColorChoice==4){ VDP_setPalette(PAL3, spr_terry_pal4.palette->data); }
							if(cursorP2ColorChoice==5){ VDP_setPalette(PAL3, spr_terry_pal5.palette->data); }
							if(cursorP2ColorChoice==6){ VDP_setPalette(PAL3, spr_terry_pal6.palette->data); }
							if(cursorP2ColorChoice==7){ VDP_setPalette(PAL3, spr_terry_pal7.palette->data); }
							if(cursorP2ColorChoice==8){ VDP_setPalette(PAL3, spr_terry_pal8.palette->data); }
						}
						if(cursorP2_ID==2) //mai
						{
							if(cursorP2ColorChoice==1){ VDP_setPalette(PAL3, spr_mai_pal1.palette->data); }
							if(cursorP2ColorChoice==2){ VDP_setPalette(PAL3, spr_mai_pal2.palette->data); }
							if(cursorP2ColorChoice==3){ VDP_setPalette(PAL3, spr_mai_pal3.palette->data); }
							if(cursorP2ColorChoice==4){ VDP_setPalette(PAL3, spr_mai_pal4.palette->data); }
							if(cursorP2ColorChoice==5){ VDP_setPalette(PAL3, spr_mai_pal5.palette->data); }
							if(cursorP2ColorChoice==6){ VDP_setPalette(PAL3, spr_mai_pal6.palette->data); }
							if(cursorP2ColorChoice==7){ VDP_setPalette(PAL3, spr_mai_pal7.palette->data); }
							if(cursorP2ColorChoice==8){ VDP_setPalette(PAL3, spr_mai_pal8.palette->data); }
						}
						if(cursorP2_ID==3) //bluemary
						{
							if(cursorP2ColorChoice==1){ VDP_setPalette(PAL3, spr_bluemary_pal1.palette->data); }
							if(cursorP2ColorChoice==2){ VDP_setPalette(PAL3, spr_bluemary_pal2.palette->data); }
							if(cursorP2ColorChoice==3){ VDP_setPalette(PAL3, spr_bluemary_pal3.palette->data); }
							if(cursorP2ColorChoice==4){ VDP_setPalette(PAL3, spr_bluemary_pal4.palette->data); }
							if(cursorP2ColorChoice==5){ VDP_setPalette(PAL3, spr_bluemary_pal5.palette->data); }
							if(cursorP2ColorChoice==6){ VDP_setPalette(PAL3, spr_bluemary_pal6.palette->data); }
							if(cursorP2ColorChoice==7){ VDP_setPalette(PAL3, spr_bluemary_pal7.palette->data); }
							if(cursorP2ColorChoice==8){ VDP_setPalette(PAL3, spr_bluemary_pal8.palette->data); }
						}
						//Movimentacao de Sprite do cursor de cor
						//SPR_setPosition(GE[2].sprite, 237+((cursorP2ColorChoice-1)*8), 89);
					}
					if(
						endP2Selection==0 && (
						P[2].key_JOY_A_status==1 || P[2].key_JOY_B_status==1 || P[2].key_JOY_C_status==1 || 
						P[2].key_JOY_X_status==1 || P[2].key_JOY_Y_status==1 || P[2].key_JOY_Z_status==1 || 
						P[2].key_JOY_START_status==1 || P[2].key_JOY_MODE_status==1 )
					)
					{
						endP2Selection=1;
						
						XGM_setPCM(P2_SFX, snd_confirm, sizeof(snd_confirm)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4);
						
						//if (GE[2].sprite){ SPR_releaseSprite(GE[2].sprite); GE[2].sprite = NULL; }
						//GE[2].sprite = SPR_addSpriteExSafe(&spr_color_cursor_static,  237+((cursorP2ColorChoice-1)*8), 89, TILE_ATTR(PAL0, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					}
				}
			}
			
			//Saida
			if(endP1Selection==1 && endP2Selection==1){ gTimerExit--; }
			if(gTimerExit==1)
			{ 
				if (GE[1].sprite){ SPR_releaseSprite(GE[1].sprite); GE[1].sprite = NULL; }
				if (GE[2].sprite){ SPR_releaseSprite(GE[2].sprite); GE[2].sprite = NULL; }
				if (GE[3].sprite){ SPR_releaseSprite(GE[3].sprite); GE[3].sprite = NULL; }
				if (GE[4].sprite){ SPR_releaseSprite(GE[4].sprite); GE[4].sprite = NULL; }
				if (GE[5].sprite){ SPR_releaseSprite(GE[5].sprite); GE[5].sprite = NULL; }
				if (GE[6].sprite){ SPR_releaseSprite(GE[6].sprite); GE[6].sprite = NULL; }
				CLEAR_VDP(); 
				gFrames=0; 
				gRoom=4; 
			}
			
		}
		
		if(gRoom==4) //STAGE SELECT ---------------------------------------------------------------
		{
			FUNCAO_INPUT_SYSTEM(); //Verifica os joysticks 
			
			if(gFrames==1)
			{
				gInd_tileset=1;
			
				VDP_loadTileSet(bg_stage_select_bgb.tileset,gInd_tileset,DMA); 
				VDP_setTileMapEx(BG_B,bg_stage_select_bgb.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,gInd_tileset),0,0,0,0,40,28,DMA_QUEUE);
				VDP_setPalette(PAL0, bg_stage_select_bgb.palette->data);
				gInd_tileset += bg_stage_select_bgb.tileset->numTile;
				
				VDP_loadTileSet(bg_stage1_select_bga.tileset,gInd_tileset,DMA); 
				VDP_setTileMapEx(BG_A,bg_stage1_select_bga.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,gInd_tileset),13,11,0,0,16,8,DMA_QUEUE);
				VDP_setPalette(PAL1, bg_stage1_select_bga.palette->data);
				//gInd_tileset += bg_stage1_select_bga.tileset->numTile;
				
				//Elementos Graficos
				GE[1].sprite = SPR_addSpriteExSafe(&spr_BG_desc1,  104, 64, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
				VDP_setPalette(PAL3, spr_BG_desc1.palette->data);
				GE[2].sprite = SPR_addSpriteExSafe(&kuroko_shoulder, 104, 120, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
				VDP_setPalette(PAL2, kuroko_shoulder.palette->data);
				GE[3].sprite = SPR_addSpriteExSafe(&spr_stage_sel_icon,  104, 160, TILE_ATTR(PAL0, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
			}
			
			u8 atualizaMiniatura=0;
			u8 NumberOfStages = 3; //Inserir aqui o total de stages criados!
			
			if(P[1].key_JOY_RIGHT_status==1 || P[2].key_JOY_RIGHT_status==1)
			{
				atualizaMiniatura=1;
				gBG_Choice--;
				if(gBG_Choice<1){gBG_Choice=NumberOfStages;}
			}
			if(P[1].key_JOY_LEFT_status==1 || P[2].key_JOY_LEFT_status==1)
			{
				atualizaMiniatura=1;
				gBG_Choice++;
				if(gBG_Choice>NumberOfStages){gBG_Choice=1;}
			}
			if(atualizaMiniatura==1)
			{
				XGM_setPCM(P1_SFX, snd_cursor, sizeof(snd_cursor)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3);
				
				//move o cursor 'gps'
				if(gBG_Choice==3){ SPR_setPosition(GE[3].sprite, 120, 160); }
				if(gBG_Choice==2){ SPR_setPosition(GE[3].sprite, 136, 160); }
				if(gBG_Choice==1){ SPR_setPosition(GE[3].sprite, 104, 160); }
				
				if (GE[1].sprite){ SPR_releaseSprite(GE[1].sprite); GE[1].sprite = NULL; }
				if(gBG_Choice==1)
				{ 
					GE[1].sprite = SPR_addSpriteExSafe(&spr_BG_desc1,  104, 64, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					VDP_setPalette(PAL3, spr_BG_desc1.palette->data);
					
					VDP_loadTileSet(bg_stage1_select_bga.tileset,gInd_tileset,DMA); 
					VDP_setTileMapEx(BG_A,bg_stage1_select_bga.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,gInd_tileset),13,11,0,0,16,8,DMA_QUEUE);
					VDP_setPalette(PAL1, bg_stage1_select_bga.palette->data);
				}
				if(gBG_Choice==2)
				{
					GE[1].sprite = SPR_addSpriteExSafe(&spr_BG_desc2,  104, 64, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					VDP_setPalette(PAL3, spr_BG_desc1.palette->data);
					
					VDP_loadTileSet(bg_stage2_select_bga.tileset,gInd_tileset,DMA); 
					VDP_setTileMapEx(BG_A,bg_stage2_select_bga.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,gInd_tileset),13,11,0,0,16,8,DMA_QUEUE);
					VDP_setPalette(PAL1, bg_stage2_select_bga.palette->data);
				}
				if(gBG_Choice==3)
				{
					GE[1].sprite = SPR_addSpriteExSafe(&spr_BG_desc3,  104, 64, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
					VDP_setPalette(PAL3, spr_BG_desc1.palette->data);
					
					VDP_loadTileSet(bg_stage3_select_bga.tileset,gInd_tileset,DMA); 
					VDP_setTileMapEx(BG_A,bg_stage3_select_bga.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,gInd_tileset),13,11,0,0,16,8,DMA_QUEUE);
					VDP_setPalette(PAL1, bg_stage3_select_bga.palette->data);
				}
			}
			
			if(P[1].key_JOY_START_status==1 && gFrames>5){
				if (GE[1].sprite){ SPR_releaseSprite(GE[1].sprite); GE[1].sprite = NULL; }
				if (GE[2].sprite){ SPR_releaseSprite(GE[2].sprite); GE[2].sprite = NULL; }
				if (GE[3].sprite){ SPR_releaseSprite(GE[3].sprite); GE[3].sprite = NULL; }
				gRoom=9; gDescompressionExit=10;
				gFrames=1; 
				CLEAR_VDP();
			};
		}
		
		if(gRoom==9) //DESCOMPRESSION -------------------------------------------------------------
		{
			GE[1].sprite = SPR_addSpriteExSafe(&spr_point,  0, 225, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
			GE[2].sprite = SPR_addSpriteExSafe(&spr_point,  0, 225, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
			GE[3].sprite = SPR_addSpriteExSafe(&spr_point,  0, 225, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
			
			if(gFrames==20)
			{
				if (GE[1].sprite){ SPR_releaseSprite(GE[1].sprite); GE[1].sprite = NULL; }
				if (GE[2].sprite){ SPR_releaseSprite(GE[2].sprite); GE[2].sprite = NULL; }
				if (GE[3].sprite){ SPR_releaseSprite(GE[3].sprite); GE[3].sprite = NULL; }
				gRoom=gDescompressionExit;
				gFrames=1; 
				CLEAR_VDP();
			}
			
		}
		
		if(gRoom==10) //IN GAME -------------------------------------------------------------------
		{
			if(gFrames == 1){ FUNCAO_INICIALIZACAO(); } //Inicializacao
			if(gFrames<=355){ 
				FUNCAO_ROUND_INIT(); //Rotina de Letreiramento de inicio dos rounds
			}else{
				FUNCAO_RELOGIO(); //HUD relogio
				FUNCAO_BARRAS_DE_ENERGIA(); //HUD barras
			}    
			
			//libera os graficos dos sparks
			if(Spark1_countDown>0){ Spark1_countDown--; if(Spark1_countDown==1){ SPR_releaseSprite(Spark[1]); Spark[1] = NULL; } }
			if(Spark2_countDown>0){ Spark2_countDown--; if(Spark2_countDown==1){ SPR_releaseSprite(Spark[2]); Spark[2] = NULL; } }
			
			FUNCAO_INPUT_SYSTEM(); //Verifica os joysticks 
			
			FUNCAO_ANIMACAO(); //Atualiza animacao
			
			FUNCAO_FSM(); //FSM = Finite State Machine (Maquina de Estados)
			
			FUNCAO_PHYSICS(); //Funcoes de Fisica
			
			FUNCAO_CAMERA_BGANIM();
			
			FUNCAO_SAMSHOFX(); //Efeitos do jogo SS2
			
			if(gDebug == 1){ FUNCAO_DEBUG(); } //Debug
			static int img_num = 0;
			
			// //fase do joe
			
			// if(gBG_Choice==1)
			// {
			// 	if(gPing10 == 9)
			// 	{
			// 		VDP_setTileMapEx(BG_B,gfx_bgb1.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0),6,8,6,(28*img_num+8),45,13,DMA_QUEUE);
			// 		img_num++;
			// 		if(img_num == 4)
			// 		{
			// 			img_num = 0;
			// 		}
			// 	}
			// }
			
			//fase da praia
			// if(gBG_Choice==1)
			// {
				if(gPing10 == 9)
				{
					VDP_setTileMapEx(BG_B,Cenario_Tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0),0,12,0,(28*img_num+12),64,6,DMA_QUEUE);
					img_num++;
					if(img_num == 4)
					{
						img_num = 0;
					}
				}
			// }

			//fase do geese
			// if(gBG_Choice==1)
			// {
			// 	if(gPing10 == 9)
			// 	{
			// 		VDP_setTileMapEx(BG_B,gfx_bgb1.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0),14,7,14,(28*img_num+7),57,7,DMA_QUEUE);
			// 		img_num++;
			// 		if(img_num == 4)
			// 		{
			// 			img_num = 0;
			// 		}
			// 	}
			// }

			// if(gBG_Choice==2)
			// {
			// 	if(gPing10 == 9)
			// 	{
			// 		VDP_setTileMapEx(BG_B,gfx_bgb2.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0),22,3,22,(28*img_num+3),43,11,DMA_QUEUE);
			// 		img_num++;
			// 		if(img_num == 4)
			// 		{
			// 			img_num = 0;
			// 		}
			// 	}
			// }
			
			// if(gBG_Choice==3)
			// {
			// 	if(gPing10 == 9)
			// 	{
			// 		VDP_setTileMapEx(BG_B,gfx_bgb3.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0),12,9,12,(28*img_num+9),39,7,DMA_QUEUE);
			// 		img_num++;
			// 		if(img_num == 4)
			// 		{
			// 			img_num = 0;
			// 		}
			// 	}
				
			// }

			// //krauser
			// if(gBG_Choice==4)
			// {
			// 	if(gPing10 == 9)
			// 	{
			// 		VDP_setTileMapEx(BG_B,gfx_bgb4.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0),15,4,15,(28*img_num+4),49,10,DMA_QUEUE);
			// 		img_num++;
			// 		if(img_num == 4)
			// 		{
			// 			img_num = 0;
			// 		}
			// 	}
			// }
		}
		
		
		if(gRoom==11) //AFTER MATCH ---------------------------------------------------------------
		{
			FUNCAO_INPUT_SYSTEM(); //Verifica os joysticks 
			
			if(gFrames==1)
			{
				gContinueOption=1;
				gInd_tileset=1;
				gScrollValue=0;
				VDP_setHorizontalScroll(BG_B, gScrollValue);
				
				//BACKGROUND
				if(gBG_Choice==1)
				{
					
    

					/*
					VDP_loadTileSet(gfx_bgb1_win_screen.tileset,gInd_tileset,DMA); 
					VDP_setTileMapEx(BG_B,gfx_bgb1_win_screen.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,gInd_tileset),0,3,0,0,40,22,DMA_QUEUE);
					VDP_setPalette(PAL0, gfx_bgb1_win_screen.palette->data);
					gInd_tileset += gfx_bgb1_win_screen.tileset->numTile;
					*/
				}
				if(gBG_Choice==2)
				{
					/*
					VDP_loadTileSet(gfx_bgb2_win_screen.tileset,gInd_tileset,DMA); 
					VDP_setTileMapEx(BG_B,gfx_bgb2_win_screen.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,gInd_tileset),0,3,0,0,40,22,DMA_QUEUE);
					VDP_setPalette(PAL0, gfx_bgb2_win_screen.palette->data);
					gInd_tileset += gfx_bgb2_win_screen.tileset->numTile;
					*/
				}
				
				//WINNER
				if(gWinnerID==1) //terry
				{
					VDP_loadTileSet(bg_win_terry_bga.tileset,gInd_tileset,DMA); 
					VDP_setTileMapEx(BG_A,bg_win_terry_bga.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,gInd_tileset),0,0,0,0,40,28,DMA_QUEUE);
					VDP_setPalette(PAL0, bg_win_terry_bga.palette->data);
					gInd_tileset += bg_win_terry_bga.tileset->numTile;

					VDP_loadTileSet(bg_win_terry_bgb.tileset,gInd_tileset,DMA); 
					VDP_setTileMapEx(BG_B,bg_win_terry_bgb.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,gInd_tileset),0,0,0,0,40,28,DMA_QUEUE);
					VDP_setPalette(PAL1, bg_win_terry_bgb.palette->data);
					gInd_tileset += bg_win_terry_bgb.tileset->numTile;
				}
				if(gWinnerID==2) //mai
				{
					VDP_loadTileSet(bg_win_mai_bga.tileset,gInd_tileset,DMA); 
					VDP_setTileMapEx(BG_A,bg_win_mai_bga.tilemap,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,gInd_tileset),0,0,0,0,40,28,DMA_QUEUE);
					VDP_setPalette(PAL1, bg_win_mai_bga.palette->data);
					gInd_tileset += bg_win_mai_bga.tileset->numTile;

					VDP_loadTileSet(bg_win_mai_bgb.tileset,gInd_tileset,DMA); 
					VDP_setTileMapEx(BG_B,bg_win_mai_bgb.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,gInd_tileset),0,0,0,0,40,28,DMA_QUEUE);
					VDP_setPalette(PAL0, bg_win_mai_bgb.palette->data);
					gInd_tileset += bg_win_mai_bgb.tileset->numTile;
				}
				
				//Elementos Graficos
				if(gWinnerID==1) //terry
				{
					if(gPing2==0){ 
						//frase1
						GE[1].sprite = SPR_addSpriteExSafe(&spr_terry_win_phrase1,  40,  24, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
						//GE[2].sprite = SPR_addSpriteExSafe(&spr_terry_win_phrase1B,  168, 112, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
					}
					if(gPing2==1){ 
						//frase2
						GE[1].sprite = SPR_addSpriteExSafe(&spr_terry_win_phrase2,  40,  24, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
						//GE[2].sprite = SPR_addSpriteExSafe(&spr_terry_win_phrase2B,  168, 112, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
					}
					VDP_setPalette(PAL3, spr_terry_win_phrase1.palette->data);
				}
				if(gWinnerID==2) //mai
				{
					if(gPing2==0){ 
						//frase1
						//GE[1].sprite = SPR_addSpriteExSafe(&spr_mai_win_phrase1A,  168,  40, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
						//GE[2].sprite = SPR_addSpriteExSafe(&spr_mai_win_phrase1B,  168, 112, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
					}
					if(gPing2==1){ 
						//frase2
						//GE[1].sprite = SPR_addSpriteExSafe(&spr_mai_win_phrase2A,  168,  40, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
						//GE[2].sprite = SPR_addSpriteExSafe(&spr_mai_win_phrase2B,  168, 112, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
					}
					//VDP_setPalette(PAL3, spr_mai_win_phrase1A.palette->data);
				}
				if(gWinnerID==3) //bluemary
				{
					if(gPing2==0){ 
						//frase1
						//GE[1].sprite = SPR_addSpriteExSafe(&spr_bluemary_win_phrase1A,  168,  40, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
						//GE[2].sprite = SPR_addSpriteExSafe(&spr_bluemary_win_phrase1B,  168, 112, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
					}
					if(gPing2==1){ 
						//frase2
						//GE[1].sprite = SPR_addSpriteExSafe(&spr_bluemary_win_phrase2A,  168,  40, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
						//GE[2].sprite = SPR_addSpriteExSafe(&spr_bluemary_win_phrase2B,  168, 112, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
					}
					//VDP_setPalette(PAL3, spr_bluemary_win_phrase1A.palette->data);
				}
				
				//Continue; Rematch
				GE[3].sprite = SPR_addSpriteExSafe(&spr_continue, 54, 192, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
				GE[4].sprite = SPR_addSpriteExSafe(&spr_continue_yes, 190, 192, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
				GE[5].sprite = SPR_addSpriteExSafe(&spr_continue_no , 230, 192, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
				SPR_setDepth(GE[3].sprite, 255); 
				SPR_setDepth(GE[4].sprite, 1); 
				SPR_setDepth(GE[5].sprite, 1); 
			}
			
			//Continue?
			//"gContinueOption==0" NO
			//"gContinueOption==1" YES
			bool SoundContinueChoice = 0;
			if(P[1].key_JOY_LEFT_status ==1 || P[2].key_JOY_LEFT_status ==1){ gContinueOption=1; SoundContinueChoice=1; }
			if(P[1].key_JOY_RIGHT_status==1 || P[2].key_JOY_RIGHT_status==1){ gContinueOption=0; SoundContinueChoice=1; }
			if(SoundContinueChoice==1){ XGM_setPCM(P1_SFX, snd_cursor, sizeof(snd_cursor)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(gContinueOption==1){
				if(gPing10>=1 && gPing10<=6){ SPR_setVisibility(GE[4].sprite, VISIBLE); }
				if(gPing10>=7 && gPing10<=9){ SPR_setVisibility(GE[4].sprite, HIDDEN ); }
				SPR_setVisibility(GE[5].sprite, HIDDEN);
			}
			if(gContinueOption==0){
				if(gPing10>=1 && gPing10<=6){ SPR_setVisibility(GE[5].sprite, VISIBLE); }
				if(gPing10>=7 && gPing10<=9){ SPR_setVisibility(GE[5].sprite, HIDDEN ); }
				SPR_setVisibility(GE[4].sprite, HIDDEN);
			}
			if(P[1].key_JOY_START_status==1 && gFrames>5)
			{
				CLEAR_VDP();
				gFrames=0;
				if(gContinueOption==0){ gRoom=2; }
				if(gContinueOption==1){ gRoom=9; gDescompressionExit=10; }
			};
			
		}
		
		//--- FINALIZACOES ---//
		//VDP_showFPS(1);        //Mostra a taxa de FPS
		SPR_update();          //Atualiza (desenha) os sprites
        SYS_doVBlankProcess(); //Wait for screen refresh and do all SGDK VBlank tasks
    }
	
    return 0;
}

//--- FUNCOES ---//

void PLAYER_STATE(u8 Player, u16 State)
{
	SPR_releaseSprite(P[Player].sprite);
	P[Player].animFrame      = 1;
	P[Player].frameTimeAtual = 1;
	P[Player].dataAnim[1]    = 1;
	P[Player].animFrameTotal = 1;
	P[Player].state = State;
	
	FUNCAO_PLAY_SND(Player, State); //SOUND; SFX!
	
	//virando de lado (mudanca de estado)
	if(Player==1 && P[1].direcao== 1 && P[2].x<P[1].x && State==100){ State=607; }
	if(Player==1 && P[1].direcao==-1 && P[1].x<P[2].x && State==100){ State=607; }
	if(Player==2 && P[2].direcao== 1 && P[1].x<P[2].x && State==100){ State=607; }
	if(Player==2 && P[2].direcao==-1 && P[2].x<P[1].x && State==100){ State=607; }
	if(Player==1 && P[1].direcao== 1 && P[2].x<P[1].x && State==200){ State=608; }
	if(Player==1 && P[1].direcao==-1 && P[1].x<P[2].x && State==200){ State=608; }
	if(Player==2 && P[2].direcao== 1 && P[1].x<P[2].x && State==200){ State=608; }
	if(Player==2 && P[2].direcao==-1 && P[2].x<P[1].x && State==200){ State=608; }
	
	//--- DEBUG_CHARACTER_TEST ---// #ID:0
	if(P[Player].id==0)
	{
		if(State==100 || State==610){
			P[Player].y = gAlturaPiso;
			P[Player].w = 8;
			P[Player].h = 8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_point, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
	}
	
	//--- TERRY ---// #ID:1
	if(P[Player].id==1)
	{
		if(State==100){
			P[Player].w = 9*8;
			P[Player].h = 14*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8;
			P[Player].dataAnim[1]  = 5;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 5;
			P[Player].dataAnim[6]  = 5;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_100, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==101){
			P[Player].w = 17*8;
			P[Player].h = 14*8;
			P[Player].axisX = ((P[Player].w)/2)+27;
			P[Player].axisY =  (P[Player].h)-8;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_101, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==102){
			P[Player].w = 17*8;
			P[Player].h = 14*8;
			P[Player].axisX = ((P[Player].w)/2)+27;
			P[Player].axisY =  (P[Player].h)-8;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_101, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==103){
			P[Player].w = 17*8;
			P[Player].h = 14*8;
			P[Player].axisX = ((P[Player].w)/2)+27;
			P[Player].axisY =  (P[Player].h)-8;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_101, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==104){
			P[Player].w = 16*8;
			P[Player].h = 14*8;
			P[Player].axisX = ((P[Player].w)/2)+24;
			P[Player].axisY =  (P[Player].h)-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 5;
			P[Player].animFrameTotal = 7;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_104, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==105){
			P[Player].w = 16*8;
			P[Player].h = 14*8;
			P[Player].axisX = ((P[Player].w)/2)+24;
			P[Player].axisY =  (P[Player].h)-5;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 5;
			P[Player].animFrameTotal = 7;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_104, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==106){
			P[Player].w = 14*8;
			P[Player].h = 14*8;
			P[Player].axisX = ((P[Player].w)/2)+38;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 8;
			P[Player].dataAnim[6]  = 7;
			P[Player].dataAnim[7]  = 6;
			P[Player].dataAnim[8]  = 5;
			P[Player].animFrameTotal = 8;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_106, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==107){
			P[Player].w = 9*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_107, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==108){
			P[Player].w = 9*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 12;

			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_108, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==109){
			P[Player].w = 9*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_109, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==110){
			P[Player].w = 9*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 12;

			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_108, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==151){
			P[Player].w = 17*8;
			P[Player].h = 14*8;
			P[Player].axisX = ((P[Player].w)/2)+30;
			P[Player].axisY =  (P[Player].h)-11;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 4;
			
			
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_151, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==152){
			P[Player].w = 17*8;
			P[Player].h = 14*8;
			P[Player].axisX = ((P[Player].w)/2)+30;
			P[Player].axisY =  (P[Player].h)-11;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 4;
			
			
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_151, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==153){
			P[Player].w = 17*8;
			P[Player].h = 14*8;
			P[Player].axisX = ((P[Player].w)/2)+30;
			P[Player].axisY =  (P[Player].h)-11;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 4;
			
			
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_151, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==154){
			P[Player].w = 13*8;
			P[Player].h = 13*8;
			P[Player].axisX = ((P[Player].w)/2)+14;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 5;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_154, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==155){
			P[Player].w = 13*8;
			P[Player].h = 13*8;
			P[Player].axisX = ((P[Player].w)/2)+14;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 5;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_154, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==156){
			P[Player].w = 14*8;
			P[Player].h = 17*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h)-8;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 3;
			P[Player].dataAnim[9]  = 3;
			P[Player].animFrameTotal = 9;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_156, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==200){
			P[Player].w = 7*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)-10;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_200, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==201){
			P[Player].w = 12*8;
			P[Player].h = 10*8;
			P[Player].axisX = (P[Player].w)-40;
			P[Player].axisY =  P[Player].h-8;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 1;
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_201, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}		
		if(State==202){
			P[Player].w = 17*8;
			P[Player].h = 9*8;
			P[Player].axisX = ((P[Player].w)/2)+32;
			P[Player].axisY =  (P[Player].h)-2;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 1;
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_201, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}		
		if(State==203){
			P[Player].w = 17*8;
			P[Player].h = 16*8;
			P[Player].axisX = ((P[Player].w)/2)+37;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 2;
			P[Player].dataAnim[9]  = 2;
			P[Player].animFrameTotal = 9;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_203, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==204){
			P[Player].w = 15*8;
			P[Player].h = 11*8;
			P[Player].axisX = ((P[Player].w)/2)+6;
			P[Player].axisY =  (P[Player].h)-8;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_204, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==205){
			P[Player].w = 17*8;
			P[Player].h = 10*8;
			P[Player].axisX = ((P[Player].w)/2)+26;
			P[Player].axisY =  (P[Player].h)-2;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_204, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==206){
			P[Player].w = 15*8;
			P[Player].h = 11*8;
			P[Player].axisX = (P[Player].w)-32;
			P[Player].axisY =  (P[Player].h)-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 6;
			P[Player].dataAnim[6]  = 8;
			P[Player].dataAnim[7]  = 8;
			P[Player].dataAnim[8]  = 4;
			P[Player].animFrameTotal = 8;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_206, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==207){
			P[Player].w = 9*8;
			P[Player].h = 9*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h ;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_207, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==208){
			P[Player].w = 9*8;
			P[Player].h = 9*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h ;
			P[Player].dataAnim[1]  = 12;
			
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_208, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==209){
			P[Player].w = 9*8;
			P[Player].h = 9*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h ;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_209, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==210){
			P[Player].w = 9*8;
			P[Player].h = 9*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h ;
			P[Player].dataAnim[1]  = 12;
			
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_208, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==300){
			P[Player].w = 10*8;
			P[Player].h = 16*8;
			P[Player].axisX = ((P[Player].w)/2)+13;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 5;
			P[Player].dataAnim[2]  = 7;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 99;
			P[Player].animFrameTotal = 5;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_300, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==301 || State==311 || State==321){
			P[Player].w = 16*8;
			P[Player].h = 13*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h)+8;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 20;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 99;
			
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_301, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==302 || State==312 || State==322){
			P[Player].w = 17*8;
			P[Player].h = 12*8;
			P[Player].axisX = ((P[Player].w)/2)+28;
			P[Player].axisY =  (P[Player].h)+20;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 20;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 99;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe
(&spr_terry_304, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==303 || State==313 || State==323){
			P[Player].w = 17*8;
			P[Player].h = 12*8;
			P[Player].axisX = ((P[Player].w)/2)+28;
			P[Player].axisY =  (P[Player].h)+20;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 10;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 99;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_306, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==304 || State==314 || State==324){
			P[Player].w = 15*8;
			P[Player].h = 17*8;
			P[Player].axisX = ((P[Player].w)/2)+28;
			P[Player].axisY =  (P[Player].h)-8;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 20;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 99;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_304, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==305 || State==315 || State==325){
			P[Player].w = 15*8;
			P[Player].h = 17*8;
			P[Player].axisX = ((P[Player].w)/2)+28;
			P[Player].axisY =  (P[Player].h)-8;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 20;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 99;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_304, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==306 || State==316 || State==326){
			P[Player].w = 17*8;
			P[Player].h = 12*8;
			P[Player].axisX = ((P[Player].w)/2)+28;
			P[Player].axisY =  (P[Player].h)+20;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 10;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 99;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_306, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==310){
			P[Player].w = 12*8;
			P[Player].h = 16*8;
			P[Player].axisX = ((P[Player].w)/2)+4;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 99;
			P[Player].animFrameTotal = 8;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_310, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==320){
			P[Player].w = 12*8;
			P[Player].h = 16*8;
			P[Player].axisX = ((P[Player].w)/2)+13;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 99;
			P[Player].animFrameTotal = 8;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_320, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==410){
			P[Player].w = 11*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2);
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 7;
			P[Player].dataAnim[3]  = 7;
			P[Player].dataAnim[4]  = 7;
			P[Player].dataAnim[5]  = 7;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_410, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==420){
			P[Player].w = 10*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2);
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 7;
			P[Player].dataAnim[3]  = 7;
			P[Player].dataAnim[4]  = 7;
			P[Player].dataAnim[5]  = 7;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_420, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==470){
			P[Player].w = 11*8;
			P[Player].h = 11*8;
			P[Player].axisX = ((P[Player].w)/2)+0;
			P[Player].axisY =  (P[Player].h)-4;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_470, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==471){
			P[Player].w = 18*8;
			P[Player].h = 15*8;
			P[Player].axisX = ((P[Player].w)/2)+13;
			P[Player].axisY =  (P[Player].h)-4;
			P[Player].dataAnim[1]  = 7;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 5;
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_471, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==472){
			P[Player].w = 7*8;
			P[Player].h = 15*8;
			P[Player].axisX = ((P[Player].w)/2)-28;
			P[Player].axisY =  (P[Player].h)-16;
			P[Player].dataAnim[1]  = 7;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 4;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_472, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==501){
			P[Player].w = 11*8;
			P[Player].h = 12*8;
			P[Player].axisX = ((P[Player].w)/2)-1;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 6;
			P[Player].animFrameTotal = 1;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_501, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==502){
			P[Player].w = 11*8;
			P[Player].h = 12*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].animFrameTotal = 3;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_502, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==503){
			P[Player].w = 11*8;
			P[Player].h = 12*8;
			P[Player].axisX = ((P[Player].w)/2)-2;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 9;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 9;
			P[Player].animFrameTotal = 3;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_503, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==506){
			P[Player].w = 11*8;
			P[Player].h = 12*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 19;
			P[Player].animFrameTotal = 1;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_506, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==507){
			P[Player].w = 12*8;
			P[Player].h = 12*8;
			P[Player].axisX = ((P[Player].w)/2)-8;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 9;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 9;
			P[Player].animFrameTotal = 3;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_507, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==508){
			P[Player].w = 12*8;
			P[Player].h = 14*8;
			P[Player].axisX = ((P[Player].w)/2)-12;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 4;//33
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 18;
			
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_508, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==550){
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].y-=4*8; //Compensa o deslocamento do eixo Y (.axisY)
			P[Player].w = 14*8;
			P[Player].h = 11*8;
			P[Player].axisX = ((P[Player].w)/2);
			P[Player].axisY =  (P[Player].h)-4*8;
			P[Player].dataAnim[1]  = 9;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 9;
			P[Player].dataAnim[4]  = 99;
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_550, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==551){
			P[Player].w = 15*8;
			P[Player].h =  7*8;
			P[Player].axisX = ((P[Player].w)/2);
			P[Player].axisY =  (P[Player].h)-3*8;
			P[Player].dataAnim[1]  = 99;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_551, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==552){
			P[Player].w = 16*8;
			P[Player].h = 15*8;
			P[Player].axisX = ((P[Player].w)/2);
			P[Player].axisY =  (P[Player].h)-2*8;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_552, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==570){
			P[Player].w = 15*8;
			P[Player].h =  5*8;
			P[Player].axisX = ((P[Player].w)/2);
			P[Player].axisY =  (P[Player].h)-2*8;
			P[Player].dataAnim[1]  = 60;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_570, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==601){
			P[Player].w = 7*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)-10;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 2;
			P[Player].animFrameTotal = 2;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_601, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==602){
			P[Player].w = 7*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)-10;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 1;
			P[Player].animFrameTotal = 2;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_602, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==603 || State==604 || State==605){
			P[Player].w = 10*8;
			P[Player].h = 15*8;
			P[Player].axisX = (P[Player].w/2)+1;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 2;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_604, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==606){
			P[Player].w = 6*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)-14;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].animFrameTotal = 3;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_606, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==607){
			P[Player].w = 9*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2);
			P[Player].axisY = P[Player].h -4;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].animFrameTotal = 2;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_607, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==610){
			P[Player].w = 10*8;
			P[Player].h = 14*8;
			P[Player].axisX = ((P[Player].w)/2);
			P[Player].axisY =  (P[Player].h)-11;
			P[Player].dataAnim[1]  = 5;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 10;
			P[Player].dataAnim[5]  = 10;
			P[Player].dataAnim[6]  = 10;
			P[Player].dataAnim[7]  = 10;
			P[Player].dataAnim[8]  = 10;
			P[Player].dataAnim[9]  = 10;
			P[Player].dataAnim[10] = 10;
			P[Player].dataAnim[11] = 10;
			P[Player].dataAnim[12] = 10;
			P[Player].dataAnim[13] = 5;
			P[Player].dataAnim[14] = 5;
			P[Player].animFrameTotal = 14;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_610, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==611){
			P[Player].w = 12*8;
			P[Player].h = 16*8;
			P[Player].axisX = ((P[Player].w)/2);
			P[Player].axisY =  (P[Player].h)-11;
			P[Player].dataAnim[1]  = 10;
			P[Player].dataAnim[2]  = 10;
			P[Player].dataAnim[3]  = 10;
			P[Player].dataAnim[4]  = 10;
			P[Player].dataAnim[5]  = 10;
			P[Player].dataAnim[6]  = 10;
			P[Player].dataAnim[7]  = 10;
			P[Player].dataAnim[8]  = 10;
			P[Player].dataAnim[9]  = 10;
			P[Player].dataAnim[10]  = 10;
			P[Player].dataAnim[11]  = 10;
			P[Player].dataAnim[12]  = 10;
			P[Player].dataAnim[13]  = 5;
			P[Player].dataAnim[14]  = 60;
			
			P[Player].animFrameTotal = 14;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_611, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==612){
			P[Player].w = 12*8;
			P[Player].h = 16*8;
			P[Player].axisX = ((P[Player].w)/2);
			P[Player].axisY =  (P[Player].h)-11;
			P[Player].dataAnim[1]  = 10;
			P[Player].dataAnim[2]  = 10;
			P[Player].dataAnim[3]  = 10;
			P[Player].dataAnim[4]  = 10;
			P[Player].dataAnim[5]  = 10;
			P[Player].dataAnim[6]  = 10;
			P[Player].dataAnim[7]  = 10;
			P[Player].dataAnim[8]  = 10;
			P[Player].dataAnim[9]  = 10;
			P[Player].dataAnim[10]  = 10;
			P[Player].dataAnim[11]  = 10;
			P[Player].dataAnim[12]  = 10;
			P[Player].dataAnim[13]  = 5;
			P[Player].dataAnim[14]  = 60;
			
			P[Player].animFrameTotal = 14;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_611, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==613){
			P[Player].w = 12*8;
			P[Player].h = 16*8;
			P[Player].axisX = ((P[Player].w)/2);
			P[Player].axisY =  (P[Player].h)-11;
			P[Player].dataAnim[1]  = 10;
			P[Player].dataAnim[2]  = 10;
			P[Player].dataAnim[3]  = 10;
			P[Player].dataAnim[4]  = 10;
			P[Player].dataAnim[5]  = 10;
			P[Player].dataAnim[6]  = 10;
			P[Player].dataAnim[7]  = 10;
			P[Player].dataAnim[8]  = 10;
			P[Player].dataAnim[9]  = 10;
			P[Player].dataAnim[10]  = 10;
			P[Player].dataAnim[11]  = 10;
			P[Player].dataAnim[12]  = 10;
			P[Player].dataAnim[13]  = 5;
			P[Player].dataAnim[14]  = 60;
			
			P[Player].animFrameTotal = 14;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_611, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==614){
			P[Player].w = 12*8;
			P[Player].h = 16*8;
			P[Player].axisX = ((P[Player].w)/2);
			P[Player].axisY =  (P[Player].h)-11;
			P[Player].dataAnim[1]  = 10;
			P[Player].dataAnim[2]  = 10;
			P[Player].dataAnim[3]  = 10;
			P[Player].dataAnim[4]  = 10;
			P[Player].dataAnim[5]  = 10;
			P[Player].dataAnim[6]  = 10;
			P[Player].dataAnim[7]  = 10;
			P[Player].dataAnim[8]  = 10;
			P[Player].dataAnim[9]  = 10;
			P[Player].dataAnim[10]  = 10;
			P[Player].dataAnim[11]  = 10;
			P[Player].dataAnim[12]  = 10;
			P[Player].dataAnim[13]  = 5;
			P[Player].dataAnim[14]  = 60;
			
			P[Player].animFrameTotal = 14;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_611, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==700){
			if(Spark1_countDown>0){ SPR_releaseSprite(Spark[1]); }
			if(Spark2_countDown>0){ SPR_releaseSprite(Spark[2]); }
			P[Player].w = 12*8;
			P[Player].h = 15*8;
			P[Player].axisX = ((P[Player].w)/2)+1;
			P[Player].axisY =  (P[Player].h)-1;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 3;
			P[Player].dataAnim[9]  = 2;
			P[Player].dataAnim[10] = 2;
			P[Player].dataAnim[11] = 2;
			P[Player].dataAnim[12] = 2;
			P[Player].dataAnim[13] = 3;
			P[Player].dataAnim[14] = 99;
			
		
			P[Player].animFrameTotal = 14;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_700, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==710){
			if(Spark1_countDown>0){ SPR_releaseSprite(Spark[1]); }
			if(Spark2_countDown>0){ SPR_releaseSprite(Spark[2]); }
			P[Player].w = 17*8;
			P[Player].h = 13*8;
			P[Player].axisX = ((P[Player].w)/2)+28;
			P[Player].axisY =  (P[Player].h)+8;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 33;
			P[Player].dataAnim[9]  = 3;
			P[Player].dataAnim[10] = 3;
			
			
			P[Player].animFrameTotal = 10;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_710, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==713){
			
			P[Player].axisX = (P[Player].w)-40;
			P[Player].axisY =  (P[Player].h)+40;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 3;
			P[Player].dataAnim[9]  = 3;
			P[Player].dataAnim[10] = 3;
			P[Player].dataAnim[11] = 3;
			P[Player].dataAnim[12] = 3;
			P[Player].dataAnim[13] = 10;
			P[Player].dataAnim[14] = 10;
			P[Player].dataAnim[15] = 3;
			
			P[Player].animFrameTotal = 15;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_713, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==720){
			// terry crack shoot
	
			P[Player].w = 13*8;
			P[Player].h = 19*8;
			P[Player].axisX = (P[Player].w)-40;
			P[Player].axisY =  (P[Player].h)-8;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 2;
			P[Player].dataAnim[7]  = 2;
			P[Player].dataAnim[8]  = 2;
			P[Player].dataAnim[9]  = 2;
			P[Player].dataAnim[10] = 3;
			P[Player].dataAnim[11] = 3;
			P[Player].dataAnim[12] = 3;
			P[Player].dataAnim[13] = 10;
			P[Player].dataAnim[14] = 10;
			P[Player].dataAnim[15] = 3;
			P[Player].animFrameTotal = 15;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_713, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==800){
			if(Spark1_countDown>0){ SPR_releaseSprite(Spark[1]); }
			if(Spark2_countDown>0){ SPR_releaseSprite(Spark[2]); }
			P[Player].w = 17*8;
			P[Player].h = 13*8;
			P[Player].axisX = ((P[Player].w)/2)+28;
			P[Player].axisY =  (P[Player].h)-1;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 2;
			P[Player].dataAnim[7]  = 2;
			P[Player].dataAnim[8]  = 2;
			P[Player].dataAnim[9]  = 16;
			P[Player].dataAnim[10] = 2;
			P[Player].dataAnim[11] = 2;
			P[Player].dataAnim[12] = 2;
			
			P[Player].animFrameTotal = 12;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_800, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==810){
			P[Player].axisX = (P[Player].w)-42;
			P[Player].axisY =  (P[Player].h)+5;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 2;
			P[Player].dataAnim[7]  = 2;
			P[Player].dataAnim[8]  = 2;
			P[Player].dataAnim[9]  = 2;
			P[Player].dataAnim[10] = 2;
			P[Player].dataAnim[11] = 2;
			P[Player].dataAnim[12] = 2;
			P[Player].dataAnim[13] = 2;
			P[Player].dataAnim[14] = 8;
			P[Player].dataAnim[15] = 8;
			
			P[Player].animFrameTotal = 15;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_terry_810, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
	}
	
	//--- MAI ---// #ID:2
	if(P[Player].id==2)
	{
		if(State==100){
			P[Player].w = 9*8;
			P[Player].h = 12*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 5;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 5;
			P[Player].dataAnim[6]  = 5;
			P[Player].dataAnim[7]  = 5;
			P[Player].dataAnim[8]  = 5;
			P[Player].animFrameTotal = 8;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_100, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==101){
			P[Player].w = 16*8;
			P[Player].h = 16*8;
			P[Player].axisX = P[Player].w-24;
			P[Player].axisY = P[Player].h-8;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 2;
			P[Player].dataAnim[7]  = 2;
			P[Player].dataAnim[8]  = 2;
			P[Player].animFrameTotal = 8;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_101, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==104){
			P[Player].w = 15*8;
			P[Player].h = 12*8;
			P[Player].axisX = P[Player].w;
			P[Player].axisY =  P[Player].h-8;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 2;
			P[Player].animFrameTotal = 7;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_104, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==106){
			P[Player].w = 17*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w - 4*8;
			P[Player].axisY =  P[Player].h-1*8;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 5;
			P[Player].dataAnim[6]  = 7;
			P[Player].dataAnim[7]  = 8;
			
			P[Player].animFrameTotal = 7;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_106, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==107){
			P[Player].w = 7*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_107, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==108){
			P[Player].w = 7*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 250;
			
			
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_108, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==109){
			P[Player].w = 7*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_109, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==110){
			P[Player].w = 7*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 12;
			
			
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_108, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==151){
			P[Player].w = 12*8;
			P[Player].h = 12*8;
			P[Player].axisX = P[Player].w- 4*8;
			P[Player].axisY =  P[Player].h-1*8;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 2;
			P[Player].dataAnim[7]  = 2;
			
			P[Player].animFrameTotal = 7;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_151, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==154){
			P[Player].w = 15*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w - 4*8;
			P[Player].axisY =  P[Player].h-1*8;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_154, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==156){
			P[Player].w = 11*8;
			P[Player].h = 16*8;
			P[Player].axisX = P[Player].w-16;
			P[Player].axisY =  P[Player].h -8;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].animFrameTotal = 8;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_156, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==160){
			P[Player].w = 16*8;
			P[Player].h = 18*8;
			P[Player].axisX = P[Player].w-32;
			P[Player].axisY =  P[Player].h -8;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 1;
			P[Player].dataAnim[7]  = 10;
			
			P[Player].animFrameTotal = 7;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_160, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==161){
			P[Player].w = 16*8;
			P[Player].h = 18*8;
			P[Player].axisX = P[Player].w-24;
			P[Player].axisY =  P[Player].h -8;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 1;
			P[Player].dataAnim[7]  = 10;
			
			P[Player].animFrameTotal = 7;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_161, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==162){
			P[Player].w = 16*8;
			P[Player].h = 18*8;
			P[Player].axisX = P[Player].w-32;
			P[Player].axisY =  P[Player].h -8;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 1;
			P[Player].dataAnim[7]  = 10;
			
			P[Player].animFrameTotal = 7;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_160, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==163){
			P[Player].w = 16*8;
			P[Player].h = 18*8;
			P[Player].axisX = P[Player].w-24;
			P[Player].axisY =  P[Player].h -8;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 1;
			P[Player].dataAnim[7]  = 10;
			
			P[Player].animFrameTotal = 7;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_161, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==164){
			P[Player].w = 22*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w-32;
			P[Player].axisY =  P[Player].h -8;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 1;
			P[Player].dataAnim[7]  = 1;
			P[Player].dataAnim[8]  = 1;
			P[Player].dataAnim[9]  = 14;
			
			P[Player].animFrameTotal = 9;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_164, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==165){
			P[Player].w = 15*8;
			P[Player].h = 12*8;
			P[Player].axisX = P[Player].w-16;
			P[Player].axisY =  P[Player].h-8;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 1;
			P[Player].dataAnim[5]  = 1;
			P[Player].dataAnim[6]  = 1;
			P[Player].dataAnim[7]  = 10;
			P[Player].animFrameTotal = 7;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_104, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==166){
			P[Player].w = 16*8;
			P[Player].h = 16*8;
			P[Player].axisX = P[Player].w-32;
			P[Player].axisY =  P[Player].h -8;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 2;
			P[Player].dataAnim[7]  = 2;
			P[Player].dataAnim[8]  = 2;
			P[Player].dataAnim[9]  = 8;
			
			P[Player].animFrameTotal = 9;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_166, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==200){
			P[Player].w = 8*8;
			P[Player].h = 8*8;
			P[Player].axisX = P[Player].w - 25;
			P[Player].axisY = P[Player].h - 8;
			P[Player].dataAnim[1]  = 250;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_200, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==201){
			P[Player].w = 12*8;
			P[Player].h = 9*8;
			P[Player].axisX = P[Player].w - 20;
			P[Player].axisY = P[Player].h - 10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 2;
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_201, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==204){
			P[Player].w = 12*8;
			P[Player].h = 9*8;
			P[Player].axisX = P[Player].w - 20;
			P[Player].axisY = P[Player].h - 10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 2;
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_204, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==206){
			P[Player].w = 14*8;
			P[Player].h = 8*8;
			P[Player].axisX = P[Player].w - 20;
			P[Player].axisY = P[Player].h - 10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 2;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_206, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==207){
			P[Player].w = 8*8;
			P[Player].h = 9*8;
			P[Player].axisX = P[Player].w - 20;
			P[Player].axisY = P[Player].h - 10;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_207, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==208){
			P[Player].w = 8*8;
			P[Player].h = 9*8;
			P[Player].axisX = P[Player].w - 20;
			P[Player].axisY = P[Player].h - 10;
			P[Player].dataAnim[1]  = 250;
			
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_208, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==209){
			P[Player].w = 8*8;
			P[Player].h = 9*8;
			P[Player].axisX = P[Player].w - 20;
			P[Player].axisY = P[Player].h - 10;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_209, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==210){
			P[Player].w = 8*8;
			P[Player].h = 9*8;
			P[Player].axisX = P[Player].w - 20;
			P[Player].axisY = P[Player].h - 10;
			P[Player].dataAnim[1]  = 12;
			
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_208, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==300){
			P[Player].w = 7*8;
			P[Player].h = 17*8;
			P[Player].axisX = P[Player].w -16;
			P[Player].axisY =  P[Player].h - 16;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 99;
			P[Player].animFrameTotal = 7;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_300, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==301 || State==311 || State==321){
			P[Player].w = 14*8;
			P[Player].h = 13*8;
			P[Player].axisX = ((P[Player].w)/2)+4;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 10;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 99;
			
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_301, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==304 || State==314 || State==324){
			P[Player].w = 13*8;
			P[Player].h = 14*8;
			P[Player].axisX = ((P[Player].w)/2);
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 8;
			P[Player].dataAnim[5]  = 99;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_304, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==306 || State==316 || State==326){
			P[Player].w = 14*8;
			P[Player].h = 11*8;
			P[Player].axisX = ((P[Player].w)/2)+28;
			P[Player].axisY =  (P[Player].h)+20;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 99;
			
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_306, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==310){
			P[Player].w = 14*8;
			P[Player].h = 16*8;
			P[Player].axisX = ((P[Player].w)/2)-5;
			P[Player].axisY =  (P[Player].h)-16;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 3;
			P[Player].dataAnim[9]  = 3;
			P[Player].dataAnim[10]  = 3;
			P[Player].dataAnim[11]  = 3;
			P[Player].dataAnim[12]  = 3;
			P[Player].dataAnim[13]  = 99;
			P[Player].animFrameTotal = 13;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_310, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==320){
			P[Player].w = 14*8;
			P[Player].h = 16*8;
			P[Player].axisX = ((P[Player].w)/2)+13;
			P[Player].axisY =  (P[Player].h)-16;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 3;
			P[Player].dataAnim[9]  = 3;
			P[Player].dataAnim[10]  = 3;
			P[Player].dataAnim[11]  = 3;
			P[Player].dataAnim[12]  = 3;
			P[Player].dataAnim[13]  = 99;
			P[Player].animFrameTotal = 13;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_320, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==330){
			P[Player].w = 7*8;
			P[Player].h = 17*8;
			P[Player].axisX = P[Player].w -16;
			P[Player].axisY =  P[Player].h - 16;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 99;
			
			P[Player].animFrameTotal = 6;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_330, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==410){
			P[Player].w = 8*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2);
			P[Player].axisY = P[Player].h-8;
			P[Player].dataAnim[1]  = 5;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 5;
			P[Player].dataAnim[5]  = 5;
			P[Player].dataAnim[5]  = 5;
			P[Player].animFrameTotal = 7;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_410, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==420){
			P[Player].w = 8*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2);
			P[Player].axisY = P[Player].h-8;
			P[Player].dataAnim[1]  = 5;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 5;
			P[Player].dataAnim[5]  = 5;
			P[Player].dataAnim[5]  = 5;
			P[Player].animFrameTotal = 7;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_420, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==471){
			P[Player].w = 11*8;
			P[Player].h = 12*8;
			P[Player].axisX = P[Player].w-48;
			P[Player].axisY =  P[Player].h-3;
			P[Player].dataAnim[1]  = 5;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_471, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==472){
			P[Player].w = 11*8;
			P[Player].h = 12*8;
			P[Player].axisX = P[Player].w-32;
			P[Player].axisY =  P[Player].h-8;
			P[Player].dataAnim[1]  = 5;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_472, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==501){
			P[Player].w = 10*8;
			P[Player].h = 11*8;
			P[Player].axisX = ((P[Player].w)/2)-1;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 6;
			P[Player].animFrameTotal = 1;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_501, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==502){
			P[Player].w = 9*8;
			P[Player].h = 12*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h)-8;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].animFrameTotal = 3;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_502, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==503){
			P[Player].w = 9*8;
			P[Player].h = 12*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h)-8;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].animFrameTotal = 3;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_502, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==506){
			P[Player].w = 10*8;
			P[Player].h = 11*8;
			P[Player].axisX = ((P[Player].w)/2)-1;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 18;
			P[Player].animFrameTotal = 1;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_501, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==507){
			P[Player].w = 11*8;
			P[Player].h = 11*8;
			P[Player].axisX = P[Player].w -40;
			P[Player].axisY =  P[Player].h -8;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].animFrameTotal = 3;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_507, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==550){
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].y-=0*8; //Compensa o deslocamento do eixo Y (.axisY)
			P[Player].w = 13*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w -72;
			P[Player].axisY =  P[Player].h - 10;
			P[Player].dataAnim[1]  = 9;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 9;
			P[Player].dataAnim[4]  = 99;
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_550, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==551){
			P[Player].w = 16*8;
			P[Player].h =  6*8;
			P[Player].axisX = ((P[Player].w)/2);
			P[Player].axisY =  (P[Player].h)-3*8;
			P[Player].dataAnim[1]  = 20;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_551, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==552){
			P[Player].w = 13*8;
			P[Player].h = 13*8;
			P[Player].axisX = ((P[Player].w)/2);
			P[Player].axisY =  (P[Player].h)-2*8;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_552, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==555){
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].y-=0*8; //Compensa o deslocamento do eixo Y (.axisY)
			P[Player].w = 13*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w -72;
			P[Player].axisY =  P[Player].h - 10;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 3;
			P[Player].dataAnim[9]  = 3;
			P[Player].dataAnim[10]  = 3;
			P[Player].dataAnim[11]  = 3;
			P[Player].dataAnim[12]  = 99;
			
			P[Player].animFrameTotal = 12;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_555, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR( P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
			//Se o P1 estiver no estado 555, entao quem atacou foi o P2...
			//if(Player==1 && P[2].palID==1){VDP_setPalette(PAL2, spr_mai_pal1.palette ->data);}	
			//if(Player==1 && P[2].palID==2){VDP_setPalette(PAL2, spr_mai_pal2.palette ->data);}	
			//if(Player==1 && P[2].palID==3){VDP_setPalette(PAL2, spr_mai_pal3.palette ->data);}	
			//if(Player==1 && P[2].palID==4){VDP_setPalette(PAL2, spr_mai_pal4.palette ->data);}	
			//if(Player==1 && P[2].palID==5){VDP_setPalette(PAL2, spr_mai_pal5.palette ->data);}	
			//if(Player==1 && P[2].palID==6){VDP_setPalette(PAL2, spr_mai_pal6.palette ->data);}	
			//if(Player==1 && P[2].palID==7){VDP_setPalette(PAL2, spr_mai_pal7.palette ->data);}	
			//if(Player==1 && P[2].palID==8){VDP_setPalette(PAL2, spr_mai_pal8.palette ->data);}	
			//Se o P2 estiver no estado 555, entao quem atacou foi o P1...
			//if(Player==2 && P[1].palID==1){VDP_setPalette(PAL3, spr_mai_pal1.palette ->data);}	
			//if(Player==2 && P[1].palID==2){VDP_setPalette(PAL3, spr_mai_pal2.palette ->data);}	
			//if(Player==2 && P[1].palID==3){VDP_setPalette(PAL3, spr_mai_pal3.palette ->data);}	
			//if(Player==2 && P[1].palID==4){VDP_setPalette(PAL3, spr_mai_pal4.palette ->data);}	
			//if(Player==2 && P[1].palID==5){VDP_setPalette(PAL3, spr_mai_pal5.palette ->data);}	
			//if(Player==2 && P[1].palID==6){VDP_setPalette(PAL3, spr_mai_pal6.palette ->data);}	
			//if(Player==2 && P[1].palID==7){VDP_setPalette(PAL3, spr_mai_pal7.palette ->data);}	
			//if(Player==2 && P[1].palID==8){VDP_setPalette(PAL3, spr_mai_pal8.palette ->data);}	
		}
		if(State==570){
			P[Player].w = 16*8;
			P[Player].h =  6*8;
			P[Player].axisX = ((P[Player].w)/2);
			P[Player].axisY =  (P[Player].h)-4*8;
			P[Player].dataAnim[1]  = 20;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_551, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==601){
			P[Player].w = 8*8;
			P[Player].h = 14*8;
			P[Player].axisX = P[Player].w-28;
			P[Player].axisY = P[Player].h -8;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 2;
			P[Player].animFrameTotal = 2;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_601, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==602){
			P[Player].w = 7*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)-2;
			P[Player].axisY = P[Player].h;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 1;
			P[Player].animFrameTotal = 2;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_602, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
			//Atualiza paleta do P1
			if(Player==1 && P[1].palID==1){VDP_setPalette(PAL2, spr_mai_pal1.palette ->data);}	
			if(Player==1 && P[1].palID==2){VDP_setPalette(PAL2, spr_mai_pal2.palette ->data);}	
			if(Player==1 && P[1].palID==3){VDP_setPalette(PAL2, spr_mai_pal3.palette ->data);}	
			if(Player==1 && P[1].palID==4){VDP_setPalette(PAL2, spr_mai_pal4.palette ->data);}	
			if(Player==1 && P[1].palID==5){VDP_setPalette(PAL2, spr_mai_pal5.palette ->data);}	
			if(Player==1 && P[1].palID==6){VDP_setPalette(PAL2, spr_mai_pal6.palette ->data);}	
			if(Player==1 && P[1].palID==7){VDP_setPalette(PAL2, spr_mai_pal7.palette ->data);}	
			if(Player==1 && P[1].palID==8){VDP_setPalette(PAL2, spr_mai_pal8.palette ->data);}	
			//Atualiza paleta do P2
			if(Player==2 && P[2].palID==1){VDP_setPalette(PAL3, spr_mai_pal1.palette ->data);}	
			if(Player==2 && P[2].palID==2){VDP_setPalette(PAL3, spr_mai_pal2.palette ->data);}	
			if(Player==2 && P[2].palID==3){VDP_setPalette(PAL3, spr_mai_pal3.palette ->data);}	
			if(Player==2 && P[2].palID==4){VDP_setPalette(PAL3, spr_mai_pal4.palette ->data);}	
			if(Player==2 && P[2].palID==5){VDP_setPalette(PAL3, spr_mai_pal5.palette ->data);}	
			if(Player==2 && P[2].palID==6){VDP_setPalette(PAL3, spr_mai_pal6.palette ->data);}	
			if(Player==2 && P[2].palID==7){VDP_setPalette(PAL3, spr_mai_pal7.palette ->data);}	
			if(Player==2 && P[2].palID==8){VDP_setPalette(PAL3, spr_mai_pal8.palette ->data);}	
		}
		if(State==603 || State==604 || State==605){
			P[Player].w = 8*8;
			P[Player].h = 14*8;
			P[Player].axisX = P[Player].w-28;
			P[Player].axisY = P[Player].h -8;
			P[Player].dataAnim[1]  = 2;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_603, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==606){
			P[Player].w = 8*8;
			P[Player].h = 14*8;
			P[Player].axisX = P[Player].w-28;
			P[Player].axisY = P[Player].h -8;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].animFrameTotal = 3;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_606, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==607){
			P[Player].w = 8*8;
			P[Player].h = 12*8;
			P[Player].axisX = (P[Player].w/2);
			P[Player].axisY = (P[Player].h)-8;
			P[Player].dataAnim[1]  = 5;
			P[Player].dataAnim[2]  = 5;
			P[Player].animFrameTotal = 2;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_607, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==610){
			//total frames 255
			P[Player].w = 10*8;
			P[Player].h = 12*8;
			P[Player].axisX = ((P[Player].w)/2)+4;
			P[Player].axisY =  (P[Player].h)-8;
			P[Player].dataAnim[1]  = 10;
			P[Player].dataAnim[2]  = 10;
			P[Player].dataAnim[3]  = 10;
			P[Player].dataAnim[4]  = 10;
			P[Player].dataAnim[5]  = 10;
			P[Player].dataAnim[6]  = 10;
			P[Player].dataAnim[7]  = 10;
			P[Player].dataAnim[8]  = 10;
			P[Player].dataAnim[9]  = 10;
			P[Player].dataAnim[10] = 10;
			P[Player].dataAnim[11] = 10;
			P[Player].dataAnim[12] = 10;
			P[Player].animFrameTotal = 12;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_610, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==611){
			//total frames 255
			P[Player].w = 6*8;
			P[Player].h = 14*8;
			P[Player].axisX = ((P[Player].w)/2)+4;
			P[Player].axisY =  (P[Player].h)-8;
			P[Player].dataAnim[1]  = 10;
			P[Player].dataAnim[2]  = 10;
			P[Player].dataAnim[3]  = 10;
			P[Player].dataAnim[4]  = 10;
			P[Player].dataAnim[5]  = 10;
			P[Player].dataAnim[6]  = 10;
			P[Player].dataAnim[7]  = 10;
			P[Player].dataAnim[8]  = 10;
			P[Player].dataAnim[9]  = 10;
			P[Player].dataAnim[10]  = 10;
			P[Player].dataAnim[11]  = 10;
			P[Player].dataAnim[12]  = 10;
			P[Player].dataAnim[13]  = 10;
			P[Player].dataAnim[14]  = 140;
			P[Player].animFrameTotal = 14;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_611, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==612){
			//total frames 255
			P[Player].w = 12*8;
			P[Player].h = 12*8;
			P[Player].axisX = ((P[Player].w)/2)+4;
			P[Player].axisY =  (P[Player].h)-8;
			P[Player].dataAnim[1]  = 5;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 5;
			P[Player].dataAnim[6]  = 5;
			P[Player].dataAnim[7]  = 5;
			P[Player].dataAnim[8]  = 5;
			P[Player].dataAnim[9]  = 5;
			P[Player].dataAnim[10]  = 5;
			P[Player].dataAnim[11]  = 5;
			P[Player].dataAnim[12]  = 5;
			P[Player].dataAnim[13]  = 5;
			P[Player].dataAnim[14]  = 5;
			P[Player].dataAnim[15]  = 200;
			P[Player].animFrameTotal = 15;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_612, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==613){
			//total frames 255
			P[Player].w = 12*8;
			P[Player].h = 16*8;
			P[Player].axisX = ((P[Player].w)/2)+4;
			P[Player].axisY =  (P[Player].h)-8;
			P[Player].dataAnim[1]  = 5;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 5;
			P[Player].dataAnim[7]  = 5;
			P[Player].dataAnim[8]  = 220;
			
			P[Player].animFrameTotal = 8;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_613, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==614){
			//total frames 255
			P[Player].w = 6*8;
			P[Player].h = 14*8;
			P[Player].axisX = ((P[Player].w)/2)+4;
			P[Player].axisY =  (P[Player].h)-8;
			P[Player].dataAnim[1]  = 10;
			P[Player].dataAnim[2]  = 10;
			P[Player].dataAnim[3]  = 10;
			P[Player].dataAnim[4]  = 10;
			P[Player].dataAnim[5]  = 10;
			P[Player].dataAnim[6]  = 10;
			P[Player].dataAnim[7]  = 10;
			P[Player].dataAnim[8]  = 10;
			P[Player].dataAnim[9]  = 10;
			P[Player].dataAnim[10]  = 10;
			P[Player].dataAnim[11]  = 10;
			P[Player].dataAnim[12]  = 10;
			P[Player].dataAnim[13]  = 10;
			P[Player].dataAnim[14]  = 140;
			P[Player].animFrameTotal = 14;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_614, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==710){
			if(Spark1_countDown>0){ SPR_releaseSprite(Spark[1]); }
			if(Spark2_countDown>0){ SPR_releaseSprite(Spark[2]); }
			P[Player].w = 17*8;
			P[Player].h = 14*8;
			P[Player].axisX = ((P[Player].w)/2)+20;
			P[Player].axisY =  (P[Player].h)-8;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 2;
			P[Player].dataAnim[7]  = 15;
			P[Player].dataAnim[8]  = 5;
			P[Player].dataAnim[9]  = 5;
			P[Player].dataAnim[10] = 5;
			P[Player].dataAnim[11] = 9;
			
			
			P[Player].animFrameTotal = 11;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_710, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==713){
			
			P[Player].w = 18*8;
			P[Player].h = 22*8;
			P[Player].axisX = ((P[Player].w)/2)+12;
			P[Player].axisY =  (P[Player].h)-8;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 3;
			P[Player].dataAnim[9]  = 3;
			P[Player].dataAnim[10]  = 3;
			P[Player].dataAnim[11]  = 3;
			P[Player].dataAnim[12]  = 20;
			P[Player].animFrameTotal = 12;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_713, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==800){
			
			P[Player].w = 18*8;
			P[Player].h = 17*8;
			P[Player].axisX = ((P[Player].w)/2)+30;
			P[Player].axisY =  (P[Player].h) -8;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 2;
			P[Player].dataAnim[7]  = 2;
			P[Player].dataAnim[8]  = 2;
			P[Player].dataAnim[9]  = 7;
			P[Player].dataAnim[10]  = 7;
			P[Player].dataAnim[11]  = 7;
			P[Player].dataAnim[12]  = 7;
			P[Player].dataAnim[13]  = 7;
			
			
			P[Player].animFrameTotal = 13;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_800, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==810){
			
			P[Player].w = 20*8;
			P[Player].h = 16*8;
			P[Player].axisX = ((P[Player].w)/2)+24;
			P[Player].axisY =  (P[Player].h)-8;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 3;
			P[Player].dataAnim[3]  = 3;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 3;
			P[Player].dataAnim[7]  = 3;
			P[Player].dataAnim[8]  = 3;
			P[Player].dataAnim[9]  = 3;
			P[Player].dataAnim[10]  = 3;
			P[Player].dataAnim[11]  = 3;
			P[Player].dataAnim[12]  = 3;
			P[Player].dataAnim[13]  = 20;
			
			P[Player].animFrameTotal = 13;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_mai_810, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
	}
	
	//--- BLUEMARY ---// #ID:3
	
	if(P[Player].id==3)
	{
		if(State==100){
			P[Player].y = gAlturaPiso;
			P[Player].w = 9*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 6;
			P[Player].dataAnim[6]  = 6;
			P[Player].dataAnim[7]  = 6;
			P[Player].dataAnim[8]  = 6;
			P[Player].dataAnim[9]  = 6;
			P[Player].dataAnim[10]  = 6;
			P[Player].dataAnim[11]  = 6;
			P[Player].animFrameTotal = 11;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_100, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==101 || State==104){
			P[Player].y = gAlturaPiso;
			P[Player].w = 11*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)+18;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 1;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_101, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==102 || State==105){
			P[Player].y = gAlturaPiso;
			P[Player].w = 12*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)+24;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 1;
			P[Player].dataAnim[3]  = 1;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 3;
			P[Player].dataAnim[6]  = 4;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_102, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==103 || State==106){
			P[Player].y = gAlturaPiso;
			P[Player].w = 16*8;
			P[Player].h = 12*8;
			P[Player].axisX = (P[Player].w/2)+48;
			P[Player].axisY = P[Player].h-8;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 1;
			P[Player].dataAnim[7]  = 1;
			P[Player].dataAnim[8]  = 6;
			P[Player].dataAnim[9]  = 4;
			P[Player].dataAnim[10] = 1;
			P[Player].dataAnim[11] = 1;
			P[Player].dataAnim[12] = 1;
			P[Player].dataAnim[13] = 1;
			P[Player].dataAnim[14] = 2;
			P[Player].animFrameTotal = 14;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_103, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==107){
			P[Player].w = 7*8;
			P[Player].h = 12*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_107, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==108){
			P[Player].w = 7*8;
			P[Player].h = 12*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 12;

			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_108, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==109){
			P[Player].w = 7*8;
			P[Player].h = 12*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_109, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==110){
			P[Player].w = 7*8;
			P[Player].h = 12*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 12;

			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_108, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==151 || State==154){
			P[Player].y = gAlturaPiso;
			P[Player].w = 8*8;
			P[Player].h = 12*8;
			P[Player].axisX = (P[Player].w/2)+6;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 2;
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_151, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==152 || State==155){
			P[Player].y = gAlturaPiso;
			P[Player].w = 12*8;
			P[Player].h = 15*8;
			P[Player].axisX = (P[Player].w/2)+24;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 5;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 2;
			P[Player].dataAnim[8]  = 2;
			P[Player].animFrameTotal = 8;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_152, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==153 || State==156){
			P[Player].y = gAlturaPiso;
			P[Player].w = 12*8;
			P[Player].h = 15*8;
			P[Player].axisX = (P[Player].w/2)+24;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 2;
			P[Player].dataAnim[7]  = 2;
			P[Player].dataAnim[8]  = 2;
			P[Player].dataAnim[9]  = 2;
			P[Player].animFrameTotal = 9;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_153, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==200){
			P[Player].y = gAlturaPiso;
			P[Player].w = 8*8;
			P[Player].h = 9*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 12;
			P[Player].dataAnim[2]  = 12;
			P[Player].dataAnim[3]  = 12;
			P[Player].dataAnim[4]  = 12;
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_200, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==201 || State==204){
			P[Player].y = gAlturaPiso;
			P[Player].w = 12*8;
			P[Player].h = 8*8;
			P[Player].axisX = (P[Player].w/2)+17;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_201, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==202 || State==205){
			P[Player].y = gAlturaPiso;
			P[Player].w = 15*8;
			P[Player].h = 7*8;
			P[Player].axisX = (P[Player].w/2)+16;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_202, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==203 || State==206){
			P[Player].y = gAlturaPiso;
			P[Player].w = 15*8;
			P[Player].h = 8*8;
			P[Player].axisX = (P[Player].w/2)+24;
			P[Player].axisY = P[Player].h-8;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 2;
			P[Player].dataAnim[7]  = 2;
			P[Player].animFrameTotal = 7;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_203, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==207){
			P[Player].w = 9*8;
			P[Player].h = 9*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_207, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==208){
			P[Player].w = 9*8;
			P[Player].h = 9*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 12;

			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_208, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==209){
			P[Player].w = 9*8;
			P[Player].h = 9*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_209, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==210){
			P[Player].w = 9*8;
			P[Player].h = 9*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 12;

			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_208, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==300){
			P[Player].w = 9*8;
			P[Player].h = 16*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 6;
			P[Player].dataAnim[6]  = 99;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_300, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==301 || State==304){
			P[Player].w = 10*8;
			P[Player].h = 12*8;
			P[Player].axisX = (P[Player].w/2)+8;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 12;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 99;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_301, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==311 || State==321 || State==314 || State==324){
			P[Player].w = 9*8;
			P[Player].h = 12*8;
			P[Player].axisX = (P[Player].w/2)+8;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 12;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 99;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_321, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==302 || State==312 || State==322 || State==305 || State==315 || State==325){
			P[Player].w = 11*8;
			P[Player].h = 12*8;
			P[Player].axisX = (P[Player].w/2)+8;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 12;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 6;
			P[Player].dataAnim[6]  = 6;
			P[Player].dataAnim[7]  = 99;
			P[Player].animFrameTotal = 7;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_322, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==303 || State==313 || State==323 || State==306 || State==316 || State==326){
			P[Player].w = 12*8;
			P[Player].h = 12*8;
			P[Player].axisX = (P[Player].w/2)+8;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 12;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 6;
			P[Player].dataAnim[6]  = 99;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_323, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==310){
			P[Player].w = 9*8;
			P[Player].h = 16*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 6;
			P[Player].dataAnim[6]  = 99;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_310, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==320){
			P[Player].w = 9*8;
			P[Player].h = 16*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 6;
			P[Player].dataAnim[6]  = 99;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_320, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==410){
			P[Player].y = gAlturaPiso;
			P[Player].w = 9*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 6;
			P[Player].dataAnim[6]  = 6;
			
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_410, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==420){
			P[Player].y = gAlturaPiso;
			P[Player].w = 9*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 6;
			P[Player].dataAnim[6]  = 6;
			
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_420, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==471){
			P[Player].w = 12*8;
			P[Player].h = 12*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY =  P[Player].h-10;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 16;
			P[Player].dataAnim[3]  = 12;
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_471, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==472){
			P[Player].w = 12*8;
			P[Player].h = 10*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY =  P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 19;
			P[Player].dataAnim[3]  = 10;
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_472, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==501){
			P[Player].w = 10*8;
			P[Player].h = 12*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 6;
			P[Player].animFrameTotal = 1;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_501, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==502){
			P[Player].w = 10*8;
			P[Player].h = 12*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].animFrameTotal = 3;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_502, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==503){
			P[Player].w = 10*8;
			P[Player].h = 12*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 9;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 9;
			P[Player].animFrameTotal = 3;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_503, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==506){
			P[Player].w = 10*8;
			P[Player].h = 11*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 19;
			P[Player].animFrameTotal = 1;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_506, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==507){
			P[Player].w = 10*8;
			P[Player].h = 11*8;
			P[Player].axisX = ((P[Player].w)/2)-8;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 9;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 9;
			P[Player].animFrameTotal = 3;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_507, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==508){
			P[Player].w = 10*8;
			P[Player].h = 14*8;
			P[Player].axisX = ((P[Player].w)/2)-12;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 4;//33
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 18;
			
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_508, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==511){
			P[Player].w = 9*8;
			P[Player].h = 9*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 19;
			P[Player].animFrameTotal = 1;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_511, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==512){
			P[Player].w = 9*8;
			P[Player].h = 9*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 9;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 9;
			P[Player].animFrameTotal = 3;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_512, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==550){
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].y-=4*8; //Compensa o deslocamento do eixo Y (.axisY)
			P[Player].w = 13*8;
			P[Player].h = 11*8;
			P[Player].axisX = (P[Player].w/2)+8;
			P[Player].axisY = P[Player].h-4*8;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 99;
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_550, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==551){
			P[Player].w = 13*8;
			P[Player].h = 3*8;
			P[Player].axisX = (P[Player].w/2)+8;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 99;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_551, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==552){
			P[Player].w = 13*8;
			P[Player].h = 3*8;
			P[Player].axisX = (P[Player].w/2)+8;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_552, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==570){
			P[Player].w = 13*8;
			P[Player].h = 3*8;
			P[Player].axisX = (P[Player].w/2)+8;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 6;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_551, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==601){
			P[Player].y = gAlturaPiso;
			P[Player].w = 8*8;
			P[Player].h = 10*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].animFrameTotal = 2;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_601, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==602){
			P[Player].y = gAlturaPiso;
			P[Player].w = 8*8;
			P[Player].h = 10*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].animFrameTotal = 2;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_602, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==603 || State==604 || State==605 || State==606){
			P[Player].y = gAlturaPiso;
			P[Player].w = 7*8;
			P[Player].h = 10*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 6;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_604, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==607){
			P[Player].y = gAlturaPiso;
			P[Player].w = 7*8;
			P[Player].h = 12*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].animFrameTotal = 2;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_607, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==608){
			P[Player].y = gAlturaPiso;
			P[Player].w = 9*8;
			P[Player].h = 8*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].animFrameTotal = 2;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_608, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==610){
			P[Player].w = 9*8;
			P[Player].h = 15*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h)-11;
			P[Player].dataAnim[1]  = 15;
			P[Player].dataAnim[2]  = 10;
			P[Player].dataAnim[3]  = 10;
			P[Player].dataAnim[4]  = 10;
			P[Player].dataAnim[5]  = 10;
			P[Player].dataAnim[6]  = 5;
			P[Player].dataAnim[7]  = 5;
			P[Player].dataAnim[8]  = 5;
			P[Player].dataAnim[9]  = 5;
			P[Player].dataAnim[10] = 5;
			P[Player].dataAnim[11] = 5;
			P[Player].dataAnim[12] = 5;
			P[Player].dataAnim[13] = 5;
			P[Player].dataAnim[14] = 5;
			P[Player].dataAnim[15] = 5;
			P[Player].animFrameTotal = 15;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_610, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==611){
			P[Player].w = 10*8;
			P[Player].h = 15*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h)-11;
			P[Player].dataAnim[1]  = 15;
			P[Player].dataAnim[2]  = 10;
			P[Player].dataAnim[3]  = 10;
			P[Player].dataAnim[4]  = 10;
			P[Player].dataAnim[5]  = 10;
			P[Player].dataAnim[6]  = 10;
			P[Player].dataAnim[7]  = 250;
			P[Player].animFrameTotal = 7;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_611, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==612){
			P[Player].w = 7*8;
			P[Player].h = 13*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h)-11;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 2;
			P[Player].dataAnim[7]  = 2;
			P[Player].dataAnim[8]  = 2;
			P[Player].dataAnim[9]  = 2;
			P[Player].dataAnim[10] = 2;
			P[Player].dataAnim[11] = 2;
			P[Player].dataAnim[12] = 3;
			P[Player].dataAnim[13] = 250;
			P[Player].animFrameTotal = 13;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_612, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==615){
			P[Player].w = 8*8;
			P[Player].h = 13*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h)-10;
			P[Player].dataAnim[1]  = 12;
			P[Player].dataAnim[2]  = 12;
			P[Player].dataAnim[3]  = 12;
			P[Player].dataAnim[4]  = 250;
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_bluemary_615, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
	}
	//--- JOE ---// #ID:4
	if(P[Player].id==4)
	{
		if(State==100){
			P[Player].y = gAlturaPiso;
			P[Player].w = 8*8;
			P[Player].h = 14*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 4;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 4;
			P[Player].dataAnim[7]  = 4;
			P[Player].dataAnim[8]  = 4;
			P[Player].animFrameTotal = 8;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_100, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==101 || State==104){
			P[Player].y = gAlturaPiso;
			P[Player].w = 10*8;
			P[Player].h = 14*8;
			P[Player].axisX = (P[Player].w/2)+12;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 2;
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_101, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==102 || State==105){
			P[Player].y = gAlturaPiso;
			P[Player].w = 12*8;
			P[Player].h = 14*8;
			P[Player].axisX = (P[Player].w/2)+24;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 2;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_102, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==103 || State==106){
			P[Player].y = gAlturaPiso;
			P[Player].w = 14*8;
			P[Player].h = 14*8;
			P[Player].axisX = (P[Player].w/2);
			P[Player].axisY = P[Player].h-6;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 2;
			P[Player].dataAnim[7]  = 2;
			P[Player].dataAnim[8]  = 2;
			P[Player].dataAnim[9]  = 2;
			P[Player].animFrameTotal = 9;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_103, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==107){
			P[Player].w = 7*8;
			P[Player].h = 14*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_107, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==108){
			P[Player].w = 7*8;
			P[Player].h = 14*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 12;

			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_108, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==109){
			P[Player].w = 7*8;
			P[Player].h = 14*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_109, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==110){
			P[Player].w = 7*8;
			P[Player].h = 14*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 12;

			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_108, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==151 || State==154){
			P[Player].y = gAlturaPiso;
			P[Player].w = 8*8;
			P[Player].h = 14*8;
			P[Player].axisX = (P[Player].w/2)+6;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 2;
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_151, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==152 || State==155){
			P[Player].y = gAlturaPiso;
			P[Player].w = 12*8;
			P[Player].h = 14*8;
			P[Player].axisX = (P[Player].w/2)+24;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_152, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==153 || State==156){
			P[Player].y = gAlturaPiso;
			P[Player].w = 9*8;
			P[Player].h = 14*8;
			P[Player].axisX = (P[Player].w/2)+24;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 2;
			P[Player].dataAnim[7]  = 2;
			P[Player].animFrameTotal = 7;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_153, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==200){
			P[Player].y = gAlturaPiso;
			P[Player].w = 7*8;
			P[Player].h = 10*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 99;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_200, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==201 || State==204){
			P[Player].y = gAlturaPiso;
			P[Player].w = 10*8;
			P[Player].h = 10*8;
			P[Player].axisX = (P[Player].w/2)+13;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 2;
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_201, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==202 || State==205){
			P[Player].y = gAlturaPiso;
			P[Player].w = 14*8;
			P[Player].h = 9*8;
			P[Player].axisX = (P[Player].w/2);
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 6;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_202, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==203 || State==206){
			P[Player].y = gAlturaPiso;
			P[Player].w = 13*8;
			P[Player].h = 10*8;
			P[Player].axisX = (P[Player].w/2)+19;
			P[Player].axisY = P[Player].h-12;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 3;
			P[Player].dataAnim[5]  = 4;
			P[Player].dataAnim[6]  = 6;
			P[Player].dataAnim[7]  = 10;
			P[Player].animFrameTotal = 7;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_203, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==207){
			P[Player].w = 8*8;
			P[Player].h = 10*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_207, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==208){
			P[Player].w = 8*8;
			P[Player].h = 10*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 12;

			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_208, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==209){
			P[Player].w = 8*8;
			P[Player].h = 10*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 4;
			
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_209, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==210){
			P[Player].w = 8*8;
			P[Player].h = 10*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8 ;
			P[Player].dataAnim[1]  = 12;

			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_208, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==300){
			P[Player].w = 9*8;
			P[Player].h = 16*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 99;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_300, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==301 || State==304){
			P[Player].w = 10*8;
			P[Player].h = 16*8;
			P[Player].axisX = (P[Player].w/2)+8;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 12;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 99;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_301, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==311 || State==321 || State==314 || State==324){
			P[Player].w = 9*8;
			P[Player].h = 16*8;
			P[Player].axisX = (P[Player].w/2)+8;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 12;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 6;
			P[Player].dataAnim[6]  = 99;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_321, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==302 || State==312 || State==322 || State==305 || State==315 || State==325){
			P[Player].w = 9*8;
			P[Player].h = 16*8;
			P[Player].axisX = (P[Player].w/2)+8;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 12;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 99;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_322, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==303 || State==313 || State==323 || State==306 || State==316 || State==326){
			P[Player].w = 12*8;
			P[Player].h = 16*8;
			P[Player].axisX = (P[Player].w/2)+8;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 12;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 6;
			P[Player].dataAnim[6]  = 99;
			P[Player].animFrameTotal = 6;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_323, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==310){
			P[Player].w = 11*8;
			P[Player].h = 16*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 5;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 5;
			P[Player].dataAnim[6]  = 5;
			P[Player].dataAnim[7]  = 99;
			P[Player].animFrameTotal = 7;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_310, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==320){
			P[Player].w = 11*8;
			P[Player].h = 16*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 5;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 5;
			P[Player].dataAnim[6]  = 5;
			P[Player].dataAnim[7]  = 99;
			P[Player].animFrameTotal = 7;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_320, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==410){
			P[Player].y = gAlturaPiso;
			P[Player].w = 9*8;
			P[Player].h = 15*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 6;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_410, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==420){
			P[Player].y = gAlturaPiso;
			P[Player].w = 9*8;
			P[Player].h = 15*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-8;
			P[Player].dataAnim[1]  = 6;
			P[Player].dataAnim[2]  = 6;
			P[Player].dataAnim[3]  = 6;
			P[Player].dataAnim[4]  = 6;
			P[Player].dataAnim[5]  = 6;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_420, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==471){
			P[Player].w = 8*8;
			P[Player].h = 14*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY =  P[Player].h-10;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 7;
			P[Player].dataAnim[3]  = 7;
			P[Player].dataAnim[4]  = 7;
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_471, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==472){
			P[Player].w = 11*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY =  P[Player].h-10;
			P[Player].dataAnim[1]  = 3;
			P[Player].dataAnim[2]  = 7;
			P[Player].dataAnim[3]  = 7;
			P[Player].dataAnim[4]  = 10;
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_472, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==501){
			P[Player].w = 9*8;
			P[Player].h = 14*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 6;
			P[Player].animFrameTotal = 1;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_501, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==502){
			P[Player].w = 14*8;
			P[Player].h = 14*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].animFrameTotal = 3;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_502, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==503){
			P[Player].w = 14*8;
			P[Player].h = 14*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 9;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 9;
			P[Player].animFrameTotal = 3;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_503, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==506){
			P[Player].w = 9*8;
			P[Player].h = 12*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 19;
			P[Player].animFrameTotal = 1;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_506, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==507){
			P[Player].w = 9*8;
			P[Player].h = 12*8;
			P[Player].axisX = ((P[Player].w)/2)-8;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 9;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 9;
			P[Player].animFrameTotal = 3;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_507, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==508){
			P[Player].w = 9*8;
			P[Player].h = 12*8;
			P[Player].axisX = ((P[Player].w)/2)-12;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 4;
			P[Player].dataAnim[2]  = 4;
			P[Player].dataAnim[3]  = 18;
			
			P[Player].animFrameTotal = 3;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_508, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==511){
			P[Player].w = 8*8;
			P[Player].h = 10*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 19;
			P[Player].animFrameTotal = 1;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_511, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==512){
			P[Player].w = 9*8;
			P[Player].h = 10*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h);
			P[Player].dataAnim[1]  = 9;
			P[Player].dataAnim[2]  = 9;
			P[Player].dataAnim[3]  = 9;
			P[Player].animFrameTotal = 3;

			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_512, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==550){
			P[Player].gravidadeY = gravidadePadrao; 
			P[Player].impulsoY = impulsoPadrao;
			P[Player].y-=4*8; //Compensa o deslocamento do eixo Y (.axisY)
			P[Player].w = 15*8;
			P[Player].h = 13*8;
			P[Player].axisX = (P[Player].w/2)+8;
			P[Player].axisY = P[Player].h-4*8;
			P[Player].dataAnim[1]  = 1;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 4;
			P[Player].dataAnim[4]  = 99;
			P[Player].animFrameTotal = 4;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_550, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==551){
			P[Player].w = 15*8;
			P[Player].h = 4*8;
			P[Player].axisX = (P[Player].w/2)+8;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 99;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_551, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==552){
			P[Player].w = 12*8;
			P[Player].h = 12*8;
			P[Player].axisX = (P[Player].w/2)+8;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].animFrameTotal = 5;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_552, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==570){
			P[Player].w = 15*8;
			P[Player].h = 4*8;
			P[Player].axisX = (P[Player].w/2)+8;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 6;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_551, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==601){
			P[Player].y = gAlturaPiso;
			P[Player].w = 7*8;
			P[Player].h = 12*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].animFrameTotal = 2;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_601, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==602){
			P[Player].y = gAlturaPiso;
			P[Player].w = 7*8;
			P[Player].h = 12*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].animFrameTotal = 2;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_602, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==603 || State==604 || State==605 || State==606){
			P[Player].y = gAlturaPiso;
			P[Player].w = 7*8;
			P[Player].h = 12*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 6;
			P[Player].animFrameTotal = 1;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_604, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==607){
			P[Player].y = gAlturaPiso;
			P[Player].w = 6*8;
			P[Player].h = 13*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].animFrameTotal = 2;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_607, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==608){
			P[Player].y = gAlturaPiso;
			P[Player].w = 7*8;
			P[Player].h = 9*8;
			P[Player].axisX = P[Player].w/2;
			P[Player].axisY = P[Player].h-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].animFrameTotal = 2;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_608, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==610){
			P[Player].w = 8*8;
			P[Player].h = 15*8;
			P[Player].axisX = ((P[Player].w)/2);
			P[Player].axisY =  (P[Player].h)-11;
			P[Player].dataAnim[1]  = 15;
			P[Player].dataAnim[2]  = 10;
			P[Player].dataAnim[3]  = 10;
			P[Player].dataAnim[4]  = 10;
			P[Player].dataAnim[5]  = 10;
			P[Player].dataAnim[6]  = 5;
			P[Player].dataAnim[7]  = 5;
			P[Player].dataAnim[8]  = 5;
			P[Player].dataAnim[9]  = 5;
			P[Player].dataAnim[10] = 5;
			P[Player].dataAnim[11] = 5;
			P[Player].dataAnim[12] = 5;
			P[Player].dataAnim[13] = 5;
			P[Player].dataAnim[14] = 5;
			P[Player].animFrameTotal = 14;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_610, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==611){
			P[Player].w = 11*8;
			P[Player].h = 16*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h)-11;
			P[Player].dataAnim[1]  = 5;
			P[Player].dataAnim[2]  = 5;
			P[Player].dataAnim[3]  = 5;
			P[Player].dataAnim[4]  = 5;
			P[Player].dataAnim[5]  = 5;
			P[Player].dataAnim[6]  = 5;
			P[Player].dataAnim[7]  = 5;
			P[Player].dataAnim[8]  = 5;
			P[Player].dataAnim[9]  = 5;
			P[Player].dataAnim[10] = 5;
			P[Player].dataAnim[11] = 5;
			P[Player].dataAnim[12] = 5;
			P[Player].dataAnim[13] = 5;
			P[Player].dataAnim[14] = 5;
			P[Player].dataAnim[15] = 5;
			P[Player].dataAnim[16] = 5;
			P[Player].dataAnim[17] = 200;
			P[Player].animFrameTotal = 17;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_611, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==612){
			P[Player].w = 9*8;
			P[Player].h = 16*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h)-11;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 2;
			P[Player].dataAnim[7]  = 2;
			P[Player].dataAnim[8]  = 2;
			P[Player].dataAnim[9]  = 2;
			P[Player].dataAnim[10] = 2;
			P[Player].dataAnim[11] = 2;
			P[Player].dataAnim[12] = 3;
			P[Player].dataAnim[13] = 10;
			P[Player].dataAnim[14] = 3;
			P[Player].dataAnim[15] = 200;
			P[Player].animFrameTotal = 15;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_612, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
		if(State==615){
			P[Player].w = 9*8;
			P[Player].h = 13*8;
			P[Player].axisX = ((P[Player].w)/2)+8;
			P[Player].axisY =  (P[Player].h)-10;
			P[Player].dataAnim[1]  = 2;
			P[Player].dataAnim[2]  = 2;
			P[Player].dataAnim[3]  = 2;
			P[Player].dataAnim[4]  = 2;
			P[Player].dataAnim[5]  = 2;
			P[Player].dataAnim[6]  = 2;
			P[Player].dataAnim[7]  = 2;
			P[Player].dataAnim[8]  = 2;
			P[Player].dataAnim[9]  = 250;
			P[Player].animFrameTotal = 9;
			P[Player].sprite = SPR_addSpriteExSafe(&spr_joe_615, P[Player].x-P[Player].axisX, P[Player].y-P[Player].axisY, TILE_ATTR(P[Player].paleta, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		}
	}
	
	
	
	if(P[Player].direcao==1){SPR_setHFlip(P[Player].sprite, FALSE);}else{SPR_setHFlip(P[Player].sprite, TRUE);}
	SPR_setAnimAndFrame(P[Player].sprite, 0, P[Player].animFrame-1);
	P[Player].frameTimeTotal  = P[Player].dataAnim[1];
	FUNCAO_DEPTH(Player); //Define a prioridade de desenho
	FUNCAO_SPR_POSITION(); //Define a posicao do Sprite
}

void FUNCAO_PLAY_SND(u8 Player, u16 State)
{
	//P1 utiliza o 'P1_SFX' e 'SOUND_PCM_CH3'
	//P2 utiliza o 'P2_SFX' e 'SOUND_PCM_CH4'
	
	//Samurai Shodown 2 (SS2) nota:
	//Os golpes <FRACOS> geralmente alternam entre 2 sons
	//Os golpes <MEDIOS> e <FORTES> geralmente, alternam entre 4 sons
	
	if(Player==1)
	{
		if(P[1].id==1) //Terry
		{
			if(
				State==101 || State==104 || 
				State==151 || State==154 || 
				State==201 || State==202 || State==204 || 
				State==301 || State==302 || State==311 || State==312 || State==321 || State==322
			)
			{
				if(gPing2==0){ XGM_setPCM(P1_SFX, snd_terry_101a, sizeof(snd_terry_101a)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
				if(gPing2==1){ XGM_setPCM(P1_SFX, snd_terry_101b, sizeof(snd_terry_101b)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			}
			if(
				State==102 || State==103 || State==105 || State==106 || 
				State==152 || State==153 || State==155 || State==156 || 
				State==203 || State==205 || State==206 || 
				State==303 || State==304 || State==305 || State==306 || 
				State==313 || State==314 || State==315 || State==316 || 
				State==323 || State==324 || State==325 || State==326
			)
			{
				if(gPing4==0){ XGM_setPCM(P1_SFX, snd_terry_101a, sizeof(snd_terry_101a)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
				if(gPing4==1){ XGM_setPCM(P1_SFX, snd_terry_101b, sizeof(snd_terry_101b)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
				if(gPing4==2){ XGM_setPCM(P1_SFX, snd_terry_102a, sizeof(snd_terry_102a)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
				if(gPing4==3){ XGM_setPCM(P1_SFX, snd_terry_102b, sizeof(snd_terry_102b)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			}
			if(State==110 || State==210){ XGM_setPCM(P1_SFX, snd_110, sizeof(snd_110)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==300 || State==310 || State==320){ XGM_setPCM(P1_SFX, snd_300, sizeof(snd_300)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==501 || State==506 || State==511){ XGM_setPCM(P1_SFX, snd_hit_1, sizeof(snd_hit_1)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==502 || State==507 || State==512 || State==550){ XGM_setPCM(P1_SFX, snd_hit_2, sizeof(snd_hit_2)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==503 || State==508 || State==513 || State==555){ XGM_setPCM(P1_SFX, snd_hit_3, sizeof(snd_hit_3)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==55){ XGM_setPCM(P1_SFX, snd_551, sizeof(snd_551)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==551){ XGM_setPCM(P1_SFX, snd_551, sizeof(snd_551)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==606){ XGM_setPCM(P1_SFX, snd_606, sizeof(snd_606)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==618){ XGM_setPCM(P1_SFX, snd_terry_618, sizeof(snd_terry_618)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==700){ XGM_setPCM(P1_SFX, snd_terry_700, sizeof(snd_terry_700)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==710){ XGM_setPCM(P1_SFX, snd_terry_710, sizeof(snd_terry_710)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==720){ XGM_setPCM(P1_SFX, snd_terry_720, sizeof(snd_terry_720)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==730){ XGM_setPCM(P1_SFX, snd_terry_102b, sizeof(snd_terry_102b)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==610) { XGM_setPCM(P1_SFX, snd_terry_610, sizeof(snd_terry_610)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }

			
		}
		if(P[1].id==2) //Mai
		{
			if(
				State==101 || State==104 || 
				State==151 || State==154 || 
				State==201 || State==202 || State==204 || 
				State==301 || State==302 || State==311 || State==312 || State==321 || State==322
			)
			{
				if(gPing2==0){ XGM_setPCM(P1_SFX, snd_mai_101a, sizeof(snd_mai_101a)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
				if(gPing2==1){ XGM_setPCM(P1_SFX, snd_mai_101b, sizeof(snd_mai_101b)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			}
			if(
				State==102 || State==103 || State==105 || State==106 || 
				State==152 || State==153 || State==155 || State==156 || 
				State==203 || State==205 || State==206 || 
				State==303 || State==304 || State==305 || State==306 || 
				State==313 || State==314 || State==315 || State==316 || 
				State==323 || State==324 || State==325 || State==326
			)
			{
				if(gPing4==0){ XGM_setPCM(P1_SFX, snd_mai_101a, sizeof(snd_mai_101a)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
				if(gPing4==1){ XGM_setPCM(P1_SFX, snd_mai_101b, sizeof(snd_mai_101b)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
				if(gPing4==2){ XGM_setPCM(P1_SFX, snd_mai_102a, sizeof(snd_mai_102a)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
				if(gPing4==3){ XGM_setPCM(P1_SFX, snd_mai_102b, sizeof(snd_mai_102b)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			}
			if(State==110 || State==210){ XGM_setPCM(P1_SFX, snd_110, sizeof(snd_110)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==300 || State==310 || State==320){ XGM_setPCM(P1_SFX, snd_300, sizeof(snd_300)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==501 || State==506 || State==511){ XGM_setPCM(P1_SFX, snd_hit_1, sizeof(snd_hit_1)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==502 || State==507 || State==512 || State==550){ XGM_setPCM(P1_SFX, snd_hit_2, sizeof(snd_hit_2)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==503 || State==508 || State==513 || State==555){ XGM_setPCM(P1_SFX, snd_hit_3, sizeof(snd_hit_3)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==551){ XGM_setPCM(P1_SFX, snd_551, sizeof(snd_551)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==606){ XGM_setPCM(P1_SFX, snd_606, sizeof(snd_606)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==618){ XGM_setPCM(P1_SFX, snd_mai_102b, sizeof(snd_mai_102b)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==700){ XGM_setPCM(P1_SFX, snd_mai_700, sizeof(snd_mai_700)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==710){ XGM_setPCM(P1_SFX, snd_mai_710, sizeof(snd_mai_710)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==720){ XGM_setPCM(P1_SFX, snd_mai_720, sizeof(snd_mai_720)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==730){ XGM_setPCM(P1_SFX, snd_mai_730, sizeof(snd_mai_730)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==750){ XGM_setPCM(P1_SFX, snd_mai_750, sizeof(snd_mai_750)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==610){ XGM_setPCM(P1_SFX, snd_mai_610, sizeof(snd_mai_610)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==612){ XGM_setPCM(P1_SFX, snd_mai_612, sizeof(snd_mai_612)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==613){ XGM_setPCM(P1_SFX, snd_mai_613, sizeof(snd_mai_613)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
		}
		if(P[1].id==3) //bluemary
		{
			if(
				State==101 || State==104 || 
				State==151 || State==154 || 
				State==201 || State==202 || State==204 || 
				State==301 || State==302 || State==311 || State==312 || State==321 || State==322
			)
			{
				if(gPing2==0){ XGM_setPCM(P1_SFX, snd_bluemary_101a, sizeof(snd_bluemary_101a)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
				if(gPing2==1){ XGM_setPCM(P1_SFX, snd_bluemary_101b, sizeof(snd_bluemary_101b)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			}
			if(
				State==102 || State==103 || State==105 || State==106 || 
				State==152 || State==153 || State==155 || State==156 || 
				State==203 || State==205 || State==206 || 
				State==303 || State==304 || State==305 || State==306 || 
				State==313 || State==314 || State==315 || State==316 || 
				State==323 || State==324 || State==325 || State==326
			)
			{
				if(gPing4==0){ XGM_setPCM(P1_SFX, snd_bluemary_101a, sizeof(snd_bluemary_101a)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
				if(gPing4==1){ XGM_setPCM(P1_SFX, snd_bluemary_101b, sizeof(snd_bluemary_101b)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
				if(gPing4==2){ XGM_setPCM(P1_SFX, snd_bluemary_102a, sizeof(snd_bluemary_102a)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
				if(gPing4==3){ XGM_setPCM(P1_SFX, snd_bluemary_102b, sizeof(snd_bluemary_102b)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			}
			if(State==110 || State==210){ XGM_setPCM(P1_SFX, snd_110, sizeof(snd_110)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==300 || State==310 || State==320){ XGM_setPCM(P1_SFX, snd_300, sizeof(snd_300)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==501 || State==506 || State==511){ XGM_setPCM(P1_SFX, snd_hit_1, sizeof(snd_hit_1)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==502 || State==507 || State==512 || State==550){ XGM_setPCM(P1_SFX, snd_hit_2, sizeof(snd_hit_2)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==503 || State==508 || State==513 || State==555){ XGM_setPCM(P1_SFX, snd_hit_3, sizeof(snd_hit_3)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==551){ XGM_setPCM(P1_SFX, snd_551, sizeof(snd_551)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==606){ XGM_setPCM(P1_SFX, snd_606, sizeof(snd_606)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==618){ XGM_setPCM(P1_SFX, snd_bluemary_102b, sizeof(snd_bluemary_102b)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==700){ XGM_setPCM(P1_SFX, snd_bluemary_700, sizeof(snd_bluemary_700)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==710){ XGM_setPCM(P1_SFX, snd_bluemary_710, sizeof(snd_bluemary_710)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==720){ XGM_setPCM(P1_SFX, snd_bluemary_720, sizeof(snd_bluemary_720)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==730){ XGM_setPCM(P1_SFX, snd_bluemary_730, sizeof(snd_bluemary_730)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==750){ XGM_setPCM(P1_SFX, snd_bluemary_750, sizeof(snd_bluemary_750)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==610){ XGM_setPCM(P1_SFX, snd_bluemary_610, sizeof(snd_bluemary_610)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==612){ XGM_setPCM(P1_SFX, snd_bluemary_612, sizeof(snd_bluemary_612)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==613){ XGM_setPCM(P1_SFX, snd_bluemary_612, sizeof(snd_bluemary_612)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
		}
	}
	
	if(Player==1)
	{
		if(P[1].id==4) //joe
		{
			if(
				State==101 || State==104 || 
				State==151 || State==154 || 
				State==201 || State==202 || State==204 || 
				State==301 || State==302 || State==311 || State==312 || State==321 || State==322
			)
			{
				if(gPing2==0){ XGM_setPCM(P1_SFX, snd_joe_101a, sizeof(snd_joe_101a)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
				if(gPing2==1){ XGM_setPCM(P1_SFX, snd_joe_101b, sizeof(snd_joe_101b)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			}
			if(
				State==102 || State==103 || State==105 || State==106 || 
				State==152 || State==153 || State==155 || State==156 || 
				State==203 || State==205 || State==206 || 
				State==303 || State==304 || State==305 || State==306 || 
				State==313 || State==314 || State==315 || State==316 || 
				State==323 || State==324 || State==325 || State==326
			)
			{
				if(gPing4==0){ XGM_setPCM(P1_SFX, snd_joe_101a, sizeof(snd_joe_101a)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
				if(gPing4==1){ XGM_setPCM(P1_SFX, snd_joe_101b, sizeof(snd_joe_101b)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
				if(gPing4==2){ XGM_setPCM(P1_SFX, snd_joe_102a, sizeof(snd_joe_102a)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
				if(gPing4==3){ XGM_setPCM(P1_SFX, snd_joe_102b, sizeof(snd_joe_102b)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			}
			if(State==110 || State==210){ XGM_setPCM(P1_SFX, snd_110, sizeof(snd_110)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==300 || State==310 || State==320){ XGM_setPCM(P1_SFX, snd_300, sizeof(snd_300)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==501 || State==506 || State==511){ XGM_setPCM(P1_SFX, snd_hit_1, sizeof(snd_hit_1)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==502 || State==507 || State==512 || State==550){ XGM_setPCM(P1_SFX, snd_hit_2, sizeof(snd_hit_2)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==503 || State==508 || State==513 || State==555){ XGM_setPCM(P1_SFX, snd_hit_3, sizeof(snd_hit_3)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==55){ XGM_setPCM(P1_SFX, snd_551, sizeof(snd_551)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==551){ XGM_setPCM(P1_SFX, snd_551, sizeof(snd_551)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==606){ XGM_setPCM(P1_SFX, snd_606, sizeof(snd_606)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==618){ XGM_setPCM(P1_SFX, snd_terry_618, sizeof(snd_terry_618)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==700){ XGM_setPCM(P1_SFX, snd_terry_700, sizeof(snd_terry_700)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==710){ XGM_setPCM(P1_SFX, snd_terry_710, sizeof(snd_terry_710)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==720){ XGM_setPCM(P1_SFX, snd_terry_720, sizeof(snd_terry_720)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==730){ XGM_setPCM(P1_SFX, snd_terry_102b, sizeof(snd_terry_102b)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(State==610) { XGM_setPCM(P1_SFX, snd_joe_610, sizeof(snd_joe_610)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }

			
		}
	}	
	if(Player==2)
	{
		if(P[2].id==1) //Terry
		{		
			if(
				State==101 || State==104 || 
				State==151 || State==154 || 
				State==201 || State==202 || State==204 || 
				State==301 || State==302 || State==311 || State==312 || State==321 || State==322
			)
			{
				if(gPing2==0){ XGM_setPCM(P2_SFX, snd_terry_101a, sizeof(snd_terry_101a)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
				if(gPing2==1){ XGM_setPCM(P2_SFX, snd_terry_101b, sizeof(snd_terry_101b)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			}
			if(
				State==102 || State==103 || State==105 || State==106 || 
				State==152 || State==153 || State==155 || State==156 || 
				State==203 || State==205 || State==206 || 
				State==303 || State==304 || State==305 || State==306 || 
				State==313 || State==314 || State==315 || State==316 || 
				State==323 || State==324 || State==325 || State==326
			)
			{
				if(gPing4==0){ XGM_setPCM(P2_SFX, snd_terry_101a, sizeof(snd_terry_101a)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
				if(gPing4==1){ XGM_setPCM(P2_SFX, snd_terry_101b, sizeof(snd_terry_101b)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
				if(gPing4==2){ XGM_setPCM(P2_SFX, snd_terry_102a, sizeof(snd_terry_102a)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
				if(gPing4==3){ XGM_setPCM(P2_SFX, snd_terry_102b, sizeof(snd_terry_102b)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			}
			if(State==110 || State==210){ XGM_setPCM(P2_SFX, snd_110, sizeof(snd_110)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==300 || State==310 || State==320){ XGM_setPCM(P2_SFX, snd_300, sizeof(snd_300)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==501 || State==506 || State==511){ XGM_setPCM(P2_SFX, snd_hit_1, sizeof(snd_hit_1)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==502 || State==507 || State==512 || State==550){ XGM_setPCM(P2_SFX, snd_hit_2, sizeof(snd_hit_2)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==503 || State==508 || State==513 || State==555){ XGM_setPCM(P2_SFX, snd_hit_3, sizeof(snd_hit_3)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==551){ XGM_setPCM(P2_SFX, snd_551, sizeof(snd_551)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==606){ XGM_setPCM(P2_SFX, snd_606, sizeof(snd_606)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==618){ XGM_setPCM(P2_SFX, snd_terry_618, sizeof(snd_terry_618)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==700){ XGM_setPCM(P2_SFX, snd_terry_700, sizeof(snd_terry_700)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==710){ XGM_setPCM(P2_SFX, snd_terry_710, sizeof(snd_terry_710)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==720){ XGM_setPCM(P2_SFX, snd_terry_720, sizeof(snd_terry_720)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==730){ XGM_setPCM(P2_SFX, snd_terry_102b, sizeof(snd_terry_102b)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==610) { XGM_setPCM(P1_SFX, snd_terry_610, sizeof(snd_terry_610)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH4); }
		}
		if(P[2].id==2) //Mai
		{		
			if(
				State==101 || State==104 || 
				State==151 || State==154 || 
				State==201 || State==202 || State==204 || 
				State==301 || State==302 || State==311 || State==312 || State==321 || State==322
			)
			{
				if(gPing2==0){ XGM_setPCM(P2_SFX, snd_mai_101a, sizeof(snd_mai_101a)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
				if(gPing2==1){ XGM_setPCM(P2_SFX, snd_mai_101b, sizeof(snd_mai_101b)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			}
			if(
				State==102 || State==103 || State==105 || State==106 || 
				State==152 || State==153 || State==155 || State==156 || 
				State==203 || State==205 || State==206 || 
				State==303 || State==304 || State==305 || State==306 || 
				State==313 || State==314 || State==315 || State==316 || 
				State==323 || State==324 || State==325 || State==326
			)
			{
				if(gPing4==0){ XGM_setPCM(P2_SFX, snd_mai_101a, sizeof(snd_mai_101a)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
				if(gPing4==1){ XGM_setPCM(P2_SFX, snd_mai_101b, sizeof(snd_mai_101b)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
				if(gPing4==2){ XGM_setPCM(P2_SFX, snd_mai_102a, sizeof(snd_mai_102a)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
				if(gPing4==3){ XGM_setPCM(P2_SFX, snd_mai_102b, sizeof(snd_mai_102b)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			}
			if(State==110 || State==210){ XGM_setPCM(P2_SFX, snd_110, sizeof(snd_110)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==300 || State==310 || State==320){ XGM_setPCM(P2_SFX, snd_300, sizeof(snd_300)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==501 || State==506 || State==511){ XGM_setPCM(P2_SFX, snd_hit_1, sizeof(snd_hit_1)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==502 || State==507 || State==512 || State==550){ XGM_setPCM(P2_SFX, snd_hit_2, sizeof(snd_hit_2)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==503 || State==508 || State==513 || State==555){ XGM_setPCM(P2_SFX, snd_hit_3, sizeof(snd_hit_3)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==551){ XGM_setPCM(P2_SFX, snd_551, sizeof(snd_551)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==606){ XGM_setPCM(P2_SFX, snd_606, sizeof(snd_606)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==618){ XGM_setPCM(P2_SFX, snd_mai_102b, sizeof(snd_mai_102b)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==700){ XGM_setPCM(P2_SFX, snd_mai_700, sizeof(snd_mai_700)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==710){ XGM_setPCM(P2_SFX, snd_mai_710, sizeof(snd_mai_710)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==720){ XGM_setPCM(P2_SFX, snd_mai_720, sizeof(snd_mai_720)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==730){ XGM_setPCM(P2_SFX, snd_mai_730, sizeof(snd_mai_730)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==750){ XGM_setPCM(P2_SFX, snd_mai_750, sizeof(snd_mai_750)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==610){ XGM_setPCM(P1_SFX, snd_mai_610, sizeof(snd_mai_610)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH4); }
			if(State==612){ XGM_setPCM(P1_SFX, snd_mai_612, sizeof(snd_mai_612)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH4); }
			if(State==613){ XGM_setPCM(P1_SFX, snd_mai_612, sizeof(snd_mai_612)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH4); }
		}
		if(P[2].id==3) //bluemary
		{
			if(
				State==101 || State==104 || 
				State==151 || State==154 || 
				State==201 || State==202 || State==204 || 
				State==301 || State==302 || State==311 || State==312 || State==321 || State==322
			)
			{
				if(gPing2==0){ XGM_setPCM(P2_SFX, snd_bluemary_101a, sizeof(snd_bluemary_101a)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
				if(gPing2==1){ XGM_setPCM(P2_SFX, snd_bluemary_101b, sizeof(snd_bluemary_101b)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			}
			if(
				State==102 || State==103 || State==105 || State==106 || 
				State==152 || State==153 || State==155 || State==156 || 
				State==203 || State==205 || State==206 || 
				State==303 || State==304 || State==305 || State==306 || 
				State==313 || State==314 || State==315 || State==316 || 
				State==323 || State==324 || State==325 || State==326
			)
			{
				if(gPing4==0){ XGM_setPCM(P2_SFX, snd_bluemary_101a, sizeof(snd_bluemary_101a)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
				if(gPing4==1){ XGM_setPCM(P2_SFX, snd_bluemary_101b, sizeof(snd_bluemary_101b)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
				if(gPing4==2){ XGM_setPCM(P2_SFX, snd_bluemary_102a, sizeof(snd_bluemary_102a)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
				if(gPing4==3){ XGM_setPCM(P2_SFX, snd_bluemary_102b, sizeof(snd_bluemary_102b)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			}
			if(State==110 || State==210){ XGM_setPCM(P2_SFX, snd_110, sizeof(snd_110)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==300 || State==310 || State==320){ XGM_setPCM(P2_SFX, snd_300, sizeof(snd_300)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==501 || State==506 || State==511){ XGM_setPCM(P2_SFX, snd_hit_1, sizeof(snd_hit_1)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==502 || State==507 || State==512 || State==550){ XGM_setPCM(P2_SFX, snd_hit_2, sizeof(snd_hit_2)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==503 || State==508 || State==513 || State==555){ XGM_setPCM(P2_SFX, snd_hit_3, sizeof(snd_hit_3)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==551){ XGM_setPCM(P2_SFX, snd_551, sizeof(snd_551)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==606){ XGM_setPCM(P2_SFX, snd_606, sizeof(snd_606)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==618){ XGM_setPCM(P2_SFX, snd_bluemary_102b, sizeof(snd_bluemary_102b)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH4); }
			if(State==700){ XGM_setPCM(P2_SFX, snd_bluemary_700, sizeof(snd_bluemary_700)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==710){ XGM_setPCM(P2_SFX, snd_bluemary_710, sizeof(snd_bluemary_710)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==720){ XGM_setPCM(P2_SFX, snd_bluemary_720, sizeof(snd_bluemary_720)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==730){ XGM_setPCM(P2_SFX, snd_bluemary_730, sizeof(snd_bluemary_730)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==750){ XGM_setPCM(P2_SFX, snd_bluemary_750, sizeof(snd_bluemary_750)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==610){ XGM_setPCM(P2_SFX, snd_bluemary_610, sizeof(snd_bluemary_610)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==612){ XGM_setPCM(P2_SFX, snd_bluemary_612, sizeof(snd_bluemary_612)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
			if(State==613){ XGM_setPCM(P2_SFX, snd_bluemary_612, sizeof(snd_bluemary_612)); XGM_startPlayPCM(P2_SFX, 1, SOUND_PCM_CH4); }
		}
	}
	
}

void FUNCAO_RELOGIO()
{
	if(gClockTimer>0 && (gClockLTimer>0 || gClockRTimer>0) && (P[1].energiaBase>0 && P[2].energiaBase>0) ){ gClockTimer--; }
	if(gClockTimer==0)
	{
		gClockRTimer--;
		SPR_releaseSprite(ClockR);
		if(gClockRTimer==9){ ClockR = SPR_addSpriteEx(&spr_n9, 160, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		if(gClockRTimer==8){ ClockR = SPR_addSpriteEx(&spr_n8, 160, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		if(gClockRTimer==7){ ClockR = SPR_addSpriteEx(&spr_n7, 160, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		if(gClockRTimer==6){ ClockR = SPR_addSpriteEx(&spr_n6, 160, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		if(gClockRTimer==5){ ClockR = SPR_addSpriteEx(&spr_n5, 160, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		if(gClockRTimer==4){ ClockR = SPR_addSpriteEx(&spr_n4, 160, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		if(gClockRTimer==3){ ClockR = SPR_addSpriteEx(&spr_n3, 160, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		if(gClockRTimer==2){ ClockR = SPR_addSpriteEx(&spr_n2, 160, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		if(gClockRTimer==1){ ClockR = SPR_addSpriteEx(&spr_n1, 160, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		if(gClockRTimer==0){ ClockR = SPR_addSpriteEx(&spr_n0, 160, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		
		if(gClockRTimer==-1)
		{
			if(gClockLTimer>0)
			{
				gClockLTimer--;
				SPR_releaseSprite(ClockL);
				if(gClockLTimer==9){ ClockL = SPR_addSpriteEx(&spr_n9, 144, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
				if(gClockLTimer==8){ ClockL = SPR_addSpriteEx(&spr_n8, 144, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
				if(gClockLTimer==7){ ClockL = SPR_addSpriteEx(&spr_n7, 144, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
				if(gClockLTimer==6){ ClockL = SPR_addSpriteEx(&spr_n6, 144, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
				if(gClockLTimer==5){ ClockL = SPR_addSpriteEx(&spr_n5, 144, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
				if(gClockLTimer==4){ ClockL = SPR_addSpriteEx(&spr_n4, 144, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
				if(gClockLTimer==3){ ClockL = SPR_addSpriteEx(&spr_n3, 144, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
				if(gClockLTimer==2){ ClockL = SPR_addSpriteEx(&spr_n2, 144, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
				if(gClockLTimer==1){ ClockL = SPR_addSpriteEx(&spr_n1, 144, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
				if(gClockLTimer==0){ ClockL = SPR_addSpriteEx(&spr_n0, 144, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
				gClockRTimer=9;
				SPR_releaseSprite(ClockR);
				ClockR = SPR_addSpriteEx(&spr_n9, 160, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
			}
		}
		gClockTimer=38; //reset Clock Count timer
		SPR_setVRAMTileIndex(ClockL,1703); //define uma posicao especifica para o GFX na VRAM
		SPR_setVRAMTileIndex(ClockR,1707); //define uma posicao especifica para o GFX na VRAM
	}
	
	//if(gClockRTimer==-1 && gClockRTimer==-1)
	//{ 
		/*TIMER OVER!*/ 
		//ClockR = SPR_addSpriteEx(&spr_n0, 160, 32, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		//SPR_setVRAMTileIndex(ClockR,1707); //define uma posicao especifica para o GFX na VRAM
	//}
}

void FUNCAO_BARRAS_DE_ENERGIA()
{
	for(i=1; i<=2; i++)
	{
		if( P[i].energia != P[i].energiaBase )
		{ 
			if(P[i].energia > P[i].energiaBase){ P[i].energia--; } //decrementa a 'energia' aos poucos, até igualar a 'energiaBase'
			if(P[i].energia < P[i].energiaBase){ P[i].energia++; } //incrementa a 'energia' aos poucos, até igualar a 'energiaBase'
		} 
		
		u8 subEnergyPos;
		if(                           P[i].energia==200){ SPR_setAnimAndFrame(GE[2+i].sprite, 0, 0); subEnergyPos= 0;
		}else if(P[i].energia< 200 && P[i].energia>=192){ SPR_setAnimAndFrame(GE[2+i].sprite, 0, 1); subEnergyPos= 1;
		}else if(P[i].energia< 192 && P[i].energia>=184){ SPR_setAnimAndFrame(GE[2+i].sprite, 0, 2); subEnergyPos= 2; 
		}else if(P[i].energia< 184 && P[i].energia>=176){ SPR_setAnimAndFrame(GE[2+i].sprite, 0, 3); subEnergyPos= 3;
		}else if(P[i].energia< 176 && P[i].energia>=168){ SPR_setAnimAndFrame(GE[2+i].sprite, 0, 4); subEnergyPos= 4;
		}else if(P[i].energia< 168 && P[i].energia>=160){ SPR_setAnimAndFrame(GE[2+i].sprite, 0, 5); subEnergyPos= 5;
		}else if(P[i].energia< 160 && P[i].energia>=152){ SPR_setAnimAndFrame(GE[2+i].sprite, 0, 6); subEnergyPos= 6;
		}else if(P[i].energia< 152 && P[i].energia>=144){ SPR_setAnimAndFrame(GE[2+i].sprite, 0, 7); subEnergyPos= 7;
		}else if(P[i].energia< 144 && P[i].energia>=136){ SPR_setAnimAndFrame(GE[2+i].sprite, 0, 8); subEnergyPos= 8;
		}else if(P[i].energia< 136 && P[i].energia>=128){ SPR_setAnimAndFrame(GE[2+i].sprite, 0, 9); subEnergyPos= 9;
		}else if(P[i].energia< 128 && P[i].energia>=120){ SPR_setAnimAndFrame(GE[2+i].sprite, 0,10); subEnergyPos=10;
		}else if(P[i].energia< 120 && P[i].energia>=112){ SPR_setAnimAndFrame(GE[2+i].sprite, 0,11); subEnergyPos=11;
		}else if(P[i].energia< 112 && P[i].energia>=104){ SPR_setAnimAndFrame(GE[2+i].sprite, 0,12); subEnergyPos=12;
		}else if(P[i].energia< 104 && P[i].energia>= 96){ SPR_setAnimAndFrame(GE[2+i].sprite, 0,13); subEnergyPos=13;
		}else if(P[i].energia<  96 && P[i].energia>= 88){ SPR_setAnimAndFrame(GE[2+i].sprite, 0,14); subEnergyPos=14;
		}else if(P[i].energia<  88 && P[i].energia>= 80){ SPR_setAnimAndFrame(GE[2+i].sprite, 0,15); subEnergyPos=15;
		}else if(P[i].energia<  80 && P[i].energia>= 72){ SPR_setAnimAndFrame(GE[2+i].sprite, 0,16); subEnergyPos=16;
		}else if(P[i].energia<  72 && P[i].energia>= 64){ SPR_setAnimAndFrame(GE[2+i].sprite, 0,17); subEnergyPos=17;
		}else if(P[i].energia<  64 && P[i].energia>= 56){ SPR_setAnimAndFrame(GE[2+i].sprite, 0,18); subEnergyPos=18;
		}else if(P[i].energia<  56 && P[i].energia>= 48){ SPR_setAnimAndFrame(GE[2+i].sprite, 0,19); subEnergyPos=19;
		}else if(P[i].energia<  48 && P[i].energia>= 40){ SPR_setAnimAndFrame(GE[2+i].sprite, 0,20); subEnergyPos=20;
		}else if(P[i].energia<  40 && P[i].energia>= 32){ SPR_setAnimAndFrame(GE[2+i].sprite, 0,21); subEnergyPos=21;
		}else if(P[i].energia<  32 && P[i].energia>= 24){ SPR_setAnimAndFrame(GE[2+i].sprite, 0,22); subEnergyPos=22;
		}else if(P[i].energia<  24 && P[i].energia>= 16){ SPR_setAnimAndFrame(GE[2+i].sprite, 0,23); subEnergyPos=23;
		}else if(P[i].energia<  16 && P[i].energia>=  8){ SPR_setAnimAndFrame(GE[2+i].sprite, 0,24); subEnergyPos=24;
		}else if(P[i].energia<   8 && P[i].energia>=  1){ SPR_setAnimAndFrame(GE[2+i].sprite, 0,25); subEnergyPos=25;
		}else if(                     P[i].energia==  0){ SPR_setAnimAndFrame(GE[2+i].sprite, 0,25); subEnergyPos=25;
		}
		if(i==1){ SPR_setPosition(GE[4+i].sprite,   40+(subEnergyPos*8), 20); }
		if(i==2){ SPR_setPosition(GE[4+i].sprite, 272-(subEnergyPos*8), 20); }
		
		int a=0;
		if(subEnergyPos== 0){ a=200; }
		if(subEnergyPos== 1){ a=192; }
		if(subEnergyPos== 2){ a=184; }
		if(subEnergyPos== 3){ a=176; }
		if(subEnergyPos== 4){ a=168; }
		if(subEnergyPos== 5){ a=160; }
		if(subEnergyPos== 6){ a=152; }
		if(subEnergyPos== 7){ a=144; }
		if(subEnergyPos== 8){ a=136; }
		if(subEnergyPos== 9){ a=128; }
		if(subEnergyPos==10){ a=120; }
		if(subEnergyPos==11){ a=112; }
		if(subEnergyPos==12){ a=104; }
		if(subEnergyPos==13){ a= 96; }
		if(subEnergyPos==14){ a= 88; }
		if(subEnergyPos==15){ a= 80; }
		if(subEnergyPos==16){ a= 72; }
		if(subEnergyPos==17){ a= 64; }
		if(subEnergyPos==18){ a= 56; }
		if(subEnergyPos==19){ a= 48; }
		if(subEnergyPos==20){ a= 40; }
		if(subEnergyPos==21){ a= 32; }
		if(subEnergyPos==22){ a= 24; }
		if(subEnergyPos==23){ a= 16; }
		if(subEnergyPos==24){ a=  8; }
		if(subEnergyPos==25){ a=  0; }
		
		if (P[i].energia<=104)
		{
			SPR_setAnimAndFrame(GE[4+i].sprite, 0, (P[i].energia-a)+9 );
		}else{
		SPR_setAnimAndFrame(GE[4+i].sprite, 0, (P[i].energia-a) );
		}
		//Exibe a barra de especial, e fica piscando, caso seja == 32
		if(P[i].energiaSP<32)
		{
			SPR_setAnimAndFrame(GE[6+i].sprite, 0, P[i].energiaSP);
		}else{
			if(gPing2==1){
				SPR_setAnimAndFrame(GE[6+i].sprite, 0, 32);
			}else{
				SPR_setAnimAndFrame(GE[6+i].sprite, 0, 33);
			}
		}
		
	}
	
	if(P[1].energiaSP>=200){ SPR_setVisibility(GE[ 9].sprite, VISIBLE); }else{ SPR_setVisibility(GE[ 9].sprite, HIDDEN); }
	if(P[2].energiaSP>=200){ SPR_setVisibility(GE[10].sprite, VISIBLE); }else{ SPR_setVisibility(GE[10].sprite, HIDDEN); }
	
}

void FUNCAO_ANIMACAO()
{
	//CONTROLE DE ANIMACAO E END ANIMATION
	if(P[1].hitPause==0 && P[2].hitPause==0)
	{ 
		gASG_system = 0; //A.S.G.S. Anti Sprite Glitch System; (Evita a troca simultanea de sprites dos players)
		
		for(i=1; i<=2; i++)
		{
			P[i].frameTimeAtual++; 
			if(P[i].frameTimeAtual>P[i].frameTimeTotal && gASG_system==0) //hora de trocar o frame!
			{
				P[i].animFrame++;
				if(P[i].animFrame>P[i].animFrameTotal) //hora de trocar ou recarregar a animacao!
				{
					gASG_system = 1;
					if(P[i].state==100){ PLAYER_STATE(i,100); }
					if(P[i].state==200){ PLAYER_STATE(i,200); }
					
					// estes aqui previnem bug, caso o tempo de animacao dos Aereos seja definido errado
					// e o player nao tenha tempo de chegar ao solo...
					if(P[i].state==300 || P[i].state==310 || P[i].state==320){ PLAYER_STATE(i,100); }
					if(P[i].state==301 || P[i].state==302 || P[i].state==303){ PLAYER_STATE(i,100); }
					if(P[i].state==304 || P[i].state==305 || P[i].state==306){ PLAYER_STATE(i,100); }
					if(P[i].state==311 || P[i].state==312 || P[i].state==313){ PLAYER_STATE(i,100); }
					if(P[i].state==314 || P[i].state==315 || P[i].state==316){ PLAYER_STATE(i,100); }
					if(P[i].state==321 || P[i].state==322 || P[i].state==323){ PLAYER_STATE(i,100); }
					if(P[i].state==324 || P[i].state==325 || P[i].state==326){ PLAYER_STATE(i,100); }
					
					if(P[i].state==410){ PLAYER_STATE(i,410); }
					if(P[i].state==420){ PLAYER_STATE(i,420); }
					if(P[i].state==470 || P[i].state==471 || P[i].state==472){ if(P[i].key_JOY_DOWN_status>0){ PLAYER_STATE(i,200); }else{ PLAYER_STATE(i,100); } }
					if(P[i].state==601){ PLAYER_STATE(i,200); }
					if(P[i].state==602){ PLAYER_STATE(i,100); }
					if(P[i].state==603){ P[i].y-=1; P[i].puloTimer=1; PLAYER_STATE(i,310); }
					if(P[i].state==604){ P[i].y-=1; P[i].puloTimer=1; PLAYER_STATE(i,300); }
					if(P[i].state==605){ P[i].y-=1; P[i].puloTimer=1; PLAYER_STATE(i,320); }
					if(P[i].state==607){ PLAYER_STATE(i,100); } //virando em pe
					if(P[i].state==608){ PLAYER_STATE(i,200); } //virando abaixado
					if(P[i].state>=101 && P[i].state<=106){ PLAYER_STATE(i,100); }
					if(P[i].state==107){ PLAYER_STATE(i,108); }
					if(P[i].state==108){ PLAYER_STATE(i,108); }
					if(P[i].state==109){ PLAYER_STATE(i,100); }
					if(P[i].state==110){ PLAYER_STATE(i,109); }
					if(P[i].state>=151 && P[i].state<=156){ PLAYER_STATE(i,100); }
					if(P[i].state>=201 && P[i].state<=206){ PLAYER_STATE(i,200); }
					if(P[i].state==207){ PLAYER_STATE(i,208); }
					if(P[i].state==208){ PLAYER_STATE(i,208); }
					if(P[i].state==209){ PLAYER_STATE(i,200); }
					if(P[i].state==210){ PLAYER_STATE(i,209); }
					if(P[i].state>=501 && P[i].state<=550){ PLAYER_STATE(i,100); } //end hurts
					if(P[i].state==552){ PLAYER_STATE(i,100); } //end queda
					if(P[i].state==570){ if(P[i].energiaBase>0){ PLAYER_STATE(i,552); }else{ PLAYER_STATE(i,570); } } //end pulos
					if(P[i].state==606){ PLAYER_STATE(i,100); } //end pulos
					if(P[i].state==610){ PLAYER_STATE(i,100); } //end Intro
					if(P[i].state==611 || P[i].state==612 || P[i].state==613 || P[i].state==614){ FUNCAO_ROUND_RESTART(); } //end WIN Pose
					if(P[i].state==615){ FUNCAO_ROUND_RESTART(); } //end time lose
					if(P[i].state==618){ PLAYER_STATE(i,100); } //end Rage Explosion
					if(P[i].state>=700 && P[i].state<=790){ PLAYER_STATE(i,100); } //end magias
				}
				P[i].frameTimeAtual=1;
				P[i].frameTimeTotal = P[i].dataAnim[P[i].animFrame];
				SPR_setAnimAndFrame(P[i].sprite, 0, P[i].animFrame-1);
				FUNCAO_FSM_HITBOXES(i); //Atualiza as Hurt / Hitboxes
			}else if(P[i].frameTimeAtual>P[i].frameTimeTotal && gASG_system==1){ //A.S.G.S. 
				if(P[i].frameTimeAtual>1){ P[i].frameTimeAtual--; }
			}
			//*NOTA: No caso do SS2, a animacao abaixando é encurtada (para se abaixar mais rapido) caso se mantenha pressionado para baixo apos o pulo //end pulos 
			/*samsho2*/ if(P[i].state==606 && P[i].animFrame==2 && P[i].frameTimeAtual>=3 && P[i].key_JOY_DOWN_status>=1){ PLAYER_STATE(i,200); } 
		}
	}
	FUNCAO_SPR_POSITION(); //Define a posicao do Sprite
	
}

void FUNCAO_SPR_POSITION()
{
	//ajusta posicao do sprite
	if(P[1].direcao== 1){ SPR_setPosition(P[1].sprite, P[1].x-(P[1].w-P[1].axisX), P[1].y-P[1].axisY); }
	if(P[1].direcao==-1){ SPR_setPosition(P[1].sprite, P[1].x- P[1].axisX        , P[1].y-P[1].axisY); }
	if(P[2].direcao== 1){ SPR_setPosition(P[2].sprite, P[2].x-(P[2].w-P[2].axisX), P[2].y-P[2].axisY); }
	if(P[2].direcao==-1){ SPR_setPosition(P[2].sprite, P[2].x- P[2].axisX        , P[2].y-P[2].axisY); }
	/*
	if(gSombraStyle==2)
	{
		SPR_setPosition(P[1].sombra, P[1].x-32, gAlturaPiso-5); // *NOTA: A sombra do SS2 tem 80px, entao desloquei -40px no eixo X
		SPR_setPosition(P[2].sombra, P[2].x-32, gAlturaPiso-5); // para que ela ficasse perfeitamente alinhada aos players
	}else{
		if(gPing2==0){ SPR_setPosition(P[1].sombra, P[1].x-32, gAlturaPiso-5); }
		if(gPing2==1){ SPR_setPosition(P[1].sombra, P[2].x-32, gAlturaPiso-5); }
	}
	*/
}

void FUNCAO_INPUT_SYSTEM()
{
	
	//joystick P1
	u16 JOY1 = JOY_readJoypad(JOY_1);
	if(JOY1 & BUTTON_UP    ) { JOY1_UP    = TRUE; } else { JOY1_UP    = FALSE; }
	if(JOY1 & BUTTON_DOWN  ) { JOY1_DOWN  = TRUE; } else { JOY1_DOWN  = FALSE; }
	if(JOY1 & BUTTON_LEFT  ) { JOY1_LEFT  = TRUE; } else { JOY1_LEFT  = FALSE; }
	if(JOY1 & BUTTON_RIGHT ) { JOY1_RIGHT = TRUE; } else { JOY1_RIGHT = FALSE; }
	if(JOY1 & BUTTON_A     ) { JOY1_A     = TRUE; } else { JOY1_A     = FALSE; }
	if(JOY1 & BUTTON_B     ) { JOY1_B     = TRUE; } else { JOY1_B     = FALSE; }
	if(JOY1 & BUTTON_C     ) { JOY1_C     = TRUE; } else { JOY1_C     = FALSE; }
	if(JOY1 & BUTTON_X     ) { JOY1_X     = TRUE; } else { JOY1_X     = FALSE; }
	if(JOY1 & BUTTON_Y     ) { JOY1_Y     = TRUE; } else { JOY1_Y     = FALSE; }
	if(JOY1 & BUTTON_Z     ) { JOY1_Z     = TRUE; } else { JOY1_Z     = FALSE; }
	if(JOY1 & BUTTON_START ) { JOY1_START = TRUE; } else { JOY1_START = FALSE; }
	if(JOY1 & BUTTON_MODE  ) { JOY1_MODE  = TRUE; } else { JOY1_MODE  = FALSE; }
	
	//joystick P2
	u16 JOY2 = JOY_readJoypad(JOY_2);
	if(JOY2 & BUTTON_UP    ) { JOY2_UP    = TRUE; } else { JOY2_UP    = FALSE; }
	if(JOY2 & BUTTON_DOWN  ) { JOY2_DOWN  = TRUE; } else { JOY2_DOWN  = FALSE; }
	if(JOY2 & BUTTON_LEFT  ) { JOY2_LEFT  = TRUE; } else { JOY2_LEFT  = FALSE; }
	if(JOY2 & BUTTON_RIGHT ) { JOY2_RIGHT = TRUE; } else { JOY2_RIGHT = FALSE; }
	if(JOY2 & BUTTON_A     ) { JOY2_A     = TRUE; } else { JOY2_A     = FALSE; }
	if(JOY2 & BUTTON_B     ) { JOY2_B     = TRUE; } else { JOY2_B     = FALSE; }
	if(JOY2 & BUTTON_C     ) { JOY2_C     = TRUE; } else { JOY2_C     = FALSE; }
	if(JOY2 & BUTTON_X     ) { JOY2_X     = TRUE; } else { JOY2_X     = FALSE; }
	if(JOY2 & BUTTON_Y     ) { JOY2_Y     = TRUE; } else { JOY2_Y     = FALSE; }
	if(JOY2 & BUTTON_Z     ) { JOY2_Z     = TRUE; } else { JOY2_Z     = FALSE; }
	if(JOY2 & BUTTON_START ) { JOY2_START = TRUE; } else { JOY2_START = FALSE; }
	if(JOY2 & BUTTON_MODE  ) { JOY2_MODE  = TRUE; } else { JOY2_MODE  = FALSE; }
	
	if(gPodeMover==0)
	{
		JOY1_UP    = FALSE; JOY2_UP    = FALSE;
		JOY1_DOWN  = FALSE; JOY2_DOWN  = FALSE;
		JOY1_LEFT  = FALSE; JOY2_LEFT  = FALSE;
		JOY1_RIGHT = FALSE; JOY2_RIGHT = FALSE;
		JOY1_A     = FALSE; JOY2_A     = FALSE;
		JOY1_B     = FALSE; JOY2_B     = FALSE;
		JOY1_C     = FALSE; JOY2_C     = FALSE;
		JOY1_X     = FALSE; JOY2_X     = FALSE;
		JOY1_Y     = FALSE; JOY2_Y     = FALSE;
		JOY1_Z     = FALSE; JOY2_Z     = FALSE;
		JOY1_START = FALSE; JOY2_START = FALSE;
		JOY1_MODE  = FALSE; JOY2_MODE  = FALSE;
	}
	
	//---P1
	
	if (JOY1_UP) {
	if (key_JOY1_UP_pressed==1 && key_JOY1_UP_hold==0) { key_JOY1_UP_hold=1; key_JOY1_UP_pressed=0; }
	if (key_JOY1_UP_pressed==0 && key_JOY1_UP_hold==0) { key_JOY1_UP_pressed=1; }}
	if (!JOY1_UP) {
	if (key_JOY1_UP_released==1) { key_JOY1_UP_released=0; key_JOY1_UP_pressed=0; key_JOY1_UP_hold=0; }
	if (key_JOY1_UP_pressed>0 || key_JOY1_UP_hold>0) {key_JOY1_UP_released=1;}}
	if (key_JOY1_UP_pressed==0 && key_JOY1_UP_hold==0 && key_JOY1_UP_released==0){ P[1].key_JOY_UP_status=0; }
	if (key_JOY1_UP_pressed  ==1 ) { P[1].key_JOY_UP_status=1; }
	if (key_JOY1_UP_hold     ==1 ) { P[1].key_JOY_UP_status=2; }
	if (key_JOY1_UP_released ==1 ) { P[1].key_JOY_UP_status=3; }
	
	if (JOY1_DOWN) {
	if (key_JOY1_DOWN_pressed==1 && key_JOY1_DOWN_hold==0) { key_JOY1_DOWN_hold=1; key_JOY1_DOWN_pressed=0; }
	if (key_JOY1_DOWN_pressed==0 && key_JOY1_DOWN_hold==0) { key_JOY1_DOWN_pressed=1; }}
	if (!JOY1_DOWN) {
	if (key_JOY1_DOWN_released==1) { key_JOY1_DOWN_released=0; key_JOY1_DOWN_pressed=0; key_JOY1_DOWN_hold=0; }
	if (key_JOY1_DOWN_pressed>0 || key_JOY1_DOWN_hold>0) {key_JOY1_DOWN_released=1;}}
	if (key_JOY1_DOWN_pressed==0 && key_JOY1_DOWN_hold==0 && key_JOY1_DOWN_released==0){ P[1].key_JOY_DOWN_status=0; }
	if (key_JOY1_DOWN_pressed  ==1 ) { P[1].key_JOY_DOWN_status=1; }
	if (key_JOY1_DOWN_hold     ==1 ) { P[1].key_JOY_DOWN_status=2; }
	if (key_JOY1_DOWN_released ==1 ) { P[1].key_JOY_DOWN_status=3; }
	
	if (JOY1_LEFT) {
	if (key_JOY1_LEFT_pressed==1 && key_JOY1_LEFT_hold==0) { key_JOY1_LEFT_hold=1; key_JOY1_LEFT_pressed=0; }
	if (key_JOY1_LEFT_pressed==0 && key_JOY1_LEFT_hold==0) { key_JOY1_LEFT_pressed=1; }}
	if (!JOY1_LEFT) {
	if (key_JOY1_LEFT_released==1) { key_JOY1_LEFT_released=0; key_JOY1_LEFT_pressed=0; key_JOY1_LEFT_hold=0; }
	if (key_JOY1_LEFT_pressed>0 || key_JOY1_LEFT_hold>0) {key_JOY1_LEFT_released=1;}}
	if (key_JOY1_LEFT_pressed==0 && key_JOY1_LEFT_hold==0 && key_JOY1_LEFT_released==0){ P[1].key_JOY_LEFT_status=0; }
	if (key_JOY1_LEFT_pressed  ==1 ) { P[1].key_JOY_LEFT_status=1; }
	if (key_JOY1_LEFT_hold     ==1 ) { P[1].key_JOY_LEFT_status=2; }
	if (key_JOY1_LEFT_released ==1 ) { P[1].key_JOY_LEFT_status=3; }
	
	if (JOY1_RIGHT) {
	if (key_JOY1_RIGHT_pressed==1 && key_JOY1_RIGHT_hold==0) { key_JOY1_RIGHT_hold=1; key_JOY1_RIGHT_pressed=0; }
	if (key_JOY1_RIGHT_pressed==0 && key_JOY1_RIGHT_hold==0) { key_JOY1_RIGHT_pressed=1; }}
	if (!JOY1_RIGHT) {
	if (key_JOY1_RIGHT_released==1) { key_JOY1_RIGHT_released=0; key_JOY1_RIGHT_pressed=0; key_JOY1_RIGHT_hold=0; }
	if (key_JOY1_RIGHT_pressed>0 || key_JOY1_RIGHT_hold>0) {key_JOY1_RIGHT_released=1;}}
	if (key_JOY1_RIGHT_pressed==0 && key_JOY1_RIGHT_hold==0 && key_JOY1_RIGHT_released==0){ P[1].key_JOY_RIGHT_status=0; }
	if (key_JOY1_RIGHT_pressed  ==1 ) { P[1].key_JOY_RIGHT_status=1; }
	if (key_JOY1_RIGHT_hold     ==1 ) { P[1].key_JOY_RIGHT_status=2; }
	if (key_JOY1_RIGHT_released ==1 ) { P[1].key_JOY_RIGHT_status=3; }
	
	if (JOY1_A) {
	if (key_JOY1_A_pressed==1 && key_JOY1_A_hold==0) { key_JOY1_A_hold=1; key_JOY1_A_pressed=0; }
	if (key_JOY1_A_pressed==0 && key_JOY1_A_hold==0) { key_JOY1_A_pressed=1; }}
	if (!JOY1_A) {
	if (key_JOY1_A_released==1) { key_JOY1_A_released=0; key_JOY1_A_pressed=0; key_JOY1_A_hold=0; }
	if (key_JOY1_A_pressed>0 || key_JOY1_A_hold>0) {key_JOY1_A_released=1;}}
	if (key_JOY1_A_pressed==0 && key_JOY1_A_hold==0 && key_JOY1_A_released==0){ P[1].key_JOY_A_status=0; }
	if (key_JOY1_A_pressed  ==1 ) { P[1].key_JOY_A_status=1; }
	if (key_JOY1_A_hold     ==1 ) { P[1].key_JOY_A_status=2; }
	if (key_JOY1_A_released ==1 ) { P[1].key_JOY_A_status=3; }
	
	if (JOY1_B) {
	if (key_JOY1_B_pressed==1 && key_JOY1_B_hold==0) { key_JOY1_B_hold=1; key_JOY1_B_pressed=0; }
	if (key_JOY1_B_pressed==0 && key_JOY1_B_hold==0) { key_JOY1_B_pressed=1; }}
	if (!JOY1_B) {
	if (key_JOY1_B_released==1) { key_JOY1_B_released=0; key_JOY1_B_pressed=0; key_JOY1_B_hold=0; }
	if (key_JOY1_B_pressed>0 || key_JOY1_B_hold>0) {key_JOY1_B_released=1;}}
	if (key_JOY1_B_pressed==0 && key_JOY1_B_hold==0 && key_JOY1_B_released==0){ P[1].key_JOY_B_status=0; }
	if (key_JOY1_B_pressed  ==1 ) { P[1].key_JOY_B_status=1; }
	if (key_JOY1_B_hold     ==1 ) { P[1].key_JOY_B_status=2; }
	if (key_JOY1_B_released ==1 ) { P[1].key_JOY_B_status=3; }
	
	if (JOY1_C) {
	if (key_JOY1_C_pressed==1 && key_JOY1_C_hold==0) { key_JOY1_C_hold=1; key_JOY1_C_pressed=0; }
	if (key_JOY1_C_pressed==0 && key_JOY1_C_hold==0) { key_JOY1_C_pressed=1; }}
	if (!JOY1_C) {
	if (key_JOY1_C_released==1) { key_JOY1_C_released=0; key_JOY1_C_pressed=0; key_JOY1_C_hold=0; }
	if (key_JOY1_C_pressed>0 || key_JOY1_C_hold>0) {key_JOY1_C_released=1;}}
	if (key_JOY1_C_pressed==0 && key_JOY1_C_hold==0 && key_JOY1_C_released==0){ P[1].key_JOY_C_status=0; }
	if (key_JOY1_C_pressed  ==1 ) { P[1].key_JOY_C_status=1; }
	if (key_JOY1_C_hold     ==1 ) { P[1].key_JOY_C_status=2; }
	if (key_JOY1_C_released ==1 ) { P[1].key_JOY_C_status=3; }
	
	if (JOY1_X) {
	if (key_JOY1_X_pressed==1 && key_JOY1_X_hold==0) { key_JOY1_X_hold=1; key_JOY1_X_pressed=0; }
	if (key_JOY1_X_pressed==0 && key_JOY1_X_hold==0) { key_JOY1_X_pressed=1; }}
	if (!JOY1_X) {
	if (key_JOY1_X_released==1) { key_JOY1_X_released=0; key_JOY1_X_pressed=0; key_JOY1_X_hold=0; }
	if (key_JOY1_X_pressed>0 || key_JOY1_X_hold>0) {key_JOY1_X_released=1;}}
	if (key_JOY1_X_pressed==0 && key_JOY1_X_hold==0 && key_JOY1_X_released==0){ P[1].key_JOY_X_status=0; }
	if (key_JOY1_X_pressed  ==1 ) { P[1].key_JOY_X_status=1; }
	if (key_JOY1_X_hold     ==1 ) { P[1].key_JOY_X_status=2; }
	if (key_JOY1_X_released ==1 ) { P[1].key_JOY_X_status=3; }
	
	if (JOY1_Y) {
	if (key_JOY1_Y_pressed==1 && key_JOY1_Y_hold==0) { key_JOY1_Y_hold=1; key_JOY1_Y_pressed=0; }
	if (key_JOY1_Y_pressed==0 && key_JOY1_Y_hold==0) { key_JOY1_Y_pressed=1; }}
	if (!JOY1_Y) {
	if (key_JOY1_Y_released==1) { key_JOY1_Y_released=0; key_JOY1_Y_pressed=0; key_JOY1_Y_hold=0; }
	if (key_JOY1_Y_pressed>0 || key_JOY1_Y_hold>0) {key_JOY1_Y_released=1;}}
	if (key_JOY1_Y_pressed==0 && key_JOY1_Y_hold==0 && key_JOY1_Y_released==0){ P[1].key_JOY_Y_status=0; }
	if (key_JOY1_Y_pressed  ==1 ) { P[1].key_JOY_Y_status=1; }
	if (key_JOY1_Y_hold     ==1 ) { P[1].key_JOY_Y_status=2; }
	if (key_JOY1_Y_released ==1 ) { P[1].key_JOY_Y_status=3; }
	
	if (JOY1_Z) {
	if (key_JOY1_Z_pressed==1 && key_JOY1_Z_hold==0) { key_JOY1_Z_hold=1; key_JOY1_Z_pressed=0; }
	if (key_JOY1_Z_pressed==0 && key_JOY1_Z_hold==0) { key_JOY1_Z_pressed=1; }}
	if (!JOY1_Z) {
	if (key_JOY1_Z_released==1) { key_JOY1_Z_released=0; key_JOY1_Z_pressed=0; key_JOY1_Z_hold=0; }
	if (key_JOY1_Z_pressed>0 || key_JOY1_Z_hold>0) {key_JOY1_Z_released=1;}}
	if (key_JOY1_Z_pressed==0 && key_JOY1_Z_hold==0 && key_JOY1_Z_released==0){ P[1].key_JOY_Z_status=0; }
	if (key_JOY1_Z_pressed  ==1 ) { P[1].key_JOY_Z_status=1; }
	if (key_JOY1_Z_hold     ==1 ) { P[1].key_JOY_Z_status=2; }
	if (key_JOY1_Z_released ==1 ) { P[1].key_JOY_Z_status=3; }
	
	if (JOY1_START) {
	if (key_JOY1_START_pressed==1 && key_JOY1_START_hold==0) { key_JOY1_START_hold=1; key_JOY1_START_pressed=0; }
	if (key_JOY1_START_pressed==0 && key_JOY1_START_hold==0) { key_JOY1_START_pressed=1; }}
	if (!JOY1_START) {
	if (key_JOY1_START_released==1) { key_JOY1_START_released=0; key_JOY1_START_pressed=0; key_JOY1_START_hold=0; }
	if (key_JOY1_START_pressed>0 || key_JOY1_START_hold>0) {key_JOY1_START_released=1;}}
	if (key_JOY1_START_pressed==0 && key_JOY1_START_hold==0 && key_JOY1_START_released==0){ P[1].key_JOY_START_status=0; }
	if (key_JOY1_START_pressed  ==1 ) { P[1].key_JOY_START_status=1; }
	if (key_JOY1_START_hold     ==1 ) { P[1].key_JOY_START_status=2; }
	if (key_JOY1_START_released ==1 ) { P[1].key_JOY_START_status=3; }
	
	if (JOY1_MODE) {
	if (key_JOY1_MODE_pressed==1 && key_JOY1_MODE_hold==0) { key_JOY1_MODE_hold=1; key_JOY1_MODE_pressed=0; }
	if (key_JOY1_MODE_pressed==0 && key_JOY1_MODE_hold==0) { key_JOY1_MODE_pressed=1; }}
	if (!JOY1_MODE) {
	if (key_JOY1_MODE_released==1) { key_JOY1_MODE_released=0; key_JOY1_MODE_pressed=0; key_JOY1_MODE_hold=0; }
	if (key_JOY1_MODE_pressed>0 || key_JOY1_MODE_hold>0) {key_JOY1_MODE_released=1;}}
	if (key_JOY1_MODE_pressed==0 && key_JOY1_MODE_hold==0 && key_JOY1_MODE_released==0){ P[1].key_JOY_MODE_status=0; }
	if (key_JOY1_MODE_pressed  ==1 ) { P[1].key_JOY_MODE_status=1; }
	if (key_JOY1_MODE_hold     ==1 ) { P[1].key_JOY_MODE_status=2; }
	if (key_JOY1_MODE_released ==1 ) { P[1].key_JOY_MODE_status=3; }
	
	//---P2
	
	if (JOY2_UP) {
	if (key_JOY2_UP_pressed==1 && key_JOY2_UP_hold==0) { key_JOY2_UP_hold=1; key_JOY2_UP_pressed=0; }
	if (key_JOY2_UP_pressed==0 && key_JOY2_UP_hold==0) { key_JOY2_UP_pressed=1; }}
	if (!JOY2_UP) {
	if (key_JOY2_UP_released==1) { key_JOY2_UP_released=0; key_JOY2_UP_pressed=0; key_JOY2_UP_hold=0; }
	if (key_JOY2_UP_pressed>0 || key_JOY2_UP_hold>0) {key_JOY2_UP_released=1;}}
	if (key_JOY2_UP_pressed==0 && key_JOY2_UP_hold==0 && key_JOY2_UP_released==0){ P[2].key_JOY_UP_status=0; }
	if (key_JOY2_UP_pressed  ==1 ) { P[2].key_JOY_UP_status=1; }
	if (key_JOY2_UP_hold     ==1 ) { P[2].key_JOY_UP_status=2; }
	if (key_JOY2_UP_released ==1 ) { P[2].key_JOY_UP_status=3; }
	
	if (JOY2_DOWN) {
	if (key_JOY2_DOWN_pressed==1 && key_JOY2_DOWN_hold==0) { key_JOY2_DOWN_hold=1; key_JOY2_DOWN_pressed=0; }
	if (key_JOY2_DOWN_pressed==0 && key_JOY2_DOWN_hold==0) { key_JOY2_DOWN_pressed=1; }}
	if (!JOY2_DOWN) {
	if (key_JOY2_DOWN_released==1) { key_JOY2_DOWN_released=0; key_JOY2_DOWN_pressed=0; key_JOY2_DOWN_hold=0; }
	if (key_JOY2_DOWN_pressed>0 || key_JOY2_DOWN_hold>0) {key_JOY2_DOWN_released=1;}}
	if (key_JOY2_DOWN_pressed==0 && key_JOY2_DOWN_hold==0 && key_JOY2_DOWN_released==0){ P[2].key_JOY_DOWN_status=0; }
	if (key_JOY2_DOWN_pressed  ==1 ) { P[2].key_JOY_DOWN_status=1; }
	if (key_JOY2_DOWN_hold     ==1 ) { P[2].key_JOY_DOWN_status=2; }
	if (key_JOY2_DOWN_released ==1 ) { P[2].key_JOY_DOWN_status=3; }
	
	if (JOY2_LEFT) {
	if (key_JOY2_LEFT_pressed==1 && key_JOY2_LEFT_hold==0) { key_JOY2_LEFT_hold=1; key_JOY2_LEFT_pressed=0; }
	if (key_JOY2_LEFT_pressed==0 && key_JOY2_LEFT_hold==0) { key_JOY2_LEFT_pressed=1; }}
	if (!JOY2_LEFT) {
	if (key_JOY2_LEFT_released==1) { key_JOY2_LEFT_released=0; key_JOY2_LEFT_pressed=0; key_JOY2_LEFT_hold=0; }
	if (key_JOY2_LEFT_pressed>0 || key_JOY2_LEFT_hold>0) {key_JOY2_LEFT_released=1;}}
	if (key_JOY2_LEFT_pressed==0 && key_JOY2_LEFT_hold==0 && key_JOY2_LEFT_released==0){ P[2].key_JOY_LEFT_status=0; }
	if (key_JOY2_LEFT_pressed  ==1 ) { P[2].key_JOY_LEFT_status=1; }
	if (key_JOY2_LEFT_hold     ==1 ) { P[2].key_JOY_LEFT_status=2; }
	if (key_JOY2_LEFT_released ==1 ) { P[2].key_JOY_LEFT_status=3; }
	
	if (JOY2_RIGHT) {
	if (key_JOY2_RIGHT_pressed==1 && key_JOY2_RIGHT_hold==0) { key_JOY2_RIGHT_hold=1; key_JOY2_RIGHT_pressed=0; }
	if (key_JOY2_RIGHT_pressed==0 && key_JOY2_RIGHT_hold==0) { key_JOY2_RIGHT_pressed=1; }}
	if (!JOY2_RIGHT) {
	if (key_JOY2_RIGHT_released==1) { key_JOY2_RIGHT_released=0; key_JOY2_RIGHT_pressed=0; key_JOY2_RIGHT_hold=0; }
	if (key_JOY2_RIGHT_pressed>0 || key_JOY2_RIGHT_hold>0) {key_JOY2_RIGHT_released=1;}}
	if (key_JOY2_RIGHT_pressed==0 && key_JOY2_RIGHT_hold==0 && key_JOY2_RIGHT_released==0){ P[2].key_JOY_RIGHT_status=0; }
	if (key_JOY2_RIGHT_pressed  ==1 ) { P[2].key_JOY_RIGHT_status=1; }
	if (key_JOY2_RIGHT_hold     ==1 ) { P[2].key_JOY_RIGHT_status=2; }
	if (key_JOY2_RIGHT_released ==1 ) { P[2].key_JOY_RIGHT_status=3; }
	
	if (JOY2_A) {
	if (key_JOY2_A_pressed==1 && key_JOY2_A_hold==0) { key_JOY2_A_hold=1; key_JOY2_A_pressed=0; }
	if (key_JOY2_A_pressed==0 && key_JOY2_A_hold==0) { key_JOY2_A_pressed=1; }}
	if (!JOY2_A) {
	if (key_JOY2_A_released==1) { key_JOY2_A_released=0; key_JOY2_A_pressed=0; key_JOY2_A_hold=0; }
	if (key_JOY2_A_pressed>0 || key_JOY2_A_hold>0) {key_JOY2_A_released=1;}}
	if (key_JOY2_A_pressed==0 && key_JOY2_A_hold==0 && key_JOY2_A_released==0){ P[2].key_JOY_A_status=0; }
	if (key_JOY2_A_pressed  ==1 ) { P[2].key_JOY_A_status=1; }
	if (key_JOY2_A_hold     ==1 ) { P[2].key_JOY_A_status=2; }
	if (key_JOY2_A_released ==1 ) { P[2].key_JOY_A_status=3; }
	
	if (JOY2_B) {
	if (key_JOY2_B_pressed==1 && key_JOY2_B_hold==0) { key_JOY2_B_hold=1; key_JOY2_B_pressed=0; }
	if (key_JOY2_B_pressed==0 && key_JOY2_B_hold==0) { key_JOY2_B_pressed=1; }}
	if (!JOY2_B) {
	if (key_JOY2_B_released==1) { key_JOY2_B_released=0; key_JOY2_B_pressed=0; key_JOY2_B_hold=0; }
	if (key_JOY2_B_pressed>0 || key_JOY2_B_hold>0) {key_JOY2_B_released=1;}}
	if (key_JOY2_B_pressed==0 && key_JOY2_B_hold==0 && key_JOY2_B_released==0){ P[2].key_JOY_B_status=0; }
	if (key_JOY2_B_pressed  ==1 ) { P[2].key_JOY_B_status=1; }
	if (key_JOY2_B_hold     ==1 ) { P[2].key_JOY_B_status=2; }
	if (key_JOY2_B_released ==1 ) { P[2].key_JOY_B_status=3; }
	
	if (JOY2_C) {
	if (key_JOY2_C_pressed==1 && key_JOY2_C_hold==0) { key_JOY2_C_hold=1; key_JOY2_C_pressed=0; }
	if (key_JOY2_C_pressed==0 && key_JOY2_C_hold==0) { key_JOY2_C_pressed=1; }}
	if (!JOY2_C) {
	if (key_JOY2_C_released==1) { key_JOY2_C_released=0; key_JOY2_C_pressed=0; key_JOY2_C_hold=0; }
	if (key_JOY2_C_pressed>0 || key_JOY2_C_hold>0) {key_JOY2_C_released=1;}}
	if (key_JOY2_C_pressed==0 && key_JOY2_C_hold==0 && key_JOY2_C_released==0){ P[2].key_JOY_C_status=0; }
	if (key_JOY2_C_pressed  ==1 ) { P[2].key_JOY_C_status=1; }
	if (key_JOY2_C_hold     ==1 ) { P[2].key_JOY_C_status=2; }
	if (key_JOY2_C_released ==1 ) { P[2].key_JOY_C_status=3; }
	
	if (JOY2_X) {
	if (key_JOY2_X_pressed==1 && key_JOY2_X_hold==0) { key_JOY2_X_hold=1; key_JOY2_X_pressed=0; }
	if (key_JOY2_X_pressed==0 && key_JOY2_X_hold==0) { key_JOY2_X_pressed=1; }}
	if (!JOY2_X) {
	if (key_JOY2_X_released==1) { key_JOY2_X_released=0; key_JOY2_X_pressed=0; key_JOY2_X_hold=0; }
	if (key_JOY2_X_pressed>0 || key_JOY2_X_hold>0) {key_JOY2_X_released=1;}}
	if (key_JOY2_X_pressed==0 && key_JOY2_X_hold==0 && key_JOY2_X_released==0){ P[2].key_JOY_X_status=0; }
	if (key_JOY2_X_pressed  ==1 ) { P[2].key_JOY_X_status=1; }
	if (key_JOY2_X_hold     ==1 ) { P[2].key_JOY_X_status=2; }
	if (key_JOY2_X_released ==1 ) { P[2].key_JOY_X_status=3; }
	
	if (JOY2_Y) {
	if (key_JOY2_Y_pressed==1 && key_JOY2_Y_hold==0) { key_JOY2_Y_hold=1; key_JOY2_Y_pressed=0; }
	if (key_JOY2_Y_pressed==0 && key_JOY2_Y_hold==0) { key_JOY2_Y_pressed=1; }}
	if (!JOY2_Y) {
	if (key_JOY2_Y_released==1) { key_JOY2_Y_released=0; key_JOY2_Y_pressed=0; key_JOY2_Y_hold=0; }
	if (key_JOY2_Y_pressed>0 || key_JOY2_Y_hold>0) {key_JOY2_Y_released=1;}}
	if (key_JOY2_Y_pressed==0 && key_JOY2_Y_hold==0 && key_JOY2_Y_released==0){ P[2].key_JOY_Y_status=0; }
	if (key_JOY2_Y_pressed  ==1 ) { P[2].key_JOY_Y_status=1; }
	if (key_JOY2_Y_hold     ==1 ) { P[2].key_JOY_Y_status=2; }
	if (key_JOY2_Y_released ==1 ) { P[2].key_JOY_Y_status=3; }
	
	if (JOY2_Z) {
	if (key_JOY2_Z_pressed==1 && key_JOY2_Z_hold==0) { key_JOY2_Z_hold=1; key_JOY2_Z_pressed=0; }
	if (key_JOY2_Z_pressed==0 && key_JOY2_Z_hold==0) { key_JOY2_Z_pressed=1; }}
	if (!JOY2_Z) {
	if (key_JOY2_Z_released==1) { key_JOY2_Z_released=0; key_JOY2_Z_pressed=0; key_JOY2_Z_hold=0; }
	if (key_JOY2_Z_pressed>0 || key_JOY2_Z_hold>0) {key_JOY2_Z_released=1;}}
	if (key_JOY2_Z_pressed==0 && key_JOY2_Z_hold==0 && key_JOY2_Z_released==0){ P[2].key_JOY_Z_status=0; }
	if (key_JOY2_Z_pressed  ==1 ) { P[2].key_JOY_Z_status=1; }
	if (key_JOY2_Z_hold     ==1 ) { P[2].key_JOY_Z_status=2; }
	if (key_JOY2_Z_released ==1 ) { P[2].key_JOY_Z_status=3; }
	
	if (JOY2_START) {
	if (key_JOY2_START_pressed==1 && key_JOY2_START_hold==0) { key_JOY2_START_hold=1; key_JOY2_START_pressed=0; }
	if (key_JOY2_START_pressed==0 && key_JOY2_START_hold==0) { key_JOY2_START_pressed=1; }}
	if (!JOY2_START) {
	if (key_JOY2_START_released==1) { key_JOY2_START_released=0; key_JOY2_START_pressed=0; key_JOY2_START_hold=0; }
	if (key_JOY2_START_pressed>0 || key_JOY2_START_hold>0) {key_JOY2_START_released=1;}}
	if (key_JOY2_START_pressed==0 && key_JOY2_START_hold==0 && key_JOY2_START_released==0){ P[2].key_JOY_START_status=0; }
	if (key_JOY2_START_pressed  ==1 ) { P[2].key_JOY_START_status=1; }
	if (key_JOY2_START_hold     ==1 ) { P[2].key_JOY_START_status=2; }
	if (key_JOY2_START_released ==1 ) { P[2].key_JOY_START_status=3; }
	
	if (JOY2_MODE) {
	if (key_JOY2_MODE_pressed==1 && key_JOY2_MODE_hold==0) { key_JOY2_MODE_hold=1; key_JOY2_MODE_pressed=0; }
	if (key_JOY2_MODE_pressed==0 && key_JOY2_MODE_hold==0) { key_JOY2_MODE_pressed=1; }}
	if (!JOY2_MODE) {
	if (key_JOY2_MODE_released==1) { key_JOY2_MODE_released=0; key_JOY2_MODE_pressed=0; key_JOY2_MODE_hold=0; }
	if (key_JOY2_MODE_pressed>0 || key_JOY2_MODE_hold>0) {key_JOY2_MODE_released=1;}}
	if (key_JOY2_MODE_pressed==0 && key_JOY2_MODE_hold==0 && key_JOY2_MODE_released==0){ P[2].key_JOY_MODE_status=0; }
	if (key_JOY2_MODE_pressed  ==1 ) { P[2].key_JOY_MODE_status=1; }
	if (key_JOY2_MODE_hold     ==1 ) { P[2].key_JOY_MODE_status=2; }
	if (key_JOY2_MODE_released ==1 ) { P[2].key_JOY_MODE_status=3; }
	
	//---InputTimerCountDown; InputArray
	u8 dir=0;
	
	if(P[1].key_JOY_UP_status   ==1){ P[1].joyDirTimer[8]=gInputTimerCountDown+1; dir=8; }
	if(P[1].key_JOY_DOWN_status ==1){ P[1].joyDirTimer[2]=gInputTimerCountDown+1; dir=2; }
	if(P[1].key_JOY_LEFT_status ==1){ P[1].joyDirTimer[4]=gInputTimerCountDown+1; dir=4; }
	if(P[1].key_JOY_RIGHT_status==1){ P[1].joyDirTimer[6]=gInputTimerCountDown+1; dir=6; }
	
	if(dir!=0)
	{
		P[1].inputArray[4] = P[1].inputArray[3];
		P[1].inputArray[3] = P[1].inputArray[2];
		P[1].inputArray[2] = P[1].inputArray[1];
		P[1].inputArray[1] = P[1].inputArray[0];
		P[1].inputArray[0] = dir;
	}
	
	dir=0;
	
	if(P[2].key_JOY_UP_status   ==1){ P[2].joyDirTimer[8]=gInputTimerCountDown+1; dir=8; }
	if(P[2].key_JOY_DOWN_status ==1){ P[2].joyDirTimer[2]=gInputTimerCountDown+1; dir=2; }
	if(P[2].key_JOY_LEFT_status ==1){ P[2].joyDirTimer[4]=gInputTimerCountDown+1; dir=4; }
	if(P[2].key_JOY_RIGHT_status==1){ P[2].joyDirTimer[6]=gInputTimerCountDown+1; dir=6; }
	
	if(dir!=0)
	{
		P[2].inputArray[4] = P[2].inputArray[3];
		P[2].inputArray[3] = P[2].inputArray[2];
		P[2].inputArray[2] = P[2].inputArray[1];
		P[2].inputArray[1] = P[2].inputArray[0];
		P[2].inputArray[0] = dir;
	}
	
	if( P[1].joyDirTimer[8]>0 ){ P[1].joyDirTimer[8]--; }
	if( P[1].joyDirTimer[2]>0 ){ P[1].joyDirTimer[2]--; }
	if( P[1].joyDirTimer[4]>0 ){ P[1].joyDirTimer[4]--; }
	if( P[1].joyDirTimer[6]>0 ){ P[1].joyDirTimer[6]--; }
	
	if( P[2].joyDirTimer[8]>0 ){ P[2].joyDirTimer[8]--; }
	if( P[2].joyDirTimer[2]>0 ){ P[2].joyDirTimer[2]--; }
	if( P[2].joyDirTimer[4]>0 ){ P[2].joyDirTimer[4]--; }
	if( P[2].joyDirTimer[6]>0 ){ P[2].joyDirTimer[6]--; }
	//---AttackPower & AttackButton
	//P[1].attackPower =0;
	P[1].attackButton=0;
	//P[2].attackPower =0;
	P[2].attackButton=0;
	
	if(P[1].key_JOY_A_status==1){ P[1].attackPower=1; P[1].attackButton=4; }
	if(P[1].key_JOY_B_status==1){ P[1].attackPower=2; P[1].attackButton=5; }
	if(P[1].key_JOY_C_status==1){ P[1].attackPower=3; P[1].attackButton=6; }
	if(P[1].key_JOY_X_status==1){ P[1].attackPower=1; P[1].attackButton=1; }
	if(P[1].key_JOY_Y_status==1){ P[1].attackPower=2; P[1].attackButton=2; }
	if(P[1].key_JOY_Z_status==1){ P[1].attackPower=3; P[1].attackButton=3; }
	
	if(P[2].key_JOY_A_status==1){ P[2].attackPower=1; P[2].attackButton=4; }
	if(P[2].key_JOY_B_status==1){ P[2].attackPower=2; P[2].attackButton=5; }
	if(P[2].key_JOY_C_status==1){ P[2].attackPower=3; P[2].attackButton=6; }
	if(P[2].key_JOY_X_status==1){ P[2].attackPower=1; P[2].attackButton=1; }
	if(P[2].key_JOY_Y_status==1){ P[2].attackPower=2; P[2].attackButton=2; }
	if(P[2].key_JOY_Z_status==1){ P[2].attackPower=3; P[2].attackButton=3; }
	//---
	if((P[1].key_JOY_MODE_status>=1 && P[1].key_JOY_START_status==1) && gDebug==0) //Ativa o Debug com P1_MODE
	{ 
		gDebug=1; 
		P[1].key_JOY_MODE_status=0; 
		P[1].key_JOY_START_status=0; 
		SPR_setVisibility(GE[1].sprite, VISIBLE);
		SPR_setVisibility(GE[2].sprite, VISIBLE);
	} 
	if((P[1].key_JOY_MODE_status>=1 && P[1].key_JOY_START_status==1) && gDebug==1) //Desativa o Debug com P1_MODE
	{ 
		gDebug=0; 
		P[1].key_JOY_MODE_status=0; 
		P[1].key_JOY_START_status=0; 
		VDP_drawText("                              ", 1, 1);
		VDP_drawText("                              ", 1, 2);
		VDP_drawText("                              ", 1, 3);
		SPR_setPosition(Rect1BB1_Q1,-8,-8); SPR_setPosition(Rect1BB1_Q2,-8,-8); SPR_setPosition(Rect1BB1_Q3,-8,-8); SPR_setPosition(Rect1BB1_Q4,-8,-8);
		SPR_setPosition(Rect1HB1_Q1,-8,-8); SPR_setPosition(Rect1HB1_Q2,-8,-8); SPR_setPosition(Rect1HB1_Q3,-8,-8); SPR_setPosition(Rect1HB1_Q4,-8,-8);
		SPR_setPosition(Rect2BB1_Q1,-8,-8); SPR_setPosition(Rect2BB1_Q2,-8,-8); SPR_setPosition(Rect2BB1_Q3,-8,-8); SPR_setPosition(Rect2BB1_Q4,-8,-8);
		SPR_setPosition(Rect2HB1_Q1,-8,-8); SPR_setPosition(Rect2HB1_Q2,-8,-8); SPR_setPosition(Rect2HB1_Q3,-8,-8); SPR_setPosition(Rect2HB1_Q4,-8,-8);
		SPR_setVisibility(GE[1].sprite, HIDDEN);
		SPR_setVisibility(GE[2].sprite, HIDDEN);
	} 
	//---
	
	// Troca de Paleta
	if(P[1].key_JOY_MODE_status==1)
	{
		if( (P[1].id==P[2].id) && P[1].palID==P[2].palID-1){ P[1].palID++; } //evita paleta repetida in game
		
		if(P[1].id==1) //TERRY
		{
			if(      P[1].palID==1){ VDP_setPalette(PAL2, spr_terry_pal2.palette->data); P[1].palID=2; 
			}else if(P[1].palID==2){ VDP_setPalette(PAL2, spr_terry_pal3.palette->data); P[1].palID=3; 
			}else if(P[1].palID==3){ VDP_setPalette(PAL2, spr_terry_pal4.palette->data); P[1].palID=4; 
			}else if(P[1].palID==4){ VDP_setPalette(PAL2, spr_terry_pal5.palette->data); P[1].palID=5; 
			}else if(P[1].palID==5){ VDP_setPalette(PAL2, spr_terry_pal6.palette->data); P[1].palID=6; 
			}else if(P[1].palID==6){ VDP_setPalette(PAL2, spr_terry_pal7.palette->data); P[1].palID=7; 
			}else if(P[1].palID==7){ VDP_setPalette(PAL2, spr_terry_pal8.palette->data); P[1].palID=8; 
			}else if(P[1].palID==8){ VDP_setPalette(PAL2, spr_terry_pal1.palette->data); P[1].palID=1; 
			}
		}
		if(P[1].id==2) //MAI
		{
			if(      P[1].palID==1){ VDP_setPalette(PAL2, spr_mai_pal2.palette->data); P[1].palID=2; 
			}else if(P[1].palID==2){ VDP_setPalette(PAL2, spr_mai_pal3.palette->data); P[1].palID=3;
			}else if(P[1].palID==3){ VDP_setPalette(PAL2, spr_mai_pal4.palette->data); P[1].palID=4;
			}else if(P[1].palID==4){ VDP_setPalette(PAL2, spr_mai_pal5.palette->data); P[1].palID=5;
			}else if(P[1].palID==5){ VDP_setPalette(PAL2, spr_mai_pal6.palette->data); P[1].palID=6;
			}else if(P[1].palID==6){ VDP_setPalette(PAL2, spr_mai_pal7.palette->data); P[1].palID=7;
			}else if(P[1].palID==7){ VDP_setPalette(PAL2, spr_mai_pal8.palette->data); P[1].palID=8;
			}else if(P[1].palID==8){ VDP_setPalette(PAL2, spr_mai_pal1.palette->data); P[1].palID=1;
			}
		}
		if(P[1].id==3) //BLUEMARY
		{
			if(      P[1].palID==1){ VDP_setPalette(PAL2, spr_bluemary_pal2.palette->data); P[1].palID=2; 
			}else if(P[1].palID==2){ VDP_setPalette(PAL2, spr_bluemary_pal3.palette->data); P[1].palID=3;
			}else if(P[1].palID==3){ VDP_setPalette(PAL2, spr_bluemary_pal4.palette->data); P[1].palID=4;
			}else if(P[1].palID==4){ VDP_setPalette(PAL2, spr_bluemary_pal5.palette->data); P[1].palID=5;
			}else if(P[1].palID==5){ VDP_setPalette(PAL2, spr_bluemary_pal6.palette->data); P[1].palID=6;
			}else if(P[1].palID==6){ VDP_setPalette(PAL2, spr_bluemary_pal7.palette->data); P[1].palID=7;
			}else if(P[1].palID==7){ VDP_setPalette(PAL2, spr_bluemary_pal8.palette->data); P[1].palID=8;
			}else if(P[1].palID==8){ VDP_setPalette(PAL2, spr_bluemary_pal1.palette->data); P[1].palID=1;
			}
		}
		if(P[2].id==4) //joe
		{
			if(      P[1].palID==1){ VDP_setPalette(PAL3, spr_joe_pal2.palette->data); P[2].palID=2; 
			}else if(P[1].palID==2){ VDP_setPalette(PAL3, spr_joe_pal3.palette->data); P[2].palID=3;
			}else if(P[1].palID==3){ VDP_setPalette(PAL3, spr_joe_pal4.palette->data); P[2].palID=4;
			}else if(P[1].palID==4){ VDP_setPalette(PAL3, spr_joe_pal5.palette->data); P[2].palID=5;
			}else if(P[1].palID==5){ VDP_setPalette(PAL3, spr_joe_pal6.palette->data); P[2].palID=6;
			}else if(P[1].palID==6){ VDP_setPalette(PAL3, spr_joe_pal7.palette->data); P[2].palID=7;
			}else if(P[1].palID==7){ VDP_setPalette(PAL3, spr_joe_pal8.palette->data); P[2].palID=8;
			}else if(P[1].palID==8){ VDP_setPalette(PAL3, spr_joe_pal1.palette->data); P[2].palID=1;
			}
		}
	}
	if(P[2].key_JOY_MODE_status==1)
	{
		if( (P[1].id==P[2].id) && P[2].palID==P[1].palID-1){ P[2].palID++; } //evita paleta repetida in game
		
		if(P[2].id==1) //TERRY
		{
			if(      P[2].palID==1){ VDP_setPalette(PAL3, spr_terry_pal2.palette->data); P[2].palID=2; 
			}else if(P[2].palID==2){ VDP_setPalette(PAL3, spr_terry_pal3.palette->data); P[2].palID=3; 
			}else if(P[2].palID==3){ VDP_setPalette(PAL3, spr_terry_pal4.palette->data); P[2].palID=4; 
			}else if(P[2].palID==4){ VDP_setPalette(PAL3, spr_terry_pal5.palette->data); P[2].palID=5; 
			}else if(P[2].palID==5){ VDP_setPalette(PAL3, spr_terry_pal6.palette->data); P[2].palID=6; 
			}else if(P[2].palID==6){ VDP_setPalette(PAL3, spr_terry_pal7.palette->data); P[2].palID=7; 
			}else if(P[2].palID==7){ VDP_setPalette(PAL3, spr_terry_pal8.palette->data); P[2].palID=8; 
			}else if(P[2].palID==8){ VDP_setPalette(PAL3, spr_terry_pal1.palette->data); P[2].palID=1; 
			}
		}
		if(P[2].id==2) //MAI
		{
			if(      P[2].palID==1){ VDP_setPalette(PAL3, spr_mai_pal2.palette->data); P[2].palID=2; 
			}else if(P[2].palID==2){ VDP_setPalette(PAL3, spr_mai_pal3.palette->data); P[2].palID=3;
			}else if(P[2].palID==3){ VDP_setPalette(PAL3, spr_mai_pal4.palette->data); P[2].palID=4;
			}else if(P[2].palID==4){ VDP_setPalette(PAL3, spr_mai_pal5.palette->data); P[2].palID=5;
			}else if(P[2].palID==5){ VDP_setPalette(PAL3, spr_mai_pal6.palette->data); P[2].palID=6;
			}else if(P[2].palID==6){ VDP_setPalette(PAL3, spr_mai_pal7.palette->data); P[2].palID=7;
			}else if(P[2].palID==7){ VDP_setPalette(PAL3, spr_mai_pal8.palette->data); P[2].palID=8;
			}else if(P[2].palID==8){ VDP_setPalette(PAL3, spr_mai_pal1.palette->data); P[2].palID=1;
			}
		}
		if(P[2].id==3) //BLUEMARY
		{
			if(      P[2].palID==1){ VDP_setPalette(PAL3, spr_bluemary_pal2.palette->data); P[2].palID=2; 
			}else if(P[2].palID==2){ VDP_setPalette(PAL3, spr_bluemary_pal3.palette->data); P[2].palID=3;
			}else if(P[2].palID==3){ VDP_setPalette(PAL3, spr_bluemary_pal4.palette->data); P[2].palID=4;
			}else if(P[2].palID==4){ VDP_setPalette(PAL3, spr_bluemary_pal5.palette->data); P[2].palID=5;
			}else if(P[2].palID==5){ VDP_setPalette(PAL3, spr_bluemary_pal6.palette->data); P[2].palID=6;
			}else if(P[2].palID==6){ VDP_setPalette(PAL3, spr_bluemary_pal7.palette->data); P[2].palID=7;
			}else if(P[2].palID==7){ VDP_setPalette(PAL3, spr_bluemary_pal8.palette->data); P[2].palID=8;
			}else if(P[2].palID==8){ VDP_setPalette(PAL3, spr_bluemary_pal1.palette->data); P[2].palID=1;
			}
		}
		if(P[2].id==4) //joe
		{
			if(      P[2].palID==1){ VDP_setPalette(PAL3, spr_joe_pal2.palette->data); P[2].palID=2; 
			}else if(P[2].palID==2){ VDP_setPalette(PAL3, spr_joe_pal3.palette->data); P[2].palID=3;
			}else if(P[2].palID==3){ VDP_setPalette(PAL3, spr_joe_pal4.palette->data); P[2].palID=4;
			}else if(P[2].palID==4){ VDP_setPalette(PAL3, spr_joe_pal5.palette->data); P[2].palID=5;
			}else if(P[2].palID==5){ VDP_setPalette(PAL3, spr_joe_pal6.palette->data); P[2].palID=6;
			}else if(P[2].palID==6){ VDP_setPalette(PAL3, spr_joe_pal7.palette->data); P[2].palID=7;
			}else if(P[2].palID==7){ VDP_setPalette(PAL3, spr_joe_pal8.palette->data); P[2].palID=8;
			}else if(P[2].palID==8){ VDP_setPalette(PAL3, spr_joe_pal1.palette->data); P[2].palID=1;
			}
		}
		
	}

}

void FUNCAO_FSM()
{
	
	if(P[1].direcao==1){gDistancia=P[2].x-P[1].x;}else{gDistancia=P[1].x-P[2].x;}
	for(i=1; i<=2; i++)
	{
		u8 PA; //Player Attack (Player que ataca)
		u8 PR; //Player Receive (Player que recebe o dano)
		if(i==1){ PA=1; PR=2; }else{ PA=2; PR=1; }
		
		
		//O calculo abaixo é usado para defender magias :)
		u8 distancia_Player_X_Magia=250;
		if(P[PR].fBallX!=0 && P[PR].fBallY!=0)
		{
			if(P[PA].direcao== 1){ distancia_Player_X_Magia=P[PR].fBallX-P[PA].x+25; }
			if(P[PA].direcao==-1){ distancia_Player_X_Magia=P[PA].x-P[PR].fBallX; }
		}		
		
		//defesa start - EM PE e ABAIXADO!
		if
		(
			(
				(P[PR].state>=101 && P[PR].state<=106) || 
				(P[PR].state>=151 && P[PR].state<=156) || 
				(P[PR].state>=201 && P[PR].state<=206) || 
				(P[PR].state>=301 && P[PR].state<=306) || 
				(P[PR].state>=311 && P[PR].state<=316) || 
				(P[PR].state>=321 && P[PR].state<=326) || 
				(P[PR].state>=700 && P[PR].state<=790) || 
				(distancia_Player_X_Magia<=70)
			) && 
			(
			  (
			    (P[PA].direcao== 1 && P[PA].key_JOY_LEFT_status ==2) || 
			    (P[PA].direcao==-1 && P[PA].key_JOY_RIGHT_status==2)
			  ) && (P[PA].state==100 || P[PA].state==200 || P[PA].state==410 || P[PA].state==420)
			) 
		){
			if(P[i].key_JOY_DOWN_status==2){ PLAYER_STATE(i,207); }else{ PLAYER_STATE(i,107); } //defesa (107 = em pe ; 207 = abaixado)
		}
		
		//soltou a defesa - EM PE
		if( (P[i].state==107 || P[i].state==108) && 
		(
		    (P[PA].direcao==1 && P[PA].key_JOY_LEFT_status==0) || 
		    (P[PA].direcao==-1 && P[PA].key_JOY_RIGHT_status==0)
		)
		){ PLAYER_STATE(i,109); } 
		
		//defesa se encerrou com o final do ataque - EM PE
		if(P[i].state==108 && 
			(P[PR].state==100 || P[PR].state==200 || P[PR].state==410 || P[PR].state==420)
		){ PLAYER_STATE(i,109); } 
		
		//soltou a defesa - ABAIXADO
		if( (P[i].state==207 || P[i].state==208) && 
		(
		    (P[PA].direcao==1 && P[PA].key_JOY_LEFT_status==0) || 
		    (P[PA].direcao==-1 && P[PA].key_JOY_RIGHT_status==0)
		)
		){ PLAYER_STATE(i,209); } 
		
		//defesa se encerrou com o final do ataque - ABAIXADO
		if(P[i].state==208 && 
			(P[PR].state==100 || P[PR].state==200 || P[PR].state==410 || P[PR].state==420)
		){ PLAYER_STATE(i,209); } 
		
		//variacao de defesa em pe e abaixado
		if(P[i].state==108 && P[i].key_JOY_DOWN_status==2){ PLAYER_STATE(i,208); }
		if(P[i].state==208 && P[i].key_JOY_DOWN_status==0){ PLAYER_STATE(i,108); }
		
		//golpes normais!
		if(gDistancia>64)
		{
			//golpes em pe de longe
			if(P[i].key_JOY_X_status==1 && (P[i].state==100 || P[i].state==481 || P[i].state==410 || P[i].state==420)){ PLAYER_STATE(i,101); }
			if(P[i].key_JOY_Y_status==1 && (P[i].state==100 || P[i].state==481 || P[i].state==410 || P[i].state==420)){ PLAYER_STATE(i,102); }
			if(P[i].key_JOY_Z_status==1 && (P[i].state==100 || P[i].state==481 || P[i].state==410 || P[i].state==420)){ PLAYER_STATE(i,103); }
			if(P[i].key_JOY_A_status==1 && (P[i].state==100 || P[i].state==481 || P[i].state==410 || P[i].state==420)){ PLAYER_STATE(i,104); }
			if(P[i].key_JOY_B_status==1 && (P[i].state==100 || P[i].state==481 || P[i].state==410 || P[i].state==420)){ PLAYER_STATE(i,105); }
			if(P[i].key_JOY_C_status==1 && (P[i].state==100 || P[i].state==481 || P[i].state==410 || P[i].state==420)){ PLAYER_STATE(i,106); }
		}else{
			//golpes em pe de perto
			if(P[i].key_JOY_X_status==1 && (P[i].state==100 || P[i].state==481 || P[i].state==410 || P[i].state==420)){ PLAYER_STATE(i,151); }
			if(P[i].key_JOY_Y_status==1 && (P[i].state==100 || P[i].state==481 || P[i].state==410 || P[i].state==420)){ PLAYER_STATE(i,152); }
			if(P[i].key_JOY_Z_status==1 && (P[i].state==100 || P[i].state==481 || P[i].state==410 || P[i].state==420)){ PLAYER_STATE(i,153); }
			if(P[i].key_JOY_A_status==1 && (P[i].state==100 || P[i].state==481 || P[i].state==410 || P[i].state==420)){ PLAYER_STATE(i,154); }
			if(P[i].key_JOY_B_status==1 && (P[i].state==100 || P[i].state==481 || P[i].state==410 || P[i].state==420)){ PLAYER_STATE(i,155); }
			if(P[i].key_JOY_C_status==1 && (P[i].state==100 || P[i].state==481 || P[i].state==410 || P[i].state==420)){ PLAYER_STATE(i,156); }
		}
		
		//golpes abaixados
		if(P[i].key_JOY_X_status==1 && P[i].state==200){ PLAYER_STATE(i,201); }
		if(P[i].key_JOY_Y_status==1 && P[i].state==200){ PLAYER_STATE(i,202); }
		if(P[i].key_JOY_Z_status==1 && P[i].state==200){ PLAYER_STATE(i,203); }
		if(P[i].key_JOY_A_status==1 && P[i].state==200){ PLAYER_STATE(i,204); }
		if(P[i].key_JOY_B_status==1 && P[i].state==200){ PLAYER_STATE(i,205); }
		if(P[i].key_JOY_C_status==1 && P[i].state==200){ PLAYER_STATE(i,206); }
		
		//abaixando ; esquivando; levantando
		if((P[i].key_JOY_DOWN_status==1 || P[i].key_JOY_DOWN_status==2) && (P[i].state==100 || P[i].state==410 || P[i].state==420))
		{ 
			if(P[i].key_JOY_countdown[2]==0){ PLAYER_STATE(i,601); }else{ PLAYER_STATE(i,470); } //abaixar ou esquivar
		}
		if(P[i].key_JOY_DOWN_status==0 && P[i].state==200){ PLAYER_STATE(i,602); } //levantar
		
		//interrupcao da esquiva abaixada
		if(P[i].state==470 && P[i].animFrame==2 && P[i].key_JOY_DOWN_status==0)
		{ 
			P[i].animFrame=3; 
			P[i].frameTimeAtual=1; 
			P[i].frameTimeTotal=P[i].dataAnim[3]; 
		}
		
		//virando de lado (mudanca de estado)
		if(P[PA].direcao== 1 && P[PR].x<P[PA].x && P[PA].state==100){ PLAYER_STATE(i,607); P[PA].direcao=-1; }
		if(P[PA].direcao==-1 && P[PA].x<P[PR].x && P[PA].state==100){ PLAYER_STATE(i,607); P[PA].direcao= 1; }
		if(P[PA].direcao== 1 && P[PR].x<P[PA].x && P[PA].state==200){ PLAYER_STATE(i,608); P[PA].direcao=-1; }
		if(P[PA].direcao==-1 && P[PA].x<P[PR].x && P[PA].state==200){ PLAYER_STATE(i,608); P[PA].direcao= 1; }
		
		//ini pulos
		if((P[i].key_JOY_UP_status==1 || P[i].key_JOY_UP_status==2) && (P[i].state==100 || P[i].state==410 || P[i].state==420 || P[i].state==602))
		{ 
			if(P[i].key_JOY_RIGHT_status==2){ if(P[i].direcao==1){PLAYER_STATE(i,605);}else{PLAYER_STATE(i,603);} }
			if(P[i].key_JOY_RIGHT_status==0 && P[i].key_JOY_LEFT_status==0)PLAYER_STATE(i,604); 
			if(P[i].key_JOY_LEFT_status ==2){ if(P[i].direcao==1){PLAYER_STATE(i,603);}else{PLAYER_STATE(i,605);} }
		}
		
		//end pulos
		if((P[i].state==300 || P[i].state==310 || P[i].state==320) && P[i].y>=gAlturaPiso){ P[i].y=gAlturaPiso; P[i].puloTimer=0; P[i].hSpeed=0; PLAYER_STATE(i,606); }
		if((P[i].state>=301 && P[i].state<=326) && P[i].y>=gAlturaPiso){ P[i].y=gAlturaPiso; P[i].hSpeed=0; PLAYER_STATE(i,606); } 
		
		//golpes aereos
		if(P[i].key_JOY_X_status==1 && P[i].state==300){ PLAYER_STATE(i,301); }
		if(P[i].key_JOY_Y_status==1 && P[i].state==300){ PLAYER_STATE(i,302); }
		if(P[i].key_JOY_Z_status==1 && P[i].state==300){ PLAYER_STATE(i,303); }
		if(P[i].key_JOY_A_status==1 && P[i].state==300){ PLAYER_STATE(i,304); }
		if(P[i].key_JOY_B_status==1 && P[i].state==300){ PLAYER_STATE(i,305); }
		if(P[i].key_JOY_C_status==1 && P[i].state==300){ PLAYER_STATE(i,306); }
		
		if(P[i].key_JOY_X_status==1 && P[i].state==310){ PLAYER_STATE(i,311); }
		if(P[i].key_JOY_Y_status==1 && P[i].state==310){ PLAYER_STATE(i,312); }
		if(P[i].key_JOY_Z_status==1 && P[i].state==310){ PLAYER_STATE(i,313); }
		if(P[i].key_JOY_A_status==1 && P[i].state==310){ PLAYER_STATE(i,314); }
		if(P[i].key_JOY_B_status==1 && P[i].state==310){ PLAYER_STATE(i,315); }
		if(P[i].key_JOY_C_status==1 && P[i].state==310){ PLAYER_STATE(i,316); }
		
		if(P[i].key_JOY_X_status==1 && P[i].state==320){ PLAYER_STATE(i,321); }
		if(P[i].key_JOY_Y_status==1 && P[i].state==320){ PLAYER_STATE(i,322); }
		if(P[i].key_JOY_Z_status==1 && P[i].state==320){ PLAYER_STATE(i,323); }
		if(P[i].key_JOY_A_status==1 && P[i].state==320){ PLAYER_STATE(i,324); }
		if(P[i].key_JOY_B_status==1 && P[i].state==320){ PLAYER_STATE(i,325); }
		if(P[i].key_JOY_C_status==1 && P[i].state==320){ PLAYER_STATE(i,326); }
		
		//andando
		if(P[i].direcao==1)
		{
			if(P[i].key_JOY_LEFT_status>0 && P[i].state==100){ 
				if(P[i].key_JOY_countdown[4]==0){ PLAYER_STATE(i,410); }else{ PLAYER_STATE(i,471); }}
			if(P[i].key_JOY_RIGHT_status>0 && P[i].state==100){ 
				if(P[i].key_JOY_countdown[6]==0){ PLAYER_STATE(i,420); }else{ PLAYER_STATE(i,472); }}
			
			if(P[i].key_JOY_LEFT_status==0 && P[i].state==410){ PLAYER_STATE(i,100); } //end walk
			if(P[i].key_JOY_RIGHT_status==0 && P[i].state==420){ PLAYER_STATE(i,100); }
		}else{
			if(P[i].key_JOY_LEFT_status>0 && P[i].state==100){ 
				if(P[i].key_JOY_countdown[4]==0){ PLAYER_STATE(i,420); }else{ PLAYER_STATE(i,472);}}
			if(P[i].key_JOY_RIGHT_status>0 && P[i].state==100){ 
				if(P[i].key_JOY_countdown[6]==0){ PLAYER_STATE(i,410); }else{ PLAYER_STATE(i,471);}}
			
			if(P[i].key_JOY_LEFT_status ==0 && P[i].state==420){ P[i].hSpeed=0; PLAYER_STATE(i,100); } //end walk
			if(P[i].key_JOY_RIGHT_status==0 && P[i].state==410){ P[i].hSpeed=0; PLAYER_STATE(i,100); }
		}
		
		//key_JOY_countdown é um timer regressivo, ativado quando se aperta algum botao direcional, usado para Corrida e Esquiva, entre outros...
		if(P[i].key_JOY_countdown[8]>0){ P[i].key_JOY_countdown[8]--; }
		if(P[i].key_JOY_countdown[2]>0){ P[i].key_JOY_countdown[2]--; }
		if(P[i].key_JOY_countdown[4]>0){ P[i].key_JOY_countdown[4]--; }
		if(P[i].key_JOY_countdown[6]>0){ P[i].key_JOY_countdown[6]--; }
		
		//ativacao de 'key_JOY_countdown'...
		if(P[i].state==410 || P[i].state==420 || P[i].state==601 || P[i].state==603 || P[i].state==604 || P[i].state==605)
		{
			if(P[i].key_JOY_UP_status   ==1){ P[i].key_JOY_countdown[8]=12; P[i].key_JOY_countdown[2]=0; }
			if(P[i].key_JOY_LEFT_status ==1){ P[i].key_JOY_countdown[4]=12; P[i].key_JOY_countdown[6]=0; }
			if(P[i].key_JOY_RIGHT_status==1){ P[i].key_JOY_countdown[6]=12; P[i].key_JOY_countdown[4]=0; }
			if(P[i].key_JOY_DOWN_status ==1){ P[i].key_JOY_countdown[2]=12; P[i].key_JOY_countdown[8]=0; P[i].key_JOY_countdown[4]=0; P[i].key_JOY_countdown[6]=0; }
		}
		
		//time over decision; timeover
		if((gClockLTimer==0 && gClockRTimer==0) && P[1].state!=611 && P[1].state!=615)
		{
			bool updateWinsFlag=0;
			if(P[1].energiaBase> P[2].energiaBase){ PLAYER_STATE(1,611); P[1].wins++; updateWinsFlag=1; PLAYER_STATE(2,615); }
			if(P[1].energiaBase< P[2].energiaBase){ PLAYER_STATE(2,611); P[2].wins++; updateWinsFlag=2; PLAYER_STATE(1,615); }
			if(P[1].energiaBase==P[2].energiaBase){ PLAYER_STATE(1,615); PLAYER_STATE(2,615); }
			if(updateWinsFlag!=0){
				if(updateWinsFlag==1) //P1
				{
					if(P[1].wins>=1){ SPR_setVisibility(GE[12].sprite, VISIBLE); }
					if(P[1].wins>=2){ SPR_setVisibility(GE[13].sprite, VISIBLE); }
				}
				if(updateWinsFlag==2) //P2
				{
					if(P[2].wins>=1){ SPR_setVisibility(GE[15].sprite, VISIBLE); }
					if(P[2].wins>=2){ SPR_setVisibility(GE[14].sprite, VISIBLE); }
				}
			}
		}
		
		/////////////
		// Colisao //
		/////////////
		
		if( FUNCAO_COLISAO(
		 P[PA].x+P[PA].dataHBox[0], P[PA].y+P[PA].dataHBox[1], P[PA].x+P[PA].dataHBox[2], P[PA].y+P[PA].dataHBox[3], 
		 P[PR].x+P[PR].dataBBox[0], P[PR].y+P[PR].dataBBox[1], P[PR].x+P[PR].dataBBox[2], P[PR].y+P[PR].dataBBox[3]
		)==1 )
		{
			if(P[PR].state==107 || P[PR].state==108 || P[PR].state==109 || P[PR].state==110 ||
			   P[PR].state==207 || P[PR].state==208 || P[PR].state==209 || P[PR].state==210) 
			{
				//DEFENDEU!
				if(P[PR].state>=107 && P[PR].state<=109){ PLAYER_STATE(PR,110); P[PR].hitPause=12; }
				if(P[PR].state>=207 && P[PR].state<=209){ PLAYER_STATE(PR,210); P[PR].hitPause=12; }
				
				u8 AlturaDoHit = P[PA].y+P[PA].dataHBox[3];
				P[PA].dataHBox[0]=0; P[PA].dataHBox[1]=0; P[PA].dataHBox[2]=0; P[PA].dataHBox[3]=0; //desativa a hitbox
				
				if(Spark1_countDown>0){ SPR_releaseSprite(Spark[1]); }
				if(Spark2_countDown>0){ SPR_releaseSprite(Spark[2]); }
				
				if(
					P[PA].state==104 || P[PA].state==105 || P[PA].state==106 ||
					P[PA].state==154 || P[PA].state==155 || P[PA].state==156 || 
					P[PA].state==204 || P[PA].state==205 || P[PA].state==206 || 
					P[PA].state==304 || P[PA].state==305 || P[PA].state==306 || 
					P[PA].state==314 || P[PA].state==315 || P[PA].state==316 || 
					P[PA].state==324 || P[PA].state==325 || P[PA].state==326
				){
					//spark de defesa contra chutes
					if(PR==1){Spark1_countDown=12;}
					if(PR==2){Spark2_countDown=12;}
					Spark[PR] = SPR_addSpriteExSafe( &spr_spark_def2, P[PR].x-20, AlturaDoHit-30, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
				}else{
					//spark de defesa contra espada
					if(PR==1){Spark1_countDown=12;}
					if(PR==2){Spark2_countDown=12;}
					Spark[PR] = SPR_addSpriteExSafe( &spr_spark_def1, P[PR].x-20, AlturaDoHit-30, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
				}
				if(P[PR].direcao == -1){ SPR_setHFlip(Spark[PR], TRUE); }
				SPR_setDepth(Spark[PR], 1);
				
			}else{
				//NAO DEFENDEU!
				u8 AlturaDoHit = P[PA].y+P[PA].dataHBox[3];
				P[PA].dataHBox[0]=0; P[PA].dataHBox[1]=0; P[PA].dataHBox[2]=0; P[PA].dataHBox[3]=0; //desativa a hitbox
				
				u8 SparkType = 0;
				
				//golpes de longe
				if(P[PA].state==101){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,501);} FUNCAO_UPDATE_LIFESP(PR,1,-5); FUNCAO_UPDATE_LIFESP(PA,2,1); SparkType= 1; }
				if(P[PA].state==102){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,501);} FUNCAO_UPDATE_LIFESP(PR,1,-10); FUNCAO_UPDATE_LIFESP(PA,2,2); SparkType= 1; }
				if(P[PA].state==103)
				{ 
					if(P[1].fBallActive==1 || P[2].fBallActive==1)
					{
						if(P[PR].y!=gAlturaPiso){ PLAYER_STATE(PR,550);
						}else{PLAYER_STATE(PR,503);} SparkType= 0; 
					}else{
						if(P[PR].y!=gAlturaPiso){ PLAYER_STATE(PR,550);
						}else{PLAYER_STATE(PR,508);} SparkType= 3; 
					} 
					FUNCAO_UPDATE_LIFESP(PR,1,-3); FUNCAO_UPDATE_LIFESP(PA,2,3);
				}
				if(P[PA].state==104){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,507);} FUNCAO_UPDATE_LIFESP(PR,1,-5); FUNCAO_UPDATE_LIFESP(PA,2,1); SparkType=1; }
				if(P[PA].state==105){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,507);} FUNCAO_UPDATE_LIFESP(PR,1,-10); FUNCAO_UPDATE_LIFESP(PA,2,2); SparkType=1; }
				if(P[PA].state==106){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,503);} FUNCAO_UPDATE_LIFESP(PR,1,-15); FUNCAO_UPDATE_LIFESP(PA,2,3); SparkType=2; }
				
				//golpes de perto
				if(P[PA].state==151){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,501);} FUNCAO_UPDATE_LIFESP(PR,1,-5); FUNCAO_UPDATE_LIFESP(PA,2,1); SparkType= 1; }
				if(P[PA].state==152){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,501);} FUNCAO_UPDATE_LIFESP(PR,1,-10); FUNCAO_UPDATE_LIFESP(PA,2,2); SparkType= 1; }
				if(P[PA].state==153 && P[PA].animFrame<12){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,503);} FUNCAO_UPDATE_LIFESP(PR,1,-20); FUNCAO_UPDATE_LIFESP(PR,2,3); SparkType= 2; }
				if(P[PA].state==153 && P[PA].animFrame>12)
				{ 
					if(P[1].fBallActive==1 && P[2].fBallActive==1)
					{
						if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);
						}else{PLAYER_STATE(PR,503);} SparkType= 0; 
					}else{
						if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);
						}else{PLAYER_STATE(PR,508);} SparkType= 1;/*3*/ 
					} 
					FUNCAO_UPDATE_LIFESP(PR,1,-20); FUNCAO_UPDATE_LIFESP(PR,2,3); 
				} 
				if(P[PA].state==154){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,507);} FUNCAO_UPDATE_LIFESP(PR,1,-5); FUNCAO_UPDATE_LIFESP(PA,2,1); SparkType= 1; }
				if(P[PA].state==155){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,507);} FUNCAO_UPDATE_LIFESP(PR,1,-10); FUNCAO_UPDATE_LIFESP(PA,2,2); SparkType= 1; }
				if(P[PA].state==156){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,503);} FUNCAO_UPDATE_LIFESP(PR,1,-15); FUNCAO_UPDATE_LIFESP(PA,2,3); SparkType= 2; }
				
				//golpes abaixado
				if(P[PA].state==201){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,507);} FUNCAO_UPDATE_LIFESP(PR,1,-5); FUNCAO_UPDATE_LIFESP(PA,2,1); SparkType=  1; }
				if(P[PA].state==202){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,507);} FUNCAO_UPDATE_LIFESP(PR,1,-10); FUNCAO_UPDATE_LIFESP(PA,2,2); SparkType=  1; }
				if(P[PA].state==203)
				{ 
					if(P[1].fBallActive==1 && P[2].fBallActive==1)
					{
						if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);
						}else{PLAYER_STATE(PR,503);} SparkType= 2; 
					}else{
						if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);
						}else{PLAYER_STATE(PR,508);} SparkType=  2;/*3*/ 
					} 
					FUNCAO_UPDATE_LIFESP(PR,1,-15); FUNCAO_UPDATE_LIFESP(PA,2,3); 
				} 
				if(P[PA].state==204){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,507);} FUNCAO_UPDATE_LIFESP(PR,1,-5); FUNCAO_UPDATE_LIFESP(PA,2,1); SparkType= 1; }
				if(P[PA].state==205){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,507);} FUNCAO_UPDATE_LIFESP(PR,1,-15); FUNCAO_UPDATE_LIFESP(PA,2,2); SparkType= 1; }
				if(P[PA].state==206){ PLAYER_STATE(PR,550); FUNCAO_UPDATE_LIFESP(PR,1,-30); FUNCAO_UPDATE_LIFESP(PR,2,11); SparkType= 2; }
				
				//golpes aereos
				if(P[PA].state==301 || P[PA].state==311 || P[PA].state==321){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,501);} FUNCAO_UPDATE_LIFESP(PR,1,-5); FUNCAO_UPDATE_LIFESP(PA,2,1); SparkType= 1; }
				if(P[PA].state==302 || P[PA].state==312 || P[PA].state==322){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,501);} FUNCAO_UPDATE_LIFESP(PR,1,-10); FUNCAO_UPDATE_LIFESP(PA,2,2); SparkType= 1; }
				if(P[PA].state==303 || P[PA].state==313 || P[PA].state==323){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,503);} FUNCAO_UPDATE_LIFESP(PR,1,-15); FUNCAO_UPDATE_LIFESP(PA,2,3); SparkType= 2; }
				if(P[PA].state==304 || P[PA].state==314 || P[PA].state==324){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,501);} FUNCAO_UPDATE_LIFESP(PR,1,-5); FUNCAO_UPDATE_LIFESP(PA,2,1); SparkType=1; }
				if(P[PA].state==305 || P[PA].state==315 || P[PA].state==325){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,501);} FUNCAO_UPDATE_LIFESP(PR,1,-10); FUNCAO_UPDATE_LIFESP(PA,2,2); SparkType=1; }
				if(P[PA].state==306 || P[PA].state==316 || P[PA].state==326){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,503);} FUNCAO_UPDATE_LIFESP(PR,1,-15); FUNCAO_UPDATE_LIFESP(PA,2,3); SparkType=2; }
				
				//MAGIAS HITS /////////////////////////////
				
				//TERRY ID:1
				if(P[PA].id==1)
				{
					if(P[PA].state==700){ PLAYER_STATE(PR,550); FUNCAO_UPDATE_LIFESP(PR,1,-5); FUNCAO_UPDATE_LIFESP(PA,2,1); SparkType= 10; }
					if(P[PA].state==720){ PLAYER_STATE(PR,503); FUNCAO_UPDATE_LIFESP(PR,1,-5); FUNCAO_UPDATE_LIFESP(PA,2,1); SparkType= 10; }
					if(P[PA].state==730){ if(P[PR].y!=gAlturaPiso){PLAYER_STATE(PR,550);}else{PLAYER_STATE(PR,508);} FUNCAO_UPDATE_LIFESP(PR,1,-30); FUNCAO_UPDATE_LIFESP(PR,2,5); SparkType=12; }
				}
				//GILLIUS ID:2
				if(P[PA].id==2)
				{
					if(P[PA].state==710){ PLAYER_STATE(PR,550); FUNCAO_UPDATE_LIFESP(PR,1,-30); FUNCAO_UPDATE_LIFESP(PR,2,5); SparkType= 1; }
					if(P[PA].state==720){ PLAYER_STATE(PR,550); FUNCAO_UPDATE_LIFESP(PR,1,-30); FUNCAO_UPDATE_LIFESP(PR,2,5); SparkType= 1; }
					if(P[PA].state==730){ PLAYER_STATE(PR,506); FUNCAO_UPDATE_LIFESP(PR,1,-10); FUNCAO_UPDATE_LIFESP(PR,2,3); SparkType= 1; }
				}
				
				///////////////////////////////////////////
				
				
				//oponente zero energy = dead
				if((P[PR].state>=501 && P[PR].state<=508) && P[PR].energiaBase==0)
				{ 
					SPR_releaseSprite(GE[11].sprite);
					PLAYER_STATE(PR,550); 
				}
				
				//Spark, efeito FX, do golpe acertando o oponente
				if(SparkType!=0)
				{
					P[PR].hitPause=16; //hitpause Shake
					if(Spark1_countDown>0){ SPR_releaseSprite(Spark[1]); }
					if(Spark2_countDown>0){ SPR_releaseSprite(Spark[2]); }
					if(PR==1){Spark1_countDown=12;}
					if(PR==2){Spark2_countDown=12;}
					
					if(SparkType== 1){ Spark[PR] = SPR_addSpriteExSafe( &spr_spark_hit1, P[PR].x-12, AlturaDoHit-16- 3, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
					if(SparkType== 2){ Spark[PR] = SPR_addSpriteExSafe( &spr_spark_hit2, P[PR].x-30, AlturaDoHit-24, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
					if(SparkType== 3){ Spark[PR] = SPR_addSpriteExSafe( &spr_spark_hit3, P[PR].x-28, AlturaDoHit-36-10, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
					if(SparkType== 4){ Spark[PR] = SPR_addSpriteExSafe( &spr_spark_hit4, P[PR].x-20, AlturaDoHit- 8- 8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
					if(SparkType== 5){ Spark[PR] = SPR_addSpriteExSafe( &spr_spark_hit5, P[PR].x-32, AlturaDoHit-1, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
					if(SparkType== 6){ Spark[PR] = SPR_addSpriteExSafe( &spr_spark_hit6, P[PR].x-44, AlturaDoHit-16-16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
					if(SparkType== 7){ Spark[PR] = SPR_addSpriteExSafe( &spr_spark_hit7, P[PR].x-12, AlturaDoHit-16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
					if(SparkType== 8){ Spark[PR] = SPR_addSpriteExSafe( &spr_spark_hit8, P[PR].x-16, AlturaDoHit-24, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
					if(SparkType== 9){ Spark[PR] = SPR_addSpriteExSafe( &spr_spark_hit9, P[PR].x-24, AlturaDoHit-40, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
					if(SparkType==10){ Spark[PR] = SPR_addSpriteExSafe(&spr_spark_hit10, P[PR].x-38, AlturaDoHit-48, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
					if(SparkType==11){ Spark[PR] = SPR_addSpriteExSafe(&spr_spark_hit11, P[PR].x-12, AlturaDoHit-12, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
					if(SparkType==12){ Spark[PR] = SPR_addSpriteExSafe(&spr_spark_hit12, P[PR].x-16, AlturaDoHit-16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
					
					if(P[PR].direcao == 1){ SPR_setHFlip(Spark[PR], TRUE); }
					SPR_setDepth(Spark[PR], 1);
				}
			}
		}
		
		////////////////////
		// CORPO x MAGIAS //
		//////////////////// ─=≡ΣO)) x PLAYER
		
		if( FUNCAO_COLISAO(
		 (P[PA].fBallX+20)-20, (P[PA].fBallY+20)-20, (P[PA].fBallX+20)+20, (P[PA].fBallY+20)+20,
		 P[PR].x+P[PR].dataBBox[0], P[PR].y+P[PR].dataBBox[1], P[PR].x+P[PR].dataBBox[2], P[PR].y+P[PR].dataBBox[3]
		)==1 && (P[PA].fBallX!=0 && P[PA].fBallY!=0) )
		{
			if(P[PR].state==107 || P[PR].state==108 || P[PR].state==109 || P[PR].state==110 ||
			   P[PR].state==207 || P[PR].state==208 || P[PR].state==209 || P[PR].state==210) 
			{
				//DEFENDEU!
				if(P[PR].state>=107 && P[PR].state<=109){ PLAYER_STATE(PR,110); P[PR].hitPause=20; }
				if(P[PR].state>=207 && P[PR].state<=209){ PLAYER_STATE(PR,210); P[PR].hitPause=20; }
			}else{
				//NAO DEFENDEU!
				P[PR].hitPause=20;
				FUNCAO_UPDATE_LIFESP(PR,1,-30); 
				FUNCAO_UPDATE_LIFESP(PR,2, 11);
				if(P[PR].energiaBase==0)
				{
					PLAYER_STATE(PR,550);
				}else{
					if(P[PR].y!=gAlturaPiso)
					{ 
						if(P[PA].id==1)
						{
							PLAYER_STATE(PR,516); //magia do haomaru faz o oponente subir
						}else{
							PLAYER_STATE(PR,550); //cai apos cair sobre a magia
						}
						
					}else{
						if(P[PA].id==1)
						{
							PLAYER_STATE(PR,516); //magia do haomaru faz o oponente subir
						}else{
							PLAYER_STATE(PR,503); //recebe o hit da magia em pe
						}
					}
				}
				
			}
			
			//desativa a magia
			if(P[PA].fBallActive==1)
			{ 
				if(P[PA].fBall)
				{ 
					SPR_releaseSprite(P[PA].fBall); 
					P[PA].fBall = NULL; 
				} 
				P[PA].fBallActive=0;
			}			
			
			//SPARK DE MAGIA SUMINDO
			if(Spark1_countDown>0){ SPR_releaseSprite(Spark[1]); }
			if(Spark2_countDown>0){ SPR_releaseSprite(Spark[2]); }
			if(PA==1){Spark1_countDown=20;}
			if(PA==2){Spark2_countDown=20;}
			
			if(PA==1)
			{
				//mai
				//if(P[PA].id==2){ Spark[PA] = SPR_addSpriteExSafe( &spr_mai_702, P[PA].fBallX-20, P[PA].fBallY, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			}
			if(PA==2)
			{
				//mai
				//if(P[PA].id==2){ Spark[PA] = SPR_addSpriteExSafe( &spr_mai_702, P[PA].fBallX-20, P[PA].fBallY, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			}
			
			//se o spark foi carregado com sucesso...
			if (Spark[PA])
			{ 
				if(P[PA].direcao == -1){ SPR_setHFlip(Spark[PA], TRUE); }
				SPR_setDepth(Spark[PA], 1); 
			}
			
			//reseta posicao da magia
			P[PA].fBallX=0; P[PA].fBallY=0; 
			
		}
		
		/////////////////////
		// MAGIAS x MAGIAS //
		///////////////////// ─=≡ΣO)) x ((O3≡=-
		
		if( FUNCAO_COLISAO(
		 (P[1].fBallX+20)-20, (P[1].fBallY+20)-20, (P[1].fBallX+20)+20, (P[1].fBallY+20)+20,
		 (P[2].fBallX+20)-20, (P[2].fBallY+20)-20, (P[2].fBallX+20)+20, (P[2].fBallY+20)+20
		)==1 && (P[PA].fBallX!=0 && P[PA].fBallY!=0) )
		{
			//desativa a magia P1
			if(P[1].fBallActive==1)
			{ 
				if(P[1].fBall)
				{ 
					SPR_releaseSprite(P[1].fBall); 
					P[1].fBall = NULL; 
				} 
				P[1].fBallActive=0;
				P[1].fBallX=0; P[1].fBallY=0; 
			}
			//desativa a magia P2
			if(P[2].fBallActive==1)
			{ 
				if(P[2].fBall)
				{ 
					SPR_releaseSprite(P[2].fBall); 
					P[2].fBall = NULL; 
				} 
				P[2].fBallActive=0;
				P[2].fBallX=0; P[2].fBallY=0; 
			}
		}
		
		
		//MAGIAS STARTUP (FSM CHANGESTATE)
		//Condicoes para que se possa soltar as magias, e posterior Acionamento delas!
		
		if( 
			(P[i].state >= 100 && P[i].state <= 156) || 
			P[i].state == 410 || 
			P[i].state == 420 || 
			P[i].state == 471 || 
			P[i].state == 472 || 
			P[i].state == 602 || 
			P[i].state == 606 
		)
		{
			u8 magic_avaliable=1;
			
			if(P[i].id==1) //TERRY
			{
				//700
				//Ougi Kogetsu Zan (Secret Skill Arc Moon Slash) 
				if // >, v, v>, > + P ; frente, baixo, baixo+frente, frente + Soco
				(
					magic_avaliable==1 && 
					(
						(P[i].direcao== 1 && ( P[i].inputArray[2]==6 && P[i].joyDirTimer[2] < P[i].joyDirTimer[6] ) && (P[i].joyDirTimer[2]!=0) ) 
						|| 
						(P[i].direcao==-1 && ( P[i].inputArray[2]==4 && P[i].joyDirTimer[2] < P[i].joyDirTimer[4] ) && (P[i].joyDirTimer[2]!=0) )
					) &&			
					( P[i].attackButton==1 ||  P[i].attackButton==2 || P[i].attackButton==3)
				)
				{
					if(Spark1_countDown>0){ SPR_releaseSprite(Spark[1]); Spark1_countDown=0; }
					if(Spark2_countDown>0){ SPR_releaseSprite(Spark[2]); Spark2_countDown=0; }
					PLAYER_STATE(i,700);
					magic_avaliable=0; 
				}
				
				//710
				//Ougi Senpuu Retsu Zan (Secret Skill Whirlwind Rending Slash)
				if // v, v>, > + P ; baixo, baixo+frente, frente + Soco
				(
					magic_avaliable==1 && 
					(
						(P[i].direcao== 1 && ( P[i].joyDirTimer[2] < P[i].joyDirTimer[6] ) && (P[i].joyDirTimer[2]!=0) ) 
						|| 
						(P[i].direcao==-1 && ( P[i].joyDirTimer[2] < P[i].joyDirTimer[4] ) && (P[i].joyDirTimer[2]!=0) )
					) &&			
					( P[i].attackButton==1 ||  P[i].attackButton==2 || P[i].attackButton==3)
				)
				{
					PLAYER_STATE(i,710);
					magic_avaliable=0;
				}
				
				//720
				//Ougi Resshin Zan (Secret Skill Violent Quake Slash)
				if // >, v, v>, > + P ; baixo, baixo+frente, frente + Chute
				(
					magic_avaliable==1 && 
					(
						(P[i].direcao== 1 && ( P[i].inputArray[2]==6 && P[i].joyDirTimer[2] < P[i].joyDirTimer[6] ) && (P[i].joyDirTimer[2]!=0) ) 
						|| 
						(P[i].direcao==-1 && ( P[i].inputArray[2]==4 && P[i].joyDirTimer[2] < P[i].joyDirTimer[4] ) && (P[i].joyDirTimer[2]!=0) )
					) &&			
					( P[i].attackButton==4 ||  P[i].attackButton==5 || P[i].attackButton==6)
				)
				{
					PLAYER_STATE(i,720);
					magic_avaliable=0;
				}
				
				//730
				//Sake Kougeki (Sake Attack)
				if // v, <v, < + LP ; baixo, baixo+tras, tras + Soco Fraco
				(
					magic_avaliable==1 && 
					(
						(P[i].direcao==-1 && ( P[i].joyDirTimer[2] < P[i].joyDirTimer[6] ) && (P[i].joyDirTimer[2]!=0) ) 
						|| 
						(P[i].direcao== 1 && ( P[i].joyDirTimer[2] < P[i].joyDirTimer[4] ) && (P[i].joyDirTimer[2]!=0) )
					) &&			
					( P[i].attackButton==1 ) //*Apenas com Soco Fraco!
				)
				{
					PLAYER_STATE(i,730);
					magic_avaliable=0;
				}
				
			}//---end char; TERRY
			
			if(P[i].id==2) //GILLIUS
			{
				//700
				//Rock Throw
				if // v, v>, > + P ; baixo, baixo+frente, frente + Soco
				(
					magic_avaliable==1 && 
					(
						(P[i].direcao== 1 && ( P[i].joyDirTimer[2] < P[i].joyDirTimer[6] ) && (P[i].joyDirTimer[2]!=0) ) 
						|| 
						(P[i].direcao==-1 && ( P[i].joyDirTimer[2] < P[i].joyDirTimer[4] ) && (P[i].joyDirTimer[2]!=0) )
					) &&			
					( P[i].attackButton==1 ||  P[i].attackButton==2 || P[i].attackButton==3 )
				)
				{
					PLAYER_STATE(i,700);
					magic_avaliable=0;
				}
				
				//710
				//Axe Spin
				if // v, <v, < + P ; baixo, baixo+tras, tras + Soco
				(
					magic_avaliable==1 && 
					(
						(P[i].direcao==-1 && ( P[i].joyDirTimer[2] < P[i].joyDirTimer[6] ) && (P[i].joyDirTimer[2]!=0) ) 
						|| 
						(P[i].direcao== 1 && ( P[i].joyDirTimer[2] < P[i].joyDirTimer[4] ) && (P[i].joyDirTimer[2]!=0) )
					) &&			
					( P[i].attackButton==1 ||  P[i].attackButton==2 || P[i].attackButton==3 ) 
				)
				{
					PLAYER_STATE(i,710);
					magic_avaliable=0;
				}
				
				//720
				//Shoulder Ram
				if // v, v>, > + K ; baixo, baixo+frente, frente + Chute
				(
					magic_avaliable==1 && 
					(
						(P[i].direcao== 1 && ( P[i].joyDirTimer[2] < P[i].joyDirTimer[6] ) && (P[i].joyDirTimer[2]!=0) ) 
						|| 
						(P[i].direcao==-1 && ( P[i].joyDirTimer[2] < P[i].joyDirTimer[4] ) && (P[i].joyDirTimer[2]!=0) )
					) &&			
					( P[i].attackButton==4 ||  P[i].attackButton==5 || P[i].attackButton==6)
				)
				{
					PLAYER_STATE(i,720);
					magic_avaliable=0;
				}
				
			}//---end char; GILLIUS
			
			if(magic_avaliable==0)
			{
				//reset input array
				P[i].inputArray[0]=0; P[i].inputArray[1]=0; P[i].inputArray[2]=0; P[i].inputArray[3]=0; P[i].inputArray[4]=0;
			}
			
		}
		
		/*
		//Rage Explosion! Rage timer!
		if( 
		P[i].rageTimerCountdown==RAGETIMER && P[i].energiaSP>=32 
		&& (P[i].state==100 || P[i].state==200 || P[i].state==410 || P[i].state==420) 
		&& gPodeMover==1
		)
		{
			P[i].rageTimerCountdown--;
			PLAYER_STATE(i,618);
		}
		if(P[i].energiaSP>=32 && P[i].rageTimerCountdown<RAGETIMER && P[i].energiaBase>0 && gFrames>350){ if(P[i].rageTimerCountdown>0){ P[i].rageTimerCountdown--; } }
		if(P[i].rageTimerCountdown==0 && (P[i].state==100 || P[i].state==200)){ P[i].rageTimerCountdown=RAGETIMER; P[i].energiaSP=0; }
		*/
		
		//Vitoria
		if(
		 (P[PA].state==100 || P[PA].state==200 || P[PA].state==410 || P[PA].state==420) 
		 && (P[PA].state!=611 || P[PA].state!=612 || P[PA].state!=613 || P[PA].state!=614) 
		 && (P[PR].energiaBase<=0)
		)
		{
			//Carrega 1 entre 4 poses de vitoria
			//Ajuste a probabilidade conforme preferir. gPing10 = 10% ; variacao de 0 a 9
			if(gPing10>=1 && gPing10<=3){ PLAYER_STATE(PA,611); P[PA].wins++; }
			if(gPing10>=4 && gPing10<=6){ PLAYER_STATE(PA,612); P[PA].wins++; }
			if(gPing10>=7 && gPing10<=9){ PLAYER_STATE(PA,613); P[PA].wins++; }
			if(gPing10==0              ){ PLAYER_STATE(PA,613); P[PA].wins++; } //Vc pode carregar uma pose diferente aqui, eu estou usando 3!
			
			//Exibe o contador de Wins correspondente
			if(PA==1) //P1
			{
				if(P[PA].wins>=1){ SPR_setVisibility(GE[12].sprite, VISIBLE); }
				if(P[PA].wins>=2){ SPR_setVisibility(GE[13].sprite, VISIBLE); }
			}
			if(PA==2) //P2
			{
				if(P[PA].wins>=1){ SPR_setVisibility(GE[15].sprite, VISIBLE); }
				if(P[PA].wins>=2){ SPR_setVisibility(GE[14].sprite, VISIBLE); }
			}
			
		}
		
	}
	
}

void FUNCAO_FSM_HITBOXES(u8 Player)
{
	bool reset_BBox=0;
	bool reset_HBox=0;
	
	if(P[Player].id==1) //terry hitboxes
	{
	if(P[Player].state==100 && P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
	if(P[Player].state==101 && P[Player].animFrame==3){ P[Player].dataHBox[0]= 0; P[Player].dataHBox[1]=-80; P[Player].dataHBox[2]=60; P[Player].dataHBox[3]=-70; }
	if(P[Player].state==101 && P[Player].animFrame==4){ reset_HBox=1; }
	if(P[Player].state==102 && P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 65; P[Player].dataHBox[2]=125; P[Player].dataHBox[3]=-40; }
	if(P[Player].state==102 && P[Player].animFrame==6){ reset_HBox=1; }
	if(P[Player].state==103 && P[Player].animFrame==8){ P[Player].dataHBox[0]= 70; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]=170; P[Player].dataHBox[3]=-40; }
	if(P[Player].state==103 && P[Player].animFrame==9){ reset_HBox=1; }
	if((P[Player].state==104 || P[Player].state==105) && P[Player].animFrame==4){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]=90; P[Player].dataHBox[3]=-70; }
	if((P[Player].state==104 || P[Player].state==105) && P[Player].animFrame==5){ reset_HBox=1; }
	if(P[Player].state==106 && P[Player].animFrame==5){ P[Player].dataHBox[0]= 30; P[Player].dataHBox[1]=-60; P[Player].dataHBox[2]= 100; P[Player].dataHBox[3]=-80; }
	if(P[Player].state==106 && P[Player].animFrame==6){ reset_HBox=1; }
	if(P[Player].state==151 && P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=-60; P[Player].dataHBox[2]=70; P[Player].dataHBox[3]=-50; }
	if(P[Player].state==151 && P[Player].animFrame==3){ reset_HBox=1; }
	if(P[Player].state==152 && P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=-60; P[Player].dataHBox[2]=70; P[Player].dataHBox[3]=-50; }
	if(P[Player].state==152 && P[Player].animFrame==3){ reset_HBox=1; }
	if(P[Player].state==153 && P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=-60; P[Player].dataHBox[2]=70; P[Player].dataHBox[3]=-50; }
	if(P[Player].state==153 && P[Player].animFrame==3){ reset_HBox=1; }
	if(P[Player].state==154 && P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=-100; P[Player].dataHBox[2]=100; P[Player].dataHBox[3]=-50; }
	if(P[Player].state==154 && P[Player].animFrame==4){ reset_HBox=1; }
	if(P[Player].state==155 && P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=-100; P[Player].dataHBox[2]=100; P[Player].dataHBox[3]=-50; }
	if(P[Player].state==155 && P[Player].animFrame==4){ reset_HBox=1; }
	if(P[Player].state==156 && P[Player].animFrame==4){ P[Player].dataHBox[0]=  1; P[Player].dataHBox[1]=-100; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-70; }
	if(P[Player].state==156 && P[Player].animFrame==5){ P[Player].dataHBox[0]=  1; P[Player].dataHBox[1]=-100; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-70; }
	if(P[Player].state==156 && P[Player].animFrame==6){ reset_HBox=1; }
	
	if(P[Player].state==200 && P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 20; P[Player].dataBBox[3]=  0; }
	if((P[Player].state==201 || P[Player].state==202) && P[Player].animFrame==2){ P[Player].dataHBox[0]= 0; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-40; }
	if((P[Player].state==201 || P[Player].state==202) && P[Player].animFrame==3){ reset_HBox=1; }
	if(P[Player].state==203 && P[Player].animFrame== 5){ P[Player].dataHBox[0]= 0; P[Player].dataHBox[1]=-120; P[Player].dataHBox[2]= 35; P[Player].dataHBox[3]=-50; }
	if(P[Player].state==203 && P[Player].animFrame==7){ reset_HBox=1; }
	if((P[Player].state==204 || P[Player].state==205) && P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 30; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-10; }
	if((P[Player].state==204 || P[Player].state==205) && P[Player].animFrame==3){ reset_HBox=1; }
	if(P[Player].state==206 && P[Player].animFrame==4){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 30; P[Player].dataHBox[2]=100; P[Player].dataHBox[3]=-10; }
	if(P[Player].state==206 && P[Player].animFrame==6){ reset_HBox=1; }
	
	if((P[Player].state==301 || P[Player].state==311 || P[Player].state==321) && P[Player].animFrame==4){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 65; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-40; }
	if((P[Player].state==301 || P[Player].state==311 || P[Player].state==321) && P[Player].animFrame==6){ reset_HBox=1; }
	if((P[Player].state==302 || P[Player].state==312 || P[Player].state==322) && P[Player].animFrame==4){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 65; P[Player].dataHBox[2]= 90; P[Player].dataHBox[3]=-40; }
	if((P[Player].state==302 || P[Player].state==312 || P[Player].state==322) && P[Player].animFrame==6){ reset_HBox=1; }
	if((P[Player].state==303 || P[Player].state==313 || P[Player].state==323) && P[Player].animFrame==4){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]= 80; P[Player].dataHBox[3]=-20; }
	if((P[Player].state==303 || P[Player].state==313 || P[Player].state==323) && P[Player].animFrame==6){ reset_HBox=1; }
	if((P[Player].state==304 || P[Player].state==314 || P[Player].state==324) && P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 30; P[Player].dataHBox[2]=80; P[Player].dataHBox[3]=-10; }
	if((P[Player].state==304 || P[Player].state==314 || P[Player].state==324) && P[Player].animFrame==4){ reset_HBox=1; }
	if((P[Player].state==305 || P[Player].state==315 || P[Player].state==325) && P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 30; P[Player].dataHBox[2]=80; P[Player].dataHBox[3]=-10; }
	if((P[Player].state==305 || P[Player].state==315 || P[Player].state==325) && P[Player].animFrame==4){ reset_HBox=1; }
	if((P[Player].state==306 || P[Player].state==316 || P[Player].state==326) && P[Player].animFrame==3){ P[Player].dataHBox[0]= 20; P[Player].dataHBox[1]=- 30; P[Player].dataHBox[2]=85; P[Player].dataHBox[3]=-10; }
	if((P[Player].state==306 || P[Player].state==316 || P[Player].state==326) && P[Player].animFrame==4){ reset_HBox=1; }
	
	if(P[Player].state==470 && P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
	if(P[Player].state==470 && P[Player].animFrame==2){ reset_BBox=1; }
	if(P[Player].state==470 && P[Player].animFrame==3){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
	
	if(P[Player].state==471 && P[Player].animFrame<=2){ reset_BBox=1; }
	if(P[Player].state==472 && P[Player].animFrame<=2){ reset_BBox=1; }
	
	if(P[Player].state==601 && P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 20; P[Player].dataBBox[3]=  0; }
	if(P[Player].state==602 && P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 20; P[Player].dataBBox[3]=  0; }
	
	if(P[Player].state==700 && P[Player].animFrame== 3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]=70; P[Player].dataHBox[3]=-30; }
	if(P[Player].state==700 && P[Player].animFrame==17){ reset_HBox=1; }

	if(P[Player].state==720 && P[Player].animFrame==5){ P[Player].dataHBox[0]=  -90; P[Player].dataHBox[1]=-120; P[Player].dataHBox[2]= 70; P[Player].dataHBox[3]= -50; }
	if(P[Player].state==720 && P[Player].animFrame==7){ P[Player].dataHBox[0]=  1; P[Player].dataHBox[1]=-100; P[Player].dataHBox[2]= 70; P[Player].dataHBox[3]= -40; }
	if(P[Player].state==720 && P[Player].animFrame==9){ P[Player].dataHBox[0]=  1; P[Player].dataHBox[1]=-100; P[Player].dataHBox[2]= 70; P[Player].dataHBox[3]= -40; }
	
	if(P[Player].state==720 && P[Player].animFrame==10){ reset_HBox=1; }
	
	
	
	if(P[Player].state==730 && P[Player].animFrame== 6){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]=110; P[Player].dataHBox[3]=-30; }
	if(P[Player].state==730 && P[Player].animFrame== 7){ reset_HBox=1; }
	}
	
	if(P[Player].id==2) //mai hitboxes
	{
	if(P[Player].state==100 && P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 30; P[Player].dataBBox[3]=  0; }
	if(P[Player].state==101 && P[Player].animFrame==4){ P[Player].dataHBox[0]= 30; P[Player].dataHBox[1]=-50; P[Player].dataHBox[2]=100; P[Player].dataHBox[3]=-30; }
	if(P[Player].state==101 && P[Player].animFrame==5){ reset_HBox=1; }
	if(P[Player].state==102 && P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 65; P[Player].dataHBox[2]=110; P[Player].dataHBox[3]=-40; }
	if(P[Player].state==102 && P[Player].animFrame==5){ reset_HBox=1; }
	if(P[Player].state==103 && P[Player].animFrame==5){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]=120; P[Player].dataHBox[3]=-40; }
	if(P[Player].state==103 && P[Player].animFrame==9){ reset_HBox=1; }
	if(P[Player].state==104 && P[Player].animFrame==3){ P[Player].dataHBox[0]= 30; P[Player].dataHBox[1]=- 80; P[Player].dataHBox[2]=115; P[Player].dataHBox[3]=-60; }
	if(P[Player].state==104 && P[Player].animFrame==6){ reset_HBox=1; }
	if(P[Player].state==105 && P[Player].animFrame==4){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=-110; P[Player].dataHBox[2]=90; P[Player].dataHBox[3]=-60; }
	if(P[Player].state==105 && P[Player].animFrame==5){ reset_HBox=1; }
	if(P[Player].state==106 && P[Player].animFrame==4){ P[Player].dataHBox[0]= 20; P[Player].dataHBox[1]=-80; P[Player].dataHBox[2]=100; P[Player].dataHBox[3]=-70; }
	if(P[Player].state==106 && P[Player].animFrame==5){ P[Player].dataHBox[0]= 20; P[Player].dataHBox[1]=-80; P[Player].dataHBox[2]=100; P[Player].dataHBox[3]=-70; }
	if(P[Player].state==106 && P[Player].animFrame==6){ reset_HBox=1; }
	if(P[Player].state==151 && P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=-80; P[Player].dataHBox[2]=70; P[Player].dataHBox[3]=-60; }
	if(P[Player].state==151 && P[Player].animFrame==5){ reset_HBox=1; }
	if(P[Player].state==152 && P[Player].animFrame==2){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 30; P[Player].dataBBox[3]=  0; }
	if(P[Player].state==152 && P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=-70; P[Player].dataHBox[2]=90; P[Player].dataHBox[3]=-40; }
	if(P[Player].state==152 && P[Player].animFrame==3){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 50; P[Player].dataBBox[3]=  0; }
	if(P[Player].state==152 && P[Player].animFrame==4){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 30; P[Player].dataBBox[3]=  0; }
	if(P[Player].state==152 && P[Player].animFrame==4){ reset_HBox=1; }
	if(P[Player].state==153 && P[Player].animFrame==4){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=-120; P[Player].dataHBox[2]=85; P[Player].dataHBox[3]=-40; }
	if(P[Player].state==153 && P[Player].animFrame==5){ reset_HBox=1; }
	if(P[Player].state==153 && P[Player].animFrame==9){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=-120; P[Player].dataHBox[2]=100; P[Player].dataHBox[3]=-40; }
	if(P[Player].state==153 && P[Player].animFrame==11){ reset_HBox=1; }
	if(P[Player].state==154 && P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=-50; P[Player].dataHBox[2]=50; P[Player].dataHBox[3]=-20; }
	if(P[Player].state==154 && P[Player].animFrame==3){ reset_HBox=1; }
	if(P[Player].state==155 && P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=-70; P[Player].dataHBox[2]=80; P[Player].dataHBox[3]=-50; }
	if(P[Player].state==155 && P[Player].animFrame==4){ reset_HBox=1; }
	if(P[Player].state==156 && P[Player].animFrame==4){ P[Player].dataHBox[0]=  1; P[Player].dataHBox[1]=-120; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-70; }
	if(P[Player].state==156 && P[Player].animFrame==5){ P[Player].dataHBox[0]=  1; P[Player].dataHBox[1]=-120; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-70; }
	if(P[Player].state==156 && P[Player].animFrame==6){ reset_HBox=1; }
	if(P[Player].state==200 && P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 20; P[Player].dataBBox[3]=  0; }
	if(P[Player].state==201 && P[Player].animFrame==2){ P[Player].dataHBox[0]= 0; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]=75; P[Player].dataHBox[3]=-30; }
	if(P[Player].state==201 && P[Player].animFrame==3){ reset_HBox=1; }
	if(P[Player].state==202 && P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]=120; P[Player].dataHBox[3]=-30; }
	if(P[Player].state==202 && P[Player].animFrame==5){ reset_HBox=1; }
	if(P[Player].state==203 && P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]=120; P[Player].dataHBox[3]=-40; }
	if(P[Player].state==203 && P[Player].animFrame==5){ reset_HBox=1; }
	if(P[Player].state==204 && P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 20; P[Player].dataHBox[2]= 75; P[Player].dataHBox[3]=0; }
	if(P[Player].state==204 && P[Player].animFrame==3){ reset_HBox=1; }
	if(P[Player].state==205 && P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 30; P[Player].dataHBox[2]=110; P[Player].dataHBox[3]=-10; }
	if(P[Player].state==205 && P[Player].animFrame==3){ reset_HBox=1; }
	if(P[Player].state==206 && P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 20; P[Player].dataHBox[2]=85; P[Player].dataHBox[3]=0; }
	if(P[Player].state==206 && P[Player].animFrame==5){ reset_HBox=1; }
	
	if((P[Player].state==301 || P[Player].state==311 || P[Player].state==321) && P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]= 80; P[Player].dataHBox[3]=-1; }
	if((P[Player].state==301 || P[Player].state==311 || P[Player].state==321) && P[Player].animFrame==4){ reset_HBox=1; }
	if((P[Player].state==302 || P[Player].state==312 || P[Player].state==322) && P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]= 80; P[Player].dataHBox[3]=-1; }
	if((P[Player].state==302 || P[Player].state==312 || P[Player].state==322) && P[Player].animFrame==6){ reset_HBox=1; }
	if((P[Player].state==303 || P[Player].state==313 || P[Player].state==323) && P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]= 80; P[Player].dataHBox[3]=-1; }
	if((P[Player].state==303 || P[Player].state==313 || P[Player].state==323) && P[Player].animFrame==6){ reset_HBox=1; }
	if((P[Player].state==304 || P[Player].state==314 || P[Player].state==324) && P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]=60; P[Player].dataHBox[3]=-10; }
	if((P[Player].state==304 || P[Player].state==314 || P[Player].state==324) && P[Player].animFrame==3){ reset_HBox=1; }
	if((P[Player].state==305 || P[Player].state==315 || P[Player].state==325) && P[Player].animFrame==3){ P[Player].dataHBox[0]=-20; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]=70; P[Player].dataHBox[3]=-10; }
	if((P[Player].state==305 || P[Player].state==315 || P[Player].state==325) && P[Player].animFrame==4){ reset_HBox=1; }
	if((P[Player].state==306 || P[Player].state==316 || P[Player].state==326) && P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]=70; P[Player].dataHBox[3]=-10; }
	if((P[Player].state==306 || P[Player].state==316 || P[Player].state==326) && P[Player].animFrame==4){ reset_HBox=1; }
	
	if(P[Player].state==470 && P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
	if(P[Player].state==470 && P[Player].animFrame==2){ reset_BBox=1; }
	if(P[Player].state==470 && P[Player].animFrame==3){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
	
	if(P[Player].state==471 && P[Player].animFrame<=2){ reset_BBox=1; }
	if(P[Player].state==472 && P[Player].animFrame<=2){ reset_BBox=1; }
	
	if(P[Player].state==601 && P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 20; P[Player].dataBBox[3]=  0; }
	if(P[Player].state==602 && P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 20; P[Player].dataBBox[3]=  0; }
	
	if(P[Player].state==710 && P[Player].animFrame== 3){ P[Player].dataHBox[0]= -40; P[Player].dataHBox[1]=-100; P[Player].dataHBox[2]=40; P[Player].dataHBox[3]=-10; }
	if(P[Player].state==710 && P[Player].animFrame==17){ reset_HBox=1; }
	
	if(P[Player].state==720 && P[Player].animFrame== 3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]=70; P[Player].dataHBox[3]=-20; }
	if(P[Player].state==720 && P[Player].animFrame== 7){ reset_HBox=1; }
	
	if(P[Player].state==730 && P[Player].animFrame== 6){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]=110; P[Player].dataHBox[3]=-30; }
	if(P[Player].state==730 && P[Player].animFrame== 7){ reset_HBox=1; }

	
	}
	if(P[Player].id==3) //bluemary hitboxes
	{
	if(P[Player].state==100 && P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 30; P[Player].dataBBox[3]=  0; }
	if(P[Player].state==101 && P[Player].animFrame==3){ P[Player].dataHBox[0]= 5; P[Player].dataHBox[1]=-70; P[Player].dataHBox[2]=60; P[Player].dataHBox[3]=-50; }
	if(P[Player].state==101 && P[Player].animFrame==4){ reset_HBox=1; }
	if(P[Player].state==102 && P[Player].animFrame==4){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 60; P[Player].dataHBox[2]=80; P[Player].dataHBox[3]=-40; }
	if(P[Player].state==102 && P[Player].animFrame==5){ reset_HBox=1; }
	if(P[Player].state==103 && P[Player].animFrame==6){ P[Player].dataHBox[0]= 40; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]=110; P[Player].dataHBox[3]=-40; }
	if(P[Player].state==103 && P[Player].animFrame==9){ reset_HBox=1; }
	if(P[Player].state==104 && P[Player].animFrame==3){ P[Player].dataHBox[0]= 5; P[Player].dataHBox[1]=-70; P[Player].dataHBox[2]=60; P[Player].dataHBox[3]=-50; }
	if(P[Player].state==104 && P[Player].animFrame==4){ reset_HBox=1; }
	if(P[Player].state==105 && P[Player].animFrame==4){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 60; P[Player].dataHBox[2]=80; P[Player].dataHBox[3]=-40; }
	if(P[Player].state==105 && P[Player].animFrame==5){ reset_HBox=1; }
	if(P[Player].state==106 && P[Player].animFrame==6){ P[Player].dataHBox[0]= 40; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]=110; P[Player].dataHBox[3]=-40; }
	if(P[Player].state==106 && P[Player].animFrame==9){ reset_HBox=1; }
	if(P[Player].state==151 && P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=-80; P[Player].dataHBox[2]=70; P[Player].dataHBox[3]=-60; }
	if(P[Player].state==151 && P[Player].animFrame==5){ reset_HBox=1; }
	if(P[Player].state==152 && P[Player].animFrame==4){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 60; P[Player].dataHBox[2]=80; P[Player].dataHBox[3]=-40; }
	if(P[Player].state==152 && P[Player].animFrame==5){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 60; P[Player].dataHBox[2]=80; P[Player].dataHBox[3]=-40; }
	if(P[Player].state==152 && P[Player].animFrame==6){ reset_HBox=1; }
	if(P[Player].state==153 && P[Player].animFrame==4){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=-120; P[Player].dataHBox[2]=85; P[Player].dataHBox[3]=-40; }
	if(P[Player].state==153 && P[Player].animFrame==5){ reset_HBox=1; }
	if(P[Player].state==153 && P[Player].animFrame==9){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=-120; P[Player].dataHBox[2]=100; P[Player].dataHBox[3]=-40; }
	if(P[Player].state==153 && P[Player].animFrame==11){ reset_HBox=1; }
	if(P[Player].state==154 && P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=-80; P[Player].dataHBox[2]=70; P[Player].dataHBox[3]=-60; }
	if(P[Player].state==154 && P[Player].animFrame==5){ reset_HBox=1; }
	if(P[Player].state==155 && P[Player].animFrame==4){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 60; P[Player].dataHBox[2]=80; P[Player].dataHBox[3]=-40; }
	if(P[Player].state==155 && P[Player].animFrame==5){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 60; P[Player].dataHBox[2]=80; P[Player].dataHBox[3]=-40; }
	if(P[Player].state==155 && P[Player].animFrame==6){ reset_HBox=1; }
	if(P[Player].state==156 && P[Player].animFrame==4){ P[Player].dataHBox[0]=  1; P[Player].dataHBox[1]=-120; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-70; }
	if(P[Player].state==156 && P[Player].animFrame==5){ P[Player].dataHBox[0]=  1; P[Player].dataHBox[1]=-120; P[Player].dataHBox[2]= 60; P[Player].dataHBox[3]=-70; }
	if(P[Player].state==156 && P[Player].animFrame==6){ reset_HBox=1; }
	if(P[Player].state==200 && P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 20; P[Player].dataBBox[3]=  0; }
	if(P[Player].state==201 && P[Player].animFrame==3){ P[Player].dataHBox[0]= 0; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]=65; P[Player].dataHBox[3]=-30; }
	if(P[Player].state==201 && P[Player].animFrame==4){ reset_HBox=1; }
	if(P[Player].state==202 && P[Player].animFrame==2){ P[Player].dataHBox[0]= 0; P[Player].dataHBox[1]=-20; P[Player].dataHBox[2]=80; P[Player].dataHBox[3]=0; }
	if(P[Player].state==202 && P[Player].animFrame==3){ reset_HBox=1; }
	if(P[Player].state==203 && P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 70; P[Player].dataHBox[2]=120; P[Player].dataHBox[3]=-40; }
	if(P[Player].state==203 && P[Player].animFrame==5){ reset_HBox=1; }
	if(P[Player].state==204 && P[Player].animFrame==3){ P[Player].dataHBox[0]= 0; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]=65; P[Player].dataHBox[3]=-30; }
	if(P[Player].state==204 && P[Player].animFrame==4){ reset_HBox=1; }
	if(P[Player].state==205 && P[Player].animFrame==2){ P[Player].dataHBox[0]= 0; P[Player].dataHBox[1]=-20; P[Player].dataHBox[2]=80; P[Player].dataHBox[3]=0; }
	if(P[Player].state==205 && P[Player].animFrame==3){ reset_HBox=1; }
	if(P[Player].state==206 && P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 20; P[Player].dataHBox[2]=85; P[Player].dataHBox[3]=0; }
	if(P[Player].state==206 && P[Player].animFrame==5){ reset_HBox=1; }
	
	if((P[Player].state==301 || P[Player].state==311 || P[Player].state==321) && P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]= 80; P[Player].dataHBox[3]=-1; }
	if((P[Player].state==301 || P[Player].state==311 || P[Player].state==321) && P[Player].animFrame==4){ reset_HBox=1; }
	if((P[Player].state==302 || P[Player].state==312 || P[Player].state==322) && P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]= 80; P[Player].dataHBox[3]=-1; }
	if((P[Player].state==302 || P[Player].state==312 || P[Player].state==322) && P[Player].animFrame==6){ reset_HBox=1; }
	if((P[Player].state==303 || P[Player].state==313 || P[Player].state==323) && P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]= 80; P[Player].dataHBox[3]=-1; }
	if((P[Player].state==303 || P[Player].state==313 || P[Player].state==323) && P[Player].animFrame==6){ reset_HBox=1; }
	if((P[Player].state==304 || P[Player].state==314 || P[Player].state==324) && P[Player].animFrame==2){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]=60; P[Player].dataHBox[3]=-10; }
	if((P[Player].state==304 || P[Player].state==314 || P[Player].state==324) && P[Player].animFrame==3){ reset_HBox=1; }
	if((P[Player].state==305 || P[Player].state==315 || P[Player].state==325) && P[Player].animFrame==3){ P[Player].dataHBox[0]=-20; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]=70; P[Player].dataHBox[3]=-10; }
	if((P[Player].state==305 || P[Player].state==315 || P[Player].state==325) && P[Player].animFrame==4){ reset_HBox=1; }
	if((P[Player].state==306 || P[Player].state==316 || P[Player].state==326) && P[Player].animFrame==3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]=70; P[Player].dataHBox[3]=-10; }
	if((P[Player].state==306 || P[Player].state==316 || P[Player].state==326) && P[Player].animFrame==4){ reset_HBox=1; }
	
	if(P[Player].state==470 && P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
	if(P[Player].state==470 && P[Player].animFrame==2){ reset_BBox=1; }
	if(P[Player].state==470 && P[Player].animFrame==3){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 90; P[Player].dataBBox[2]= 15; P[Player].dataBBox[3]=  0; }
	
	if(P[Player].state==471 && P[Player].animFrame<=2){ reset_BBox=1; }
	if(P[Player].state==472 && P[Player].animFrame<=2){ reset_BBox=1; }
	
	if(P[Player].state==601 && P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 20; P[Player].dataBBox[3]=  0; }
	if(P[Player].state==602 && P[Player].animFrame==1){ P[Player].dataBBox[0]=-20; P[Player].dataBBox[1]=- 70; P[Player].dataBBox[2]= 20; P[Player].dataBBox[3]=  0; }
	
	if(P[Player].state==710 && P[Player].animFrame== 3){ P[Player].dataHBox[0]= -40; P[Player].dataHBox[1]=-100; P[Player].dataHBox[2]=40; P[Player].dataHBox[3]=-10; }
	if(P[Player].state==710 && P[Player].animFrame==17){ reset_HBox=1; }
	
	if(P[Player].state==720 && P[Player].animFrame== 3){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]=70; P[Player].dataHBox[3]=-20; }
	if(P[Player].state==720 && P[Player].animFrame== 7){ reset_HBox=1; }
	
	if(P[Player].state==730 && P[Player].animFrame== 6){ P[Player].dataHBox[0]= 10; P[Player].dataHBox[1]=- 50; P[Player].dataHBox[2]=110; P[Player].dataHBox[3]=-30; }
	if(P[Player].state==730 && P[Player].animFrame== 7){ reset_HBox=1; }

	
	}
	
	
	if(P[Player].direcao==-1)//dataHBox[0]<P[Player].dataHBox[2]) //Normaliza a box
	{
		s16 a;
		a = P[Player].dataHBox[0];
		P[Player].dataHBox[0] = P[Player].dataHBox[2];
		P[Player].dataHBox[2] = a;
		P[Player].dataHBox[0] = P[Player].dataHBox[0] * P[Player].direcao;
		P[Player].dataHBox[2] = P[Player].dataHBox[2] * P[Player].direcao;
	}
	
	//padroes
	if(P[Player].state==100 && P[Player].animFrame==1){ reset_HBox=1; } //parado
	if( (P[Player].state>=610 && P[Player].state<=618) /*&& P[Player].animFrame==1*/ ){ reset_BBox=1; } //win poses, time over; rage explosion
	if( (P[Player].state==550) /*&& P[Player].animFrame==1*/ ){ reset_BBox=1; } //win poses, time over; rage explosion
	
	if(Player==1 && (P[2].state==110 || P[2].state==210)){ reset_HBox=1; }
	if(Player==2 && (P[1].state==110 || P[1].state==210)){ reset_HBox=1; }
	
	//resets
	if(reset_HBox==1){ P[Player].dataHBox[0]=0; P[Player].dataHBox[1]=0; P[Player].dataHBox[2]=0; P[Player].dataHBox[3]=0; } //Hit Boxes
	if(reset_BBox==1){ P[Player].dataBBox[0]=0; P[Player].dataBBox[1]=0; P[Player].dataBBox[2]=0; P[Player].dataBBox[3]=0; } //Body Boxes
	
}

void FUNCAO_PHYSICS() //FISICA!
{
	for(i=1; i<=2; i++)
	{
		//-----------------------------------------------------------------------------------------
		
		//-----------------------------------------------------------------------------------------
		
		//Hits Movement, deslocamento dos Players para trás, durante os ataques
		if(P[i].state>=500 && P[i].state<=550 && (P[1].hitPause==0 && P[2].hitPause==0) )
		{
			u8 PA=0; //Player que Ataca
			u8 PR=0; //Player que Recebe o Ataque
			if(i == 1){ PA = 2; PR = 1; }else{ PA = 1; PR = 2; }
			
			//Nota, sem as condicoes abaixo, o deslocamento será continuo, enquanto durar a animacao de Hit
			//Esse algoritimo simples simula a desaceleracao, ou friccao, causada pelo atrito com o solo
			if(P[PR].animFrame<=3)
			{ 
				if(P[PR].animFrame==1) { P[PR].hSpeed = 3; } //vel 3
				if(P[PR].animFrame==2) { P[PR].hSpeed = 2; } //vel 2
				if(P[PR].animFrame==3) { P[PR].hSpeed = 1; } //vel 1
				P[PR].x += P[PR].hSpeed*(P[PR].direcao*-1);
			}			
			
			if(P[PR].x >= gLimiteCenarioD)
			{
				P[PA].hSpeed = 2;
				if(gPing2 == 1 && (P[PR].state!=550) ){ P[PA].x += P[PA].hSpeed*(P[PA].direcao*-1); }
			}
			if(P[PR].x <= gLimiteCenarioE)
			{
				P[PA].hSpeed = 2;
				if(gPing2 == 1 && (P[PR].state!=550) ){ P[PA].x += P[PA].hSpeed*(P[PA].direcao*-1); }
			}
		}
		
		//hitpause shake
		if(P[i].hitPause>0)
		{
			if(gPing2==0){ P[i].x-=2; }
			if(gPing2==1){ P[i].x+=2; }
			P[i].hitPause--;
		}
		
		//andar
		if(P[i].state==410 || P[i].state==420)
		{
			P[i].hSpeed=2;
			if(P[i].state==410){P[i].x+=P[i].hSpeed*(P[i].direcao*-1);}
			if(P[i].state==420){P[i].x+=P[i].hSpeed*(P[i].direcao);}
		}
		
		//esquiva frente e tras
		if(P[i].state==471 || P[i].state==472)
		{
			if(P[i].animFrame>=1){ P[i].hSpeed=3; if(P[i].state==471){ P[i].hSpeed=4; } }
			if(P[i].animFrame>=3){ P[i].hSpeed=2; }
			if(P[i].animFrame>=5){ P[i].hSpeed=1; }
			if(P[i].animFrame>=6){ P[i].hSpeed=0; } //anula o deslocamento
			if(P[i].state==471){P[i].x+=P[i].hSpeed*(P[i].direcao*-1);}
			if(P[i].state==472){P[i].x+=P[i].hSpeed*(P[i].direcao);}
		}
		
		//defesas
		if( (P[i].state==110 || P[i].state==210) && (P[1].hitPause==0 && P[2].hitPause==0) )
		{
			P[i].hSpeed=4;
			if(P[i].state==110){P[i].x+=P[i].hSpeed*(P[i].direcao*-1);}//defesa em pe
			if(P[i].state==210){P[i].x+=P[i].hSpeed*(P[i].direcao*-1);}//defesa abaixado
		}
		
		//pulos
		if(P[1].hitPause==0 && P[2].hitPause==0)
		{
			if(P[i].puloTimer!=0){ P[i].puloTimer++; }
		
			if(P[i].state>=300 && P[i].state<=326)
			{ 
				P[i].hSpeed=3;
				/*samsho2*/ //hard code :P
				//o metodo a seguir é extremamente preciso, mas exige notacao manual!
				//para outros metodos de movimento, ver fisica no state 550, ou outros golpes por exemplo
				if(P[i].puloTimer== 1){ P[i].y-=11; }
				if(P[i].puloTimer== 2){ P[i].y-=11; }
				if(P[i].puloTimer== 3){ P[i].y-=10; }
				if(P[i].puloTimer== 4){ P[i].y-= 9; }
				if(P[i].puloTimer== 5){ P[i].y-= 9; }
				if(P[i].puloTimer== 6){ P[i].y-= 8; }
				if(P[i].puloTimer== 7){ P[i].y-= 8; }
				if(P[i].puloTimer== 8){ P[i].y-= 8; }
				if(P[i].puloTimer== 9){ P[i].y-= 7; }
				if(P[i].puloTimer==10){ P[i].y-= 6; } 
				if(P[i].puloTimer==11){ P[i].y-= 6; }
				if(P[i].puloTimer==12){ P[i].y-= 5; }
				if(P[i].puloTimer==13){ P[i].y-= 5; }
				if(P[i].puloTimer==14){ P[i].y-= 4; }
				if(P[i].puloTimer==15){ P[i].y-= 4; }
				if(P[i].puloTimer==16){ P[i].y-= 3; }
				if(P[i].puloTimer==17){ P[i].y-= 3; }
				if(P[i].puloTimer==18){ P[i].y-= 3; }
				if(P[i].puloTimer==19){ P[i].y-= 1; }
				if(P[i].puloTimer==20){ P[i].y-= 2; } 
				if(P[i].puloTimer==21){ P[i].y-= 0; }
				if(P[i].puloTimer==22){ P[i].y-= 1; }
				if(P[i].puloTimer==23){ P[i].y+= 1; }
				if(P[i].puloTimer==24){ P[i].y+= 0; }
				if(P[i].puloTimer==25){ P[i].y+= 2; }
				if(P[i].puloTimer==26){ P[i].y+= 1; }
				if(P[i].puloTimer==27){ P[i].y+= 3; }
				if(P[i].puloTimer==28){ P[i].y+= 3; }
				if(P[i].puloTimer==29){ P[i].y+= 3; }
				if(P[i].puloTimer==30){ P[i].y+= 4; } 
				if(P[i].puloTimer==31){ P[i].y+= 4; }
				if(P[i].puloTimer==32){ P[i].y+= 5; }
				if(P[i].puloTimer==33){ P[i].y+= 5; }
				if(P[i].puloTimer==34){ P[i].y+= 6; }
				if(P[i].puloTimer==35){ P[i].y+= 6; }
				if(P[i].puloTimer==36){ P[i].y+= 7; }
				if(P[i].puloTimer==37){ P[i].y+= 8; }
				if(P[i].puloTimer==38){ P[i].y+= 8; }
				if(P[i].puloTimer==39){ P[i].y+= 8; }
				if(P[i].puloTimer==40){ P[i].y+= 9; } 
				if(P[i].puloTimer==41){ P[i].y+= 9; }
				if(P[i].puloTimer==42){ P[i].y+=10; }
				if(P[i].puloTimer==43){ P[i].y+=11; }
				if(P[i].puloTimer==44){ P[i].y+=11; }
				if(P[i].puloTimer==45){ P[i].y+= 1; }
				
				if(P[i].state>=320 && P[i].state<=326){ P[i].x+=P[i].hSpeed*(P[i].direcao); }
				if(P[i].state>=310 && P[i].state<=316){ P[i].x+=P[i].hSpeed*(P[i].direcao*-1); }
			}
		}
		
		//corrige posX //limites do cenario
		if(P[i].x<gLimiteCenarioE)
		{ 
			if( P[i].state>=550 && P[i].state<=552 )
			{ 
				P[i].x=gLimiteCenarioE+1; 
			}else{ 
				P[i].x=gLimiteCenarioE; 
			}
		}
		if(P[i].x>gLimiteCenarioD)
		{ 
			if( P[i].state>=550 && P[i].state<=552 )
			{ 
				P[i].x=gLimiteCenarioD-1; 
			}else{
				P[i].x=gLimiteCenarioD; 
			}
		}
		
		/*samsho2*/
		//'516' CAINDO (Apos Magia do Haohmaru)
		if( P[1].hitPause==0 && P[2].hitPause==0 )
		{
			P[i].cicloInteracoesGravidadeCont++;
			
			if(P[i].state==516)
			{
				if(P[i].cicloInteracoesGravidadeCont>=P[i].cicloInteracoesGravidade){
					P[i].cicloInteracoesGravidadeCont=0;
					P[i].impulsoY = P[i].impulsoY+P[i].gravidadeY;
					P[i].impulsoY-=1;
				}
				P[i].y += P[i].impulsoY;
				if(gPing2==1){ if(P[i].direcao==1){ P[i].x-=1; }else{ P[i].x+=1; } }
			}
		}
		
		//'516' > Altura Piso
		if(P[i].state==516 && P[i].y>gAlturaPiso)
		{
			P[i].y=gAlturaPiso-1;
			P[i].gravidadeY = gravidadePadrao;
			P[i].impulsoY = -6;//-14
			PLAYER_STATE(i,551);
		}
		
		//'550' CAINDO
		if( P[1].hitPause==0 && P[2].hitPause==0 )
		{
			P[i].cicloInteracoesGravidadeCont++;
			
			if(P[i].state==550)
			{
				if(P[i].cicloInteracoesGravidadeCont>=P[i].cicloInteracoesGravidade){
					P[i].cicloInteracoesGravidadeCont=0;
					P[i].impulsoY = P[i].impulsoY+P[i].gravidadeY;
					P[i].impulsoY-=1;
				}
				P[i].y += P[i].impulsoY;
				if(P[i].direcao==1){ P[i].x-=2; }else{ P[i].x+=2; }
			}
		}
		
		//'550' > Altura Piso
		if(P[i].state==550 && P[i].y>gAlturaPiso)
		{
			P[i].y=gAlturaPiso-1;
			P[i].gravidadeY = gravidadePadrao;
			P[i].impulsoY = -6;//-14
			PLAYER_STATE(i,551);
		}
		
		if(P[i].state==551){
			if(P[i].cicloInteracoesGravidadeCont>=P[i].cicloInteracoesGravidade){
				P[i].cicloInteracoesGravidadeCont=0;
				P[i].impulsoY = P[i].impulsoY+P[i].gravidadeY;
				P[i].impulsoY-=1;
			}
			P[i].y += P[i].impulsoY;
			if(P[i].direcao==1){ P[i].x-=2; }else{ P[i].x+=2; }
		}
		
		//'551' > Altura Piso
		if(P[i].state==551 && P[i].y>gAlturaPiso)
		{
			P[i].y=gAlturaPiso;
			PLAYER_STATE(i,570);
		}
		
		//-----------------------------------------------------------------------------------------
		
		//'MAGIAS' FISICA
		if( P[1].hitPause==0 && P[2].hitPause==0 )
		{
			//P[i].cicloInteracoesGravidadeCont++;
			
			if( (P[i].state==710 || P[i].state==720 || P[i].state==730 || P[i].state==740 || P[i].state==750) && P[i].y!=gAlturaPiso )
			{
				if(P[i].cicloInteracoesGravidadeCont>=P[i].cicloInteracoesGravidade){
					P[i].cicloInteracoesGravidadeCont=0;
					P[i].impulsoY = P[i].impulsoY+P[i].gravidadeY;
					P[i].impulsoY-=1;
				}
				P[i].y += P[i].impulsoY;
				if(P[i].direcao==-1){ P[i].x-=2; }else{ P[i].x+=2; }
			}
		}
		
		//'MAGIAS' FISICA > Altura Piso
		//TERRY (magia 720)
		if( (P[i].state==720) && P[i].y+2>gAlturaPiso)
		{
			P[i].y = gAlturaPiso;
			P[i].frameTimeAtual = P[i].frameTimeTotal; //Passa para o proximo frame de animacao
		}

		// terry rising tackle
		if(P[i].id==1 && P[i].state==700 && P[i].animFrame==6)
		{
			P[i].gravidadeY = gravidadePadrao; 
			P[i].impulsoY = impulsoPadrao;
		}
		//'700' gravidade
		if( P[1].hitPause==0 && P[2].hitPause==0 )
		{
			if(P[i].id==1 && P[i].state==700 && P[i].animFrame>=6)
			{
				if(P[i].cicloInteracoesGravidadeCont>=P[i].cicloInteracoesGravidade){
					P[i].cicloInteracoesGravidadeCont=0;
					P[i].impulsoY = P[i].impulsoY+P[i].gravidadeY;
					P[i].impulsoY-=1;
				}
				P[i].y += P[i].impulsoY;
				if(P[i].direcao==1){ P[i].x+=3; }else{ P[i].x-=3; }  //se quiser um deslocamento lateral...
			}
		}
		//'700' > Altura Piso
		if(P[i].state==700 && P[i].y>gAlturaPiso)
		{
			P[i].y=gAlturaPiso;
			P[i].gravidadeY = 0;
			P[i].impulsoY = 0;
			PLAYER_STATE(i,100);
		}


		// terry crack shoot
		if(P[i].id==1 && P[i].state==720 && P[i].animFrame>=3 && P[i].animFrame<=10 && (P[1].hitPause==0 && P[2].hitPause==0)) //crack shoot
		{
			P[i].hSpeed=6;
			P[i].x+=P[i].hSpeed*(P[i].direcao);
		}
	
		//-----------------------------------------------------------------------------------------
		
		//Movimento das Magias
		if(P[i].fBallActive == 1)
		{
			P[i].fBallX += P[i].fBallSpeedX * P[i].fBallDirecao;
			SPR_setPosition( P[i].fBall, P[i].fBallX, P[i].fBallY );
			if(P[i].fBallX<gLimiteCenarioE-100){ SPR_releaseSprite(P[i].fBall); P[i].fBallActive=0; }
			if(P[i].fBallX>gLimiteCenarioD+100){ SPR_releaseSprite(P[i].fBall); P[i].fBallActive=0; }
		}
		
	}
	
	//Colisao Mass Boxes
	if(FUNCAO_COLISAO(
	P[1].x+P[1].dataMBox[0], P[1].y+P[1].dataMBox[1], P[1].x+P[1].dataMBox[2], P[1].y+P[1].dataMBox[3], 
	P[2].x+P[2].dataMBox[0], P[2].y+P[2].dataMBox[1], P[2].x+P[2].dataMBox[2], P[2].y+P[2].dataMBox[3]
	)){
		u8 tempBigHSpeed=0;
		if(P[1].hSpeed>=P[2].hSpeed){tempBigHSpeed=P[1].hSpeed;}else{tempBigHSpeed=P[2].hSpeed;}
		if(P[1].x<P[2].x){
			P[1].x-=tempBigHSpeed;
			P[2].x+=tempBigHSpeed;
		}else{
			P[1].x+=tempBigHSpeed;
			P[2].x-=tempBigHSpeed;
		}
		
	}
}

void FUNCAO_CAMERA_BGANIM()
{
	
	//Animacao do Cenario / Background!
	if(gBG_Choice==1){ 
		u8 BGtimer = 33;
		gBgAnim++;
		/*
		if(gBgAnim==BGtimer*1){ VDP_loadTileData(gfx_bgb1B.tileset->tiles + INDEX_MAR*8, INDEX_MAR+1, 468, DMA_QUEUE); }
		if(gBgAnim==BGtimer*2){ VDP_loadTileData(gfx_bgb1C.tileset->tiles + INDEX_MAR*8, INDEX_MAR+1, 468, DMA_QUEUE); }
		if(gBgAnim==BGtimer*3){ VDP_loadTileData(gfx_bgb1D.tileset->tiles + INDEX_MAR*8, INDEX_MAR+1, 468, DMA_QUEUE); }
		if(gBgAnim==BGtimer*4){ VDP_loadTileData(gfx_bgb1A.tileset->tiles + INDEX_MAR*8, INDEX_MAR+1, 468, DMA_QUEUE); }
		*/
		if(gBgAnim==BGtimer*4){ gBgAnim = 0; }
	}
	
    s16 pMax = max(P[1].x, P[2].x);
	s16 pMin = min(P[1].x, P[2].x);
	bool walk1= FALSE, walk2 = FALSE;

	// percebe se o player está parado ou andando, tem que checar os outros casos de parado
	if(P[1].state != 100)
	{
		walk1 = TRUE;
	}
	if(P[2].state != 100)
	{
		walk2 = TRUE;
	}

	if((pMax > 250) & (pMin > 40) & (walk1 | walk2) & (gScrollValue > (320-(gBG_Width))))
	{
		if(walk2 == FALSE)
		{
			P[2].x -= P[1].hSpeed;
			P[1].x -= P[1].hSpeed;
			gScrollValue -= P[1].hSpeed;
		}
		else if(walk1 == FALSE)
		{
			P[1].x -= P[2].hSpeed;
			P[2].x -= P[2].hSpeed;
			gScrollValue -= P[2].hSpeed;
		}
		if(gScrollValue == -192)
		{
			if(gBG_Choice==1){ VDP_setTileMapEx(BG_B,gfx_bgb1.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0),0,0,64,0,8,28,DMA_QUEUE); }
			if(gBG_Choice==3){ VDP_setTileMapEx(BG_B,gfx_bgb3.tilemap ,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0),0,0,64,0,8,28,DMA_QUEUE); }
		}
		KLog_S1("scroll negativo: ", gScrollValue);
	}
	if((pMin < 70) & (pMax < 280) & (walk1 | walk2) & (gScrollValue < 0)) 
	{
		if(walk2 == FALSE)
		{
			P[2].x += P[1].hSpeed;
			P[1].x += P[1].hSpeed;
			gScrollValue += P[1].hSpeed;
		}
		else if(walk1 == FALSE)
		{
			P[1].x += P[2].hSpeed;
			P[2].x += P[2].hSpeed;
			gScrollValue += P[2].hSpeed;
		}
		if(gScrollValue == -60)
		{
			if(gBG_Choice==1){ VDP_setTileMapEx(BG_B,gfx_bgb1.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0),0,0,0,0,8,28,DMA_QUEUE); }
			if(gBG_Choice==3){ VDP_setTileMapEx(BG_B,gfx_bgb3.tilemap ,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,0),0,0,0,0,8,28,DMA_QUEUE); }
		}
		KLog_S1("scroll positivo: ", gScrollValue);
	}
	VDP_setHorizontalScroll(BG_B, gScrollValue);

}

void FUNCAO_DEPTH(u8 Player)
{
	//Qto menor o 'depth', mais para frente o objeto fica na tela
	u8 depth;
	if(P[Player].state>=500 && P[Player].state<=572)                         {depth = Player+20;} //Hurts, Caindo
	if(P[Player].state==100)                                                 {depth = Player+10;} //Parado
	if(P[Player].state==471 || P[Player].state==472)                         {depth = Player+10;} //Rolamento, Dash, Esquiva
	if(P[Player].state==410 || P[Player].state==420)                         {depth = Player+10;} //Andando
	if(P[Player].state>=107 && P[Player].state<=110)                         {depth = Player+10;} //Defesa em Pe
	if(P[Player].state>=207 && P[Player].state<=210)                         {depth = Player+10;} //Defesa Abaixado
	if(P[Player].state>=101 && P[Player].state<=156)                         {depth = Player+ 8;} //Golpes em Pe
	if(P[Player].state>=200 && P[Player].state<=206)                         {depth = Player+ 6;} //Golpes Abaixados
	if(P[Player].state==300 || P[Player].state==310 || P[Player].state==320) {depth = Player+ 4;} //Pulos
	if(P[Player].state>=301 && P[Player].state<=326)                         {depth = Player+ 2;} //Golpes Aereos
	if(P[Player].state>=611 && P[Player].state<=614)                         {depth = Player+ 2;} //Vitoria
	if(P[Player].state>=700 && P[Player].state<=790)                         {depth = Player+ 2;} //Magias
	//define o Depth, SE e SOMENTE SE o sprite estiver carregado. 
	//O comando 'SPR_setDepth' pode ser potencialmente perigoso e TRAVAR o jogo se o SPRITE nao estiver completamente carregado na VRAM
	if(P[Player].sprite){ SPR_setDepth(P[Player].sprite, depth); }
}

void FUNCAO_SAMSHOFX() //ESPECIFICO DO SAMURAI SHODOWN 2
{
	//Carregar Sprite da Onda (Splash da Onda)
	//Eu optei por desabilitar temporariamente esse efeito para liberar VRAM
	/*
	if(gFrames==355 && gBG_Choice==1)
	{
		//GE[11].sprite = SPR_addSpriteExSafe(&spr_point, P[1].x, P[1].y, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		if (GE[11].sprite)
		{
		  SPR_releaseSprite(GE[11].sprite);
		  GE[11].sprite = NULL;
		}
		GE[11].sprite = SPR_addSpriteExSafe(&spr_wave_splash_big, 50, 88, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		SPR_setDepth(GE[11].sprite, 255 );
		SPR_setVRAMTileIndex(GE[11].sprite, 1453); //define uma posicao especifica para o GFX na VRAM
	}
	*/
	
	//ESPECIFICO DE SAMURAI SHODOW. AJUSTE DA BARRA DE RAGE
	//if(P[1].energiaSP ==  0){  P[1].rageLvl=0; }
	//if(P[1].energiaSP >   0 && P[1].energiaSP<=15){ P[1].rageLvl=0; }
	//if(P[1].energiaSP >= 16 && P[1].energiaSP<=31){ P[1].rageLvl=1; }
	//if(P[1].energiaSP >= 32){  P[1].rageLvl=2; }
	
	//if(P[2].energiaSP ==  0){  P[2].rageLvl=0; }
	//if(P[2].energiaSP >   0 && P[2].energiaSP<=15){ P[2].rageLvl=0; }
	//if(P[2].energiaSP >= 16 && P[2].energiaSP<=31){ P[2].rageLvl=1; }
	//if(P[2].energiaSP >= 32){  P[2].rageLvl=2; }
	
	//SOMBRA
	if(gPing2==0)
	{ 
		//P1
		if(gSombraStyle==2){ SPR_setVisibility(P[1].sombra, VISIBLE); }
		//P2
		if(gSombraStyle==2){ SPR_setVisibility(P[2].sombra, HIDDEN); }
	}
	if(gPing2==1)
	{ 
		//P1
		if(gSombraStyle==2){ SPR_setVisibility(P[1].sombra, HIDDEN); }
		//P2
		if(gSombraStyle==2){ SPR_setVisibility(P[2].sombra, VISIBLE); }
	}
}

void FUNCAO_DEBUG()
{
	//Atencao! O debug só é funcional se você não printar sprites em cima dos seus caracteres alfanuméricos!
	// Procure pelo comando: SPR_setVRAMTileIndex(GE[XX].sprite, 1441); 
	// Onde, 1441 é o comeco dos tiles alfanumericos, e 1535 é o final desses tiles!
	
	//Debug em texto, desativado:
	//VDP_drawText("HAMOOPIG ENGINE", 1, 1);
	//sprintf(gStr, "P1-> %i,%i S:%i T:%i/%i F:%i/%i    ", P[1].x, P[1].y, P[1].state, P[1].frameTimeAtual, P[1].frameTimeTotal, P[1].animFrame, P[1].animFrameTotal ); 
	//sprintf(gStr, "P1-> S:%i T:%i/%i F:%i/%i    ", P[1].state, P[1].frameTimeAtual, P[1].frameTimeTotal, P[1].animFrame, P[1].animFrameTotal ); 
	//VDP_drawText(gStr, 1, 2);
	//sprintf(gStr, "P2-> S:%i T:%i/%i F:%i/%i    ", P[2].state, P[2].frameTimeAtual, P[2].frameTimeTotal, P[2].animFrame, P[2].animFrameTotal ); 
	//VDP_drawText(gStr, 1, 3);
	//sprintf(gStr, "UP:%i DOWN:%i LEFT:%i RIGHT:%i ATK_B:%i  ", P[1].joyDirTimer[8], P[1].joyDirTimer[2], P[1].joyDirTimer[4], P[1].joyDirTimer[6], P[1].attackButton ); 
	
	SPR_setPosition(GE[1].sprite, P[1].x-4, P[1].y-5);
	SPR_setPosition(GE[2].sprite, P[2].x-4, P[2].y-5);
	
	//P1
	//bodyboxes
	if(P[1].dataBBox[0]==0 && P[1].dataBBox[2]==0){
		SPR_setPosition(Rect1BB1_Q1,-8,-8); SPR_setPosition(Rect1BB1_Q2,-8,-8); SPR_setPosition(Rect1BB1_Q3,-8,-8); SPR_setPosition(Rect1BB1_Q4,-8,-8);
	}else{
		SPR_setPosition(Rect1BB1_Q1, P[1].x+P[1].dataBBox[0]  , P[1].y+P[1].dataBBox[1]);
		SPR_setPosition(Rect1BB1_Q2, P[1].x+P[1].dataBBox[2]-8, P[1].y+P[1].dataBBox[1]);
		SPR_setPosition(Rect1BB1_Q3, P[1].x+P[1].dataBBox[0]  , P[1].y+P[1].dataBBox[3]-8);
		SPR_setPosition(Rect1BB1_Q4, P[1].x+P[1].dataBBox[2]-8, P[1].y+P[1].dataBBox[3]-8);
	}
	//hitboxes
	if(P[1].dataHBox[0]==0 && P[1].dataHBox[2]==0){
		SPR_setPosition(Rect1HB1_Q1,-8,-8); SPR_setPosition(Rect1HB1_Q2,-8,-8); SPR_setPosition(Rect1HB1_Q3,-8,-8); SPR_setPosition(Rect1HB1_Q4,-8,-8);
	}else{
		SPR_setPosition(Rect1HB1_Q1, P[1].x+P[1].dataHBox[0]  , P[1].y+P[1].dataHBox[1]);
		SPR_setPosition(Rect1HB1_Q2, P[1].x+P[1].dataHBox[2]-8, P[1].y+P[1].dataHBox[1]);
		SPR_setPosition(Rect1HB1_Q3, P[1].x+P[1].dataHBox[0]  , P[1].y+P[1].dataHBox[3]-8);
		SPR_setPosition(Rect1HB1_Q4, P[1].x+P[1].dataHBox[2]-8, P[1].y+P[1].dataHBox[3]-8);
	}
	
	//P2
	//bodyboxes
	if(P[2].dataBBox[0]==0 && P[2].dataBBox[2]==0){
		SPR_setPosition(Rect2BB1_Q1,-8,-8); SPR_setPosition(Rect2BB1_Q2,-8,-8); SPR_setPosition(Rect2BB1_Q3,-8,-8); SPR_setPosition(Rect2BB1_Q4,-8,-8);
	}else{
		SPR_setPosition(Rect2BB1_Q1, P[2].x+P[2].dataBBox[0]  , P[2].y+P[2].dataBBox[1]);
		SPR_setPosition(Rect2BB1_Q2, P[2].x+P[2].dataBBox[2]-8, P[2].y+P[2].dataBBox[1]);
		SPR_setPosition(Rect2BB1_Q3, P[2].x+P[2].dataBBox[0]  , P[2].y+P[2].dataBBox[3]-8);
		SPR_setPosition(Rect2BB1_Q4, P[2].x+P[2].dataBBox[2]-8, P[2].y+P[2].dataBBox[3]-8);
	}
	//hitboxes
	if(P[2].dataHBox[0]==0 && P[2].dataHBox[2]==0){
		SPR_setPosition(Rect2HB1_Q1,-8,-8); SPR_setPosition(Rect2HB1_Q2,-8,-8); SPR_setPosition(Rect2HB1_Q3,-8,-8); SPR_setPosition(Rect2HB1_Q4,-8,-8);
	}else{
		SPR_setPosition(Rect2HB1_Q1, P[2].x+P[2].dataHBox[0]  , P[2].y+P[2].dataHBox[1]);
		SPR_setPosition(Rect2HB1_Q2, P[2].x+P[2].dataHBox[2]-8, P[2].y+P[2].dataHBox[1]);
		SPR_setPosition(Rect2HB1_Q3, P[2].x+P[2].dataHBox[0]  , P[2].y+P[2].dataHBox[3]-8);
		SPR_setPosition(Rect2HB1_Q4, P[2].x+P[2].dataHBox[2]-8, P[2].y+P[2].dataHBox[3]-8);
	}
	
	//metodo de display HBoxes Info, (piscando) = reduz os tiles simultaneos em tela (DESATIVADO)
	//para facilitar a vida de outros desenvolvedores, decidi manter a exibicao das caixas o tempo todo
	if(gPing2==0)
	{
		/*
		SPR_setVisibility(Rect1HB1_Q1, VISIBLE); SPR_setVisibility(Rect1HB1_Q2, VISIBLE);
		SPR_setVisibility(Rect1HB1_Q3, VISIBLE); SPR_setVisibility(Rect1HB1_Q4, VISIBLE);
		SPR_setVisibility(Rect1BB1_Q1, VISIBLE); SPR_setVisibility(Rect1BB1_Q2, VISIBLE);
		SPR_setVisibility(Rect1BB1_Q3, VISIBLE); SPR_setVisibility(Rect1BB1_Q4, VISIBLE);
		
		SPR_setVisibility(Rect2HB1_Q1, HIDDEN); SPR_setVisibility(Rect2HB1_Q2, HIDDEN);
		SPR_setVisibility(Rect2HB1_Q3, HIDDEN); SPR_setVisibility(Rect2HB1_Q4, HIDDEN);
		SPR_setVisibility(Rect2BB1_Q1, HIDDEN); SPR_setVisibility(Rect2BB1_Q2, HIDDEN);
		SPR_setVisibility(Rect2BB1_Q3, HIDDEN); SPR_setVisibility(Rect2BB1_Q4, HIDDEN);
	}else{
		SPR_setVisibility(Rect1HB1_Q1, HIDDEN); SPR_setVisibility(Rect1HB1_Q2, HIDDEN);
		SPR_setVisibility(Rect1HB1_Q3, HIDDEN); SPR_setVisibility(Rect1HB1_Q4, HIDDEN);
		SPR_setVisibility(Rect1BB1_Q1, HIDDEN); SPR_setVisibility(Rect1BB1_Q2, HIDDEN);
		SPR_setVisibility(Rect1BB1_Q3, HIDDEN); SPR_setVisibility(Rect1BB1_Q4, HIDDEN);
		
		SPR_setVisibility(Rect2HB1_Q1, VISIBLE); SPR_setVisibility(Rect2HB1_Q2, VISIBLE);
		SPR_setVisibility(Rect2HB1_Q3, VISIBLE); SPR_setVisibility(Rect2HB1_Q4, VISIBLE);
		SPR_setVisibility(Rect2BB1_Q1, VISIBLE); SPR_setVisibility(Rect2BB1_Q2, VISIBLE);
		SPR_setVisibility(Rect2BB1_Q3, VISIBLE); SPR_setVisibility(Rect2BB1_Q4, VISIBLE);
		*/
	}
}

bool FUNCAO_COLISAO(s16 R1x1, s16 R1y1, s16 R1x2, s16 R1y2, s16 R2x1, s16 R2y1, s16 R2x2, s16 R2y2)
{
	
	s16 temp=0;
	if(R1x1>R1x2){ temp=R1x1; R1x1=R1x2; R1x2=temp; }
	if(R2x1>R2x2){ temp=R2x1; R2x1=R2x2; R2x2=temp; }
	
	if ( (R1x1+(R1x2-R1x1)>R2x1) && (R1x1<R2x1+(R2x2-R2x1)) && 
		 (R1y1+(R1y2-R1y1)>R2y1) && (R1y1<R2y1+(R2y2-R2y1)) )
	{ 
		
		if(R1x1==R1x2 && R1y1==R1y2){
			return 0;
			}else if(R2x1==R2x2 && R2y1==R2y2){
				return 0;
			}else{
				/*houve colisao*/
				return 1;
			}
	}else{ 
		/*nao houve colisao*/
		return 0;
	}
}

void FUNCAO_UPDATE_LIFESP(u8 Player, u8 EnergyType, s8 Value)
{
	if(EnergyType==1) //energia
	{
		if(P[Player].energiaBase<Value*-1){ P[Player].energiaBase=0;
		}else{ P[Player].energiaBase = P[Player].energiaBase+Value; }
	}
	if(EnergyType==2) //energia especial
	{
		if(P[Player].energiaSP<Value*-1){ P[Player].energiaSP=0;
		}else{ P[Player].energiaSP = P[Player].energiaSP+Value; }
	}
}

void FUNCAO_INICIALIZACAO()
{
	gPodeMover=0;
	
	//BG_B
	gInd_tileset=0; //Antes de carregar o Background, definir o ponto de inicio de carregamento na VRAM
	
	if(Cenario_Tilemap !=NULL)
	{
		//caso já esteja alocado desaloca
		MEM_free(Cenario_Tilemap);
	}
	//Load the tileset 'BGB1'
	if(gBG_Choice==1){ 
		gBG_Width = 512; // Largura do Cenario em pixels!
		gScrollValue=-(gBG_Width-320)/2;
		Cenario_Tilemap = unpackTileMap(gfx_bgb1.tilemap,NULL);
		VDP_loadTileSet(gfx_bgb1.tileset,gInd_tileset,DMA); 
		VDP_setTileMapEx(BG_B,Cenario_Tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,gInd_tileset),0,0,0,0,gBG_Width/8,28,DMA_QUEUE);
		VDP_setPalette(PAL0, gfx_bgb1.palette->data);
		gInd_tileset += gfx_bgb1.tileset->numTile;

		
	} 
	
	//Load the tileset 'BGB2'
	if(gBG_Choice==2){ 
		gBG_Width = 512; // Largura do Cenario em pixels!
		gScrollValue=-(gBG_Width-320)/2;
		VDP_loadTileSet(gfx_bgb2.tileset,gInd_tileset,DMA); 
		VDP_setTileMapEx(BG_B,gfx_bgb2.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,gInd_tileset),0,0,0,0,gBG_Width/8,28,DMA_QUEUE);
		VDP_setPalette(PAL0, gfx_bgb2.palette->data);
		gInd_tileset += gfx_bgb2.tileset->numTile;
		Cenario_Tilemap = unpackTileMap(gfx_bgb2.tilemap,NULL);
	} 
	
	//Load the tileset 'BGB3'
	if(gBG_Choice==3){ 
		gBG_Width = 512; // Largura do Cenario em pixels!
		gScrollValue=-(gBG_Width-320)/2;
		VDP_loadTileSet(gfx_bgb3.tileset,gInd_tileset,DMA); 
		VDP_setTileMapEx(BG_B,gfx_bgb3.tilemap,TILE_ATTR_FULL(PAL0,0,FALSE,FALSE,gInd_tileset),0,0,0,0,gBG_Width/8,28,DMA_QUEUE);
		VDP_setPalette(PAL0, gfx_bgb3.palette->data);
		gInd_tileset += gfx_bgb3.tileset->numTile;
		Cenario_Tilemap = unpackTileMap(gfx_bgb3.tilemap,NULL);
	} 
	
	//load palette HUD in PAL1, GFX load AFTER round intro...
	VDP_setPalette(PAL1, spr_n0.palette->data); 
	
	gAlturaPiso = (224/20)*18;
	
	for(i=1; i<=2; i++) { P[i].key_JOY_countdown[2]=0; P[i].key_JOY_countdown[4]=0; P[i].key_JOY_countdown[6]=0; P[i].key_JOY_countdown[8]=0; }
	
	//P1
	P[1].energia = 200;
	P[1].energiaBase = 200;
	P[1].energiaSP = 0;
	P[1].rageTimerCountdown=RAGETIMER;
	P[1].wins = 0;
	P[1].x = (320/4);
	P[1].y = gAlturaPiso;
	P[1].hitPause = 0;
	P[1].direcao = 1;
	P[1].state = 610; //610, Intro State!
	P[1].puloTimer = 0;
	P[1].dataMBox[0] = -BODYSPACE;
	P[1].dataMBox[1] = -60;
	P[1].dataMBox[2] = +BODYSPACE;
	P[1].dataMBox[3] = - 1;
	P[1].paleta = PAL2;
	P[1].cicloInteracoesGravidade = CIGD; //CIGD eh uma definicao global! ver inicio do codigo
	P[1].cicloInteracoesGravidadeCont = 0;
	P[1].fBallActive = 0;
	
	//P2
	P[2].energia = 200;
	P[2].energiaBase = 200;
	P[2].energiaSP = 0;
	P[2].rageTimerCountdown=RAGETIMER;
	P[2].wins = 0;
	P[2].x = (320/4)*3;
	P[2].y = gAlturaPiso;
	P[2].hitPause = 0;
	P[2].direcao = -1;
	P[2].state = 610; //610, Intro State!
	P[2].puloTimer = 0;
	P[2].dataMBox[0] = -BODYSPACE;
	P[2].dataMBox[1] = -60;
	P[2].dataMBox[2] = +BODYSPACE;
	P[2].dataMBox[3] = - 1;
	P[2].paleta = PAL3;
	P[2].cicloInteracoesGravidade = CIGD; //CIGD eh uma definicao global! ver inicio do codigo
	P[2].cicloInteracoesGravidadeCont = 0;
	P[2].fBallActive = 0;
	
	//P[1].sombra = SPR_addSpriteExSafe(&spr_sombra, P[1].x-P[1].axisX, P[1].y-2, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	//if(gSombraStyle==2)
	//{
	//	P[2].sombra = SPR_addSpriteExSafe(&spr_sombra, P[2].x-P[1].axisX, P[2].y-2, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	//}
	
	//reset Graphic Elements
	if (GE[ 1].sprite){ SPR_releaseSprite(GE[ 1].sprite); GE[ 1].sprite = NULL; }
	if (GE[ 2].sprite){ SPR_releaseSprite(GE[ 2].sprite); GE[ 2].sprite = NULL; }
	if (GE[ 3].sprite){ SPR_releaseSprite(GE[ 3].sprite); GE[ 3].sprite = NULL; }
	if (GE[ 4].sprite){ SPR_releaseSprite(GE[ 4].sprite); GE[ 4].sprite = NULL; }
	if (GE[ 5].sprite){ SPR_releaseSprite(GE[ 5].sprite); GE[ 5].sprite = NULL; }
	if (GE[ 6].sprite){ SPR_releaseSprite(GE[ 6].sprite); GE[ 6].sprite = NULL; }
	if (GE[ 7].sprite){ SPR_releaseSprite(GE[ 7].sprite); GE[ 7].sprite = NULL; }
	if (GE[ 8].sprite){ SPR_releaseSprite(GE[ 8].sprite); GE[ 8].sprite = NULL; }
	if (GE[ 9].sprite){ SPR_releaseSprite(GE[ 9].sprite); GE[ 9].sprite = NULL; }
	if (GE[10].sprite){ SPR_releaseSprite(GE[10].sprite); GE[10].sprite = NULL; }
	if (GE[11].sprite){ SPR_releaseSprite(GE[11].sprite); GE[11].sprite = NULL; }
	if (GE[12].sprite){ SPR_releaseSprite(GE[12].sprite); GE[12].sprite = NULL; }
	if (GE[13].sprite){ SPR_releaseSprite(GE[13].sprite); GE[13].sprite = NULL; }
	if (GE[14].sprite){ SPR_releaseSprite(GE[14].sprite); GE[14].sprite = NULL; }
	if (GE[15].sprite){ SPR_releaseSprite(GE[15].sprite); GE[15].sprite = NULL; }
	if (GE[16].sprite){ SPR_releaseSprite(GE[16].sprite); GE[16].sprite = NULL; }
	if (GE[17].sprite){ SPR_releaseSprite(GE[17].sprite); GE[17].sprite = NULL; }
	if (GE[18].sprite){ SPR_releaseSprite(GE[18].sprite); GE[18].sprite = NULL; }
	if (GE[19].sprite){ SPR_releaseSprite(GE[19].sprite); GE[19].sprite = NULL; }
	if (GE[20].sprite){ SPR_releaseSprite(GE[20].sprite); GE[20].sprite = NULL; }
	
	gClockTimer=60; //Relogio, timer de 1 seg
	gClockLTimer=9; //Digito esquerdo do Relogio
	gClockRTimer=9; //Digito direito do Relogio
	gRound=1;       //Round Number
	
	//Marcadores de Vitoria e Retratos da Interface ingame
	GE[12].sprite = SPR_addSpriteExSafe(&spr_icon_victory, 40, 32, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	if(GE[12].sprite != NULL)
	{
		SPR_setDepth(GE[12].sprite, 255 );
		SPR_setVRAMTileIndex(GE[12].sprite, 1530); //define uma posicao especifica para o GFX na VRAM
		SPR_setVisibility(GE[12].sprite, HIDDEN); 
	}
	else{
		KLog("GE[12].sprite == NULL");
	}
	GE[13].sprite = SPR_addSpriteExSafe(&spr_icon_victory, 64, 32, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	if(GE[13].sprite != NULL)
	{
		SPR_setDepth(GE[13].sprite, 255 );
		SPR_setVRAMTileIndex(GE[13].sprite, 1530); //define uma posicao especifica para o GFX na VRAM
		SPR_setVisibility(GE[13].sprite, HIDDEN); 
	}
	else{
		KLog("GE[13].sprite == NULL");
	}
	GE[14].sprite = SPR_addSpriteExSafe(&spr_icon_victory,240, 32, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	if(GE[14].sprite != NULL)
	{
		SPR_setDepth(GE[14].sprite, 255 );
		SPR_setVRAMTileIndex(GE[14].sprite, 1530); //define uma posicao especifica para o GFX na VRAM
		SPR_setVisibility(GE[14].sprite, HIDDEN);
	}
	else{
		KLog("GE[14].sprite == NULL");
	}
	GE[15].sprite = SPR_addSpriteExSafe(&spr_icon_victory,264, 32, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	if(GE[15].sprite != NULL)
	{
		SPR_setDepth(GE[15].sprite, 255 );
		SPR_setVRAMTileIndex(GE[15].sprite, 1530); //define uma posicao especifica para o GFX na VRAM
		SPR_setVisibility(GE[15].sprite, HIDDEN); 
	}
	else{
		KLog("GE[15].sprite == NULL");
	}
	
	//Retratos
	//RHEO, mudar aqui, conforme adicionar chars!
	if(GE[16].sprite == NULL)
	{
		if(P[1].id==1){ GE[16].sprite = SPR_addSpriteExSafe(&spr_terry_000, 24, 16, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		if(P[1].id==2){ GE[16].sprite = SPR_addSpriteExSafe(&spr_mai_000,  24, 16, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		if(P[1].id==3){ GE[16].sprite = SPR_addSpriteExSafe(&spr_bluemary_000,  24, 16, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		if(P[1].id==4){ GE[16].sprite = SPR_addSpriteExSafe(&spr_joe_000,  24, 16, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
	}
	else
	{
		KLog("GE[16].sprite != NULL");
	}
	
	//char4... rheo
	//char5... rheo
	//...
	if(GE[16].sprite  != NULL)
	{
		SPR_setDepth(GE[16].sprite, 255 );
		SPR_setVRAMTileIndex(GE[16].sprite, 1522); //define uma posicao especifica para o GFX na VRAM
		SPR_setVisibility(GE[16].sprite, HIDDEN); 
	}
	else
	{
		KLog("GE[16].sprite == NULL");
	}
	if(GE[17].sprite == NULL)
	{
		if(P[2].id==1){ GE[17].sprite = SPR_addSpriteExSafe(&spr_terry_000, 272, 16, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		if(P[2].id==2){ GE[17].sprite = SPR_addSpriteExSafe(&spr_mai_000,  272, 16, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		if(P[2].id==3){ GE[17].sprite = SPR_addSpriteExSafe(&spr_bluemary_000,  272, 16, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		if(P[2].id==4){ GE[17].sprite = SPR_addSpriteExSafe(&spr_joe_000,  272, 16, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
	}
	else
	{
		KLog("GE[17].sprite != NULL");
	}
	
	//char4... rheo
	//char5... rheo
	//...
	
	if(GE[17].sprite  != NULL)
	{
		SPR_setDepth(GE[17].sprite, 255 );
		if(P[1].id==P[2].id){ SPR_setVRAMTileIndex(GE[17].sprite, 1522); } //SE P1 e P2 IGUAIS, ENTAO... define uma posicao especifica para o GFX na VRAM
		SPR_setHFlip(GE[17].sprite, TRUE);
		SPR_setVisibility(GE[17].sprite, HIDDEN); 
	}
	else
	{
		KLog("GE[17].sprite == NULL");
	}
	
	if(ClockL == NULL)
	{
		ClockL = SPR_addSpriteEx(&spr_n9, 144, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	}
	else
	{
		KLog("ClockL != NULL");
	}
	if(ClockR == NULL)
	{
		ClockR = SPR_addSpriteEx(&spr_n9, 160, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	}
	else
	{
		KLog("ClockR != NULL");
	}
	if(ClockL != NULL && ClockR != NULL)
	{
		SPR_setVisibility(ClockL, HIDDEN); 
		SPR_setVisibility(ClockR, HIDDEN); 
		SPR_setVRAMTileIndex(ClockL,1703); //define uma posicao especifica para o GFX na VRAM
		SPR_setVRAMTileIndex(ClockR,1707); //define uma posicao especifica para o GFX na VRAM
	}
	else
	{
		KLog("ClockL == NULL || ClockR == NULL");
	}
	

	if(Rect1BB1_Q1 == NULL)
	{
		Rect1BB1_Q1 = SPR_addSpriteEx(&spr_rect_bb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	}
	else
	{
		KLog("Rect1BB1_Q1 != NULL");
	}
	if(Rect1BB1_Q2 == NULL)
	{
		Rect1BB1_Q2 = SPR_addSpriteEx(&spr_rect_bb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	}
	else
	{
		KLog("Rect1BB1_Q2 != NULL");
	}
	if(Rect1BB1_Q3 == NULL)
	{
		Rect1BB1_Q3 = SPR_addSpriteEx(&spr_rect_bb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	}
	else
	{
		KLog("Rect1BB1_Q3 != NULL");
	}
	if(Rect1BB1_Q4 == NULL)
	{
		Rect1BB1_Q4 = SPR_addSpriteEx(&spr_rect_bb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	}
	else
	{
		KLog("Rect1BB1_Q4 != NULL");
	}
	if(Rect1HB1_Q1 == NULL)
	{
		Rect1HB1_Q1 = SPR_addSpriteEx(&spr_rect_hb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	}
	else
	{
		KLog("Rect1HB1_Q1 != NULL");
	}
	if(Rect1HB1_Q2 == NULL)
	{
		Rect1HB1_Q2 = SPR_addSpriteEx(&spr_rect_hb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	}
	else
	{
		KLog("Rect1HB1_Q2 != NULL");
	}
	if(Rect1HB1_Q3 == NULL)
	{
		Rect1HB1_Q3 = SPR_addSpriteEx(&spr_rect_hb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	}
	else
	{
		KLog("Rect1HB1_Q3 != NULL");
	}
	if(Rect1HB1_Q4 == NULL)
	{
		Rect1HB1_Q4 = SPR_addSpriteEx(&spr_rect_hb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	}
	else{
		KLog("Rect1HB1_Q4 != NULL");
	}
	
	if(Rect2BB1_Q1 == NULL)
	{
		Rect2BB1_Q1 = SPR_addSpriteEx(&spr_rect_bb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	}
	else
	{
		KLog("Rect2BB1_Q1 != NULL");
	}
	if(Rect2BB1_Q2 == NULL)
	{
		Rect2BB1_Q2 = SPR_addSpriteEx(&spr_rect_bb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	}
	else
	{
		KLog("Rect2BB1_Q2 != NULL");
	}
	if(Rect2BB1_Q3 == NULL)
	{
		Rect2BB1_Q3 = SPR_addSpriteEx(&spr_rect_bb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	}
	else
	{
		KLog("Rect2BB1_Q3 != NULL");
	}
	if(Rect2BB1_Q4 == NULL)
	{
		Rect2BB1_Q4 = SPR_addSpriteEx(&spr_rect_bb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	}
	else
	{
		KLog("Rect2BB1_Q4 != NULL");
	}
	if(Rect2HB1_Q1 == NULL)
	{
		Rect2HB1_Q1 = SPR_addSpriteEx(&spr_rect_hb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	}
	else
	{
		KLog("Rect2HB1_Q1 != NULL");
	}
	if(Rect2HB1_Q2 == NULL)
	{
		Rect2HB1_Q2 = SPR_addSpriteEx(&spr_rect_hb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	}
	else
	{
		KLog("Rect2HB1_Q2 != NULL");
	}
	if(Rect2HB1_Q3 == NULL)
	{
		Rect2HB1_Q3 = SPR_addSpriteEx(&spr_rect_hb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	}
	else{
		KLog("Rect2HB1_Q3 != NULL");
	}
	if(Rect2HB1_Q4 == NULL)
	{
		Rect2HB1_Q4 = SPR_addSpriteEx(&spr_rect_hb, -8, -8, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	}
	else
	{
		KLog("Rect2HB1_Q4 != NULL");
	}
	
	if(Rect1BB1_Q2 != NULL)
	{
		SPR_setHFlip(Rect1BB1_Q2, TRUE);
	}
	else
	{
		KLog("Rect1BB1_Q2 == NULL");
	}
	if(Rect1BB1_Q3 != NULL)
	{
		SPR_setHFlip(Rect1BB1_Q3, TRUE);
	}
	else
	{
		KLog("Rect1BB1_Q3 == NULL");
	}
	if(Rect1BB1_Q4 != NULL)
	{
		SPR_setHFlip(Rect1BB1_Q4, TRUE);
		SPR_setVFlip(Rect1BB1_Q4, TRUE);
	}
	else
	{
		KLog("Rect1BB1_Q4 == NULL");
	}
	if(Rect1HB1_Q2 != NULL)
	{
		SPR_setHFlip(Rect1HB1_Q2, TRUE);
	}
	else
	{
		KLog("Rect1HB1_Q2 == NULL");
	}
	if(Rect1HB1_Q3 != NULL)
	{
		SPR_setVFlip(Rect1HB1_Q3, TRUE);
	}
	else
	{
		KLog("Rect1HB1_Q3 == NULL");
	}
	if(Rect1HB1_Q4 != NULL)
	{
		SPR_setHFlip(Rect1HB1_Q4, TRUE);
		SPR_setVFlip(Rect1HB1_Q4, TRUE);
	}
	else
	{
		KLog("Rect1HB1_Q4 == NULL");
	}
	
	if(Rect2BB1_Q2 != NULL)
	{
		SPR_setHFlip(Rect2BB1_Q2, TRUE);
	}
	else
	{
		KLog("Rect2BB1_Q2 == NULL");
	}
	if(Rect2BB1_Q3 != NULL)
	{
		SPR_setVFlip(Rect2BB1_Q3, TRUE);
	}
	else
	{
		KLog("Rect2BB1_Q3 == NULL");
	}
	if(Rect2BB1_Q4 != NULL)
	{
		SPR_setHFlip(Rect2BB1_Q4, TRUE);
		SPR_setVFlip(Rect2BB1_Q4, TRUE);
	}
	else
	{
		KLog("Rect2BB1_Q4 == NULL");
	}
	if(Rect2HB1_Q2 != NULL)
	{
		SPR_setHFlip(Rect2HB1_Q2, TRUE);
	}
	else
	{
		KLog("Rect2HB1_Q2 == NULL");
	}
	if(Rect2HB1_Q3 != NULL)
	{
		SPR_setVFlip(Rect2HB1_Q3, TRUE);
	}
	else
	{
		KLog("Rect2HB1_Q3 == NULL");
	}
	if(Rect2HB1_Q4 != NULL)
	{
		SPR_setHFlip(Rect2HB1_Q4, TRUE);
		SPR_setVFlip(Rect2HB1_Q4, TRUE);
	}
	else
	{
		KLog("Rect2HB1_Q4 == NULL");
	}

	if(Rect1BB1_Q1 != NULL)
	{
		SPR_setDepth(Rect1BB1_Q1, 1);
	}
	else
	{
		KLog("Rect1BB1_Q1 == NULL");
	}
	if(Rect1BB1_Q2 != NULL)
	{
		SPR_setDepth(Rect1BB1_Q2, 1);
	}
	else
	{
		KLog("Rect1BB1_Q2 == NULL");
	}
	if(Rect1BB1_Q3 != NULL)
	{
		SPR_setDepth(Rect1BB1_Q3, 1);
	}
	else
	{
		KLog("Rect1BB1_Q3 == NULL");
	}
	if(Rect1BB1_Q4 != NULL)
	{
		SPR_setDepth(Rect1BB1_Q4, 1);
	}
	else
	{
		KLog("Rect1BB1_Q4 == NULL");
	}
	if(Rect1HB1_Q1 != NULL)
	{
		SPR_setDepth(Rect1HB1_Q1, 1);
	}
	else
	{
		KLog("Rect1HB1_Q1 == NULL");
	}
	if(Rect1HB1_Q2 != NULL)
	{
		SPR_setDepth(Rect1HB1_Q2, 1);
	}
	else
	{
		KLog("Rect1HB1_Q2 == NULL");
	}
	if(Rect1HB1_Q3 != NULL)
	{
		SPR_setDepth(Rect1HB1_Q3, 1);
	}
	else
	{
		KLog("Rect1HB1_Q3 == NULL");
	}
	if(Rect1HB1_Q4 != NULL)
	{
		SPR_setDepth(Rect1HB1_Q4, 1);
	}
	else
	{
		KLog("Rect1HB1_Q4 == NULL");
	}
	
	if(Rect2BB1_Q1 != NULL)
	{
		SPR_setDepth(Rect2BB1_Q1, 1);
	}
	else
	{
		KLog("Rect2BB1_Q1 == NULL");
	}
	if(Rect2BB1_Q2 != NULL)
	{
		SPR_setDepth(Rect2BB1_Q2, 1);
	}
	else
	{
		KLog("Rect2BB1_Q2 == NULL");
	}
	if(Rect2BB1_Q3 != NULL)
	{
		SPR_setDepth(Rect2BB1_Q3, 1);
	}
	else
	{
		KLog("Rect2BB1_Q3 == NULL");
	}
	if(Rect2BB1_Q4 != NULL)
	{
		SPR_setDepth(Rect2BB1_Q4, 1);
	}
	else
	{
		KLog("Rect2BB1_Q4 == NULL");
	}
	if(Rect2HB1_Q1 != NULL)
	{
		SPR_setDepth(Rect2HB1_Q1, 1);
	}
	else
	{
		KLog("Rect2HB1_Q1 == NULL");
	}
	if(Rect2HB1_Q2 != NULL)
	{
		SPR_setDepth(Rect2HB1_Q2, 1);
	}
	else
	{
		KLog("Rect2HB1_Q2 == NULL");
	}
	if(Rect2HB1_Q3 != NULL)
	{
		SPR_setDepth(Rect2HB1_Q3, 1);
	}
	else
	{
		KLog("Rect2HB1_Q3 == NULL");
	}
	if(Rect2HB1_Q4 != NULL)
	{
		SPR_setDepth(Rect2HB1_Q4, 1);
	}
	else
	{
		KLog("Rect2HB1_Q4 == NULL");
	}
	
	
	//Intro State, Player Start
	if(P[1].sprite == NULL)
	{
		P[1].sprite = SPR_addSpriteExSafe(&spr_point, P[1].x-P[1].axisX, P[1].y-P[1].axisY, TILE_ATTR(PAL2, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	}
	else
	{
		KLog("P[1].sprite != NULL");
	}
	if(P[2].sprite == NULL)
	{
		P[2].sprite = SPR_addSpriteExSafe(&spr_point, P[2].x-P[2].axisX, P[2].y-P[2].axisY, TILE_ATTR(PAL3, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
	}
	else 
	{
		KLog("P[2].sprite != NULL");
	}
	
	//Se Player1 e Player2 com o mesmo personagem, desliguei a intro, pois elas somadas, usam muitos tiles /*samsho2*/
	if(P[1].id!=P[2].id)
	{
		PLAYER_STATE(1,610);
		PLAYER_STATE(2,610);
	}else{
		PLAYER_STATE(1,100);
		PLAYER_STATE(2,100);
	}
	
	if(P[2].sprite != NULL)
	{
		SPR_setHFlip(P[2].sprite, TRUE);
	}
	
	//Define a paleta dos players...
	P[1].palID=cursorP1ColorChoice;
	P[2].palID=cursorP2ColorChoice; 
	
	if( P[1].id==P[2].id && P[1].palID==P[2].palID)
	{
		//se ambos os players forem iguais e com a mesma paleta...
		P[1].palID=1; //Por padrao, P1 fica com a paleta 1
		P[2].palID=2; //Por padrao, P2 fica com a paleta 2 'cor alternativa'
	}
	
	if(P[1].id==1 && P[1].palID==1){ VDP_setPalette(PAL2, spr_terry_pal1.palette->data); } //terry
	if(P[1].id==1 && P[1].palID==2){ VDP_setPalette(PAL2, spr_terry_pal2.palette->data); } //terry
	if(P[1].id==1 && P[1].palID==3){ VDP_setPalette(PAL2, spr_terry_pal3.palette->data); } //terry
	if(P[1].id==1 && P[1].palID==4){ VDP_setPalette(PAL2, spr_terry_pal4.palette->data); } //terry
	if(P[1].id==1 && P[1].palID==5){ VDP_setPalette(PAL2, spr_terry_pal5.palette->data); } //terry
	if(P[1].id==1 && P[1].palID==6){ VDP_setPalette(PAL2, spr_terry_pal6.palette->data); } //terry
	if(P[1].id==1 && P[1].palID==7){ VDP_setPalette(PAL2, spr_terry_pal7.palette->data); } //terry
	if(P[1].id==1 && P[1].palID==8){ VDP_setPalette(PAL2, spr_terry_pal8.palette->data); } //terry
	
	if(P[1].id==2 && P[1].palID==1){ VDP_setPalette(PAL2, spr_mai_pal1.palette->data); } //mai
	if(P[1].id==2 && P[1].palID==2){ VDP_setPalette(PAL2, spr_mai_pal2.palette->data); } //mai
	if(P[1].id==2 && P[1].palID==3){ VDP_setPalette(PAL2, spr_mai_pal3.palette->data); } //mai
	if(P[1].id==2 && P[1].palID==4){ VDP_setPalette(PAL2, spr_mai_pal4.palette->data); } //mai
	if(P[1].id==2 && P[1].palID==5){ VDP_setPalette(PAL2, spr_mai_pal5.palette->data); } //mai
	if(P[1].id==2 && P[1].palID==6){ VDP_setPalette(PAL2, spr_mai_pal6.palette->data); } //mai
	if(P[1].id==2 && P[1].palID==7){ VDP_setPalette(PAL2, spr_mai_pal7.palette->data); } //mai
	if(P[1].id==2 && P[1].palID==8){ VDP_setPalette(PAL2, spr_mai_pal8.palette->data); } //mai
	
	if(P[1].id==3 && P[1].palID==1){ VDP_setPalette(PAL2, spr_bluemary_pal1.palette->data); } //bluemary
	if(P[1].id==3 && P[1].palID==2){ VDP_setPalette(PAL2, spr_bluemary_pal2.palette->data); } //bluemary
	if(P[1].id==3 && P[1].palID==3){ VDP_setPalette(PAL2, spr_bluemary_pal3.palette->data); } //bluemary
	if(P[1].id==3 && P[1].palID==4){ VDP_setPalette(PAL2, spr_bluemary_pal4.palette->data); } //bluemary
	if(P[1].id==3 && P[1].palID==5){ VDP_setPalette(PAL2, spr_bluemary_pal5.palette->data); } //bluemary
	if(P[1].id==3 && P[1].palID==6){ VDP_setPalette(PAL2, spr_bluemary_pal6.palette->data); } //bluemary
	if(P[1].id==3 && P[1].palID==7){ VDP_setPalette(PAL2, spr_bluemary_pal7.palette->data); } //bluemary
	if(P[1].id==3 && P[1].palID==8){ VDP_setPalette(PAL2, spr_bluemary_pal8.palette->data); } //bluemary

	if(P[1].id==4 && P[1].palID==1){ VDP_setPalette(PAL2, spr_joe_pal1.palette->data); } // joe
	if(P[1].id==4 && P[1].palID==2){ VDP_setPalette(PAL2, spr_joe_pal2.palette->data); } // joe
	if(P[1].id==4 && P[1].palID==3){ VDP_setPalette(PAL2, spr_joe_pal3.palette->data); } // joe
	if(P[1].id==4 && P[1].palID==4){ VDP_setPalette(PAL2, spr_joe_pal4.palette->data); } // joe
	if(P[1].id==4 && P[1].palID==5){ VDP_setPalette(PAL2, spr_joe_pal5.palette->data); } // joe
	if(P[1].id==4 && P[1].palID==6){ VDP_setPalette(PAL2, spr_joe_pal6.palette->data); } // joe
	if(P[1].id==4 && P[1].palID==7){ VDP_setPalette(PAL2, spr_joe_pal7.palette->data); } // joe
	if(P[1].id==4 && P[1].palID==8){ VDP_setPalette(PAL2, spr_joe_pal8.palette->data); } // joe
	
	if(P[2].id==1 && P[2].palID==1){ VDP_setPalette(PAL3, spr_terry_pal1.palette->data); } //terry
	if(P[2].id==1 && P[2].palID==2){ VDP_setPalette(PAL3, spr_terry_pal2.palette->data); } //terry
	if(P[2].id==1 && P[2].palID==3){ VDP_setPalette(PAL3, spr_terry_pal3.palette->data); } //terry
	if(P[2].id==1 && P[2].palID==4){ VDP_setPalette(PAL3, spr_terry_pal4.palette->data); } //terry
	if(P[2].id==1 && P[2].palID==5){ VDP_setPalette(PAL3, spr_terry_pal5.palette->data); } //terry
	if(P[2].id==1 && P[2].palID==6){ VDP_setPalette(PAL3, spr_terry_pal6.palette->data); } //terry
	if(P[2].id==1 && P[2].palID==7){ VDP_setPalette(PAL3, spr_terry_pal7.palette->data); } //terry
	if(P[2].id==1 && P[2].palID==8){ VDP_setPalette(PAL3, spr_terry_pal8.palette->data); } //terry
	
	if(P[2].id==2 && P[2].palID==1){ VDP_setPalette(PAL3, spr_mai_pal1.palette->data); } //mai
	if(P[2].id==2 && P[2].palID==2){ VDP_setPalette(PAL3, spr_mai_pal2.palette->data); } //mai
	if(P[2].id==2 && P[2].palID==3){ VDP_setPalette(PAL3, spr_mai_pal3.palette->data); } //mai
	if(P[2].id==2 && P[2].palID==4){ VDP_setPalette(PAL3, spr_mai_pal4.palette->data); } //mai
	if(P[2].id==2 && P[2].palID==5){ VDP_setPalette(PAL3, spr_mai_pal5.palette->data); } //mai
	if(P[2].id==2 && P[2].palID==6){ VDP_setPalette(PAL3, spr_mai_pal6.palette->data); } //mai
	if(P[2].id==2 && P[2].palID==7){ VDP_setPalette(PAL3, spr_mai_pal7.palette->data); } //mai
	if(P[2].id==2 && P[2].palID==8){ VDP_setPalette(PAL3, spr_mai_pal8.palette->data); } //mai
	
	if(P[2].id==3 && P[2].palID==1){ VDP_setPalette(PAL3, spr_bluemary_pal1.palette->data); } //bluemary
	if(P[2].id==3 && P[2].palID==2){ VDP_setPalette(PAL3, spr_bluemary_pal2.palette->data); } //bluemary
	if(P[2].id==3 && P[2].palID==3){ VDP_setPalette(PAL3, spr_bluemary_pal3.palette->data); } //bluemary
	if(P[2].id==3 && P[2].palID==4){ VDP_setPalette(PAL3, spr_bluemary_pal4.palette->data); } //bluemary
	if(P[2].id==3 && P[2].palID==5){ VDP_setPalette(PAL3, spr_bluemary_pal5.palette->data); } //bluemary
	if(P[2].id==3 && P[2].palID==6){ VDP_setPalette(PAL3, spr_bluemary_pal6.palette->data); } //bluemary
	if(P[2].id==3 && P[2].palID==7){ VDP_setPalette(PAL3, spr_bluemary_pal7.palette->data); } //bluemary
	if(P[2].id==3 && P[2].palID==8){ VDP_setPalette(PAL3, spr_bluemary_pal8.palette->data); } //bluemary
	
	if(P[1].id==4 && P[2].palID==1){ VDP_setPalette(PAL2, spr_joe_pal1.palette->data); } // joe
	if(P[1].id==4 && P[2].palID==2){ VDP_setPalette(PAL2, spr_joe_pal2.palette->data); } // joe
	if(P[1].id==4 && P[2].palID==3){ VDP_setPalette(PAL2, spr_joe_pal3.palette->data); } // joe
	if(P[1].id==4 && P[2].palID==4){ VDP_setPalette(PAL2, spr_joe_pal4.palette->data); } // joe
	if(P[1].id==4 && P[2].palID==5){ VDP_setPalette(PAL2, spr_joe_pal5.palette->data); } // joe
	if(P[1].id==4 && P[2].palID==6){ VDP_setPalette(PAL2, spr_joe_pal6.palette->data); } // joe
	if(P[1].id==4 && P[2].palID==7){ VDP_setPalette(PAL2, spr_joe_pal7.palette->data); } // joe
	if(P[1].id==4 && P[2].palID==8){ VDP_setPalette(PAL2, spr_joe_pal8.palette->data); } // joe

	//AXIS
	if (GE[1].sprite){ SPR_releaseSprite(GE[1].sprite); GE[1].sprite = NULL; }
	if (GE[2].sprite){ SPR_releaseSprite(GE[2].sprite); GE[2].sprite = NULL; }
	if(GE[1].sprite != NULL)
	{
		GE[1].sprite = SPR_addSpriteExSafe(&spr_point, P[1].x-4, P[1].y-5, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		SPR_setVisibility(GE[1].sprite, HIDDEN);
		SPR_setDepth(GE[1].sprite, 1 );
	}
	else
	{
		KLog("Error: GE[1].sprite is NULL");
	}
	if(GE[2].sprite != NULL)
	{
		GE[2].sprite = SPR_addSpriteExSafe(&spr_point, P[2].x-4, P[2].y-5, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		SPR_setVisibility(GE[2].sprite, HIDDEN);
		SPR_setDepth(GE[2].sprite, 2 );
	}
	else
	{
		KLog("Error: GE[2].sprite is NULL");
	}
	
	
	//depth 3 e 4 reservados
	if(P[1].sprite != NULL)
	{
		SPR_setDepth(P[1].sprite,  5 );
	}
	else
	{
		KLog("Error: P[1].sprite is NULL");
	}
	if(P[2].sprite != NULL)
	{
		SPR_setDepth(P[2].sprite,  6 );
	}
	else
	{
		KLog("Error: P[2].sprite is NULL");
	}
	//depth 7 e 8 reservados
	if(P[1].sombra != NULL)
	{
		SPR_setDepth(P[1].sombra, 98 );
	}
	else
	{
		KLog("Error: P[1].sombra is NULL");
	}
	if(P[2].sombra != NULL)
	{
		if(gSombraStyle==2){ SPR_setDepth(P[2].sombra, 99 ); }
	}
	else
	{
		KLog("Error: P[2].sombra is NULL");
	}
}

void FUNCAO_ROUND_INIT()
{	
	if(gFrames==1)
	 {
		if(ClockL != NULL)
		{
			SPR_setVisibility(ClockL, VISIBLE); //reativado apos a intro...
		}
		else{
			KLog("ClockL NULL");
		}

		if(ClockR != NULL)
		{
			SPR_setVisibility(ClockR, VISIBLE);	
		}
		else
		{
			KLog("ClockR NULL");
		}
		
		
		//BG_A
		VDP_loadTileSet(gfx_bga.tileset,gInd_tileset,DMA); //Load the tileset
		VDP_setTileMapEx(BG_A,gfx_bga.tilemap,TILE_ATTR_FULL(PAL1,1,FALSE,FALSE,gInd_tileset),0,0,0,0,40,28,DMA_QUEUE);
		VDP_setPalette(PAL1, gfx_bga.palette->data);
		gInd_tileset += gfx_bga.tileset->numTile;
		
		//Barras de Energia
		if(gRound==1)
		{
			if (GE[3].sprite){ SPR_releaseSprite(GE[3].sprite); GE[3].sprite = NULL; }
			if (GE[4].sprite){ SPR_releaseSprite(GE[4].sprite); GE[4].sprite = NULL; }
			GE[3].sprite = SPR_addSpriteExSafe(&spr_bar_energy,  48, 20, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
			GE[4].sprite = SPR_addSpriteExSafe(&spr_bar_energy, 176, 20, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
			
			if(GE[3].sprite != NULL)
			{
				SPR_setVRAMTileIndex(GE[3].sprite,1648); //define uma posicao especifica para o GFX na VRAM
				SPR_setHFlip(GE[3].sprite, TRUE);
				SPR_setDepth(GE[3].sprite, 2 ); // Barra de energia P1
			}
			else
			{
				KLog("GE[3].sprite NULL");
			}
			if(GE[4].sprite != NULL)
			{
				SPR_setVRAMTileIndex(GE[4].sprite,1664); //define uma posicao especifica para o GFX na VRAM
				SPR_setDepth(GE[4].sprite, 2 ); // Barra de energia P2
			}
			else
			{
				KLog("GE[4].sprite NULL");
			}
			
			//Retratos
			if(GE[16].sprite != NULL)
			{
				SPR_setVisibility(GE[16].sprite, VISIBLE); 
			}
			else
			{
				KLog("GE[16].sprite NULL");
			}
			if(GE[17].sprite != NULL)
			{
				SPR_setVisibility(GE[17].sprite, VISIBLE); 
			}
			else
			{
				KLog("GE[17].sprite NULL");
			}
		}
		
		if (GE[5].sprite){ SPR_releaseSprite(GE[5].sprite); GE[5].sprite = NULL; }
		if (GE[6].sprite){ SPR_releaseSprite(GE[6].sprite); GE[6].sprite = NULL; }
		GE[5].sprite = SPR_addSpriteExSafe(&spr_bar_energy_sub,   48, 20, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		GE[6].sprite = SPR_addSpriteExSafe(&spr_bar_energy_sub, 176, 20, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		if(GE[5].sprite != NULL)
		{
			SPR_setVRAMTileIndex(GE[5].sprite,1681); //define uma posicao especifica para o GFX na VRAM
			SPR_setVisibility(GE[5].sprite, HIDDEN); 
			SPR_setHFlip(GE[5].sprite, TRUE);
			SPR_setDepth(GE[5].sprite, 1 ); // Barra de energia (complemento) P1
		}
		else
		{
			KLog("GE[5].sprite NULL");
		}
		if(GE[6].sprite != NULL)
		{
			SPR_setVRAMTileIndex(GE[6].sprite,1682); //define uma posicao especifica para o GFX na VRAM
			SPR_setVisibility(GE[6].sprite, HIDDEN);
			SPR_setDepth(GE[6].sprite, 1 ); // Barra de energia (complemento) P2
		}
		else
		{
			KLog("GE[6].sprite NULL");
		}
		
		
		if (GE[7].sprite){ SPR_releaseSprite(GE[7].sprite); GE[7].sprite = NULL; }
		if (GE[8].sprite){ SPR_releaseSprite(GE[8].sprite); GE[8].sprite = NULL; }
		GE[7].sprite = SPR_addSpriteExSafe(&spr_bar_sp,  48, 208, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		GE[8].sprite = SPR_addSpriteExSafe(&spr_bar_sp, 208, 208, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		if(GE[7].sprite != NULL)
		{
			SPR_setVRAMTileIndex(GE[7].sprite,1683); //define uma posicao especifica para o GFX na VRAM
			SPR_setDepth(GE[7].sprite, 1 ); // Barra de energia especial P1
		}
		else
		{
			KLog("GE[7].sprite NULL");
		}
		if(GE[8].sprite != NULL)
		{
			SPR_setVRAMTileIndex(GE[8].sprite,1683+8); //define uma posicao especifica para o GFX na VRAM
			SPR_setHFlip(GE[8].sprite, TRUE);
			SPR_setDepth(GE[8].sprite, 1 ); // Barra de energia especial P2
		}
		else
		{
			KLog("GE[8].sprite NULL");
		}
		
		if (GE[9].sprite){ SPR_releaseSprite(GE[9].sprite); GE[9].sprite = NULL; }
		if (GE[10].sprite){ SPR_releaseSprite(GE[10].sprite); GE[10].sprite = NULL; }
		//GE[9].sprite  = SPR_addSpriteExSafe(&spr_pow,  22, 192, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		//GE[10].sprite = SPR_addSpriteExSafe(&spr_pow, 266, 192, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		//SPR_setVRAMTileIndex(GE[9].sprite ,1441); //define uma posicao especifica para o GFX na VRAM 
		//SPR_setVRAMTileIndex(GE[10].sprite,1441); //define uma posicao especifica para o GFX na VRAM *1699
		
		
		
		
		
		//POW icons invisiveis (barra de SP vazia)
		//SPR_setVisibility(GE[ 9].sprite, HIDDEN); 
		//SPR_setVisibility(GE[10].sprite, HIDDEN);
		
		//DEPTH
		
		
		
		
		// SPR_setDepth(GE[9].sprite, 1 ); // POW icon P1
		// SPR_setDepth(GE[10].sprite,1 ); // POW icon P2
		
	 }
	/*samsho2*/ //-Intro Rotine -BEGIN-//
	//  if(gFrames== 3){ 
		 
		
	// 	//clear sprite
	// 	//if (HUD_Lethers){ SPR_releaseSprite(HUD_Lethers); HUD_Lethers = NULL; }
		
	// 	//Gairyu Isle Evening
	// 	//if(gBG_Choice==1){ HUD_Lethers = SPR_addSpriteEx(&spr_BG_desc1, 72, 48, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); SPR_setDepth(HUD_Lethers, 1 ); }
	// 	//Gairyu Isle Night
	// 	//if(gBG_Choice==2){ HUD_Lethers = SPR_addSpriteEx(&spr_BG_desc2, 72, 48, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); SPR_setDepth(HUD_Lethers, 1 ); }
	// 	//Himalaya Top of The Montain
	// 	//if(gBG_Choice==3){ HUD_Lethers = SPR_addSpriteEx(&spr_BG_desc3, 72, 48, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); SPR_setDepth(HUD_Lethers, 1 ); }
	//  }
	 //if(gFrames== 63){ SPR_releaseSprite(HUD_Lethers); }
	 
	 //start SFX
	 // if(gFrames== 64 && gRound==1)
	 //{ 
	 	//XGM_setPCM(INGAME_SFX, snd_round_init, sizeof(snd_round_init)); XGM_startPlayPCM(INGAME_SFX, 1, SOUND_PCM_CH3); 
	 //}
	 
	 if(gFrames== 65)
	{ 
		if( HUD_Lethers == NULL)
		{
			if(gRound==1){HUD_Lethers = SPR_addSpriteExSafe(&spr_engarde,   72, 64, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(gRound==2){HUD_Lethers = SPR_addSpriteExSafe(&spr_engarde2,  72, 64, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(gRound==3){HUD_Lethers = SPR_addSpriteExSafe(&spr_engarde2,  72, 64, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(gRound==4){HUD_Lethers = SPR_addSpriteExSafe(&spr_engarde2,  72, 64, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
			if(gRound==5){HUD_Lethers = SPR_addSpriteExSafe(&spr_engarde2,  72, 64, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
		}
		else
		{
			KLog("HUD_Lethers != NULL");
		}
	
	}
	if(HUD_Lethers != NULL)
	{

		if(gFrames== 67){ SPR_setAnimAndFrame(HUD_Lethers, 0, 1); }
		if(gFrames== 70){ SPR_setAnimAndFrame(HUD_Lethers, 0, 2); }
		if(gFrames== 73){ SPR_setAnimAndFrame(HUD_Lethers, 0, 3); }
		if(gFrames== 76){ SPR_setAnimAndFrame(HUD_Lethers, 0, 4); }
		if(gFrames== 79){ SPR_setAnimAndFrame(HUD_Lethers, 0, 5); }
		if(gFrames== 82){ SPR_setAnimAndFrame(HUD_Lethers, 0, 6); }
		if(gFrames== 85){ SPR_setAnimAndFrame(HUD_Lethers, 0, 7); }
		if(gFrames== 88){ SPR_setAnimAndFrame(HUD_Lethers, 0, 8); }
		if(gFrames== 91){ SPR_setAnimAndFrame(HUD_Lethers, 0, 9); }
		if(gFrames== 94){ SPR_setAnimAndFrame(HUD_Lethers, 0,10); }
		if(gFrames== 97){ SPR_setAnimAndFrame(HUD_Lethers, 0,11); }
		if(gFrames==150){ SPR_setAnimAndFrame(HUD_Lethers, 0,12); }
		if(gFrames==153){ SPR_setAnimAndFrame(HUD_Lethers, 0,13); }
		if(gFrames==156){ SPR_setAnimAndFrame(HUD_Lethers, 0,14); }
		if(gFrames==159){ SPR_setAnimAndFrame(HUD_Lethers, 0,15); }
		if(gFrames==162){ SPR_setAnimAndFrame(HUD_Lethers, 0,16); }
		//if(gFrames==165){ SPR_setAnimAndFrame(HUD_Lethers, 0,17); }
		if(gFrames==168){ SPR_releaseSprite(HUD_Lethers); HUD_Lethers = NULL;}
	}

	 //if(gFrames==193)
	// { 
	//	if(gRound==1){ HUD_Lethers = SPR_addSpriteExSafe(&spr_duel1, 112, 64, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
	//	if(gRound==2){ HUD_Lethers = SPR_addSpriteExSafe(&spr_duel2, 112, 64, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
	//	if(gRound==3){ HUD_Lethers = SPR_addSpriteExSafe(&spr_duel3, 112, 64, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
	//	if(gRound==4){ HUD_Lethers = SPR_addSpriteExSafe(&spr_duel4, 112, 64, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
	//	if(gRound==5){ HUD_Lethers = SPR_addSpriteExSafe(&spr_duel5, 112, 64, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
	// }
	if(HUD_Lethers == NULL)
	{

	 	if(gFrames==180){ HUD_Lethers = SPR_addSpriteExSafe(&spr_begin, 108, 64, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); }
	}

	 if(HUD_Lethers != NULL)
	 {
		if(gFrames==183){ SPR_setAnimAndFrame(HUD_Lethers, 0, 1); }
		if(gFrames==186){ SPR_setAnimAndFrame(HUD_Lethers, 0, 2); }
		if(gFrames==189){ SPR_setAnimAndFrame(HUD_Lethers, 0, 3); }
		if(gFrames==192){ SPR_setAnimAndFrame(HUD_Lethers, 0, 4); }
		if(gFrames==195){ SPR_setAnimAndFrame(HUD_Lethers, 0, 5); }
		//if(gFrames==240){ SPR_setAnimAndFrame(HUD_Lethers, 0, 6); }
		if(gFrames==245){ SPR_releaseSprite(HUD_Lethers); HUD_Lethers = NULL; }
	 }

	 
	if(gFrames==300 && gRound==1)
	 { 
		//comeca a tocar a musica
		XGM_startPlay(music_stage1); XGM_isPlaying(); //FIX
	 }
	 
	 
	 
	 if(gFrames==355){ gPodeMover=1; }
	/*samsho2*/ //-Intro Rotine -END-//
}
	

void FUNCAO_ROUND_RESTART()
{
	if(P[1].wins==2 || P[2].wins==2)
	{
		//A LUTA ACABOU!
		
		//Atualiza Winner e Lose ID's usadas na tela de WINNER!
		if(P[1].wins==2){ gWinnerID=P[1].id; gLoseID=P[2].id; }
		if(P[2].wins==2){ gWinnerID=P[2].id; gLoseID=P[1].id; }

		gRoom=9; gDescompressionExit=11;
		gFrames=1;
		CLEAR_VDP();
	}else{
		//A LUTA NAO ACABOU, + 1 ROUND!
		gPodeMover=0;
		gRound++;
		gFrames = 64; //reinicia o round apos os dizeres com o nome do cenario /*samsho2*/
		gInd_tileset -= gfx_bga.tileset->numTile;
		gScrollValue=-(gBG_Width-320)/2;
		gClockTimer = 60;
		gClockLTimer = 9;
		gClockRTimer = 9;
		SPR_releaseSprite(ClockL);
		SPR_releaseSprite(ClockR);
		ClockL = SPR_addSpriteEx(&spr_n9, 144, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC);
		ClockR = SPR_addSpriteEx(&spr_n9, 160, 16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), 1, SPR_FLAG_DISABLE_DELAYED_FRAME_UPDATE | SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | SPR_FLAG_AUTO_SPRITE_ALLOC); 
		SPR_setVRAMTileIndex(ClockL,1703); //define uma posicao especifica para o GFX na VRAM
		SPR_setVRAMTileIndex(ClockR,1707); //define uma posicao especifica para o GFX na VRAM
		P[1].energia = 200; P[1].energiaBase = 200;
		P[2].energia = 200; P[2].energiaBase = 200;
		P[1].x = (320/4);
		P[2].x = (320/4)*3;
		P[1].direcao = 1;
		P[2].direcao = -1;
		PLAYER_STATE(1,100);
		PLAYER_STATE(2,100);
		
		//barras de energia cheias ao reiniciar o round
			//SPR_releaseSprite(GE[3].sprite); GE[3].sprite = NULL;
			//SPR_releaseSprite(GE[4].sprite); GE[4].sprite = NULL;
		SPR_setAnimAndFrame(GE[3].sprite, 0, 0);
		SPR_setAnimAndFrame(GE[4].sprite, 0, 0);
		
		SPR_releaseSprite(GE[5].sprite); GE[5].sprite = NULL;
		SPR_releaseSprite(GE[6].sprite); GE[6].sprite = NULL;
		SPR_releaseSprite(GE[7].sprite); GE[7].sprite = NULL;
		SPR_releaseSprite(GE[8].sprite); GE[8].sprite = NULL;
		SPR_releaseSprite(GE[9].sprite); GE[9].sprite = NULL;
		SPR_releaseSprite(GE[10].sprite); GE[10].sprite = NULL;
		
		FUNCAO_ROUND_INIT();
	}
	
}

void CLEAR_VDP()
{
	SYS_disableInts();
	 SPR_reset();
	 //VDP_resetSprites();
	 //VDP_releaseAllSprites();
	 //SPR_defragVRAM();
	 VDP_clearPlane(BG_A, TRUE);
	 VDP_clearPlane(BG_B, TRUE);	
	 VDP_setPaletteColors(0, (u16*) palette_black, 64);
	 VDP_setBackgroundColor(0);
	 //VDP_resetScreen();
	SYS_enableInts();
	gInd_tileset=0;
}

//EOF - END OF FILE; by GAMEDEVBOSS 2015-2022