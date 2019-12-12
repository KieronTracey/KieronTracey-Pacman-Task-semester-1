#include "Pacman.h"
#include <time.h>
#include <sstream>

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv)
{
	Audio::Initialise();

	srand(time(NULL));
	int i;
	//Initialise member variables
	_pacman = new Player();
	_pacman->dead = false;
	_pausemenu = new Menu();


	//initialises instances of munchies
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i] = new Enemy();
		_munchies[i]->_CurrentFrameTime = 0;
		_munchies[i]->_FrameCount = rand() % 1;
		_munchies[i]->FrameTime = rand() % 500 + 50;
	}

	//initialise cherry
	Cherry = new Enemy();
	Cherry->_CurrentFrameTime = 0;
	Cherry->_FrameCount = rand() % 1;
	Cherry->FrameTime = rand() % 500 + 50;
	
	//initialisepause menu
	_pausemenu->paused = false;
	_pausemenu->pKeyDown = false;

	//initialise pacman
	_pacman->Direction = 0;
	_pacman->CurrentFrameTime = 0;
	_pacman->Frame = 0;
	_pacman->PlayerSpeed = 0.1f;
	_pacman->PlayerFrameTime = 150;
	_pacman->speedMultiplier = 1.0f;
	_pacman->Score = 0;

	//initialise ghosts
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_ghosts[i] = new MovingEnemy();
		_ghosts[i]->direction = 0;
		_ghosts[i]->speed = 0.2f;
		
	}

	//initialise sound
	_pop = new SoundEffect;
	_Cherryget = new SoundEffect;
	_ghostlaugh = new SoundEffect;
	_backgroundmusic = new SoundEffect;

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	//sets the background music to looping
	_backgroundmusic->SetLooping(true);

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();

	


}

Pacman::~Pacman()
{
	int i;

	delete _pacman->Texture;
	delete _pacman->SourceRect;

	//loops for all munchies
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		
		delete _munchies[i]->DuelMunchieTexture;
		delete _munchies[i]->_Rect;
		delete _munchies[i]->position;
		delete _munchies[i];
	}

	delete _munchies;

	//cherry
	delete Cherry->DuelCherryTexture;
	delete Cherry->_Rect;
	delete Cherry->position;
	delete Cherry;

	//ghost
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		delete _ghosts[i]->position;
		delete _ghosts[i]->texture;
		delete _ghosts[i]->sourceRect;
		delete _ghosts[i];
	}

	//sound
	delete _pop;
	delete _Cherryget;
	delete _ghostlaugh;
	delete _backgroundmusic;
}

void Pacman::LoadContent()
{
	int i;

	_pausemenu->floor = new Texture2D();
	_pausemenu->floor->Load("Textures/floor.png",false);


	// Load Pacman
	_pacman->Texture = new Texture2D();
	_pacman->Texture->Load("Textures/Pacman.tga", false);
	_pacman->Position = new Vector2(350.0f, 350.0f);
	_pacman->SourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Load Munchie
	
	Texture2D* munchieTex = new Texture2D();
	munchieTex->Load("Textures/Munchie.png", false);
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i]->DuelMunchieTexture = munchieTex; 
		_munchies[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		_munchies[i]->_Rect = new Rect(0.0f, 0.0f, 12, 12);
	}

	//load cherry
	Texture2D* CherryTex = new Texture2D();
	CherryTex->Load("Textures/CherryMerged.png", false);
	
	Cherry->DuelCherryTexture = CherryTex;
	Cherry->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
	Cherry->_Rect = new Rect(0.0f, 0.0f, 12, 12);

