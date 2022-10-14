
#pragma once

#include "raylib.h"
#include <vector>
#include <string>

typedef std::tuple<float, float, float> Point;

#define BUFFER_WIDTH 1000

// Use classes when:

// 1. The data is invariant (must be validated with internal logic)
// 2. The functionality to take on that object only will ever be taken on
//    that individual type of object.

// rules

// 1. always use private data. start with private get/set and update visibility
//    accordingly as needed.

// Only muses will be rasterized and exported, so these actions should
// belong to the muse class.

class Muse
{
public:
    // Constructors
    Muse(int _count, std::string _obj_file_path, std::string _tex_file_path);
    ~Muse();

    // Getters/Setters
    std::string getName();
    Model getModel();
    Texture2D getTexture();

    // Methods
    void rasterizeBuffer();
    void exportImage(std::string _filename);
    void exportAudio(std::string _filename);
    bool bufferReady();
    bool rasterize();
    void executePartialRender(int _current_thread, int _thread_faces_limit, int _face_index);
    std::vector<unsigned int> getAudioBuffer();

private:
    // Constructor
    Muse();

    // Properties
    std::string name;
    Model model;
    Texture2D model_texture;
    std::vector<unsigned int> audio_buffer;

    float min_distance_from_origin;
    float max_distance_from_origin;
    float min_max_distance_difference;

    // Getters/Setters
    void setName(std::string _name);
    void setModel(Model _model);

    // Methods
    void initMinMaxValues();
    float Muse::normalizeVector(std::tuple<float, float, float> vertex);
    float Muse::calculateRealMagnitude(float _temp_raw_mag);
    void setAudioBuffer(std::vector<unsigned int> _audio_buffer);
    void announce(std::string _text);
    void populateFaceRasterData(std::vector<unsigned int> &face, int face_index);
    unsigned int getFaceX1(int face_index);
    unsigned int getFaceY1(int face_index);
    unsigned int getFaceZ1(int face_index);
    unsigned int getFaceX2(int face_index);
    unsigned int getFaceY2(int face_index);
    unsigned int getFaceZ2(int face_index);
    unsigned int getFaceX3(int face_index);
    unsigned int getFaceY3(int face_index);
    unsigned int getFaceZ3(int face_index);
    unsigned int getCoordinateValue(int point_index, int dimension_index, int face_index);
    unsigned int getColorValue(int point_index, int face_index);
    float normalizeVector(std::tuple<float, float, float> vertex);
    void orderFace(std::vector<unsigned int> &face_data);
    void rasterizeFace(std::vector<unsigned int> &face);
    void rasterizeTop(Point &L, Point &M, Point &H);
    void RasterizeBottom(Point &L, Point &M, Point &H);
    float slope(Point &_u, Point &_v);
    float yIntercept(float _m, Point &_u);
    float xIntercept(int &_y, float &_b, float &_m);
    float interpolate(float _curr_x, float _curr_y, Point &_a_bound, Point &_b_bound);
    float distance2d(float _x1, float _x2, float _y1, float _y2);
    void rasterizeLine(int omega, Point &A, Point &B);
};