//
//
//  main.cpp
//

#include <cassert>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <utility>
#include <sstream>
#include <iomanip>

#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/ObjModel.h"
#include "ObjLibrary/DisplayList.h"
#include "ObjLibrary/SpriteFont.h"
#include "ObjLibrary/TextureManager.h"

#include "GetGlut.h"
#include "Sleep.h"

#include "Angles.h"
#include "BlockMap.h"
#include "PerlinNoiseField.h"
#include "Heightmap.h"
#include "SnowMap.h"
#include "Map.h"
#include "Neighbourhood.h"
#include "Player.h"
#include "BallController.h"
#include "TimeManager.h"


using namespace std;
using namespace ObjLibrary;

// prototypes of records;
struct HitBox;
struct FireBall;

void init();
void initDisplay();
void initModels();
void initOpponent();
void initDebugging();
void initHitBox();
void initPlayerHUDInfo();
void printBoxCount();
void printNoiseField();
void printEdgeGrid();
Heightmap calculateNoiseGrid(const PerlinNoiseField& field,
	int base_x, int base_z);
Heightmap calculateEdgeGrid(const Neighbourhood& neighbourhood,
	int desired_value);

void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
unsigned char fixKey(unsigned char key);
void special(int special_key, int x, int y);
void specialUp(int special_key, int x, int y);

void update();
void updateForKeyboard();
void updateSnowBall();
void updateOpponent();
void updateMoveMentToFlase();
void updatePlayerVelocityAndCenterPosition(Player& player, HitBox& hit_box);
void resetPlayerInfo();

void reshape(int w, int h);
void display();
void drawSkybox(const Vector3& eye);
void drawAxes();
void drawMap();
void drawPlayer();
void drawGuard();
void drawDebuggingCube();
void drawDebuggingHeights(const Heightmap& grid,
	double min_value, double max_value);
void drawHitbox();
void updatethrowSnowBalls();
void drawSnowBall();
Vector3 getSnowBallCenter(int n);
void drawSnowBall(FireBall& fire_ball);
// This assignments functions Prototypes
void groundPushBack(const HitBox& ground, HitBox& player);
bool isCollision(const HitBox& first, const HitBox& second);
bool isCollision(HitBox& player, FireBall& fire_ball);
void collisionResolveFireBall(Player& player, FireBall& fire_ball);
bool didBallHit();
void pushBack(const HitBox& fixed, HitBox& Flex);
void checkCollision();
// Heads Up display
void drawHUDdisplay();
void drawFireBallCurvePath();
// This function takes input of player you want to check snow beneath it; but in form of HitBox;
bool isThereSnow(HitBox& hit_box);
int isThereSupportBelow(Vector3& cordinates);

const unsigned int KEY_UP_ARROW = 256;
const unsigned int KEY_DOWN_ARROW = 257;
const unsigned int KEY_LEFT_ARROW = 258;
const unsigned int KEY_RIGHT_ARROW = 259;
const unsigned int KEY_COUNT = 260;
const unsigned int KEY_SPACE_BAR = 32;
bool key_pressed[KEY_COUNT];

// Collision Detection Boolean functions for keys;
// Forward And backward direction keys;
bool W_KEY = false;
bool S_KEY = false;
// Right and left direction keys;
bool D_KEY = false;
bool A_KEY = false;
// Flag to Check if Player Moved;
bool did_player_move = false;
bool is_display_rates = false;
bool is_game_paused = false;
bool is_y_pressed = false;
bool is_u_pressed = false;
bool is_g_pressed = false;

const string RESOURCE_DIRECTORY = "../Resources/";

DisplayList skybox_list;
DisplayList ground_list;
DisplayList block_list;
DisplayList player_list;
DisplayList guard_list;
DisplayList snowball_list;

Map map;
Player player;
Player guard;

// Friction Everywhere on the ground or on crate;
const float friction = 0.05f;

//Player speed and rotation degree variables
const double SPEED_RATE = 0.1;
const double ROTATION_RATE = 5.0;
double speed_rate = SPEED_RATE;
double rotation_rate = ROTATION_RATE;

const double GRAVITY = 9.8;

int window_width = 1024;
int window_height = 768;
// SpriteFont function to get some text;
SpriteFont font;

HUDInfo PlayerInfo;

// For frameRate and Calculation of Physics every variable would be here;
TimeManager frame_update_time;

const int SNOW_RADIUS_MIN = 1;
const int SNOW_RADIUS_DEFAULT = 5;
const int SNOW_RADIUS_MAX = 10;
int snow_radius = SNOW_RADIUS_DEFAULT;

const unsigned int CUBE_SHOW_LARGE = 0;
const unsigned int CUBE_SHOW_SMALL = 1;
const unsigned int CUBE_SHOW_HIGHER = 2;
const unsigned int CUBE_SHOW_LOWER = 3;
const unsigned int CUBE_SHOW_COUNT = 4;

bool IS_DEBUGGING_CUBE_WIREFORM = false;

bool is_debugging_cube;
unsigned int debugging_cube_show;
// The collision box or Hitbox;
struct HitBox
{
	Vector3 center;
	Vector3 half_size;
	bool is_colliding = false;
	bool is_hitbox = false;
	unsigned int stack = 0;
};

struct FireBall
{
	Vector3 center;
	Vector3 half_size;
	Vector3 velocity;
	bool is_collided = false;
	bool is_out_of_hand = false;
};

HitBox hit_box;
HitBox guard_box;
HitBox ground_box;
FireBall fire_ball_1;
FireBall fire_ball_2;
FireBall fire_ball_3;
FireBall fire_ball_4;
vector<FireBall> fire_ball;
bool isMapHitBox_show = false;
vector<vector<HitBox>> MapHitBoxes;


int main(int argc, char* argv[])
{
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(0, 0);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutCreateWindow("SnowGame");
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(special);
	glutSpecialUpFunc(specialUp);
	glutIdleFunc(update);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);

	init();

	font.load(RESOURCE_DIRECTORY + "Font.bmp");

	glutMainLoop();  // contains an infinite loop, so it never returns
	return 1;
}

void init()
{
	initDisplay();
	initModels();
	initDebugging();
	initPlayerHUDInfo();
	frame_update_time = TimeManager(60, 10);
	//map = Map(RESOURCE_DIRECTORY + "map_example.txt",
	//map = Map(RESOURCE_DIRECTORY + "map_castle.txt",
	map = Map(RESOURCE_DIRECTORY + "map_plaza.txt",
		block_list, RESOURCE_DIRECTORY + "snow1.bmp");
	player.reset(map.getPlayerStart(PLAYER_HALF_HEIGHT));
	guard.reset(map.getOpponentStart(PLAYER_HALF_HEIGHT));

	initHitBox();
	printBoxCount();
	printNoiseField();
	printEdgeGrid();
}