// load ghosts
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_ghosts[i]->texture = new Texture2D();
		_ghosts[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		_ghosts[i]->sourceRect = new Rect(0.0f, 0.0f, 20, 20);
	}


	_ghosts[0]->texture->Load("Textures/GhostBlue.png", false);
	_ghosts[1]->texture->Load("Textures/GhostRed.png", false);
	_ghosts[2]->texture->Load("Textures/GhostPink.png", false);
	_ghosts[3]->texture->Load("Textures/GhostOrange.png", false);

	//load sound
	_pop->Load("Sound/pop.wav");
	_Cherryget->Load("Sound/Cherryget.wav");
	_ghostlaugh->Load("Sound/ghostlaugh.wav");
	_backgroundmusic->Load("Sound/backgroundmusic.wav");

	// Set string position
	_pacman->_stringPosition = new Vector2(10.0f, 25.0f);

	// Set Menu Paramters
	_pausemenu->Background = new Texture2D();
	_pausemenu->Background->Load("Textures/Transparency.png", false);
	_pausemenu->Rectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_pausemenu->StringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);
	_pausemenu->StringPositionScore = new Vector2(Graphics::GetViewportWidth() / 3.0f, Graphics::GetViewportHeight() / 25.0f);

	

	Audio::Play(_backgroundmusic);
}

// Function Header Block
void Pacman::Update(int elapsedTime)
{
	// Variables 
	
	//gets the current state of the keyboard
	Input::KeyboardState *keyboardState = 
		Input::Keyboard::GetState();

	//gets the current state of the mouse
	Input::MouseState* mouseState = Input::Mouse::GetState();

	// Check if game paused 
	CheckPaused(keyboardState, Input::Keys::P);

	// Update Tasks
	if (!_pausemenu->paused && !_pacman->dead)
	{
		Input(elapsedTime, keyboardState, mouseState); 
		UpdatePacman(elapsedTime); 
		CheckViewportCollision();

		for (int i = 0; i < MUNCHIECOUNT; i++)
		{
			UpdateMunchie(_munchies[i], elapsedTime);
		}
		
		UpdateCherry(Cherry, elapsedTime);

			UpdateGhost(_ghosts[0], elapsedTime);
			UpdateGhost2(_ghosts[1], elapsedTime);
			UpdateGhost3(_ghosts[2], elapsedTime);
			UpdateGhost4(_ghosts[3], elapsedTime);
		
		CheckGhostCollision();

		CheckMunchieCollision();

		CheckCherryCollision();
		
	}

}



void Pacman::Draw(int elapsedTime)
{
	

	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << _pacman->Position->X << " Y: " << _pacman->Position->Y;
	
	// Starts Drawing
	SpriteBatch::BeginDraw(); 

	SpriteBatch::Draw(_pausemenu->floor, _pausemenu->Rectangle, nullptr);

	// Draws Pacman
	if (!_pacman->dead)
	{
		SpriteBatch::Draw(_pacman->Texture, _pacman->Position, _pacman->SourceRect);
	}
	


	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		// Draws Red Munchie
		SpriteBatch::Draw(_munchies[i]->DuelMunchieTexture, _munchies[i]->position, _munchies[i]->_Rect);
	}
	
	// Draws Cherry
	SpriteBatch::Draw(Cherry->DuelCherryTexture, Cherry->position, Cherry->_Rect);

	// Draws String
	int i;

	SpriteBatch::DrawString(stream.str().c_str(), _pacman->_stringPosition, Color::Green);

	if (_pausemenu->paused)
	{
		std::stringstream menuStream;
		menuStream << "PAUSED! ";

		SpriteBatch::Draw(_pausemenu->Background, _pausemenu->Rectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _pausemenu->StringPosition, Color::Red);
	}

	//draws ghost
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		SpriteBatch::Draw(_ghosts[i]->texture, _ghosts[i]->position, _ghosts[i]->sourceRect);
	}

	if (_pacman->dead)
	{
		std::stringstream menuStream;
		menuStream << "DEAD! ";

		SpriteBatch::Draw(_pausemenu->Background, _pausemenu->Rectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _pausemenu->StringPosition, Color::Red);
	}

	//score
	std::stringstream menuStream;
	menuStream << "Current Score :" << _pacman->Score;

	SpriteBatch::DrawString(menuStream.str().c_str(), _pausemenu->StringPositionScore, Color::Yellow);

	

	SpriteBatch::EndDraw(); // Ends Drawing
}

