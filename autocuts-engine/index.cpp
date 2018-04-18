#include <cmath>
#include <nan.h>
#include <igl/readOFF.h>
#include <igl/readOBJ.h>
#include <algorithm>
#include <string>
#include <iostream>

#include "Utils.h"
#include "SolverWrapper.h"

SolverWrapper* solver_wrapper;
thread solver_thread;

enum class Param 
{ 
	LAMBDA,
	DELTA,
	BOUND,
	POSITION_WEIGHT
};

enum class FileType
{ 
	Obj,
	Off,
	Unknown
};

std::string ToLower(std::string data)
{
	transform(data.begin(), data.end(), data.begin(), ::tolower);
	return data;
}

FileType GetFilenameExtension(std::string fileName)
{
	std::string fileExtension = ToLower(fileName.substr(fileName.find_last_of(".") + 1));
	if(fileExtension == "obj")
	{
		return FileType::Obj;
	}
	else if(fileExtension == "off")
	{
		return FileType::Off;
	}

	return FileType::Unknown;
}

NAN_METHOD(loadMeshWithSoup)
{
	Eigen::MatrixXd V;
	Eigen::MatrixXi originalF;

	// Validate input arguments
	if (!info[0]->IsString()) 
	{
		Nan::ThrowTypeError("Argument should be a string");
		return;
	}

	// Get mesh file type
	v8::String::Utf8Value filePathV8(info[0]->ToString());
	auto filePath = std::string(*filePathV8);
	auto fileType = GetFilenameExtension(filePath);

	// Read mesh file
	switch (fileType)
	{
	case FileType::Obj:
		igl::readOBJ(filePath, V, originalF);
		break;

	case FileType::Off:
		igl::readOFF(filePath, V, originalF);
		break;

	default:
		Nan::ThrowTypeError("Unknown mesh type");
	}

	
	// If faces were received as quads, transform them into triangles
	MatX3i F;
	if (originalF.cols() == 4)
	{
		F = MatX3i(originalF.rows() * 2, 3);
		Vec4i face;
		for (int i = 0; i < originalF.rows(); ++i)
		{
			face = originalF.row(i);
			F.row(2 * i) << face[0], face[1], face[3];
			F.row(2 * i + 1) << face[1], face[2], face[3];
		}
	}
	else
	{
		F = originalF;
	}

	// Translate all points such that their mean will be at the origin
	V.col(0).array() -= V.col(0).mean();
	V.col(1).array() -= V.col(1).mean();
	V.col(2).array() -= V.col(2).mean();

	// Get the one-dimensional distance between the two most distant points on each axis
	double xDist = V.col(0).maxCoeff() - V.col(0).minCoeff();
	double yDist = V.col(1).maxCoeff() - V.col(1).minCoeff();
	double zDist = V.col(2).maxCoeff() - V.col(2).minCoeff();

	// Get the longest distance between all axes
	double maxDist = max(max(xDist, yDist), zDist);

	// Scale all points by '1/maxDist' such that their coordinates will be normalized to [-1, 1] domain
	V.col(0).array() /= maxDist;
	V.col(1).array() /= maxDist;
	V.col(2).array() /= maxDist;

	// Solve the parametrization of the mesh
	solver_wrapper = new SolverWrapper();
	solver_wrapper->init(V, F, V, F, Utils::Init::RANDOM);
	MatX2 Vs = solver_wrapper->solver->Vs;
	MatX3i Fs = solver_wrapper->solver->Fs;

	// Create the an object that will be used to return the vertices and faces of the original mesh and the solved parameterization
	auto meshWithSoup = Nan::New<v8::Object>();

	// Create and array that will hold all the vertices of the original mesh, and add it to the 'meshVertices' property of 'meshWithSoup'
	auto meshVertices = Nan::New<v8::Array>();
	for (auto i = 0; i < V.rows(); ++i)
	{
		auto vertex = Nan::New<v8::Object>();
		vertex->Set(Nan::New("x").ToLocalChecked(), Nan::New<v8::Number>(V(i, 0)));
		vertex->Set(Nan::New("y").ToLocalChecked(), Nan::New<v8::Number>(V(i, 1)));
		vertex->Set(Nan::New("z").ToLocalChecked(), Nan::New<v8::Number>(V(i, 2)));
		meshVertices->Set(i, vertex);
	}

	meshWithSoup->Set(Nan::New("meshVertices").ToLocalChecked(), meshVertices);

	// Create and array that will hold all the faces of the original mesh, and add it to the 'meshFaces' property of 'meshWithSoup'
	auto meshFaces = Nan::New<v8::Array>();
	for (auto i = 0; i < F.rows(); ++i) 
	{
		auto face = Nan::New<v8::Object>();
		face->Set(Nan::New("v1").ToLocalChecked(), Nan::New<v8::Number>(F(i, 0)));
		face->Set(Nan::New("v2").ToLocalChecked(), Nan::New<v8::Number>(F(i, 1)));
		face->Set(Nan::New("v3").ToLocalChecked(), Nan::New<v8::Number>(F(i, 2)));
		meshFaces->Set(i, face);
	}

	meshWithSoup->Set(Nan::New("meshFaces").ToLocalChecked(), meshFaces);

	// Create and array that will hold all the vertices of the solved parameterization, and add it to the 'solverVertices' property of 'meshWithSoup'
	auto solverVertices = Nan::New<v8::Array>();
	for (auto i = 0; i < Vs.rows(); ++i)
	{
		auto vertex = Nan::New<v8::Object>();
		vertex->Set(Nan::New("x").ToLocalChecked(), Nan::New<v8::Number>(Vs(i, 0)));
		vertex->Set(Nan::New("y").ToLocalChecked(), Nan::New<v8::Number>(Vs(i, 1)));
		vertex->Set(Nan::New("z").ToLocalChecked(), Nan::New<v8::Number>(0));
		solverVertices->Set(i, vertex);
	}

	meshWithSoup->Set(Nan::New("solverVertices").ToLocalChecked(), solverVertices);

	// Create and array that will hold all the faces of the solved parameterization, and add it to the 'solverFaces' property of 'meshWithSoup'
	auto solverFaces = Nan::New<v8::Array>();
	for (auto i = 0; i < Fs.rows(); ++i)
	{
		auto face = Nan::New<v8::Object>();
		face->Set(Nan::New("v1").ToLocalChecked(), Nan::New<v8::Number>(Fs(i, 0)));
		face->Set(Nan::New("v2").ToLocalChecked(), Nan::New<v8::Number>(Fs(i, 1)));
		face->Set(Nan::New("v3").ToLocalChecked(), Nan::New<v8::Number>(Fs(i, 2)));
		solverFaces->Set(i, face);
	}

	meshWithSoup->Set(Nan::New("solverFaces").ToLocalChecked(), solverFaces);

	// Return the object
	info.GetReturnValue().Set(meshWithSoup);
}

