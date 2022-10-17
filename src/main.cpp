
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
#include <functional>
#include <ctime>

typedef enum {
    STATE_RASTERIZED = 0,
    STATE_NOT_RASTERIZED,
} UiRasterState;

typedef enum {
    STATE_NN = 0,
    STATE_LN,
    STATE_NR,
    STATE_LR,
} UiArrowState;

typedef enum {
    STATE_EMPTY = 0,
    STATE_NOT_EMPTY
} UiEmptyState;

std::map<size_t, Muse> muse_map;
std::map<size_t, Muse>::iterator current_muse;

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
static void ButtonConvert();
static void ButtonMuffin();
static void ButtonCastle();
static UiRasterState getUiRasterState();
static UiEmptyState getUiEmptyState();
static UiArrowState getUiArrowState();

// standalone functions should be pascal case

void LoadMuse(std::string _obj, std::string _tex) {

    time_t now = time(0);
    char* dt = ctime(&now);
    std::hash<std::string> hasher;
    size_t hash = hasher(std::string(dt));

    std::cout << std::to_string(hash) << std::endl;

    muse_map.insert(
        std::pair<size_t, Muse>(
            hash,
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
const char *button_convertText = "RASTERIZE";
const char *button_muffinText = "MUFFIN";
const char *button_castleText = "CASTLE";

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

                if(GuiButton((Rectangle){anchor01.x + 434, anchor01.y + 30, 60, 18 }, button_muffinText)) ButtonMuffin();
                if(GuiButton((Rectangle){anchor01.x + 434, anchor01.y + 54, 60, 18 }, button_castleText)) ButtonCastle();

                if (GuiTextBox((Rectangle){ anchor01.x + 8, anchor01.y + 48, 408, 24 }, import_model_inputText, 128, import_model_inputEditMode)) import_model_inputEditMode = !import_model_inputEditMode;
                GuiLabel((Rectangle){ anchor01.x + 8, anchor01.y + 78, 120, 24 }, import_texture_labelText);
                if (GuiTextBox((Rectangle){ anchor01.x + 8, anchor01.y + 108, 408, 24 }, import_texture_inputText, 128, import_texture_inputEditMode)) import_texture_inputEditMode = !import_texture_inputEditMode;
                if (GuiButton((Rectangle){ anchor01.x + 424, anchor01.y + 108, 80, 24 }, import_button_submitText)) ImportButtonSubmit(); 
            }

            GuiStatusBar((Rectangle){ 0, 497, 800, 20 }, status_barText);
            if (GuiButton((Rectangle){ 8, 464, 56, 24 }, button_importText)) ButtonImport();
            
            switch(getUiArrowState()){
                case UiArrowState::STATE_NN:
                    GuiDisable();
                    if (GuiButton((Rectangle){ anchor02.x + -72, anchor02.y + 0, 24, 24 }, button_leftText)) ButtonLeft(); 
                    if (GuiButton((Rectangle){ anchor02.x + 368, anchor02.y + 0, 24, 24 }, buton_rightText)) ButonRight(); 
                    GuiEnable();
                    break;
                case UiArrowState::STATE_LN:
                    if (GuiButton((Rectangle){ anchor02.x + -72, anchor02.y + 0, 24, 24 }, button_leftText)) ButtonLeft(); 
                    GuiDisable();
                    if (GuiButton((Rectangle){ anchor02.x + 368, anchor02.y + 0, 24, 24 }, buton_rightText)) ButonRight(); 
                    GuiEnable();
                    break;
                case UiArrowState::STATE_NR:
                    GuiDisable();
                    if (GuiButton((Rectangle){ anchor02.x + -72, anchor02.y + 0, 24, 24 }, button_leftText)) ButtonLeft(); 
                    GuiEnable();
                    if (GuiButton((Rectangle){ anchor02.x + 368, anchor02.y + 0, 24, 24 }, buton_rightText)) ButonRight(); 
                    break;
                case UiArrowState::STATE_LR:
                    if (GuiButton((Rectangle){ anchor02.x + -72, anchor02.y + 0, 24, 24 }, button_leftText)) ButtonLeft(); 
                    if (GuiButton((Rectangle){ anchor02.x + 368, anchor02.y + 0, 24, 24 }, buton_rightText)) ButonRight(); 
                    break;
            }

            switch(getUiEmptyState()){
                case UiEmptyState::STATE_EMPTY:
                    GuiDisable();
                    if (GuiButton((Rectangle){ anchor02.x + -40, anchor02.y + 0, 64, 24 }, button_deleteText)) ButtonDelete(); 
                    if (GuiButton((Rectangle){ anchor02.x + 32, anchor02.y + 0, 80, 24 }, button_convertText)) ButtonConvert();
                    GuiEnable();
                    break;
                case UiEmptyState::STATE_NOT_EMPTY:
                    if (GuiButton((Rectangle){ anchor02.x + -40, anchor02.y + 0, 64, 24 }, button_deleteText)) ButtonDelete(); 
                    if (GuiButton((Rectangle){ anchor02.x + 32, anchor02.y + 0, 80, 24 }, button_convertText)) ButtonConvert();
                    break;
            }

            switch(getUiRasterState()){
                case UiRasterState::STATE_RASTERIZED:
                    if (GuiButton((Rectangle){ anchor02.x + 120, anchor02.y + 0, 88, 24 }, button_export_ppmText)) ButtonExportPpm(); 
                    if (GuiButton((Rectangle){ anchor02.x + 216, anchor02.y + 0, 88, 24 }, button_export_wavText)) ButtonExportWav(); 
                    if (GuiButton((Rectangle){ anchor02.x + 312, anchor02.y + 0, 48, 24 }, button_playText)) ButtonPlay();
                    break;
                case UiRasterState::STATE_NOT_RASTERIZED:
                    GuiDisable();
                    if (GuiButton((Rectangle){ anchor02.x + 120, anchor02.y + 0, 88, 24 }, button_export_ppmText)) ButtonExportPpm(); 
                    if (GuiButton((Rectangle){ anchor02.x + 216, anchor02.y + 0, 88, 24 }, button_export_wavText)) ButtonExportWav(); 
                    if (GuiButton((Rectangle){ anchor02.x + 312, anchor02.y + 0, 48, 24 }, button_playText)) ButtonPlay(); 
                    GuiEnable();
                    break;
            }

            //----------------------------------------------------------------------------------
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    ClearMuses();
    CloseWindow();   // Close window and OpenGL context

    return 0;
}

static UiArrowState getUiArrowState() {
    if (muse_map.size() <= 1) {
        return UiArrowState::STATE_NN;
    } else if (current_muse == muse_map.begin()) {
        return UiArrowState::STATE_NR;
    } else if (std::next(current_muse, 1) == muse_map.end()) {
        return UiArrowState::STATE_LN;
    } else {
        return UiArrowState::STATE_LR;
    }
}

static UiEmptyState getUiEmptyState() {
    if (muse_map.empty()) {
        return UiEmptyState::STATE_EMPTY;
    } else {
        return UiEmptyState::STATE_NOT_EMPTY;
    }
}

static UiRasterState getUiRasterState() {
    if (current_muse->second.bufferReady()) {
        return UiRasterState::STATE_RASTERIZED;
    } else {
        return UiRasterState::STATE_NOT_RASTERIZED;
    }
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

    // assuming the above went well ->

    current_muse = muse_map.begin();
    strcpy(status_barText, ("Loaded model \"" + current_muse->second.getName() + "\". " + std::to_string(current_muse->second.getModel().meshes[0].vertexCount) + " Vertices, " + std::to_string(current_muse->second.getModel().meshes[0].triangleCount) + " Texels. Check console for any errors.").c_str());
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
            std::map<size_t, Muse>::iterator temp_it = std::next(current_muse, 1);
            if (temp_it == muse_map.end()) temp_it = std::prev(current_muse, 1);
            muse_map.erase(current_muse);
            current_muse = temp_it;
        }
        strcpy(status_barText, ("Model \"" + model_name + "\" deleted.").c_str());
    }
}