void Pacman::Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState*mouseState) 
{
	int i;

	float pacmanSpeed = _pacman->PlayerSpeed * elapsedTime * _pacman->speedMultiplier;

	if (!_pausemenu->paused)

		// Checks if D key is pressed
		if (state->IsKeyDown(Input::Keys::D))
		{
			_pacman->Position->X += pacmanSpeed; //Moves Pacman across X axis
			_pacman->Direction = 0;
		}

		else if (state->IsKeyDown(Input::Keys::A)) //Moves Pacman across X axis
		{
			_pacman->Position->X -= pacmanSpeed;
			_pacman->Direction = 2;
		}

		else if (state->IsKeyDown(Input::Keys::S)) //Moves Pacman across Y axis
		{
			_pacman->Position->Y += pacmanSpeed;
			_pacman->Direction = 1;
		}

		else if (state->IsKeyDown(Input::Keys::W)) //Moves Pacman across Y axis
		{
			_pacman->Position->Y -= pacmanSpeed;
			_pacman->Direction = 3;
		}

	//checks if mouse button is pressed and if so reposition cherry
		else if (mouseState->LeftButton == Input::ButtonState::PRESSED)
		{
			Cherry->position->X = mouseState->X;
			Cherry->position->Y = mouseState->Y;
		}

	//SpeedBoost	
	if (state->IsKeyDown(Input::Keys::LEFTSHIFT))
	{
		//apply boost
		_pacman->speedMultiplier = 2.0f;
	}
	else
	{
		//reset boost
		_pacman->speedMultiplier = 1.0f;
	}

	//Randomise Cherry
	if (state->IsKeyDown(Input::Keys::R))
	{
		Cherry->position->X = rand() % Graphics::GetViewportWidth();
		Cherry->position->Y = rand() % Graphics::GetViewportHeight();
		
			//rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight()
	}

	
}

void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{
	int i;

	if (state->IsKeyDown(Input::Keys::P) && !_pausemenu->pKeyDown)
	{
		_pausemenu->pKeyDown = true;
		_pausemenu->paused = !_pausemenu->paused;
	}

	if (state->IsKeyUp(Input::Keys::P))

		_pausemenu->pKeyDown = false;
}

void Pacman::CheckViewportCollision()
{

	// Checks if pacman is trying to disapear of the edge of the screen
	if (_pacman->Position->X + _pacman->SourceRect->Width > Graphics::GetViewportWidth()) //1024 is game width)
	{
		// pacman hits right wall it will reset his position
		_pacman->Position->X = Graphics::GetViewportWidth() - _pacman->SourceRect->Width;
	}

	// Checks if pacman is trying to disapear of the edge of the screen
	if (_pacman->Position->X + _pacman->SourceRect->Width < 30)
	{
		// pacman hits left wall it will reset his position
		_pacman->Position->X = -35 + _pacman->SourceRect->Width;
	}

	// Checks if pacman is trying to disapear of the edge of the screen
	if (_pacman->Position->Y + _pacman->SourceRect->Height < 30)
	{
		// pacman hits ceiling it will reset his position
		_pacman->Position->Y = -35 + _pacman->SourceRect->Height;
	}

	// Checks if pacman is trying to disapear of the edge of the screen
	if (_pacman->Position->Y + _pacman->SourceRect->Height > 770)
	{
		// pacman hits floor it will reset his position
		_pacman->Position->Y = 705 + _pacman->SourceRect->Height;
	}

}

void Pacman::UpdatePacman(int elapsedTime)
{
	int i;

	_pacman->CurrentFrameTime += elapsedTime;


	if (_pacman->CurrentFrameTime > _pacman->PlayerFrameTime)
	{
		_pacman->Frame++;

		if (_pacman->Frame >= 2)
			_pacman->Frame = 0;

		_pacman->CurrentFrameTime = 0;
	}
	
	_pacman->SourceRect->X = _pacman->SourceRect->Width * _pacman->Frame;
	_pacman->SourceRect->Y = _pacman->SourceRect->Height * _pacman->Direction;

}

