#include "asset.h"

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

bool Image::load(const LoadParameters& loadParameters)
{
	assetData = nullptr;
	SDL_Surface* imageLoaded = IMG_Load(loadParameters.path);

	if (imageLoaded != nullptr)
	{
		if (loadParameters.renderer == nullptr)
		{
			return false;
		}

		SDL_SetColorKey(imageLoaded, SDL_TRUE, SDL_MapRGB(imageLoaded->format, 255, 128, 192));
		assetData = SDL_CreateTextureFromSurface(loadParameters.renderer, imageLoaded);

		if (assetData == nullptr)
		{
			return false;
		}

		SDL_FreeSurface(imageLoaded);
	}
	else
	{
		SDL_Log(IMG_GetError());
	}

	return !SDL_QueryTexture((SDL_Texture*)assetData, nullptr, nullptr, &width, &height);
}

void Image::free()
{
	SDL_DestroyTexture((SDL_Texture*)assetData);
	assetData = nullptr;
}

void Image::draw(int x, int y, double angle, double scale, uint8_t r, uint8_t g, uint8_t b, SDL_Renderer* renderer)
{
	SDL_Rect destRect;
	destRect.x = x;
	destRect.y = y;
	destRect.w = (int)(width * scale);
	destRect.h = (int)(height * scale);

	SDL_SetTextureColorMod((SDL_Texture*)assetData, r, g, b);
	SDL_RenderCopyEx(renderer, (SDL_Texture*)assetData, nullptr, &destRect, angle, nullptr, SDL_FLIP_NONE);
}

void Image::drawFrame(int x, int y, int frameWidth, int frameHeight, int frame, double angle, double scale, uint8_t r, uint8_t g, uint8_t b, SDL_Renderer* renderer)
{
	SDL_Rect destRect;
	destRect.x = x;
	destRect.y = y;
	destRect.w = (int)(frameWidth * scale);
	destRect.h = (int)(frameHeight * scale);

	int columns = width / frameWidth;

	SDL_Rect srcRect;
	srcRect.x = (frame % columns) * frameWidth;
	srcRect.y = (frame / columns) * frameHeight;
	srcRect.w = frameWidth;
	srcRect.h = frameHeight;

	SDL_SetTextureColorMod((SDL_Texture*)assetData, r, g, b);
	SDL_RenderCopyEx(renderer, (SDL_Texture*)assetData, &srcRect, &destRect, angle, nullptr, SDL_FLIP_NONE);
}

int Image::getWidth() const
{
	return width;
}

int Image::getHeight() const
{
	return height;
}

bool Font::load(const LoadParameters& loadParameters)
{
	assetData = TTF_OpenFont(loadParameters.path, loadParameters.size);
	return assetData != nullptr;
}

void Font::free()
{
	TTF_CloseFont((TTF_Font*)assetData);
	assetData = nullptr;
}

void Font::draw(const char* text, int x, int y, uint8_t r, uint8_t g, uint8_t b, SDL_Renderer* renderer)
{
	SDL_Surface* textDestination = nullptr;

	SDL_Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = 255;

	textDestination = TTF_RenderText_Solid((TTF_Font*)assetData, text, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textDestination);

	SDL_Rect destRect;

	destRect.x = x;
	destRect.y = y;
	destRect.w = textDestination->w;
	destRect.h = textDestination->h;

	SDL_RenderCopy(renderer, texture, nullptr, &destRect);

	SDL_FreeSurface(textDestination);
	SDL_DestroyTexture(texture);
}

bool Music::load(const LoadParameters& loadParameters)
{
	assetData = Mix_LoadMUS(loadParameters.path);
	return assetData != nullptr;
}

void Music::free()
{
	Mix_FreeMusic((Mix_Music*)assetData);
	assetData = nullptr;
}

void Music::play()
{
	Mix_PlayMusic((Mix_Music*)assetData, -1);
}

void Music::halt()
{
	Mix_HaltMusic();
}

bool Sound::load(const LoadParameters& loadParameters)
{
	assetData = Mix_LoadWAV(loadParameters.path);
	return assetData != nullptr;
}

void Sound::free()
{
	Mix_FreeChunk((Mix_Chunk*)assetData);
	assetData = nullptr;
}

void Sound::play()
{
	Mix_PlayChannel(-1, (Mix_Chunk*)assetData, 0);
}

void Sound::playLooped(int loops)
{
	Mix_PlayChannel(-1, (Mix_Chunk*)assetData, loops);
}