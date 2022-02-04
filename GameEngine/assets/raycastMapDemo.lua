MAP_WIDTH = 32
MAP_HEIGHT = 24

ColorRed = {255.0, 0.0, 0.0}
ColorGreen = {0.0, 255.0, 0.0}
ColorBlue = {0.0, 0.0, 255.0}
ColorPurple = {255.0, 0.0, 255.0}
ColorYellow = {255.0, 255.0, 0.0}

function LoadMap(path)
	io.input(path)
	
	while true do
		local n = io.read("*number")
		if n == nil then break end
		
		table.insert(map, n)
	end
	
	io.close()
end

function WorldRender()
	local worldRender = {}
	
	for x = 1, SCREEN_WIDTH do
		local currentXPixel = {}
		
		local cameraX = 2.0 * x / tonumber(SCREEN_WIDTH) - 1
		local rayDirX = player.dirX + player.cameraPlaneX * cameraX
		local rayDirY = player.dirY + player.cameraPlaneY * cameraX
		
		local mapX = math.floor(player.x)
		local mapY = math.floor(player.y)
		
		local sideDistX = 0.0
		local sideDistY = 0.0
		
		local deltaDistX = 0.0
		if rayDirY == 0 then
			deltaDistX = 0.0
		else
			if rayDirX == 0 then
				deltaDistX = 1
			else
				deltaDistX = math.abs(1 / rayDirX)
			end
		end
		
		local deltaDistY = 0.0
		if rayDirX == 0 then
			deltaDistY = 0.0
		else
			if rayDirY == 0 then
				deltaDistY = 1
			else
				deltaDistY = math.abs(1 / rayDirY)
			end
		end
		
		local perpWallDist = 0.0
		
		local stepX = 0
		local stepY = 0
		
		local hit = 0
		local side = 0
		if rayDirX < 0 then
			stepX = -1
			sideDistX = (player.x - mapX) * deltaDistX
		else 
			stepX = 1
			sideDistX = (mapX + 1.0 - player.x) * deltaDistX
		end
		if rayDirY < 0 then
			stepY = -1
			sideDistY = (player.y - mapY) * deltaDistY
		else
			stepY = 1
			sideDistY = (mapY + 1.0 - player.y) * deltaDistY
		end
		
		while hit == 0 do
			if sideDistX < sideDistY then
				sideDistX = sideDistX + deltaDistX
				mapX = mapX + stepX
				side = 0
			else
				sideDistY = sideDistY + deltaDistY
				mapY = mapY + stepY
				side = 1
			end
			
			if map[MAP_WIDTH * mapX + mapY] > 0 then
				hit = 1
			end
		end
		
		if side == 0 then
			perpWallDist = (mapX - player.x + (1 - stepX) / 2) / rayDirX
		else
			perpWallDist = (mapY - player.y + (1 - stepY) / 2) / rayDirY
		end
		
		local lineHeight = math.floor(SCREEN_HEIGHT / perpWallDist)
		
		local drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2
		if drawStart < 0 then
			drawStart = 0
		end
		
		local drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2
		if drawEnd >= SCREEN_HEIGHT then
			drawEnd = SCREEN_HEIGHT - 1
		end
		
		
		if map[MAP_WIDTH * mapX + mapY] == 1 then
			currentXPixel.color = {ColorRed[1], ColorRed[2], ColorRed[3]}
		elseif map[MAP_WIDTH * mapX + mapY] == 2 then
			currentXPixel.color = {ColorGreen[1], ColorGreen[2], ColorGreen[3]}
		elseif map[MAP_WIDTH * mapX + mapY] == 3 then
			currentXPixel.color = {ColorBlue[1], ColorBlue[2], ColorBlue[3]}
		elseif map[MAP_WIDTH * mapX + mapY] == 4 then
			currentXPixel.color = {ColorPurple[1], ColorPurple[2], ColorPurple[3]}
		else
			currentXPixel.color = {ColorYellow[1], ColorYellow[2], ColorYellow[3]}
		end
		
		if side == 1 then
			currentXPixel.color[1] = math.floor(currentXPixel.color[1] / 2)
			currentXPixel.color[2] = math.floor(currentXPixel.color[2] / 2)
			currentXPixel.color[3] = math.floor(currentXPixel.color[3] / 2)
		end
		
			currentXPixel.x = x
			currentXPixel.drawStart = drawStart
			currentXPixel.drawEnd = drawEnd
			
			worldRender[#worldRender + 1] = currentXPixel
	end
	
	return worldRender
end

function CheckInputs()
	local dt = GetDeltaTime()

	if IsKeyDown(SDL_SCANCODE_W) then
		if map[MAP_WIDTH * math.floor(player.x + player.dirX * player.moveSpeed * dt) + math.floor(player.y)] == 0 then
			player.x = player.x + player.dirX * player.moveSpeed * dt
		end
		if map[MAP_WIDTH * math.floor(player.x) + math.floor(player.y + player.dirY * player.moveSpeed * dt)] == 0 then
			player.y = player.y + player.dirY * player.moveSpeed * dt
		end
	end
	
	if IsKeyDown(SDL_SCANCODE_S) then
		if map[MAP_WIDTH * math.floor(player.x - player.dirX * player.moveSpeed * dt) + math.floor(player.y)] == 0 then
			player.x = player.x - player.dirX * player.moveSpeed * dt
		end
		if map[MAP_WIDTH * math.floor(player.x) + math.floor(player.y + player.dirY * player.moveSpeed * dt)] == 0 then
			player.y = player.y - player.dirY * player.moveSpeed * dt
		end
	end
	
	if IsKeyDown(SDL_SCANCODE_A) then
		local oldDirX = player.dirX
		player.dirX = player.dirX * math.cos(player.rotSpeed * dt) - player.dirY * math.sin(player.rotSpeed * dt)
		player.dirY = oldDirX * math.sin(player.rotSpeed * dt) + player.dirY * math.cos(player.rotSpeed * dt)
		
		local oldPlaneX = player.cameraPlaneX
		player.cameraPlaneX = player.cameraPlaneX * math.cos(player.rotSpeed * dt) - player.cameraPlaneY * math.sin(player.rotSpeed * dt)
		player.cameraPlaneY = oldPlaneX * math.sin(player.rotSpeed * dt) + player.cameraPlaneY * math.cos(player.rotSpeed * dt)
	end
	
	if IsKeyDown(SDL_SCANCODE_D) then
		local oldDirX = player.dirX
		player.dirX = player.dirX * math.cos(-player.rotSpeed * dt) - player.dirY * math.sin(-player.rotSpeed * dt)
		player.dirY = oldDirX * math.sin(-player.rotSpeed * dt) + player.dirY * math.cos(-player.rotSpeed * dt)
		
		local oldPlaneX = player.cameraPlaneX
		player.cameraPlaneX = player.cameraPlaneX * math.cos(-player.rotSpeed * dt) - player.cameraPlaneY * math.sin(-player.rotSpeed * dt)
		player.cameraPlaneY = oldPlaneX * math.sin(-player.rotSpeed * dt) + player.cameraPlaneY * math.cos(-player.rotSpeed * dt)
	end
	
	if IsKeyPressed(SDL_SCANCODE_R) then
		player.x = 22.0
		player.y = 11.0
		
		player.dirX = -1.0
		player.dirY = 0.0
		
		player.cameraPlaneX = 0.0
		player.cameraPlaneY = 0.66
	end
	
end

function Start()
	SCREEN_WIDTH = GetClientWidth()
	SCREEN_HEIGHT = GetClientHeight()
	
	SetWindowTitle("Lua Raycast Map Demo")

	map = {}
	path = GetBaseAssetDirectory()
	LoadMap(path .. "demo.map")
	
	player = {}
	player.x = 22.0
	player.y = 11.0
	
	player.dirX = -1.0
	player.dirY = 0.0
	
	player.cameraPlaneX = 0.0
	player.cameraPlaneY = 0.66
	
	player.moveSpeed = 5.0
	player.rotSpeed = 3.0
	
	drawBuffer = {}
end

function Update()
	drawBuffer = WorldRender()
	CheckInputs()
end

function Draw()
	ClearScreen(0, 0, 0)
	
	for i = 1, #drawBuffer do
		DrawLine(drawBuffer[i].x, drawBuffer[i].drawStart, drawBuffer[i].x, drawBuffer[i].drawEnd, drawBuffer[i].color[1], drawBuffer[i].color[2], drawBuffer[i].color[3])
	end
end