void initDisplay()
{
	glClearColor(0.5, 0.5, 0.5, 0.0);
	glColor3f(0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glutPostRedisplay();
}

void initModels()
{
	skybox_list = ObjModel(RESOURCE_DIRECTORY + "Skybox.obj").getDisplayList();
	ground_list = ObjModel(RESOURCE_DIRECTORY + "ground.obj").getDisplayList();
	block_list = ObjModel(RESOURCE_DIRECTORY + "crate.obj").getDisplayList();
	//block_list  = ObjModel(RESOURCE_DIRECTORY + "weighted_cube.obj").getDisplayList();
	player_list = ObjModel(RESOURCE_DIRECTORY + "cbabe_stand.obj").getDisplayList();
	guard_list = ObjModel(RESOURCE_DIRECTORY + "guard_stand.obj").getDisplayList();
	//player_list = ObjModel(RESOURCE_DIRECTORY + "guard_stand.obj").getDisplayList();
	snowball_list = ObjModel(RESOURCE_DIRECTORY + "snowball.obj").getDisplayList();
}

void initOpponent()
{
	Vector3 temp = Vector3::ZERO;
	guard.setVelocity(temp);

}

void initDebugging()
{
	is_debugging_cube = false;
	debugging_cube_show = CUBE_SHOW_LARGE;
}

void initHitBox()
{
	// For Player hitbox
	hit_box.is_colliding = false;
	hit_box.is_hitbox = false;
	hit_box.center = player.getPosition();
	hit_box.half_size = Vector3(0.25, 0.8, 0.25);

	guard_box.is_colliding = false;
	guard_box.is_hitbox = false;
	guard_box.center = guard.getPosition();
	guard_box.half_size = Vector3(0.25, 0.8, 0.25);

	MapHitBoxes.resize(map.getSizeX(), vector<HitBox>(map.getSizeZ()));
	// To Set Where hitboxes are;
	int numRows = MapHitBoxes.size();
	int numCols = MapHitBoxes[0].size();
	for (int i = 0; i < numRows; i++) {
		for (int j = 0; j < numCols; j++) {
			int stack = map.getBlockMap().getAt(i, j);
			if (stack != 0) {
				HitBox temp;
				temp.half_size = Vector3(0.5, 0.5, 0.5);
				temp.center = Vector3(temp.half_size.x + i, temp.half_size.y, temp.half_size.z + j);
				temp.stack = stack;
				temp.is_colliding = false;
				MapHitBoxes[i][j] = temp;
			}
		}
	}

	// For GroundHitbox;
	ground_box.half_size = Vector3(500.0, 500.0, 500.0);
	ground_box.center = Vector3(0.0, -1 * ground_box.half_size.y, 0.0);
	ground_box.is_colliding = false;
	ground_box.is_hitbox = false;
}

void initPlayerHUDInfo()
{
	PlayerInfo.SnowBall_Reservoir = 0;
}

void printBoxCount()
{
	unsigned int box_count = 0;
	for (int x = 0; x < map.getSizeX(); x++)
		for (int z = 0; z < map.getSizeZ(); z++)
			box_count += map.getBlockHeight(x, z);
	cout << "Total boxes: " << box_count << endl;
}

void printNoiseField()
{
	static const unsigned int PRINT_SIZE = 25;

	static const char CHAR_VALUES[] =
	{ ' ', ' ', ' ', ' ', ' ', ' ',
		'.', '-', '=', 'o', 'O', 'H', 'M',
		'@', '@', '@', '@', '@', '@', };
	static const unsigned int CHAR_VALUE_COUNT = sizeof(CHAR_VALUES) / sizeof(CHAR_VALUES[0]);
	static const float CHAR_HALF_COUNT = CHAR_VALUE_COUNT * 0.5f;

	cout << "Perlin noise:" << endl;

	PerlinNoiseField field(10.0f, CHAR_HALF_COUNT);

	for (unsigned int y = 0; y < PRINT_SIZE; y++)
	{
		for (unsigned int x = 0; x < PRINT_SIZE; x++)
		{
			if (x % 10 == 0 && y % 10 == 0)
				cout << "+ ";
			else
			{
				float real_value = field.perlinNoise((float)(x), (float)(y)) + CHAR_HALF_COUNT + 0.5f;
				int    int_value = (int)(real_value);
				assert(int_value >= 0);
				assert(int_value < CHAR_VALUE_COUNT);
				cout << CHAR_VALUES[int_value] << " ";
			}
		}
		cout << endl;
	}
	cout << endl;
}

void printEdgeGrid()
{
	static const unsigned int PRINT_SIZE = HEIGHTMAP_VERTEX_SIZE;

	Neighbourhood neighbourhood(1, 0, 0,
		0, 1,
		-1, -1, 0);
	Heightmap grid = calculateEdgeGrid(neighbourhood, 1);

	cout << "Edge distances:" << endl;
	for (unsigned int x = 0; x < HEIGHTMAP_VERTEX_SIZE; x++)
	{
		for (unsigned int z = 0; z < HEIGHTMAP_VERTEX_SIZE; z++)
		{
			int int_value = (int)(grid.getAt(x, z) * (PRINT_SIZE - 1));
			assert(int_value >= 0);
			assert(int_value < PRINT_SIZE);
			cout << int_value << " ";
		}
		cout << endl;
	}
	cout << endl;
}

Heightmap calculateNoiseGrid(const PerlinNoiseField& field,
	int base_x, int base_z)
{
	Heightmap grid;
	for (int x = 0; x < HEIGHTMAP_VERTEX_SIZE; x++)
		for (int z = 0; z < HEIGHTMAP_VERTEX_SIZE; z++)
		{
			float x_frac = x / (HEIGHTMAP_VERTEX_SIZE - 1.0f);
			float z_frac = z / (HEIGHTMAP_VERTEX_SIZE - 1.0f);
			grid.setAt(x, z, field.perlinNoise(base_x + x_frac, base_z + z_frac));
		}
	return grid;
}

Heightmap calculateEdgeGrid(const Neighbourhood& neighbourhood,
	int desired_value)
{
	Heightmap grid;
	for (unsigned int x = 0; x < HEIGHTMAP_VERTEX_SIZE; x++)
		for (unsigned int z = 0; z < HEIGHTMAP_VERTEX_SIZE; z++)
		{
			float x_frac = x / (HEIGHTMAP_VERTEX_SIZE - 1.0f);
			float z_frac = z / (HEIGHTMAP_VERTEX_SIZE - 1.0f);
			float distance = neighbourhood.getEdgeDistance(x_frac, z_frac, desired_value);
			grid.setAt(x, z, distance);
		}
	return grid;
}



void keyboard(unsigned char key, int x, int y)
{
	key = fixKey(key);

	bool is_support_below = isThereSupportBelow(hit_box.center);

	switch (key)
	{
	case 27: // on [ESC]
		exit(0); // normal exit
		break;
	case '1':
		if (!key_pressed['1'])
			is_debugging_cube = !is_debugging_cube;
		break;
	case '2':
		if (!key_pressed['2'])
			debugging_cube_show = (debugging_cube_show + 1) % CUBE_SHOW_COUNT;
		break;
	case '3':
		if (!key_pressed['3']) {
			hit_box.is_hitbox = !hit_box.is_hitbox;
			isMapHitBox_show = !isMapHitBox_show;
			ground_box.is_hitbox = !ground_box.is_hitbox;
			guard_box.is_hitbox = !guard_box.is_hitbox;
		}
		break;
	case 'R':
		if (!key_pressed['R'] && (!is_game_paused)) {
			player.reset(map.getPlayerStart(PLAYER_HALF_HEIGHT));
			hit_box.center = player.getPosition();
			guard.reset(map.getOpponentStart(PLAYER_HALF_HEIGHT));
			guard_box.center = guard.getPosition();
			resetPlayerInfo();
		}
		break;

	case (char) KEY_SPACE_BAR:
		if (!key_pressed[KEY_SPACE_BAR] && (!is_game_paused) && is_support_below) {
			const double verticle_velocity = 5.5;
			Vector3 desired = player.getForward();
			desired = Vector3(0.0, desired.y, 0.0);
			Vector3 velocity = player.getVelocity();
			desired.y = verticle_velocity;
			player.setVelocity(desired);
		}
		break;
	case '-':
		if (!key_pressed['-'])
			if (snow_radius > SNOW_RADIUS_MIN)
				snow_radius--;
		break;
	case '=':
		if (!key_pressed['='])
			if (snow_radius < SNOW_RADIUS_MAX)
				snow_radius++;
		break;
	case '4':
		is_display_rates = !is_display_rates;
		break;
	case 'Y':
		if (!key_pressed['Y']) {
			is_y_pressed = !is_y_pressed;
		}
		break;

	case 'U':
		if (!key_pressed['U']) {
			is_u_pressed = !is_u_pressed;
			if (is_u_pressed) {
				frame_update_time.setUpdatesPerSecond(60, 1);
			}
			else {
				frame_update_time.setUpdatesPerSecond(60, 10);
			}
		}
	case 'P':
		if (!key_pressed['P']) {
			is_game_paused = !is_game_paused;
			if (is_game_paused) {
				frame_update_time.pauseGame();
			}
			else {
				frame_update_time.playGame();
			}
		}
		break;

	case 'G':
		if (!key_pressed['G'] && PlayerInfo.SnowBall_Reservoir > 0) {
			is_g_pressed = !is_g_pressed;
		}
		break;
	case 'F':
		if (!key_pressed['F'] && (!is_game_paused)) {
			if (PlayerInfo.SnowBall_Reservoir > 0) {

				if (PlayerInfo.SnowBall_Reservoir == 1) {
					fire_ball_1.is_out_of_hand = true;

					fire_ball_3 = fire_ball_1;
					Vector3 direction = player.getForward();
					Vector3 player_velocity = player.getVelocity();
					fire_ball_3.velocity = (direction * 15) + (player_velocity);

					PlayerInfo.SnowBall_Reservoir--;
				}

				if (PlayerInfo.SnowBall_Reservoir == 2) {
					fire_ball_2.is_out_of_hand = true;

					fire_ball_4 = fire_ball_2;
					fire_ball_4.center = getSnowBallCenter(1);
					Vector3 direction = player.getForward();
					Vector3 player_velocity = player.getVelocity();
					fire_ball_4.velocity = (direction * 15) + (player_velocity);

					PlayerInfo.SnowBall_Reservoir--;
				}

				
			}
		}

	}



	key_pressed[key] = true;
}

void keyboardUp(unsigned char key, int x, int y)
{
	key = fixKey(key);
	key_pressed[key] = false;
}

unsigned char fixKey(unsigned char key)
{
	key = toupper(key);
	if (key == '_')
		key = '-';
	if (key == '+')
		key = '=';
	return key;
}

void special(int special_key, int x, int y)
{
	switch (special_key)
	{
	case GLUT_KEY_UP:
		key_pressed[KEY_UP_ARROW] = true;
		break;
	case GLUT_KEY_DOWN:
		key_pressed[KEY_DOWN_ARROW] = true;
		break;
	case GLUT_KEY_LEFT:
		key_pressed[KEY_LEFT_ARROW] = true;
		break;
	case GLUT_KEY_RIGHT:
		key_pressed[KEY_RIGHT_ARROW] = true;
		break;
	}
}

void specialUp(int special_key, int x, int y)
{
	switch (special_key)
	{
	case GLUT_KEY_UP:
		key_pressed[KEY_UP_ARROW] = false;
		break;
	case GLUT_KEY_DOWN:
		key_pressed[KEY_DOWN_ARROW] = false;
		break;
	case GLUT_KEY_LEFT:
		key_pressed[KEY_LEFT_ARROW] = false;
		break;
	case GLUT_KEY_RIGHT:
		key_pressed[KEY_RIGHT_ARROW] = false;
		break;
	}
}



void update()
{
	if (is_y_pressed) {
		sleep(0.05);
	}

	if (!is_game_paused) {
		frame_update_time.incremenetUpdateRate();
		frame_update_time.markNextUpdate();
	}

	int i = 0;
	do {
		if (!is_game_paused) {
			updateForKeyboard();
			updatethrowSnowBalls();
			updateOpponent();
			PlayerInfo.isOnSnow = isThereSnow(hit_box);
			updateSnowBall();
			checkCollision();
			updateMoveMentToFlase();
		}

		if (is_u_pressed) {
			sleep(0.05);
		}

	} while (i < frame_update_time.getMaxUpdatesFrame() && frame_update_time.isUpdateWaiting());

	frame_update_time.sleepUntilNextUpdate();
	glutPostRedisplay();
}

void updateForKeyboard()
{
	if (key_pressed['U']) {
		speed_rate = SPEED_RATE / 3;
		rotation_rate = ROTATION_RATE / 3;
	}
	else {
		speed_rate = SPEED_RATE;
		rotation_rate = ROTATION_RATE;
	}

	// FOr physics calculations;
	float max_speed = 5.0;
	float factor_per_update = 0.05;
	if (key_pressed['W']) {
		Vector3 desired = player.getForward();
		desired.y = 0.0;
		desired.setNormSafe(max_speed);
		Vector3 current = player.getVelocity();
		current.y = 0.0;
		Vector3 combined = desired * factor_per_update + current * (1.0 - factor_per_update);
		combined.y = player.getVelocity().y;
		player.setVelocity(combined);

		W_KEY = true;
	}
	if (key_pressed['S']) {
		Vector3 desired = -1.0 * player.getForward();
		desired.y = 0.0;
		desired.setNormSafe(max_speed);
		Vector3 current = player.getVelocity();
		current.y = 0.0;
		Vector3 combined = desired * factor_per_update + current * (1.0 - factor_per_update);
		combined.y = player.getVelocity().y;
		player.setVelocity(combined);

		S_KEY = true;
	}
	if (key_pressed['A']) {
		Vector3 desired = -1.0 * player.getRight();
		desired.y = 0.0;
		desired.setNormSafe(max_speed);
		Vector3 current = player.getVelocity();
		current.y = 0.0;
		Vector3 combined = desired * factor_per_update + current * (1.0 - factor_per_update);
		combined.y = player.getVelocity().y;
		player.setVelocity(combined);

		A_KEY = true;
	}
	if (key_pressed['D']) {
		Vector3 desired = player.getRight();
		desired.y = 0.0;
		desired.setNormSafe(max_speed);
		Vector3 current = player.getVelocity();
		current.y = 0.0;
		Vector3 combined = desired * factor_per_update + current * (1.0 - factor_per_update);
		combined.y = player.getVelocity().y;
		player.setVelocity(combined);

		D_KEY = true;
	}

	updatePlayerVelocityAndCenterPosition(player, hit_box);

	// Checking if player moved at all;
	did_player_move = A_KEY || S_KEY || W_KEY || D_KEY;

	if (key_pressed[KEY_UP_ARROW])
		player.addPitchDegrees(rotation_rate);
	if (key_pressed[KEY_DOWN_ARROW])
		player.addPitchDegrees(-rotation_rate);
	if (key_pressed[KEY_LEFT_ARROW])
		player.addYawDegrees(rotation_rate);
	if (key_pressed[KEY_RIGHT_ARROW])
		player.addYawDegrees(-rotation_rate);
}

void updateSnowBall()
{
	float waitTime = 3.0 * 60.0;
	int MaxBalls = 2;
	// Updating SnowBall Reservoir;
	if ((!did_player_move) && PlayerInfo.isOnSnow) {
		if (PlayerInfo.SnowBall_Reservoir < MaxBalls && PlayerInfo.clock == waitTime + 1.0) {
			PlayerInfo.SnowBall_Reservoir++;
		}

		// Updating Clock;
		if (PlayerInfo.clock > waitTime) {
			PlayerInfo.clock = 0.0;
		}
		else if (PlayerInfo.clock <= waitTime && PlayerInfo.SnowBall_Reservoir < MaxBalls) {
			PlayerInfo.clock++;
		}
	}
	else {
		PlayerInfo.clock = 0.0;
	}
}

void updateOpponent()
{
	// Changing direction ad velocity for guard
	updatePlayerVelocityAndCenterPosition(guard, guard_box);
}

void updateMoveMentToFlase()
{
	W_KEY = false; A_KEY = false; S_KEY = false; D_KEY = false;
}

void updatePlayerVelocityAndCenterPosition(Player& player, HitBox& hit_box)
{
	Vector3 velocity = player.getVelocity();
	double deltaTime = frame_update_time.getLastUpdateDuration();

	hit_box.center += velocity * frame_update_time.getLastUpdateDuration();

	Vector3 friction = -velocity;
	double max_acceleration = 25.0;
	friction.truncate(max_acceleration);

	if (velocity.getDistance(Vector3::ZERO) < 0.001) {
		friction = Vector3::ZERO;
	}
	int base_x = (int)floor(hit_box.center.x);
	int base_z = (int)floor(hit_box.center.z);
	int is_there_support_below = isThereSupportBelow(hit_box.center);

	if (map.getBlockMap().isInMap((int)floor(hit_box.center.x), (int)floor(hit_box.center.z))) {
		if (isThereSnow(hit_box)) {
			velocity += (friction * 0.025);
		}
		else if (is_there_support_below == 1) {
			velocity += (friction * 0.05);
		}
		else if (is_there_support_below == 2) {
			velocity += (friction * 0.04);
		}
		else {
			velocity.y = velocity.y - (GRAVITY * deltaTime);
		}
		if (velocity.getDistance(Vector3::ZERO) < 0.001) {
			velocity = Vector3::ZERO;
		}
	}
	else {
		if (is_there_support_below) {
			velocity += (friction * 0.05);
		}
		else {
			velocity.y = velocity.y - (GRAVITY * deltaTime);
		}
		if (velocity.getDistance(Vector3::ZERO) < 0.0001) {
			velocity = Vector3::ZERO;
		}
	}

	player.setVelocity(velocity);
}

void resetPlayerInfo()
{
	PlayerInfo.clock = 0;
	PlayerInfo.hit_count = 0;
	PlayerInfo.SnowBall_Reservoir = 0;
}

void reshape(int w, int h)
{
	window_width = w;
	window_height = h;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w / (GLdouble)h, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);

	glutPostRedisplay();
}