NAN_METHOD(startSolver) {
  solver_thread = thread(&Solver::run, solver_wrapper->solver.get());
  solver_thread.detach();
}

NAN_METHOD(solverProgressed) {
  auto has_progressed = solver_wrapper->progressed();
  auto progressed = Nan::New<v8::Boolean>(has_progressed);

  info.GetReturnValue().Set(progressed);
}

NAN_METHOD(getUpdatedMesh) {
  MatX2 Xnew;
  solver_wrapper->solver->get_mesh(Xnew);

  auto result_list = Nan::New<v8::Array>();

  auto obj = Nan::New<v8::Object>();
  obj->Set(0, Nan::New<v8::Number>(Xnew.rows()));
  result_list->Set(0, obj);

  for (auto i = 0; i < Xnew.rows(); ++i) {
    auto o = Nan::New<v8::Object>();
    o->Set(0, Nan::New<v8::Number>(Xnew(i, 0)));
    o->Set(1, Nan::New<v8::Number>(Xnew(i, 1)));
    o->Set(2, Nan::New<v8::Number>(0));
    result_list->Set(i + 1, o);
  }

  info.GetReturnValue().Set(result_list);
}

static void update_energy_param(Param p, double value) {
  switch (p)
  {
  case Param::LAMBDA:
    solver_wrapper->set_lambda(value);
    break;
  case Param::DELTA:
    solver_wrapper->set_delta(value);
    break;
  case Param::BOUND:
    solver_wrapper->set_bound(value);
    break;
  case Param::POSITION_WEIGHT:
    solver_wrapper->set_position_weight(value);
    break;
  default:
    assert(false && "Unknown energy parameter");
  }
}

NAN_METHOD(increaseLambda) {
  if (solver_wrapper->solver->energy->lambda <= 0.98)
  {
    if (solver_wrapper->solver->energy->lambda >= 0.85)
      update_energy_param(Param::LAMBDA, solver_wrapper->solver->energy->lambda + 0.01);
    else if (solver_wrapper->solver->energy->lambda <= 0.9)
      update_energy_param(Param::LAMBDA, solver_wrapper->solver->energy->lambda + 0.1);
  }

  info.GetReturnValue().Set(Nan::New<v8::Number>(solver_wrapper->solver->energy->lambda));
}

NAN_METHOD(decreaseLambda) {
  if (solver_wrapper->solver->energy->lambda > 0.9)
    update_energy_param(Param::LAMBDA, solver_wrapper->solver->energy->lambda - 0.01);
  else if (solver_wrapper->solver->energy->lambda >= 0.1)
    update_energy_param(Param::LAMBDA, solver_wrapper->solver->energy->lambda - 0.1);

  info.GetReturnValue().Set(Nan::New<v8::Number>(solver_wrapper->solver->energy->lambda));
}

NAN_METHOD(increaseDelta) {
  update_energy_param(Param::DELTA, solver_wrapper->solver->energy->separation->delta * 2.0);

  info.GetReturnValue().Set(Nan::New<v8::Number>(solver_wrapper->solver->energy->separation->delta));
}

NAN_METHOD(decreaseDelta) {
  update_energy_param(Param::DELTA, solver_wrapper->solver->energy->separation->delta * 0.5);

  info.GetReturnValue().Set(Nan::New<v8::Number>(solver_wrapper->solver->energy->separation->delta));
}

void Init(v8::Local<v8::Object> exports) {
    NAN_EXPORT(exports, loadMeshWithSoup);
    NAN_EXPORT(exports, startSolver);
    NAN_EXPORT(exports, solverProgressed);
    NAN_EXPORT(exports, getUpdatedMesh);
    NAN_EXPORT(exports, increaseLambda);
    NAN_EXPORT(exports, decreaseLambda);
    NAN_EXPORT(exports, increaseDelta);
    NAN_EXPORT(exports, decreaseDelta);
}

NODE_MODULE(addon, Init)
