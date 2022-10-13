
#pragma once

#include "raylib.h"
#include <vector>
#include <string>

// Use classes when:

// 1. The data is invariant (must be validated with internal logic)
// 2. The functionality to take on that object only will ever be taken on
//    that individual type of object.

// rules

// 1. always use private data. start with private get/set and update visibility
//    accordingly as needed.

// Only muses will be rasterized and exported, so these actions should
// belong to the muse class.

class Muse {
public:

    Muse(int _count, std::string _obj_file_path, std::string _tex_file_path);
    ~Muse();

    void rasterizeBuffer();
    void exportImage(std::string _filename);
    void exportAudio(std::string _filename);

    Model getModel();
    Texture2D getTexture();
    std::string getName();

    bool bufferReady();

private:

    Muse();

    std::string name;
    void setName(std::string _name);

    Model model;
    void setModel(Model _model);

    Texture2D model_texture;

    std::vector<unsigned int> audio_buffer;
    std::vector<unsigned int> getAudioBuffer();
    void setAudioBuffer(std::vector<unsigned int> _audio_buffer);

    void announce(std::string _text);
};