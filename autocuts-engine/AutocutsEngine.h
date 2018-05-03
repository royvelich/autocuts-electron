#include <nan.h>
#include <cmath>
#include <igl/readOFF.h>
#include <igl/readOBJ.h>
#include <algorithm>
#include <string>
#include <iostream>
#include <memory>

#include "Utils.h"
#include "SolverWrapper.h"

enum class FileType
{
    Obj,
    Off,
    Unknown
};

class AutocutsEngine : public Nan::ObjectWrap
{
  public:
    static NAN_MODULE_INIT(Init);
    static NAN_METHOD(LoadModel);
    static NAN_METHOD(StartSolver);
    static NAN_METHOD(StopSolver);
    static NAN_METHOD(SolverProgressed);

    AutocutsEngine();
    void LoadModel(std::string modelFilename);
    void StartSolver();
    void StopSolver();
    bool SolverProgressed();

    void SetDelta(double delta);
    double GetDelta();
    void SetLambda(double lambda);
    double GetLambda();

    const Eigen::MatrixXd& GetModelVertices();
    const Eigen::MatrixXi& GetModelFaces();
    const Eigen::MatrixX2d& GetSolverVertices();
    const Eigen::MatrixXi& GetSolverFaces();

  private:
    static NAN_METHOD(New);
    static NAN_METHOD(GetName);
    static NAN_GETTER(DeltaGet);
    static NAN_SETTER(DeltaSet);
    static NAN_GETTER(LambdaGet);
    static NAN_SETTER(LambdaSet);
    static NAN_GETTER(ModelVerticesGet);
    static NAN_GETTER(ModelFacesGet);
    static NAN_GETTER(SolverVerticesGet);
    static NAN_GETTER(SolverFacesGet);

    FileType GetFilenameExtension(std::string fileName);
    std::string ToLower(std::string data);

    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    Eigen::MatrixX2d Vs;
    Eigen::MatrixXi Fs;

    v8::Local<v8::Array> V_Array;
    v8::Local<v8::Array> F_Array;
    v8::Local<v8::Array> Vs_Array;
    v8::Local<v8::Array> Fs_Array;

    std::shared_ptr<SolverWrapper> solverWrapper;
    std::thread solverThread;
};