void display()
{
	frame_update_time.incrementFrameRate();
	frame_update_time.markNextFrame();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// clear the screen - any drawing before here will not display at all

	Vector3 look_at = hit_box.center;
	look_at.y += 1.0;
	Vector3 eye = look_at - player.getForward() * 5.0;

	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z,   // (x, y, z) camera eye position
		look_at.x, look_at.y, look_at.z,   // (x, y, z) camera look at position
		0.0, 1.0, 0.0);  // (x, y, z) camera up direction

	// camera is now set up - any drawing before here will display incorrectly

	drawSkybox(eye);  // must be first
	ground_list.draw();
	drawAxes();
	drawMap();
	drawPlayer();
	drawSnowBall();
	drawFireBallCurvePath();
	drawGuard();
	drawDebuggingCube();
	drawHitbox();

	// Displaying current Data and health or HUD display;
	drawHUDdisplay();
	// send the current image to the screen - any drawing after here will not display
	glutSwapBuffers();
}

void drawSkybox(const Vector3& eye)
{
	glPushMatrix();
	glTranslated(eye.x, eye.y, eye.z);
	glDepthMask(GL_FALSE);
	skybox_list.draw();
	glDepthMask(GL_TRUE);
	glPopMatrix();

}

void drawAxes()
{
	// display positive X, Y, and Z axes near origin
	glBegin(GL_LINES);
	glColor3d(1.0, 0.0, 0.0);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(2.0, 0.0, 0.0);
	glColor3d(0.0, 1.0, 0.0);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.0, 2.0, 0.0);
	glColor3d(0.0, 0.0, 1.0);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.0, 0.0, 2.0);
	glEnd();
}

