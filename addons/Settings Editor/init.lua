-- Settings Editor - Addon that can change global font face, font size, addon language (EN/JP).
--                   There are other global changes involved when changing the font face and size.

local core_mainmenu                    = require("core_mainmenu")
local lib_helpers                      = require("solylib.helpers")
local lib_theme_loaded, lib_theme      = pcall(require, "Theme Editor.theme")
local optionsLoaded, options           = pcall(require, "Settings Editor.options")
local addonName                        = "Settings Editor"
local addonHome                        = "addons/" .. addonName .. "/"
local optionsFileName                  = addonHome .. "options.lua"
local MIN_FONT_SIZE = 4
local MAX_FONT_SIZE = 48

local selectedFont                     -- index into customFontList
local selectedFontCJKMerge             -- index into customFontList for the CJK merge font
local customFontList                   -- list of fonts
local numFonts                         -- should be equal to table.getn(customFontList)
local display                          = false



 -- field, default
local _SettingsEditorDefaults = {
    {"language", "EN"},                        -- Current language so that it can be saved globally
    {"fontSize", 13},                          -- fontSize for main font
    {"fontSizeCJKMerge", 13},                  -- Chinese/Japanese/Korean glyph range font size if merging is on
    {"mergeFonts", false},                     -- merge the two fonts (fontName main, fontNameCJKMerge for CJK glyphs)
    {"fontName", "ProggyClean (Default).ttf"}, -- font used for basic Latin (and CJK if mergeFonts == false)
    {"fontNameCJKMerge", ""},                  -- font used for CJK glyph range only
    {"oversampleH", 1},                        -- be careful with this--can cause a big memory spike for some fonts
    {"oversampleV", 1},                        -- not actually used by imgui, but will remain here
    {"useCustomTheme", false},                 -- custom theme or not
    {"noResize", ""},
    {"noTitleBar", ""},
    {"noMove", ""},
    {"transparentWindow", false},
    {"fontScale", 1.0},
}

-- Return value restricted to the range
local function RestrictOptionsRange(val, min, max)
    if val < min then
        return min
    elseif val > max then
        return max
    end
    return val
end
-- Debugging
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
            
            io.write(string.format("\t%s = %s,\n", key, sval))
        end
        io.write("}\n")
        io.close(file)
    end		
end

local function SaveOptions(tbl, fileName)
    --print(addonName, "Saving Options")
    SaveTableToFile(tbl, fileName)	
end

local function SaveThisAddonsOptions()
    SaveOptions(options, optionsFileName)
end

local function SetAddonLanguage(lang)
     pso.set_language(lang)
end

-- Call the plugin function to prepare next frame
local function ChangeGlobalFont()
    local fontName1 = "./" .. addonHome .. "/" .. options.fontName
    local fontName2 = "./" .. addonHome .. "/" .. options.fontNameCJKMerge

    -- Need paths to the font files from the ROOT PSO directory
    pso.change_global_font(fontName1, options.fontSize, options.oversampleH, options.oversampleV, options.mergeFonts, fontName2, options.fontSizeCJKMerge)
end

local function GetFileExtension(file)
    return file:match("^.+(%..+)$")
end

local function CheckFileExtension(file, ext)
    return GetFileExtension(file) == ext
end

-- Load the font list. Supports only ttf for now because otf files 
-- don't work in imgui
local function LoadFontList()
    customFontList = {}
    numFonts = 0
    selectedFont = 1
    selectedFontCJKMerge = 1
    
    local filesInThisDir = pso.list_directory_files(addonName)
    for _, file in pairs(filesInThisDir) do
        -- Only care about ttf... Seems otf doesn't work in imgui
        if CheckFileExtension(file, ".ttf") then
            -- Add to table, increment the count, and check if this the saved font
            table.insert(customFontList, file)
            numFonts = numFonts + 1

            -- Check if this is one of the font combobox selections
            if options.fontName == file then
                selectedFont = numFonts
            end
            if options.fontNameCJKMerge == file then
                selectedFontCJKMerge = numFonts
            end
        end
    end
end

