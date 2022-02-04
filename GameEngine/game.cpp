#include "game.h"
#include <lua.hpp>
#include <cstdlib>
#include <ctime>
#include <fstream>
using namespace std;

#ifdef _WIN32
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#endif

static Game* pGame = nullptr;

//Buffers to hold key input
const int NUM_SDL_SCANCODES = 512;
bool prevKeys[NUM_SDL_SCANCODES];
bool keys[NUM_SDL_SCANCODES];

//Lua Scripting Stuff

void printError(lua_State* state)
{
	SDL_Log(luaL_checkstring(state, -1));
}

void runScript(lua_State* state, const char* file) {
	int result = luaL_loadfile(state, file);

	if (result == LUA_OK)
	{
		result = lua_pcall(state, 0, LUA_MULTRET, 0);

		if (result != LUA_OK)
		{
			printError(state);
		}
	}
	else
	{
		printError(state);
	}
}

void call(lua_State* state, const char* functionName)
{
	int type = lua_getglobal(state, functionName);

	if (type == LUA_TNIL)
	{
		SDL_Log("Function not found.");
	}
	else
	{
		int result = lua_pcall(state, 0, 0, 0);

		if (result != LUA_OK)
		{
			printError(state);
		}
	}
}

//End Lua Stuff

bool Game::init(int screenWidth, int screenHeight, bool fullscreen, bool vsync)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		return false;
	}

	Uint32 flags = SDL_WINDOW_SHOWN;
	if (fullscreen)
	{
		flags |= SDL_WINDOW_FULLSCREEN;
	}

	window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, flags);

	if (!window)
	{
		return false;
	}

	if (vsync)
	{
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	}
	else
	{
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	}

	if (!renderer)
	{
		return false;
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

	int imageFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imageFlags) & imageFlags))
	{
		return false;
	}

	if (Mix_OpenAudio(20050, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		return false;
	}

	if (TTF_Init() == -1)
	{
		return false;
	}

	backbuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET, screenWidth, screenHeight);

	if (!backbuffer)
	{
		return false;
	}

	unsigned int seed = (unsigned int)time(0);
	srand(seed);

	clientWidth = screenWidth;
	clientHeight = screenHeight;

	previousTime = SDL_GetPerformanceCounter();

	initLua();
	call(luaState, "Start");

	return true;
}

void Game::run()
{
	bool quit = false;

	while (!quit)
	{
		quit = pollEvents();
		call(luaState, "Update");
		
		SDL_memcpy(&prevKeys, &keys, NUM_SDL_SCANCODES);

		beginFrame();
		call(luaState, "Draw");
		endFrame();
	}
}

void Game::shutdown()
{
	lua_close(luaState);
	assetDatabase.clear();

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer); // free backbuffer
	Mix_CloseAudio();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void Game::onEvent(SDL_Event& event)
{
	// nothing by default
}

void Game::beginFrame()
{
	currentTime = SDL_GetPerformanceCounter();
	deltaTimeSeconds = (double)(currentTime - previousTime) * 1000 / SDL_GetPerformanceFrequency() / 1000.0;

	SDL_SetRenderTarget(renderer, backbuffer);
}

void Game::endFrame()
{
	SDL_SetRenderTarget(renderer, nullptr);
	SDL_RenderCopyEx(renderer, backbuffer, nullptr, nullptr, 0, nullptr, SDL_FLIP_NONE);
	SDL_RenderPresent(renderer);

	previousTime = currentTime;
}

bool Game::pollEvents()
{
	bool quit = false;
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			keys[event.key.keysym.scancode] = true;

			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				quit = true;
			}
			break;
		case SDL_KEYUP:
			keys[event.key.keysym.scancode] = false;
			break;
		case SDL_QUIT:
			quit = true;
			break;
		}

		onEvent(event);
	}

	return quit;
}

void Game::clearScreen(uint8_t r, uint8_t g, uint8_t b)
{
	SDL_SetRenderDrawColor(renderer, r, g, b, 0);
	SDL_RenderClear(renderer);
}

double Game::getDeltaTimeSeconds() const
{
	return deltaTimeSeconds;
}

void Game::drawSprite(const Sprite& sprite)
{
	Image* spriteSheet = assetDatabase.get<Image>(sprite.spriteSheetId);

	int x = (int)(sprite.x - (sprite.width / 2) * sprite.scale);
	int y = (int)(sprite.y - (sprite.height / 2) * sprite.scale);

	spriteSheet->drawFrame(x, y, sprite.width, sprite.height, sprite.frameIndex, sprite.rotation, sprite.scale, 255, 255, 255, renderer);
}

float Game::getRand()
{
	return (float)rand() / RAND_MAX;
}

int Game::getRand(int min, int max)
{
	return (int)(min + (max - min + 1) * getRand());
}