void Pacman::UpdateMunchie(Enemy* munchie,int elapsedTime)
{
	
		munchie->_CurrentFrameTime += elapsedTime;

		if (munchie->_CurrentFrameTime >= munchie->FrameTime)
		{
			munchie->_FrameCount++;

			if (munchie->_FrameCount >= 2)
				munchie->_FrameCount = 0;

			munchie->_CurrentFrameTime = 0;
			munchie->_Rect->X = munchie->_Rect->Width * munchie->_FrameCount;
		}
	
}

void Pacman::UpdateCherry(Enemy* cherry, int elapsedTime)
{

	cherry->_CurrentFrameTime += elapsedTime;

	if (cherry->_CurrentFrameTime >= cherry->FrameTime)
	{
		cherry->_FrameCount++;

		if (cherry->_FrameCount >= 2)
			cherry->_FrameCount = 0;

		cherry->_CurrentFrameTime = 0;
		cherry->_Rect->X = cherry->_Rect->Width * cherry->_FrameCount;
	}

}

//Provides primitve movement for ghosts
void Pacman::UpdateGhost(MovingEnemy* ghost, int elapsedTime)
{
	if (ghost->direction == 0) //moves right
	{
		ghost->position->X += ghost->speed * elapsedTime;
		ghost->sourceRect->X = ghost->sourceRect->Width * 20;
	}

	else if (ghost->direction == 1) // left
	{
		ghost->position->X -= ghost->speed * elapsedTime;
		ghost->sourceRect->X = ghost->sourceRect->Width * 0;
	}

	if (ghost->position->X >= Graphics::GetViewportWidth() -20) // hits right edge
	{
		ghost->direction = 1; // change direction
	}

	else if (ghost->position->X <= 0) // hits left edge
	{
		ghost->direction = 0; // change direction
	}
	

	
}

//Provides viewport patrol movement type for ghosts

void Pacman::UpdateGhost2(MovingEnemy* ghost, int elapsedTime)
{

	if (ghost->direction == 0) //moves down
	{
		ghost->position->Y += ghost->speed * elapsedTime;
		ghost->sourceRect->X = ghost->sourceRect->Width * 28;
	}

	else if (ghost->direction == 1) // up
	{
		ghost->position->Y -= ghost->speed * elapsedTime;
		ghost->sourceRect->X = ghost->sourceRect->Width * 42;
	}

	else if (ghost->direction == 2) //moves right
	{
		ghost->position->X += ghost->speed * elapsedTime;
		ghost->sourceRect->X = ghost->sourceRect->Width * 1.1;
	}

	else if (ghost->direction == 3) // left
	{
		ghost->position->X -= ghost->speed * elapsedTime;
		ghost->sourceRect->X = ghost->sourceRect->Width * 0;
	}


	if (ghost->position->Y >= Graphics::GetViewportHeight() - 30)  //hits bottom 
	{
		ghost->direction = 2; // change direction
	}

	if (ghost->position->Y >= Graphics::GetViewportHeight() - 30 && ghost->position->X >= Graphics::GetViewportWidth() - 30) //hits bottom right
	{
		ghost->direction = 1; // change direction
	}

	else if (ghost->position->Y <= 30 && ghost->position->X >= Graphics::GetViewportWidth() - 30) // hits top right
	{
		ghost->direction = 3; // change direction
	}

	else if (ghost->position->Y >= Graphics::GetViewportHeight() - 30 && ghost->position->X <= 30)//hits bottom left
	{
		ghost->direction = 2; // change direction
	}

	else if (ghost->position->X <= 30 && ghost->position->Y <= 30) // hits top left 
	{
		ghost->direction = 0; // change direction
	}
	

}

