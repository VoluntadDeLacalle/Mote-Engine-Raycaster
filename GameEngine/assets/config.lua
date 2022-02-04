baseAssetDirectory = GetBaseAssetDirectory()

--core scripts
dofile(baseAssetDirectory .. "scanCodes.lua")

--per-project scripts
dofile(baseAssetDirectory .. "raycastMapDemo.lua")