#include "assetDatabase.h";
#include <assert.h>
using namespace std;

int AssetDatabase::add(LoadParameters& loadParameters)
{
	map<string, int>::iterator it;

	string fullPath = string(basePath).append(loadParameters.path);
	loadParameters.path = fullPath.c_str();

	if (loadParameters.assetType == AssetFont)
	{
		string mangledFontPath = string(loadParameters.path).append("_" + to_string(loadParameters.size));
		it = assetPaths.find(mangledFontPath);
	}
	else
	{
		it = assetPaths.find(fullPath);
	}

	if (it != assetPaths.end())
	{
		return it->second;
	}

	Asset* asset = nullptr;

	switch (loadParameters.assetType)
	{
	case AssetImage:
		asset = new Image();
		break;
	case AssetFont:
		asset = new Font();
		break;
	case AssetSound:
		asset = new Sound();
		break;
	case AssetMusic:
		asset = new Music();
		break;
	default:
		assert(false && "No handler defined for this asset type.");
	}

	int id = ++nextId;
	assets[id] = asset;

	bool result = asset->load(loadParameters);

	if (!result)
	{
		return -1;
	}

	assert(result);

	if (loadParameters.assetType == AssetFont)
	{
		string mangledFontPath = string(loadParameters.path).append("_" + to_string(loadParameters.size));
		assetPaths[mangledFontPath] = id;
	}
	else
	{
		assetPaths[loadParameters.path] = id;
	}

	return id;
}

void AssetDatabase::clear()
{
	for (auto& assetPair : assets)
	{
		assetPair.second->free();
		delete assetPair.second;
	}

	assets.clear();
	assetPaths.clear();
}

void AssetDatabase::setBasePath(const char* path)
{
	basePath = string(path);
}

std::string AssetDatabase::getBasePath() const
{
	return basePath;
}