void drawMap()
{
	int draw_x = (int)(hit_box.center.x + 0.5);
	int draw_z = (int)(hit_box.center.z + 0.5);

	map.drawBlocks();
	if (snow_radius == SNOW_RADIUS_MAX)
		map.drawSnow(draw_x, draw_z, 999999);
	else
		map.drawSnow(draw_x, draw_z, snow_radius);
}

void drawPlayer()
{
	const Vector3& position = hit_box.center;
	glPushMatrix();
	glTranslated(position.x, position.y, position.z);
	glRotated(player.getYawDegrees(), 0.0, 1.0, 0.0);
	player_list.draw();
	glPopMatrix();
}

void drawGuard()
{
	const Vector3 position = guard_box.center;
	glPushMatrix();
	glTranslated(position.x, position.y, position.z);
	glRotated(guard.getYawDegrees(), 0.0, 1.0, 0.0);
	guard_list.draw();
	glPopMatrix();
}

void drawDebuggingCube()
{
	if (is_debugging_cube)
	{
		Vector3 ahead = hit_box.center + player.getForward() * 1.4;
		int ahead_x = (int)(floor(ahead.x));
		int ahead_z = (int)(floor(ahead.z));

		int ahead_y = 0;
		if (map.isInMap(ahead_x, ahead_z))
			ahead_y = map.getBlockHeight(ahead_x, ahead_z);

		glColor3d(0.0, 1.0, 0.0);
		glPushMatrix();
		glTranslated(ahead_x + 0.5, ahead_y + 0.5, ahead_z + 0.5);
		glutWireCube(1.0);

		Heightmap grid;
		double min_value = 0.0;
		double max_value = 1.0;
		switch (debugging_cube_show)
		{
		case CUBE_SHOW_LARGE:
			grid = calculateNoiseGrid(map.getNoiseLarge(), ahead_x, ahead_z);
			min_value = map.getNoiseLarge().getAmplitude() * -0.5;
			max_value = map.getNoiseLarge().getAmplitude() * 0.5;
			break;
		case CUBE_SHOW_SMALL:
			grid = calculateNoiseGrid(map.getNoiseSmall(), ahead_x, ahead_z);
			min_value = map.getNoiseSmall().getAmplitude() * -0.5;
			max_value = map.getNoiseSmall().getAmplitude() * 0.5;
			break;
		case CUBE_SHOW_HIGHER:
			grid = calculateEdgeGrid(map.getNeighbourhood(ahead_x, ahead_z), 1);
			min_value = 0.0;
			max_value = 1.0;
			break;
		case CUBE_SHOW_LOWER:
			grid = calculateEdgeGrid(map.getNeighbourhood(ahead_x, ahead_z), -1);
			min_value = 0.0;
			max_value = 1.0;
			break;
		}
		drawDebuggingHeights(grid, min_value, max_value);

		glPopMatrix();
	}
}

