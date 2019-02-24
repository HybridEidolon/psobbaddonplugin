-- Settings Editor - Addon that can change global font face, font size, addon language (EN/JP).
--                   There are other global changes involved when changing the font face and size.

local core_mainmenu                    = require("core_mainmenu")
local lib_helpers                      = require("solylib.helpers")
local lib_theme_loaded, lib_theme      = pcall(require, "Theme Editor.theme")
local optionsLoaded, options           = pcall(require, "Settings Editor.options")
local addonName                        = "Settings Editor"
local addonHome                        = "addons/" .. addonName .. "/"
local optionsFileName                  = addonHome .. "options.lua"

local selectedFont                     -- index into customFontList
local customFontList                   -- list of fonts
local numFonts                         -- should be equal to table.getn(customFontList)
local display                          = false
local globalOptionsChanged             = false -- all addons 
local addonOptionsChanged              = false -- just this addon


 -- field, default
local _SettingsEditorDefaults = {
	{"language", "EN"},
	{"fontSize", 13},
	{"fontName", "ProggyClean (Default).ttf"},
	{"oversampleH", 1},
	{"oversampleV", 1},
	{"useCustomTheme", false},
	{"fontScale", 1.0},
	{"noResize", ""},
	{"noTitleBar", ""},
	{"noMove", ""},
	{"transparentWindow", false},
	{"refreshTimer", 0},
}

local function PrintOptions()
	for key, val in pairs(options) do
		print(tostring(key), tostring(val))
	end
end

local function SaveTableToFile(tbl, fileName)
	local file = io.open(fileName, "w")
	if file ~= nil then
		io.output(file)
		io.write("return\n")
		io.write("{\n")
		for key, val in pairs(tbl) do
			local skey
			local ktype = type(key)			
			local sval
			local vtype = type(val)
			
			-- Quote or convert if necessary. No table handling here because
			-- options table isn't nested (yet).
			if     vtype == "string"  then sval = string.format("%q", val)
			elseif vtype == "number"  then sval = string.format("%s", val)
			elseif vtype == "boolean" then sval = tostring(val) 
			end
			
			io.write(string.format("%s = %s,\n", key, sval))
		end
		io.write("}\n")
		io.close(file)
	end		
end

local function SaveOptions(tbl, fileName)
	SaveTableToFile(tbl, fileName)	
end

-- Setup the options table 
if optionsLoaded then
	for _, opt in pairs(_SettingsEditorDefaults) do
		options[opt[1]] = lib_helpers.NotNilOrDefault(options[opt[1]], opt[2])
	end
	pso.set_language(options.language)
else
	options = {}
	for _, opt in pairs(_SettingsEditorDefaults) do
		options[opt[1]] = opt[2]
	end
	
	-- We just created the options, so we should save to have valid file
	SaveOptions(options, optionsFileName) 
end

-- Call the plugin function to prepare next frame
local function ChangeGlobalFont()
	pso.change_global_font("./" .. addonHome .. "/" .. options.fontName, options.fontSize, options.oversampleH, options.oversampleV)
end

local function GetFileExtension(file)
	return file:match("^.+(%..+)$")
end

local function CheckFileExtension(file, ext)
    return GetFileExtension(file) == ext
end

local function LoadFontList()
	customFontList = {}
	numFonts = 0
	selectedFont = 1
	
	local filesInThisDir = pso.list_directory_files(addonName)
	for _, file in pairs(filesInThisDir) do
		-- Only care about ttf and otf, nothing else.
		if CheckFileExtension(file, ".ttf") or CheckFileExtension(file, ".otf") then
			-- Add to table, increment the count, and check if this the saved font
			table.insert(customFontList, file)
			numFonts = numFonts + 1
			if options.fontName == file then
				selectedFont = numFonts
			end
		end
	end
end

