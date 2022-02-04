#pragma once

#include <string>
#include <map>
#include "asset.h"

class AssetDatabase
{
public:
	int add(LoadParameters& loadParameters);
	void clear();
	void setBasePath(const char* path);
	std::string getBasePath() const;

	template <typename T>
	T* get(int id)
	{
		return (T*)assets[id];
	}

private:
	int nextId = 0;
	std::string basePath = "assets/";
	std::map<int, Asset*> assets{};
	std::map<std::string, int> assetPaths{};
};