void drawDebuggingHeights(const Heightmap& grid,
	double value_min, double value_max)
{
	assert(value_min < value_max);

	double value_range = value_max - value_min;
	assert(value_range > 0.0);

	if (IS_DEBUGGING_CUBE_WIREFORM)
	{
		// simpler value grid (point-based)

		glPointSize(5.0);
		glBegin(GL_POINTS);
		for (int x = 0; x < HEIGHTMAP_VERTEX_SIZE; x++)
			for (int z = 0; z < HEIGHTMAP_VERTEX_SIZE; z++)
			{
				double draw_x = x / (HEIGHTMAP_VERTEX_SIZE - 1.0) - 0.5;
				double draw_z = z / (HEIGHTMAP_VERTEX_SIZE - 1.0) - 0.5;

				double value = (grid.getAt(x, z) - value_min) / value_range;
				glColor3d(1.0, value, 0.0);
				glVertex3d(draw_x, value - 0.5, draw_z);
			}
		glEnd();
	}
	else
	{
		// nicer-looking value grid (wireform)

		// lines parallel to X axis
		for (int x = 0; x < HEIGHTMAP_VERTEX_SIZE; x++)
		{
			glBegin(GL_LINE_STRIP);
			for (int z = 0; z < HEIGHTMAP_VERTEX_SIZE; z++)
			{
				double draw_x = x / (HEIGHTMAP_VERTEX_SIZE - 1.0) - 0.5;
				double draw_z = z / (HEIGHTMAP_VERTEX_SIZE - 1.0) - 0.5;

				double value = (grid.getAt(x, z) - value_min) / value_range;
				glColor3d(1.0, value, 0.0);
				glVertex3d(draw_x, value - 0.5, draw_z);
			}
			glEnd();
		}

		// lines parallel to Z axis
		for (int z = 0; z < HEIGHTMAP_VERTEX_SIZE; z++)
		{
			glBegin(GL_LINE_STRIP);
			for (int x = 0; x < HEIGHTMAP_VERTEX_SIZE; x++)
			{
				double draw_x = x / (HEIGHTMAP_VERTEX_SIZE - 1.0) - 0.5;
				double draw_z = z / (HEIGHTMAP_VERTEX_SIZE - 1.0) - 0.5;

				double value = (grid.getAt(x, z) - value_min) / value_range;
				glColor3d(1.0, value, 0.0);
				glVertex3d(draw_x, value - 0.5, draw_z);
			}
			glEnd();
		}
	}
}