// currently this is happening in the ui thread, but this needs to happen elsewhere.
static void ButtonExportPpm()
{
    std::string model_name = current_muse->second.getName();
    strcpy(status_barText, ("Exporting " + model_name + " to image...").c_str());
    current_muse->second.exportImage(model_name);
    strcpy(status_barText, ("Exported model \"" + model_name + "\" to " + model_name + ".ppm.").c_str());
}

static void ButtonExportWav()
{
    std::string model_name = current_muse->second.getName();
    strcpy(status_barText, ("Exporting " + model_name + " to audio...").c_str());
    current_muse->second.exportAudio(model_name);
    strcpy(status_barText, ("Exported model \"" + model_name + "\" to " + model_name + ".wav.").c_str());
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

static void ButtonConvert()
{
    strcpy(status_barText, ("Rasterizing \"" + current_muse->second.getName() + "\"...").c_str());
    current_muse->second.rasterizeBuffer();
    strcpy(status_barText, ("Finished rasterizing \"" + current_muse->second.getName() + "\".").c_str());
}

static void ButtonMuffin()
{
    char default_model[128] = "resources/models/muffin/muffin.obj";
    char default_texture[128] = "resources/models/muffin/muffin.png";

    strcpy(status_barText, ("Importing model \"" + std::string(default_model) + "\"...").c_str());

    LoadMuse(
        default_model,
        default_texture
    );

    // assuming the above went well ->

    // current_muse = muse_map.find(std::to_string(muse_map.size() - 1));
    current_muse = muse_map.begin();
    strcpy(status_barText, ("Loaded model \"" + current_muse->second.getName() + "\". " + std::to_string(current_muse->second.getModel().meshes[0].vertexCount) + " Vertices, " + std::to_string(current_muse->second.getModel().meshes[0].triangleCount) + " Texels. Check console for any errors.").c_str());
    import_windowActive = false;
    std::cout << muse_map.size() << std::endl;
}

static void ButtonCastle()
{
    char default_model[128] = "resources/models/castle/castle.obj";
    char default_texture[128] = "resources/models/castle/castle_diffuse.png";

    strcpy(status_barText, ("Importing model \"" + std::string(default_model) + "\"...").c_str());

    LoadMuse(
        default_model,
        default_texture
    );

    // assuming the above went well ->

    // current_muse = muse_map.find(std::to_string(muse_map.size() - 1));
    current_muse = muse_map.begin();
    strcpy(status_barText, ("Loaded model \"" + current_muse->second.getName() + "\". " + std::to_string(current_muse->second.getModel().meshes[0].vertexCount) + " Vertices, " + std::to_string(current_muse->second.getModel().meshes[0].triangleCount) + " Texels. Check console for any errors.").c_str());
    import_windowActive = false;
    std::cout << muse_map.size() << std::endl;
}