// Provides chase movement for ghosts
void Pacman::UpdateGhost3(MovingEnemy * ghost, int elapsedTime)
{
	

	if (ghost->position->X < _pacman->Position->X)//if ghost x is less than pacmans x
	{
		ghost->position->X += ghost->speed * 6; // increase ghost x
		ghost->sourceRect->X = ghost->sourceRect->Width * 0;
	}

	else if (ghost->position->X > _pacman->Position->X)//if ghost x is more than pacmans x
    {
		ghost->position->X -= ghost->speed * 6; // decrease ghost x
		ghost->sourceRect->X = ghost->sourceRect->Width * 1.1;
	}

	if (ghost->position->Y < _pacman->Position->Y)//if ghost y is less than pacmans y
	{
		ghost->position->Y += ghost->speed * 6; // increase ghost y
	}

	else if (ghost->position->Y > _pacman->Position->Y)//if ghost y is more than pacmans y
	{
		ghost->position->Y -= ghost->speed * 6; // decrease ghost y	
	}
	
}

// Provides random movement for ghosts
void Pacman::UpdateGhost4(MovingEnemy* ghost, int elapsedTime)
{
	if (ghost->direction == 0) //moves down
	{
		ghost->position->Y += ghost->speed * elapsedTime;
		ghost->sourceRect->X = ghost->sourceRect->Width * 28.3;
	}

	else if (ghost->direction == 1) //moves up
	{
		ghost->position->Y -= ghost->speed * elapsedTime;
		ghost->sourceRect->X = ghost->sourceRect->Width * 20.7;
	}

	else if (ghost->direction == 2) //moves right
	{
		ghost->position->X += ghost->speed * elapsedTime;
		ghost->sourceRect->X = ghost->sourceRect->Width * 1.1;
	}

	else if (ghost->direction == 3) // moves left
	{
		ghost->position->X -= ghost->speed * elapsedTime;
		ghost->sourceRect->X = ghost->sourceRect->Width * 0;
	}

	else if (ghost->direction == 4) // moves up+right
	{
		ghost->position->Y -= ghost->speed * elapsedTime; 
		ghost->position->X += ghost->speed * elapsedTime;
		ghost->sourceRect->X = ghost->sourceRect->Width * 4.4;
	}

	else if (ghost->direction == 5) // moves up+left
	{
		ghost->position->Y -= ghost->speed * elapsedTime;
		ghost->position->X -= ghost->speed * elapsedTime;
		ghost->sourceRect->X = ghost->sourceRect->Width * 5.5;
	}

	else if (ghost->direction == 6) // moves down+right
	{
		ghost->position->Y += ghost->speed * elapsedTime;
		ghost->position->X += ghost->speed * elapsedTime;
		ghost->sourceRect->X = ghost->sourceRect->Width * 6.6;
	}

	else if (ghost->direction == 7) // moves down+left
	{
		ghost->position->Y += ghost->speed * elapsedTime;
		ghost->position->X -= ghost->speed * elapsedTime;
		ghost->sourceRect->X = ghost->sourceRect->Width * 7.7;
	}



	if (ghost->position->Y >= Graphics::GetViewportHeight() - 30)  //hits bottom 
	{
		ghost->direction = rand() % (7 - 0 + 1) + 0; // change direction

		while (ghost->direction == 0 || ghost->direction == 6 || ghost->direction == 7 )
		{
			ghost->direction = rand() % (7 - 0 + 1) + 0;
		}
	}

	else if (ghost->position->X >= Graphics::GetViewportWidth() - 30) //hits right
	{
		ghost->direction = rand() % (7 - 0 + 1) + 0; // change direction

		while (ghost->direction == 2 || ghost->direction == 4 || ghost->direction == 6)
		{
			ghost->direction = rand() % (7 - 0 + 1) + 0;
		}
	}

	else if (ghost->position->Y <= 30) // hits top
	{
		ghost->direction = rand() % (7 - 0 + 1) + 0; // change direction

		while (ghost->direction == 1 || ghost->direction == 4 || ghost->direction == 5)
		{
			ghost->direction = rand() % (7 - 0 + 1) + 0;
		}
	}

	else if (ghost->position->X <= 30)// left
	{
		ghost->direction = rand() % (7 - 0 + 1) + 0; // change direction

		while (ghost->direction == 3 || ghost->direction == 5 || ghost->direction == 7)
		{
			ghost->direction = rand() % (7 - 0 + 1) + 0;
		}
	}

	
}

