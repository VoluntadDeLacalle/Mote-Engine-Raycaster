MAP_WIDTH = 32
MAP_HEIGHT = 24

ColorRed = {200.0, 0.0, 0.0}
ColorGreen = {0.0, 200.0, 0.0}
ColorBlue = {0.0, 0.0, 200.0}
ColorPurple = {200.0, 0.0, 200.0}
ColorYellow = {200.0, 200.0, 0.0}

function LoadMap(path)
	io.input(path)
	
	while true do
		local n = io.read("*number")
		if n == nil then break end
		
		table.insert(map, n)
	end
	
	io.close()
end

function CheckInputs()
	if IsKeyPressed(SDL_SCANCODE_W) then
		if selectionY ~= 0 then
			selectionY = selectionY - 1
		end
	end
	
	if IsKeyPressed(SDL_SCANCODE_S) then
		if selectionY ~= MAP_HEIGHT - 1 then
			selectionY = selectionY + 1
		end
	end
	
	if IsKeyPressed(SDL_SCANCODE_A) then
		if selectionX ~= 0 then
			selectionX = selectionX - 1
		end
	end
	
	if IsKeyPressed(SDL_SCANCODE_D) then
		if selectionX ~= MAP_WIDTH - 1 then
			selectionX = selectionX + 1
		end
	end
	
	if IsKeyPressed(SDL_SCANCODE_TAB) then
		colorIndex = colorIndex + 1
		if colorIndex > 5 then
			colorIndex = 0
		end
	end
	
	if IsKeyPressed(SDL_SCANCODE_RETURN) then
		if (selectionY ~= 21 or selectionX ~= 10) then 
			io.open(path .. "demo.map", "w+"):close()
			local file = io.open(path .. "demo.map", "a")
			map[MAP_WIDTH * selectionY + selectionX + 1] = colorIndex
			
			for i = 1, #map do
				file:write(map[i], " ")
			end
			
			io.close(file)
		end
	end
end

function Start()
	map = {}
	path = GetBaseAssetDirectory()
	LoadMap(path .. "demo.map")
	
	SetWindowTitle("Raycast Map Editor")
	
	selectionX = 0
	selectionY = 0
	colorIndex = 0
end

function Update()
	CheckInputs()
end

function Draw()
	ClearScreen(0, 0, 0)
	
	for x = 1, math.floor(#map / MAP_WIDTH) do
	  for y = 1, math.floor(#map / MAP_HEIGHT) do
		DrawRect(y * 16, x * 16 + 100, 16, 16, 255, 255, 255)
			local currentColor = map[MAP_WIDTH * (x - 1) + y]
			
			if x == 22 and y == 11 then
				DrawFillRect(y * 16 + 1, x * 16 + 101, 15, 15, 0, 255, 255)
				DrawFillRect(y * 16 + 3, x * 16 + 103, 12, 12, 0, 0, 0)
				DrawFillRect(y * 16 + 5, x * 16 + 105, 8, 8, 0, 255, 255)
			end
			
			if currentColor == 1 then
				DrawFillRect(y * 16 + 1, x * 16 + 101, 15, 15, ColorRed[1], ColorRed[2], ColorRed[3])
			elseif currentColor == 2 then
				DrawFillRect(y * 16 + 1, x * 16 + 101, 15, 15, ColorGreen[1], ColorGreen[2], ColorGreen[3])
			elseif currentColor == 3 then 
				DrawFillRect(y * 16 + 1, x * 16 + 101, 15, 15, ColorBlue[1], ColorBlue[2], ColorBlue[3])
			elseif currentColor == 4 then
				DrawFillRect(y * 16 + 1, x * 16 + 101, 15, 15, ColorPurple[1], ColorPurple[2], ColorPurple[3])
			elseif currentColor == 5 then
				DrawFillRect(y * 16 + 1, x * 16 + 101, 15, 15, ColorYellow[1], ColorYellow[2], ColorYellow[3])
			end
	  end
	end
	
	
	DrawRect(selectionX * 16 + 17, selectionY * 16 + 117, 15, 15, 255, 255, 255)
	DrawLine(selectionX * 16 + 17, selectionY * 16 + 117, selectionX * 16 + 17 + 15, selectionY * 16 + 117 + 15, 255, 255, 255)
	DrawLine(selectionX * 16 + 17, selectionY * 16 + 117 + 15, selectionX * 16 + 17 + 15, selectionY * 16 + 117, 255, 255, 255)
	
	--Clear
	DrawRect(550, 150, 50, 50, 255, 255, 255)
	
	--Red
	DrawFillRect(650, 150, 50, 50, ColorRed[1], ColorRed[2], ColorRed[3])
	
	--Green
	DrawFillRect(550, 250, 50, 50, ColorGreen[1], ColorGreen[2], ColorGreen[3])
	
	--Blue
	DrawFillRect(650, 250, 50, 50, ColorBlue[1], ColorBlue[2], ColorBlue[3])
	
	--Yellow
	DrawFillRect(550, 350, 50, 50, ColorPurple[1], ColorPurple[2], ColorPurple[3])
	
	--Purple
	DrawFillRect(650, 350, 50, 50, ColorYellow[1], ColorYellow[2], ColorYellow[3])
	
	if colorIndex == 0 then
		DrawLine(575, 145, 565, 135, 255, 255, 255)
		DrawLine(575, 145, 585, 135, 255, 255, 255)
	elseif colorIndex == 1 then
		DrawLine(675, 145, 665, 135, 255, 255, 255)
		DrawLine(675, 145, 685, 135, 255, 255, 255)
	elseif colorIndex == 2 then
		DrawLine(575, 245, 565, 235, 255, 255, 255)
		DrawLine(575, 245, 585, 235, 255, 255, 255)
	elseif colorIndex == 3 then 
		DrawLine(675, 245, 665, 235, 255, 255, 255)
		DrawLine(675, 245, 685, 235, 255, 255, 255)
	elseif colorIndex == 4 then
		DrawLine(575, 345, 565, 335, 255, 255, 255)
		DrawLine(575, 345, 585, 335, 255, 255, 255)
	elseif colorIndex == 5 then
		DrawLine(675, 345, 665, 335, 255, 255, 255)
		DrawLine(675, 345, 685, 335, 255, 255, 255)
	end
end