void drawHitbox()
{
	// Player Hit box is known as hit_box;
	if (hit_box.is_hitbox) {
		Vector3 position = hit_box.center;

		glColor3d(0.0, 1.0, 0.0);
		glPushMatrix();
		glTranslated(position.x, position.y, position.z);
		glRotated(player.getYawDegrees(), 0.0, 1.0, 0.0);
		glScaled(hit_box.half_size.x, 1.0, hit_box.half_size.z);
		glutWireCube(2 * hit_box.half_size.y);
		glPopMatrix();
	}

	// Guard Hit Boxe is going to be drawn here;
	if (guard_box.is_hitbox) {
		glColor3d(0.0, 1.0, 0.0);
		glPushMatrix();
		glTranslated(guard_box.center.x, guard_box.center.y, guard_box.center.z);
		glRotated(guard.getYawDegrees(), 0.0, 1.0, 0.0);
		glScaled(guard_box.half_size.x, 1.0, guard_box.half_size.z);
		glutWireCube(2 * guard_box.half_size.y);
		glPopMatrix();
	}

	// Hitboxes of all the crates;
	if (isMapHitBox_show) {
		for (int i = 0, numRows = MapHitBoxes.size(); i < numRows; i++) {
			for (int j = 0, numCols = MapHitBoxes[i].size(); j < numCols; j++) {
				if (MapHitBoxes[i][j].stack > 0) {
					int temp_iterator = 0;
					while (temp_iterator < MapHitBoxes[i][j].stack) {
						HitBox temp = MapHitBoxes[i][j];
						glColor3d(0.0, 1.0, 0.0);
						glPushMatrix();
						glTranslated(temp.center.x, temp.center.y + temp_iterator, temp.center.z);
						glutWireCube(2 * temp.half_size.x + 0.01);
						glPopMatrix();
						temp_iterator++;
					}
				}
			}
		}
	}

	// Ground Hit Box is going to be drawn
	if (ground_box.is_hitbox) {
		glColor3d(0.0, 1.0, 0.0);
		glPushMatrix();
		glTranslated(ground_box.center.x, ground_box.center.y, ground_box.center.z);
		glutWireCube(2 * ground_box.half_size.x);
		glPopMatrix();
	}
}

void drawSnowBall()
{
	if (PlayerInfo.SnowBall_Reservoir > 0) {
		fire_ball_1.center = getSnowBallCenter(1);
		drawSnowBall(fire_ball_1);

		if (PlayerInfo.SnowBall_Reservoir > 1) {
			fire_ball_2.center = getSnowBallCenter(2);
			drawSnowBall(fire_ball_2);
		}
	}

	if (fire_ball_3.is_out_of_hand) {
		drawSnowBall(fire_ball_3);
	}
	if (fire_ball_4.is_out_of_hand) {
		drawSnowBall(fire_ball_4);
	}
}

Vector3 getSnowBallCenter(int n)
{
	double player_yaw_radian = degreesToRadians(player.getYawDegrees());
	if (n == 1) {
		float x = 0.25;
		float z = 0.2;
		float initial_theta = atan(x / z);
		double distance = sqrt((x * x) + (z * z));
		initial_theta = player_yaw_radian + initial_theta;
		Vector3 snowball_pos = hit_box.center + Vector3((sin(initial_theta) * distance), 0.1, cos(initial_theta) * distance);
		return snowball_pos;
	}
	
	if (n == 2) {
		float x = 0.18;
		float z = -0.15;
		double initial_theta = atan(x / z);
		double distance = sqrt((x * x) + (z * z));
		initial_theta = PI + player_yaw_radian + initial_theta;
		Vector3 snowball_pos = hit_box.center + (Vector3(sin(initial_theta) * distance, 0.03, cos(initial_theta) * distance));
		return snowball_pos;
	}

	
}

void drawSnowBall(FireBall& fire_ball)
{
	glPushMatrix();
	glTranslated(fire_ball.center.x, fire_ball.center.y, fire_ball.center.z);
	glScaled(0.1, 0.1, 0.1);
	glRotated(player.getYawDegrees(), 0.0, 1.0, 0.0);
	snowball_list.draw();
	glPopMatrix();
}

bool isCollision(const HitBox& first, const HitBox& second)
{
	Vector3 min1 = first.center - first.half_size;
	Vector3 max1 = first.center + first.half_size;
	Vector3 min2 = second.center - second.half_size;
	Vector3 max2 = second.center + second.half_size;

	if (min1.x >= max2.x) {
		return false;
	}

	if (min2.x >= max1.x) {
		return false;
	}

	if (min1.y >= max2.y) {
		return false;
	}

	if (min2.y >= max1.y) {
		return false;
	}

	if (min1.z >= max2.z) {
		return false;
	}

	if (min2.z >= max1.z) {
		return false;
	}

	return true;

}

bool isCollision(HitBox& player, FireBall& fire_ball)
{
	Vector3 min1 = player.center - player.half_size;
	Vector3 max1 = player.center + player.half_size;
	Vector3 min2 = fire_ball.center - fire_ball.half_size;
	Vector3 max2 = fire_ball.center + fire_ball.half_size;

	if (min1.x >= max2.x) {
		return false;
	}

	if (min2.x >= max1.x) {
		return false;
	}

	if (min1.y >= max2.y) {
		return false;
	}

	if (min2.y >= max1.y) {
		return false;
	}

	if (min1.z >= max2.z) {
		return false;
	}

	if (min2.z >= max1.z) {
		return false;
	}

	return true;
}

void collisionResolveFireBall(Player& player, FireBall& fire_ball)
{
	// This function assumes that collision must have occured;
	if (fire_ball.is_out_of_hand) {
		player.setVelocity(fire_ball.velocity);
		PlayerInfo.hit_count++;
		fire_ball.is_out_of_hand = false;
	}
	
}

bool didBallHit()
{
	//For now we will just calculate distance between them that are did snowball hit or not. In next chapters we will calculate if ball hits opponent or not.
	double Distance = hit_box.center.getDistance(guard_box.center);
	Distance = fabs(Distance);
	if (Distance <= 5.0) {
		return true;
	}
	return false;
}

void groundPushBack(const HitBox& ground, HitBox& player)
{
	double yDistance = fabs(player.center.y - ground.center.y);

	player.center.y += fabs((player.half_size.y + ground.half_size.y) - yDistance);
}

