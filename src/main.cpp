
/*******************************************************************************************
*
*   raylib [models] example - Models loading
*
*   NOTE: raylib supports multiple models file formats:
*
*     - OBJ  > Text file format. Must include vertex position-texcoords-normals information,
*              if files references some .mtl materials file, it will be loaded (or try to).
*     - GLTF > Text/binary file format. Includes lot of information and it could
*              also reference external files, raylib will try loading mesh and materials data.
*     - IQM  > Binary file format. Includes mesh vertex data but also animation data,
*              raylib can load .iqm animations.
*     - VOX  > Binary file format. MagikaVoxel mesh format:
*              https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox.txt
*     - M3D  > Binary file format. Model 3D format:
*              https://bztsrc.gitlab.io/model3d
*
*   Example originally created with raylib 2.0, last time updated with raylib 4.2
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "muse.h"
#include <iostream>
#include <map>
#include <string>

std::map<std::string, Muse> muse_map;
std::map<std::string, Muse>::iterator current_muse;
Muse *muse_queue[3];

//----------------------------------------------------------------------------------
// Controls Functions Declaration
//----------------------------------------------------------------------------------
static void ButtonImport();
static void ImportButtonSubmit();
static void ButtonLeft();
static void ButtonDelete();
static void ButtonExportPpm();
static void ButtonExportWav();
static void ButonRight();
static void ButtonPlay();

// standalone functions should be pascal case

void LoadMuse(std::string _obj, std::string _tex) {
    muse_map.insert(
        std::pair<std::string, Muse>(
            std::to_string(muse_map.size()),
            Muse(muse_map.size(), _obj, _tex)
        )
    );
}

void UnloadMuse(Muse _muse) {
    UnloadTexture(_muse.getTexture());
    UnloadModel(_muse.getModel());
}

void ClearMuses() {
    for(auto muse : muse_map) {
        UnloadMuse(muse.second);
    }
    muse_map.clear();
}

// Initialization
//--------------------------------------------------------------------------------------
const int screenWidth = 800;
const int screenHeight = 513;

char status_barText[128] = "";
const char *button_importText = "IMPORT";
const char *import_windowText = "IMPORT";
const char *import_model_labelText = "MODEL FILE";
const char *import_texture_labelText = "TEXTURE FILE";
const char *import_button_submitText = "IMPORT";
const char *button_leftText = "<<";
const char *button_deleteText = "DELETE";
const char *button_export_ppmText = "EXPORT PPM";
const char *button_export_wavText = "EXPORT WAV";
const char *buton_rightText = ">>";
const char *button_playText = "PLAY";

Vector2 anchor01 = { 152, 208 };
Vector2 anchor02 = { 232, 464 };

bool import_windowActive = false;
bool import_model_inputEditMode = false;
char import_model_inputText[128] = "";
bool import_texture_inputEditMode = false;
char import_texture_inputText[128] = "";

int main(void)
{
    InitWindow(screenWidth, screenHeight, "Muser");

    // Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = (Vector3){ 50.0f, 0.0f, 0.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };     // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;                   // Camera mode type

    current_muse = muse_map.end();

    SetCameraMode(camera, CAMERA_FREE);     // Set a free camera mode
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    Vector3 position = { 0.0f, 0.0f, 0.0f };                    // Set model position

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateCamera(&camera);

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))); 
            BeginMode3D(camera);

                if(!muse_map.empty()){
                    DrawModel(current_muse->second.getModel(), position, 1.0f, WHITE);        // Draw 3d model with texture
                }

            EndMode3D();

            // raygui: controls drawing
            //----------------------------------------------------------------------------------
            if (import_windowActive)
            {
                import_windowActive = !GuiWindowBox((Rectangle){ anchor01.x + 0, anchor01.y + 0, 512, 140 }, import_windowText);
                GuiLabel((Rectangle){ anchor01.x + 8, anchor01.y + 24, 120, 24 }, import_model_labelText);
                if (GuiTextBox((Rectangle){ anchor01.x + 8, anchor01.y + 48, 408, 24 }, import_model_inputText, 128, import_model_inputEditMode)) import_model_inputEditMode = !import_model_inputEditMode;
                GuiLabel((Rectangle){ anchor01.x + 8, anchor01.y + 78, 120, 24 }, import_texture_labelText);
                if (GuiTextBox((Rectangle){ anchor01.x + 8, anchor01.y + 108, 408, 24 }, import_texture_inputText, 128, import_texture_inputEditMode)) import_texture_inputEditMode = !import_texture_inputEditMode;
                if (GuiButton((Rectangle){ anchor01.x + 424, anchor01.y + 108, 80, 24 }, import_button_submitText)) ImportButtonSubmit(); 
            }
            GuiStatusBar((Rectangle){ 0, 497, 800, 20 }, status_barText);
            if (GuiButton((Rectangle){ anchor02.x + -224, anchor02.y + 0, 56, 24 }, button_importText)) ButtonImport(); 
            if (GuiButton((Rectangle){ anchor02.x + 0, anchor02.y + 0, 24, 24 }, button_leftText)) ButtonLeft(); 
            if (GuiButton((Rectangle){ anchor02.x + 32, anchor02.y + 0, 64, 24 }, button_deleteText)) ButtonDelete(); 
            if (GuiButton((Rectangle){ anchor02.x + 104, anchor02.y + 0, 88, 24 }, button_export_ppmText)) ButtonExportPpm(); 
            if (GuiButton((Rectangle){ anchor02.x + 200, anchor02.y + 0, 88, 24 }, button_export_wavText)) ButtonExportWav(); 
            if (GuiButton((Rectangle){ anchor02.x + 352, anchor02.y + 0, 24, 24 }, buton_rightText)) ButonRight(); 
            if (GuiButton((Rectangle){ anchor02.x + 296, anchor02.y + 0, 48, 24 }, button_playText)) ButtonPlay(); 
            //----------------------------------------------------------------------------------
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    ClearMuses();
    CloseWindow();   // Close window and OpenGL context

    return 0;
}

//------------------------------------------------------------------------------------
// Controls Functions Definitions (local)
//------------------------------------------------------------------------------------
static void ButtonImport()
{
    import_windowActive = true;
}

static void ImportButtonSubmit()
{
    strcpy(status_barText, ("Importing model \"" + std::string(import_model_inputText) + "\"...").c_str());

    LoadMuse(
        import_model_inputText,
        import_texture_inputText
    );

    current_muse = muse_map.find(std::to_string(muse_map.size() - 1));

    strcpy(status_barText, ("Loaded model \"" + current_muse->second.getName() + "\". Check console for any errors.").c_str());
    import_windowActive = false;
}

static void ButtonLeft()
{
    if(muse_map.size() > 1) {
        current_muse = std::prev(current_muse, 1);
        strcpy(status_barText, ("Model \"" + current_muse->second.getName() + "\" loaded.").c_str());
    }
}

static void ButtonDelete()
{
    if(muse_map.empty()) {
        return;
    } else {
        std::string model_name = current_muse->second.getName();
        if(muse_map.size() == 1) {
            muse_map.erase(current_muse);
            current_muse = muse_map.end();
        } else {
            std::map<std::string, Muse>::iterator temp_it = std::next(current_muse, 1);
            muse_map.erase(current_muse);
            current_muse = temp_it;
        }
        strcpy(status_barText, ("Model \"" + model_name + "\" deleted.").c_str());
    }
}

static void ButtonExportPpm()
{
    // TODO: Implement control logic
}

static void ButtonExportWav()
{
    // TODO: Implement control logic
}

static void ButonRight()
{
    if(!muse_map.empty()) {
        if(current_muse != std::prev(muse_map.end(), 1)){
            current_muse = std::next(current_muse, 1);
            strcpy(status_barText, ("Model \"" + current_muse->second.getName() + "\" loaded.").c_str());
        }
    }
}

static void ButtonPlay()
{
    // TODO: Implement control logic
}

