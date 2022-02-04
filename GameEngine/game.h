#pragma once
#include "assetDatabase.h"
#include "sprite.h"

struct lua_State;
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
union SDL_Event;

class Game
{
public:
	bool init(int screenWidth, int screenHeight, bool fullscreen, bool vsync);
	void run();
	void shutdown();

	virtual void onEvent(SDL_Event& event);

	void clearScreen(uint8_t r, uint8_t g, uint8_t b);
	double getDeltaTimeSeconds() const;
	void drawSprite(const Sprite& sprite);
	float getRand();
	int getRand(int min, int max);

	int getClientWidth() const { return clientWidth; }
	int getClientHeight() const { return clientHeight; }

	AssetDatabase assetDatabase;
	SDL_Renderer* renderer = nullptr;
	SDL_Window* window = nullptr;

private:
	bool pollEvents();
	void beginFrame();
	void endFrame();
	void initLua();

	lua_State* luaState = nullptr;
	SDL_Texture* backbuffer = nullptr;
	uint64_t currentTime = 0;
	uint64_t previousTime = 0;
	double deltaTimeSeconds = 0;
	int clientWidth;
	int clientHeight;
};