double signChecker(double value)
{
	if (value < 0.0)
		return -1.0;
	else
		return 1.0;
}

void pushBack(const HitBox& fixed, HitBox& flex)
{

	double center_distance_x = flex.center.x - fixed.center.x;
	double center_distance_y = flex.center.y - fixed.center.y;
	double center_distance_z = flex.center.z - fixed.center.z;

	double overlap_x = (flex.half_size.x + fixed.half_size.x) - fabs(center_distance_x);
	double overlap_y = (flex.half_size.y + fixed.half_size.y) - fabs(center_distance_y);
	double overlap_z = (flex.half_size.z + fixed.half_size.z) - fabs(center_distance_z);

	if (overlap_x < overlap_y && overlap_x < overlap_z) {
		flex.center.x = flex.center.x + (signChecker(center_distance_x) * overlap_x);
		// Change x
	}
	else if (overlap_y < overlap_z) {
		// Change y
		flex.center.y = flex.center.y + (signChecker(center_distance_y) * overlap_y);
		if (flex.center.y < 0.8) {
			flex.center.y = 0.8;
		}
	}
	else {
		// Change z
		flex.center.z = flex.center.z + (signChecker(center_distance_z) * overlap_z);
	}
}

void checkCollision()
{
	if (isCollision(ground_box, hit_box)) {
		groundPushBack(ground_box, hit_box);
	}

	if (isCollision(ground_box, guard_box)) {
		groundPushBack(ground_box, guard_box);
	}

	if (isCollision(ground_box, fire_ball_3)) {
		fire_ball_3.is_out_of_hand = false;
	}

	if (isCollision(ground_box, fire_ball_4)) {
		fire_ball_4.is_out_of_hand = false;
	}

	if (isCollision(guard_box, hit_box)) {
		double delta_time = frame_update_time.getLastUpdateDuration();
		Vector3 direction = player.getVelocity();
		guard.setVelocity(direction);
	}

	if (isCollision(guard_box, fire_ball_3)) {
		collisionResolveFireBall(guard, fire_ball_3);
	}

	if (isCollision(guard_box, fire_ball_4)) {
		collisionResolveFireBall(guard, fire_ball_4);
	}

	unsigned int numRows = MapHitBoxes.size();
	unsigned int numCols = MapHitBoxes[0].size();

	for (unsigned int i = 0; i < numRows; i++) {
		for (unsigned int j = 0; j < numCols; j++) {
			int iterator = 0;
			while (iterator < MapHitBoxes[i][j].stack) {
				HitBox part_of_stack = MapHitBoxes[i][j];
				part_of_stack.center.y += iterator++;
				if (isCollision(part_of_stack, hit_box)) {
					pushBack(part_of_stack, hit_box);
				}
				if (isCollision(part_of_stack, guard_box)) {
					pushBack(part_of_stack, guard_box);
				}
				if (fire_ball_3.is_out_of_hand) {
					if (isCollision(part_of_stack, fire_ball_3)) {
						fire_ball_3.is_out_of_hand = false;
					}
				}

				if (fire_ball_4.is_out_of_hand) {
					if (isCollision(part_of_stack, fire_ball_4)) {
						fire_ball_4.is_out_of_hand = false;
					}
				}
			}
		}
	}
}

void drawHUDdisplay()
{
	stringstream ss;
	ss << "Hit_count: " << PlayerInfo.hit_count;

	//Declaration of frame Rate stringstream variables to display them;
	stringstream frame_count_ss, game_duration_ss, average_frame_rate_ss;
	stringstream instataneous_frame_rate_ss, smoothed_frame_rate_ss;
	stringstream update_count_ss, average_updates_per_second_ss, instataneous_updates_rate_ss, smoothed_updates_rate_ss;
	stringstream velocity_ss;
	system_clock::time_point current_time = system_clock::now();


	if (is_display_rates) {
		frame_count_ss << "Frame Count: " << frame_update_time.getFrameCount();

		double game_duration = frame_update_time.getGameDuration();
		game_duration_ss << "Game Running Time: " << frame_update_time.getGameDuration();

		double average_frame_rate = frame_update_time.getFrameRateAverage();
		average_frame_rate_ss << "Average Frame Rate: " << frame_update_time.getFrameRateAverage();

		double instataneous_frames_per_second = frame_update_time.getFrameRateInstantaneous();
		instataneous_frame_rate_ss << "Instataneous Frame Rate: " << instataneous_frames_per_second;

		double smoothed_frame_rate = frame_update_time.getSmoothedFrameRate();
		smoothed_frame_rate_ss << "Smoothed Frame Rate: " << smoothed_frame_rate;

		update_count_ss << "Update count: " << frame_update_time.getUpdateCount();

		double average_update_per_second = frame_update_time.getUpdateRateAverage();
		average_updates_per_second_ss << "Average Update Count: " << frame_update_time.getUpdateRateAverage();

		double instataneous_updates_per_second = frame_update_time.getUpdateRateInstantaneous();
		instataneous_updates_rate_ss << "Instataneous Update Rate: " << instataneous_updates_per_second;

		double smoothed_update_rate = frame_update_time.getSmoothedUpdateRate();
		smoothed_updates_rate_ss << "Smoothed Update Rates: " << smoothed_update_rate;

		double distanceFrom0 = player.getVelocity().getDistance(Vector3::ZERO);
		velocity_ss << "Speed_Rate:: " << distanceFrom0;
	}

	SpriteFont::setUp2dView(window_width, window_height, false);
	//Displaying Hit Count;
	font.draw(ss.str(), 16, 10, 0, 0, 0);

	if (is_y_pressed) {
		string toBePrinted = "Y is pressed";
		int characterCount = toBePrinted.length();
		stringstream ssy;
		ssy << toBePrinted;
		font.draw(ssy.str(), window_width - (16 * characterCount), 10, 0, 0, 0);
	}

	if (is_display_rates) {

		font.draw(frame_count_ss.str(), 16, 48, 0, 0, 0);

		font.draw(game_duration_ss.str(), 16, 68, 0, 0, 0);

		font.draw(average_frame_rate_ss.str(), 14, 88, 0, 0, 0);

		font.draw(instataneous_frame_rate_ss.str(), 16, 108, 0, 0, 0);

		font.draw(smoothed_frame_rate_ss.str(), 16, 128, 0, 0, 0);

		font.draw(update_count_ss.str(), 16, 158, 0, 0, 0);

		font.draw(average_updates_per_second_ss.str(), 16, 178, 0, 0, 0);

		font.draw(instataneous_updates_rate_ss.str(), 16, 198, 0, 0, 0);

		font.draw(smoothed_updates_rate_ss.str(), 16, 218, 0, 0, 0);

		font.draw(velocity_ss.str(), 16, 238, 0, 0, 0);
	}

	// Displaying Snowball Counts;
	glDisable(GL_DEPTH_TEST);
	TextureManager::activate(RESOURCE_DIRECTORY + "snowball2.bmp");
	glEnable(GL_TEXTURE_2D);
	int FirstWidth = 32;
	for (int i = 0; i < PlayerInfo.SnowBall_Reservoir; i++) {
		glBegin(GL_POLYGON);
		glTexCoord2d(0.0, 0.0);
		glVertex2i((i * FirstWidth) + ((i + 1) * 10), window_height - 62);
		glTexCoord2d(0.0, 1.0);
		glVertex2i((i * FirstWidth) + ((i + 1) * 10), window_height - 30);
		glTexCoord2d(1.0, 1.0);
		glVertex2i((i + 1) * (FirstWidth + 10), window_height - 30);
		glTexCoord2d(1.0, 0.0);
		glVertex2i((i + 1) * (FirstWidth + 10), window_height - 62);
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);

	// Displaying Loading time left for a new snowball;
	if (PlayerInfo.SnowBall_Reservoir < 2 && PlayerInfo.clock >= 1.0) {
		float window_first_point = 20;
		float window_last_point = 20;
		float padding = 2;
		double LastPointForWeightRec = (PlayerInfo.clock / (61.0 * 3.0)) * ((window_width - (window_first_point + window_last_point)));
		// Blue Rectangle which will be in back-ground of white rectangle;

		glColor3d(0.0, 0.0, 0.4);
		glBegin(GL_POLYGON);
		glVertex2i(window_first_point - padding, window_height - 96 - padding);
		glVertex2i(window_first_point - padding, window_height - 72 + padding);
		glVertex2i(window_width - window_last_point + padding, window_height - 72 + padding);
		glVertex2i(window_width - window_last_point + padding, window_height - 96 - padding);
		glEnd();
		// White rectangle which will load;
		glColor3d(1.0, 1.0, 1.0);
		glBegin(GL_POLYGON);
		glVertex2i(window_first_point, window_height - 96);
		glVertex2i(window_first_point, window_height - 72);
		glVertex2i(window_first_point + LastPointForWeightRec, window_height - 72);
		glVertex2i(window_first_point + LastPointForWeightRec, window_height - 96);
		glEnd();
	}

	glEnable(GL_DEPTH_TEST);
	SpriteFont::unsetUp2dView();
}