local function PresentGlobalSettings()
	local success
	local oversampleH -- imgui limits this (1,7)
	local oversampleV -- imgui actually doesn't use this ...
	local languageTable = pso.list_languages()
	local currLang = pso.get_language()
	local languageIndex = 1 -- default to EN which is first in the table
	local fontSize
	local count = 1
	
	for _,str in pairs(languageTable) do
		if str == currLang then
			languageIndex = count
		end
		count = count + 1
	end
	
	imgui.PushItemWidth(imgui.GetWindowWidth() * 0.25)
	success, languageIndex = imgui.Combo("Language", languageIndex, languageTable, table.getn(languageTable))
    if success then
		pso.set_language(languageTable[languageIndex])
		options.language = pso.get_language() 
		globalOptionsChanged = true
	end
	imgui.PopItemWidth()
	
	if imgui.TreeNodeEx("Custom Font Settings", "DefaultOpen") then
		imgui.PushItemWidth(imgui.GetWindowWidth() * 0.50)
		success, selectedFont = imgui.Combo("Font", selectedFont, customFontList, numFonts)
		if success then
			options.fontName = customFontList[selectedFont]
			globalOptionsChanged = true
		end		
		imgui.PopItemWidth()
		
		imgui.PushItemWidth(imgui.GetWindowWidth() * 0.25)
		success, fontSize = imgui.InputFloat("Custom Font Size", options.fontSize)
		if success and fontSize > 0 then
			options.fontSize = fontSize
			globalOptionsChanged = true
		end
		
		success, oversampleH = imgui.InputInt("Custom Font Oversampling (Horizontal)", options.oversampleH)
		if success and oversampleH > 0 and oversampleH < 8 then
			options.oversampleH = oversampleH
			globalOptionsChanged = true
		end
		
		success, oversampleV = imgui.InputInt("Custom Font Oversampling (Vertical)", options.oversampleV)
		if success and oversampleV > 0 and oversampleV < 8 then
			options.oversampleV = oversampleV
			globalOptionsChanged = true
		end
		imgui.PopItemWidth()
		imgui.TreePop()
	end
end

local function PresentAddonSettings()
	local success
	local refresh 
	
	if imgui.TreeNodeEx("This Addon's Configuration", "") then
		imgui.PushItemWidth(imgui.GetWindowWidth() * 0.25)
		success, refresh = imgui.InputInt("Font Change Delay (Seconds)", options.refreshTimer)
		if success and refresh >= 0 and refresh <= 10 then
			options.refreshTimer = refresh
			addonOptionsChanged = true
			globalOptionsChanged = true -- didn't change but we might be in  timer already
		end
		imgui.PopItemWidth()
		
		if imgui.Checkbox("No title bar", options.noTitleBar == "NoTitleBar") then
			if options.noTitleBar == "NoTitleBar" then
				options.noTitleBar = ""
			else
				options.noTitleBar = "NoTitleBar"
			end
            addonOptionsChanged = true
		end
			
		if imgui.Checkbox("Transparent Window?", options.transparentWindow) then
			options.transparentWindow = not options.transparentWindow
			addonOptionsChanged = true
		end
			
		if imgui.Checkbox("No Resize?", options.noResize == "NoResize") then
			if options.noResize == "NoResize" then
				options.noResize = ""
			else 
				options.noResize = "NoResize"
			end
			addonOptionsChanged = true
		end
			
		if imgui.Checkbox("No Move?", options.noMove == "NoMove") then
			if options.noMove == "NoMove" then
				options.noMove = ""
			else 
				options.noMove = "NoMove"
			end
			addonOptionsChanged = true
		end
		imgui.TreePop()
	end
end

local function PresentSettingsWindow()
	PresentGlobalSettings()
	PresentAddonSettings()	
end

-- Load font list, set the global font because it's saved here
LoadFontList()
ChangeGlobalFont()

-- Frame timer to prevent every single input change holding up the client
local saveCounter = 0
local function CheckSaveOptionsCounter()
	if saveCounter > 0 then
		saveCounter = saveCounter - 1
		if saveCounter == 0 then
			SaveOptions(options, optionsFileName)
			ChangeGlobalFont()
		end
	end
end

-- Start the timer in # frames
local function StartSaveOptionsCounter()
	saveCounter = options.refreshTimer * 30
end

-- Called from the plugin to display the addon
local function present()
	CheckSaveOptionsCounter()
	if not display then
		return
	end
	
	if addonOptionsChanged then
		-- Always save these immediately
		SaveOptions(options, optionsFileName)
	end
	
	if globalOptionsChanged then
		-- These take a while to change, so let the user wait a second
		-- in case they were typing out something like "20" for fontSize.
		-- Really don't want the addon to immediately change font to 
		-- fontSize 2 and make the user wait before they can finish typing 20.
		StartSaveOptionsCounter()
	end

	globalOptionsChanged = false
	addonOptionsChanged = false
	
	if options.transparentWindow == true then
		imgui.PushStyleColor("WindowBg", 0.0, 0.0, 0.0, 0.0)
    end
	
	local success
	success, display = imgui.Begin("Settings Editor", display, {options.noTitleBar, options.noResize, options.noMove})
	imgui.SetWindowFontScale(options.fontScale)

	PresentSettingsWindow()
	
	imgui.End()
end

local function mainMenuButtonHandler()
	display = not display
end

local function init()
	core_mainmenu.add_button("Settings Editor", mainMenuButtonHandler)
	
	return 
	{
		name = 'Settings Editor',
		version = '1.0',
		author = 'Ender',
		present = present,
		toggleable = true,
		LoadTranslationDictionary = LoadTranslationDictionary,
	}
end

return 
{
	__addon = 
	{
		init = init,
	}
}