-- Settings used by every addon (language, font)
local function PresentGlobalSettings()
    local success
    local oversampleH -- imgui limits this (1,7)
    local oversampleV -- imgui actually doesn't use this ...
    local languageTable = pso.list_languages()
    local currLang = pso.get_language()
    local languageIndex = 1 -- default to EN which is first in the table
    local fontSize
    local fontSizeCJKMerge
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
        SetAddonLanguage(languageTable[languageIndex])
        options.language = pso.get_language()
        SaveThisAddonsOptions()
    end
    imgui.PopItemWidth()
    
    if imgui.TreeNodeEx("Global Font Settings", "DefaultOpen") then

        -- Main font
        imgui.PushItemWidth(imgui.GetWindowWidth() * 0.50)
        success, selectedFont = imgui.Combo("Font", selectedFont, customFontList, numFonts)
        if success then
            options.fontName = customFontList[selectedFont]
        end		
        imgui.PopItemWidth()
        

        -- Main font size
        imgui.PushItemWidth(imgui.GetWindowWidth() * 0.25)
        success, fontSize = imgui.InputFloat("Custom Font Size", options.fontSize)
        if success then
            options.fontSize = RestrictOptionsRange(fontSize, MIN_FONT_SIZE, MAX_FONT_SIZE)
        end
        imgui.PopItemWidth()

        -- Do we want a second font?
        if imgui.Checkbox("Merge Second Font?", options.mergeFonts) then
            options.mergeFonts = not options.mergeFonts
        end

        -- If second font is selected...
        if imgui.TreeNodeEx("Second Font Options") then
            imgui.PushItemWidth(imgui.GetWindowWidth() * 0.50)
            success, selectedFontCJKMerge = imgui.Combo("Second Font (CJK Range)", selectedFontCJKMerge, customFontList, numFonts)
            if success then
                options.fontNameCJKMerge = customFontList[selectedFontCJKMerge]
            end		
            imgui.PopItemWidth()

            imgui.PushItemWidth(imgui.GetWindowWidth() * 0.25)
            success, fontSizeCJKMerge = imgui.InputFloat("Custom Font Size (CJK Range)", options.fontSizeCJKMerge)
            if success then
                options.fontSizeCJKMerge = RestrictOptionsRange(fontSizeCJKMerge, MIN_FONT_SIZE, MAX_FONT_SIZE)
            end
            imgui.PopItemWidth()

            imgui.TreePop()
        end
        
        -- Horizontal oversampling
        imgui.PushItemWidth(imgui.GetWindowWidth() * 0.25)
        success, oversampleH = imgui.InputInt("Custom Font Oversampling (Horizontal)", options.oversampleH)
        if success then
            options.oversampleH = RestrictOptionsRange(oversampleH, 1, 7)
        end
        imgui.PopItemWidth()
        
        -- Imgui doesn't support vertical oversampling on fonts yet
        --imgui.PushItemWidth(imgui.GetWindowWidth() * 0.25)
        --success, oversampleV = imgui.InputInt("Custom Font Oversampling (Vertical)", options.oversampleV)
        --if success then
            --options.oversampleV = RestrictOptionsRange(oversampleV, 1, 7)
        --end
        --imgui.PopItemWidth()

        imgui.TreePop()
    end
end

-- Settings specific to this addon
local function PresentAddonSettings()
    local success
    
    if imgui.TreeNodeEx("This Addon's Configuration", "") then
        if imgui.Checkbox("No title bar", options.noTitleBar == "NoTitleBar") then
            if options.noTitleBar == "NoTitleBar" then
                options.noTitleBar = ""
            else
                options.noTitleBar = "NoTitleBar"
            end
        end
            
        if imgui.Checkbox("Transparent Window?", options.transparentWindow) then
            options.transparentWindow = not options.transparentWindow
        end
            
        if imgui.Checkbox("No Resize?", options.noResize == "NoResize") then
            if options.noResize == "NoResize" then
                options.noResize = ""
            else 
                options.noResize = "NoResize"
            end
        end
            
        if imgui.Checkbox("No Move?", options.noMove == "NoMove") then
            if options.noMove == "NoMove" then
                options.noMove = ""
            else 
                options.noMove = "NoMove"
            end
        end
        imgui.TreePop()
    end
end

local function PresentSaveButton()
    if imgui.Button("Save") then
        SaveThisAddonsOptions()
        ChangeGlobalFont()
    end
end

-- Present options/configuration window (this addon itself)
local function PresentSettingsWindow()
    PresentGlobalSettings()
    PresentAddonSettings()	
    PresentSaveButton()
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

-- Load font list, set the global font because it's saved here
LoadFontList()
ChangeGlobalFont()

-- Called from the plugin to display the addon
local function present()
    if not display then
        return
    end
    
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
        MIN_FONT_SIZE = MIN_FONT_SIZE,
        MAX_FONT_SIZE = MAX_FONT_SIZE,
        SetAddonLanguage = SetAddonLanguage,
    }
end

return 
{
    __addon = 
    {
        init = init,
    }
}