extern "C"
{
	//Handles Images
	int LoadImage(lua_State* state)
	{
		LoadParameters loadParameters;
		loadParameters.path = lua_tostring(state, 1);
		loadParameters.assetType = AssetImage;
		loadParameters.renderer = pGame->renderer;

		int imageId = pGame->assetDatabase.add(loadParameters);

		lua_pushinteger(state, imageId);
		return 1;
	}

	int DrawImage(lua_State* state)
	{
		int imageId = (int)lua_tointeger(state, 1);
		int frameId = (int)lua_tointeger(state, 2);
		int x = (int)lua_tonumber(state, 3);
		int y = (int)lua_tonumber(state, 4);
		int frameSize = (int)lua_tointeger(state, 5);

		Image* image = pGame->assetDatabase.get<Image>(imageId);

		image->drawFrame(x, y, frameSize, frameSize, frameId, 0.0f, 1.0f, 255, 255, 255, pGame->renderer);
		return 0;
	}

	int DrawImageRotateAndScale(lua_State* state)
	{
		int imageId = (int)lua_tointeger(state, 1);
		int frameId = (int)lua_tointeger(state, 2);
		int x = (int)lua_tonumber(state, 3);
		int y = (int)lua_tonumber(state, 4);
		int frameSize = (int)lua_tointeger(state, 5);
		double angle = (double)lua_tonumber(state, 6);
		double scale = (double)lua_tonumber(state, 7);

		Image* image = pGame->assetDatabase.get<Image>(imageId);

		image->drawFrame(x, y, frameSize, frameSize, frameId, angle, scale, 255, 255, 255, pGame->renderer);
		return 0;
	}

	//Handles Sounds
	int LoadSound(lua_State* state)
	{
		LoadParameters loadParameters;
		loadParameters.path = lua_tostring(state, 1);
		loadParameters.assetType = AssetSound;

		int soundId = pGame->assetDatabase.add(loadParameters);

		lua_pushinteger(state, soundId);
		return 1;
	}

	int PlaySound(lua_State* state)
	{
		int soundId = (int)lua_tointeger(state, 1);

		Sound* sound = pGame->assetDatabase.get<Sound>(soundId);
		sound->play();
		
		return 0;
	}

	int PlaySoundLooped(lua_State* state)
	{
		int soundId = (int)lua_tointeger(state, 1);
		int loops = (int)lua_tointeger(state, 2);

		Sound* sound = pGame->assetDatabase.get<Sound>(soundId);
		sound->playLooped(loops);
		
		return 0;
	}

	//Handles Music
	int LoadMusic(lua_State* state)
	{
		LoadParameters loadParameters;
		loadParameters.path = lua_tostring(state, 1);
		loadParameters.assetType = AssetMusic;

		int musicId = pGame->assetDatabase.add(loadParameters);

		lua_pushinteger(state, musicId);
		return 1;
	}

	int PlayMusic(lua_State* state)
	{
		int musicId = (int)lua_tointeger(state, 1);

		Music* music = pGame->assetDatabase.get<Music>(musicId);
		music->play();

		return 0;
	}

	//Handles Font
	int LoadFont(lua_State* state)
	{
		LoadParameters loadParameters;
		loadParameters.path = lua_tostring(state, 1);
		loadParameters.size = lua_tointeger(state, 2);
		loadParameters.assetType = AssetFont;

		int fontId = pGame->assetDatabase.add(loadParameters);

		lua_pushinteger(state, fontId);
		return 1;
	}

	int DrawText(lua_State* state)
	{
		int fontId = (int)lua_tointeger(state, 1);
		const char* text = lua_tostring(state, 2);
		
		int x = (int)lua_tonumber(state, 3);
		int y = (int)lua_tonumber(state, 4);

		uint8_t r = (uint8_t)lua_tointeger(state, 5);
		uint8_t g = (uint8_t)lua_tointeger(state, 6);
		uint8_t b = (uint8_t)lua_tointeger(state, 7);

		Font* font = pGame->assetDatabase.get<Font>(fontId);
		font->draw(text, x, y, r, g, b, pGame->renderer);

		return 0;
	}

	//Draw basic shapes
	int DrawLine(lua_State* state)
	{
		int x1 = (int)lua_tonumber(state, 1);
		int y1 = (int)lua_tonumber(state, 2);
		int x2 = (int)lua_tonumber(state, 3);
		int y2 = (int)lua_tonumber(state, 4);

		uint8_t r = (uint8_t)lua_tointeger(state, 5);
		uint8_t g = (uint8_t)lua_tointeger(state, 6);
		uint8_t b = (uint8_t)lua_tointeger(state, 7);

		SDL_SetRenderDrawColor(pGame->renderer, r, g, b, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawLine(pGame->renderer, x1, y1, x2, y2);

		return 0;
	}

	int DrawRect(lua_State* state)
	{
		int x = lua_tointeger(state, 1);
		int y = lua_tointeger(state, 2);
		int w = lua_tointeger(state, 3);
		int h = lua_tointeger(state, 4);

		uint8_t r = (uint8_t)lua_tointeger(state, 5);
		uint8_t g = (uint8_t)lua_tointeger(state, 6);
		uint8_t b = (uint8_t)lua_tointeger(state, 7);

		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;
		SDL_SetRenderDrawColor(pGame->renderer, r, g, b, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawRect(pGame->renderer, &rect);

		return 0;
	}

	int DrawFillRect(lua_State* state)
	{
		int x = lua_tointeger(state, 1);
		int y = lua_tointeger(state, 2);
		int w = lua_tointeger(state, 3);
		int h = lua_tointeger(state, 4);

		uint8_t r = (uint8_t)lua_tointeger(state, 5);
		uint8_t g = (uint8_t)lua_tointeger(state, 6);
		uint8_t b = (uint8_t)lua_tointeger(state, 7);

		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;
		SDL_SetRenderDrawColor(pGame->renderer, r, g, b, SDL_ALPHA_OPAQUE);
		SDL_RenderFillRect(pGame->renderer, &rect);

		return 0;
	}

	//Basic Game Engine Needs
	int ClearScreen(lua_State* state)
	{
		uint8_t r = (uint8_t)lua_tointeger(state, 1);
		uint8_t g = (uint8_t)lua_tointeger(state, 2);
		uint8_t b = (uint8_t)lua_tointeger(state, 3);

		pGame->clearScreen(r, g, b);

		return 0;
	}

	int	DebugLog(lua_State* state)
	{
		const char* error = lua_tostring(state, 1);

		SDL_Log(error);
		return 0;
	}

	int GetClientWidth(lua_State* state)
	{
		int width = pGame->getClientWidth();
		lua_pushinteger(state, width);

		return 1;
	}

	int GetClientHeight(lua_State* state)
	{
		int height = pGame->getClientHeight();
		lua_pushinteger(state, height);

		return 1;
	}

	int GetDeltaTime(lua_State* state)
	{
		double deltaTime = pGame->getDeltaTimeSeconds();
		lua_pushnumber(state, deltaTime);

		return 1;
	}

	int GetBaseAssetDirectory(lua_State* state)
	{
		string directory = pGame->assetDatabase.getBasePath();
		lua_pushstring(state, directory.c_str());
		
		return 1;
	}

	int SetWindowTitle(lua_State* state)
	{
		const char* title = lua_tostring(state, 1);
		SDL_SetWindowTitle(pGame->window, title);

		return 0;
	}

	//Handles Keys
	int IsKeyDown(lua_State* state)
	{
		int scancode = (int)lua_tointeger(state, 1);
		lua_pushboolean(state, keys[scancode]);
		return 1;
	}

	int IsKeyPressed(lua_State* state)
	{
		int scancode = (int)lua_tointeger(state, 1);
		bool is = keys[scancode];
		bool was = prevKeys[scancode];

		lua_pushboolean(state, is && !was);
		return 1;
	}

	int IsKeyReleased(lua_State* state)
	{
		int scancode = (int)lua_tointeger(state, 1);
		bool is = keys[scancode];
		bool was = prevKeys[scancode];

		lua_pushboolean(state, !is && was);
		return 1;
	}

} //End of extern C

void Game::initLua()
{
	pGame = this;

	luaState = luaL_newstate();
	luaL_openlibs(luaState);

	//register lua glue functions
	lua_register(luaState, "ClearScreen", ClearScreen);
	lua_register(luaState, "DebugLog", DebugLog);
	lua_register(luaState, "GetClientWidth", GetClientWidth);
	lua_register(luaState, "GetClientHeight", GetClientHeight);
	lua_register(luaState, "GetDeltaTime", GetDeltaTime);
	lua_register(luaState, "GetBaseAssetDirectory", GetBaseAssetDirectory);
	lua_register(luaState, "SetWindowTitle", SetWindowTitle);

	lua_register(luaState, "IsKeyDown", IsKeyDown);
	lua_register(luaState, "IsKeyPressed", IsKeyPressed);
	lua_register(luaState, "IsKeyReleased", IsKeyReleased);

	lua_register(luaState, "LoadImage", LoadImage);
	lua_register(luaState, "DrawImage", DrawImage);
	lua_register(luaState, "DrawImageRotateAndScale", DrawImageRotateAndScale);

	lua_register(luaState, "LoadSound", LoadSound);
	lua_register(luaState, "PlaySound", PlaySound);
	lua_register(luaState, "PlaySoundLooped", PlaySoundLooped);

	lua_register(luaState, "LoadMusic", LoadMusic);
	lua_register(luaState, "PlayMusic", PlayMusic);

	lua_register(luaState, "LoadFont", LoadFont);
	lua_register(luaState, "DrawText", DrawText);

	lua_register(luaState, "DrawLine", DrawLine);
	lua_register(luaState, "DrawRect", DrawRect);
	lua_register(luaState, "DrawFillRect", DrawFillRect);

	string pathToConfig = assetDatabase.getBasePath() + "config.lua";
	runScript(luaState, pathToConfig.c_str());
}