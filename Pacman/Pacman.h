#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

struct Player
{
	// Data to represent Pacman
	Vector2* Position;
	Rect* SourceRect;
	Texture2D* Texture;
	int Direction;

	// Position for String
	Vector2* _stringPosition;

	// constant data for game variables
	float PlayerSpeed;

	//data for animating pacman
	int Frame;
	int CurrentFrameTime;

	int PlayerFrameTime;

	// data for speedboost
	float speedMultiplier;

	//data for life
	bool dead;

	//score
	int Score;
};

struct Enemy
{
	int FrameTime;

	int _CurrentFrameTime;
	
	// Data to represent Munchie
	int _FrameCount;
	Rect* _Rect;
	Texture2D* DuelMunchieTexture;
	Texture2D* DuelCherryTexture;
	
	Vector2* position;
	//problem - i didnt have a position variable
};

struct MovingEnemy
{
	Vector2* position;
	Texture2D* texture;
	Rect* sourceRect;
	int direction;
	float speed;
};

struct Menu
{
	//Data for Menu
	Texture2D* Background;
	Rect* Rectangle;
	Vector2* StringPosition;
	Vector2* StringPositionScore;
	bool paused;
	bool pKeyDown;
	Texture2D* floor;
};

//struct _backgroundmusic
//{
//	bool _isLooping;
//};

//defines amount of munchies
#define MUNCHIECOUNT 50

// defines number of ghosts
#define GHOSTCOUNT 4

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Pacman : public Game
{
private:

	Player* _pacman;
	Enemy* _munchies[MUNCHIECOUNT];
	Enemy* Cherry;
	Menu* _pausemenu;
	MovingEnemy* _ghosts[GHOSTCOUNT];
	SoundEffect* _pop;
	SoundEffect* _Cherryget;
	SoundEffect* _ghostlaugh;
	SoundEffect* _backgroundmusic;

	//creating prototypes
	
	void Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState);

	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);

	void CheckViewportCollision();

	void UpdatePacman(int elapsedTime);

	void UpdateMunchie(Enemy*, int elapsedTime);

	void UpdateCherry(Enemy*, int elapsedTime);

	void CheckGhostCollision();
	void CheckMunchieCollision();
	void CheckCherryCollision();

	void UpdateGhost(MovingEnemy*, int elapsedTime);
	void UpdateGhost2(MovingEnemy*, int elapsedTime);
	void UpdateGhost3(MovingEnemy*, int elapsedTime);
	void UpdateGhost4(MovingEnemy*, int elapsedTime);
public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);
};