void Pacman::CheckGhostCollision()
{
	//Loacal Variable
	int i = 0;
	int bottom1 = _pacman->Position->Y + _pacman->SourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->Position->X;
	int left2 = 0;
	int right1 = _pacman->Position->X + _pacman->SourceRect->Width;
	int right2 = 0;
	int top1 =_pacman->Position->Y;
	int top2 = 0;

	for (i = 0; i < GHOSTCOUNT; i++)
	{
		// populate variables with ghost data
		bottom2 = _ghosts[i]->position->Y + _ghosts[i]->sourceRect->Height;
		left2 = _ghosts[i]->position->X;
		right2 = _ghosts[i]->position->X + _ghosts[i]->sourceRect->Width;
		top2 = _ghosts[i]->position->Y;

		if (bottom1 > top2 && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			_pacman->dead = true;
			Audio::Play(_ghostlaugh);

			i = GHOSTCOUNT;
		}
		
	}

	if (_pacman->dead)
	{
		_ghosts[0]->texture->Load("Textures/GhostVictory.png", false);
		_ghosts[1]->texture->Load("Textures/GhostVictory.png", false);
		_ghosts[2]->texture->Load("Textures/GhostVictory.png", false);
		_ghosts[3]->texture->Load("Textures/GhostVictory.png", false);

		_ghosts[0]->sourceRect->X = _ghosts[0]->sourceRect->Width * 0;
		_ghosts[1]->sourceRect->X = _ghosts[1]->sourceRect->Width * 0;
		_ghosts[2]->sourceRect->X = _ghosts[2]->sourceRect->Width * 0;
		_ghosts[3]->sourceRect->X = _ghosts[3]->sourceRect->Width * 0;
	}
		
}

void Pacman::CheckMunchieCollision()
{
	//Loacal Variable
	int i = 0;
	int bottom1 = _pacman->Position->Y + _pacman->SourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->Position->X;
	int left2 = 0;
	int right1 = _pacman->Position->X + _pacman->SourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->Position->Y;
	int top2 = 0;

	for (i = 0; i < MUNCHIECOUNT; i++)
	{
		// populate variables with data
		bottom2 = _munchies[i]->position->Y + _munchies[i]->_Rect->Height;
		left2 = _munchies[i]->position->X;
		right2 = _munchies[i]->position->X + _munchies[i]->_Rect->Width;
		top2 = _munchies[i]->position->Y;

		if (bottom1 > top2 && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			Audio::Play(_pop);

			_munchies[i]->position->X = -100;

			_pacman->Score++;

			i = MUNCHIECOUNT;
		}

	}

}

void Pacman::CheckCherryCollision()
{
	//Loacal Variable
	int i = 0;
	int bottom1 = _pacman->Position->Y + _pacman->SourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->Position->X;
	int left2 = 0;
	int right1 = _pacman->Position->X + _pacman->SourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->Position->Y;
	int top2 = 0;

		// populate variables with data
		bottom2 = Cherry->position->Y + Cherry->_Rect->Height;
		left2 = Cherry->position->X;
		right2 = Cherry->position->X + Cherry->_Rect->Width;
		top2 = Cherry->position->Y;

		if (bottom1 > top2 && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			Audio::Play(_Cherryget);

			Cherry->position->X = -100;

			_pacman->Score = _pacman->Score + 10;
		}

	

}