void drawFireBallCurvePath()
{
	if (is_g_pressed) {
		Vector3 center = getSnowBallCenter(1);
		double delta_time = frame_update_time.getLastUpdateDuration();
		Vector3 direction = (player.getForward() * 15) + player.getVelocity();
		Vector3 current_position = center + (direction * delta_time * 0.5);

		glColor4f(1.0, 0.0, 0.0, 0.5);
		glPushMatrix();

		for (int i = 0; i < 10; i++) {
			glTranslated(current_position.x, current_position.y, current_position.z);
			glScaled(0.5, 0.5, 0.5);
			glutWireCube(1.0);
			current_position = current_position + (direction * delta_time * 0.5);
			direction.y = direction.y - (GRAVITY * delta_time);
		}

		glPopMatrix();

	}
}

bool isThereSnow(HitBox& hit_box)
{
	int base_x = (int)floor(hit_box.center.x);
	int base_z = (int)floor(hit_box.center.z);

	if (map.getBlockMap().isInMap(base_x, base_z)) {
		if (map.getSnowMap().isInMap(base_x, base_z)) {
			// Getting size of heights and stacks;
			double heightValue = map.getSnowHeightAtPoint(base_x, base_z, hit_box.center.x - base_x, hit_box.center.z - base_z);
			int stack_size = map.getBlockMap().getAt(base_x, base_z);
			if (heightValue <= 0.0) {
				return false;
			}
			else if (stack_size == 0) {
				if (hit_box.center.y < heightValue + hit_box.half_size.y) {
					hit_box.center.y = hit_box.center.y - (hit_box.center.y - heightValue - hit_box.half_size.y);
					return true;
				}
				if (hit_box.center.y > heightValue + hit_box.half_size.y + 0.01) {
					return false;
				}
			}
			if (stack_size > 0) {
				if (hit_box.center.y < stack_size + heightValue + hit_box.half_size.y + 0.05) {
					hit_box.center.y = hit_box.center.y - (hit_box.center.y - stack_size - heightValue - hit_box.half_size.y);
					return true;
				}
				if (hit_box.center.y > heightValue + hit_box.half_size.y + stack_size) {
					return false;
				}
			}

			return true;
		}
		return false;
	}
	return false;
}

int isThereSupportBelow(Vector3& cordinates)
{
	int base_x = (int)floor(cordinates.x);
	int base_z = (int)floor(cordinates.z);
	
	if (map.getBlockMap().isInMap(base_x, base_z)) {
		double heightValue = map.getSnowHeightAtPoint(base_x, base_z, cordinates.x - base_x, cordinates.z - base_z);
		if (heightValue < 0) {
			heightValue = 0.0f;
		}
		int stack_size = map.getBlockMap().getAt(base_x, base_z);
		if (stack_size == 0) {
			if (cordinates.y < heightValue + 0.8 + 0.01) {
				return 1;
			}
			return 0;
		}
		else if (stack_size > 0) {
			if (cordinates.y < stack_size + heightValue + 0.8 + 0.01) {
				return 2;
			}
			return 0;
		}
	}
	if (cordinates.y > 0.8) {
		return 0;
	}
	return 1;
}

void updatethrowSnowBalls()
{
	double delta_time = frame_update_time.getLastUpdateDuration();
	if (fire_ball_3.is_out_of_hand) {
		fire_ball_3.center = fire_ball_3.center + (fire_ball_3.velocity * delta_time);
		fire_ball_3.velocity.y = fire_ball_3.velocity.y - (GRAVITY * delta_time);
	}

	if (fire_ball_4.is_out_of_hand) {
		fire_ball_4.center = fire_ball_4.center + (fire_ball_4.velocity * delta_time);
		fire_ball_4.velocity.y = fire_ball_4.velocity.y - (GRAVITY * delta_time);

	}
}