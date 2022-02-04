#pragma once

struct SDL_Renderer;
typedef unsigned char uint8_t;

enum AssetType
{
	AssetImage,
	AssetFont,
	AssetSound,
	AssetMusic,
	AssetUnknown
};

class LoadParameters
{
public:
	const char* path = nullptr;
	int size = 0;
	SDL_Renderer* renderer = nullptr;
	AssetType assetType = AssetUnknown;
};

class Asset
{
public:
	Asset() {};
	virtual ~Asset() {};

	Asset(const Asset&) = delete;
	Asset& operator= (const Asset&) = delete;

	virtual bool load(const LoadParameters& loadParameters) = 0;
	virtual void free() = 0;

protected:
	void* assetData = nullptr;
};

class Image : public Asset
{
public:
	bool load(const LoadParameters& loadParameters) override;
	void free() override;

	void draw(int x, int y, double angle, double scale, uint8_t r, uint8_t g, uint8_t b, SDL_Renderer* renderer);
	void drawFrame(int x, int y, int frameWidth, int frameHeight, int frame, double angle, double scale, uint8_t r, uint8_t g, uint8_t b, SDL_Renderer* renderer);
	int getWidth() const;
	int getHeight() const;

private:
	int width = 0;
	int height = 0;
};

class Font : public Asset
{
public:
	bool load(const LoadParameters& loadParameters) override;
	void free() override;
	void draw(const char* text, int x, int y, uint8_t r, uint8_t g, uint8_t b, SDL_Renderer* renderer);
};

class Music : public Asset
{
public:
	bool load(const LoadParameters& loadParameters) override;
	void free() override;
	void play();
	void halt();
};

class Sound : public Asset
{
public:
	bool load(const LoadParameters& loadParameters) override;
	void free() override;
	void play();
	void playLooped(int loops);
};