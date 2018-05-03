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
    virtual ~AutocutsEngine();
    void LoadModel(std::string modelFilename);
    void StartSolver();
    void StopSolver();
    bool SolverProgressed();

    void SetDelta(double delta);
    double GetDelta();
    void SetLambda(double lambda);
    double GetLambda();

    const Nan::Persistent<v8::Array>& GetModelVerticesArray();
    const Nan::Persistent<v8::Array>& GetModelFacesArray();
    const Nan::Persistent<v8::Array>& GetSolverVerticesArray();
    const Nan::Persistent<v8::Array>& GetSolverFacesArray();

    const Nan::Persistent<v8::Array>& GetBufferedModelVertices();
    const Nan::Persistent<v8::Array>& GetBufferedSolverVertices();

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
    static NAN_GETTER(ModelBufferedVerticesGet);
    static NAN_GETTER(SolverBufferedVerticesGet);

    FileType GetFilenameExtension(std::string fileName);
    std::string ToLower(std::string data);

    Eigen::MatrixXd modelVerticesMatrix;
    Eigen::MatrixXi modelFacesMatrix;
    Eigen::MatrixX2d solverVerticesMatrix;
    Eigen::MatrixXi solverFacesMatrix;

    Nan::Persistent<v8::Array> modelVerticesArray;
    Nan::Persistent<v8::Array> modelFacesArray;
    Nan::Persistent<v8::Array> solverVerticesArray;
    Nan::Persistent<v8::Array> solverFacesArray;

    Nan::Persistent<v8::Array> bufferedModelVertices;
    Nan::Persistent<v8::Array> bufferedSolverVertices;

    std::shared_ptr<SolverWrapper> solverWrapper;
    std::thread solverThread;
};