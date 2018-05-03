#include <nan.h>
#include "AutocutsEngine.h"

//std::shared_ptr<SolverWrapper> solver_wrapper;
//thread solver_thread;
//Eigen::MatrixXi F;
//auto model_faces = Nan::New<v8::Array>();
//auto model_vertices = Nan::New<v8::Array>();
//auto solver_faces = Nan::New<v8::Array>();
//auto solver_vertices = Nan::New<v8::Array>();
//
//enum class FileType
//{
//  Obj,
//  Off,
//  Unknown
//};
//
//std::string ToLower(std::string data)
//{
//  transform(data.begin(), data.end(), data.begin(), ::tolower);
//  return data;
//}
//
//FileType GetFilenameExtension(std::string fileName)
//{
//  std::string fileExtension = ToLower(fileName.substr(fileName.find_last_of(".") + 1));
//  if (fileExtension == "obj")
//  {
//    return FileType::Obj;
//  }
//  else if (fileExtension == "off")
//  {
//    return FileType::Off;
//  }
//
//  return FileType::Unknown;
//}
//
//NAN_METHOD(loadModel)
//{
//  Eigen::MatrixXd V;
//  Eigen::MatrixXi original_F;
//
//  // Validate input arguments
//  if (!info[0]->IsString())
//  {
//    Nan::ThrowTypeError("Argument should be a string");
//    return;
//  }
//
//  // Get mesh file type
//  v8::String::Utf8Value filePathV8(info[0]->ToString());
//  auto filePath = std::string(*filePathV8);
//  auto fileType = GetFilenameExtension(filePath);
//
//  // Read mesh file
//  switch (fileType)
//  {
//  case FileType::Obj:
//    igl::readOBJ(filePath, V, original_F);
//    break;
//
//  case FileType::Off:
//    igl::readOFF(filePath, V, original_F);
//    break;
//
//  default:
//    Nan::ThrowTypeError("Unknown mesh type");
//  }
//
//  // If faces were received as quads, transform them into triangles
//  if (original_F.cols() == 4)
//  {
//    F = MatX3i(original_F.rows() * 2, 3);
//    Vec4i face;
//    for (int i = 0; i < original_F.rows(); ++i)
//    {
//      face = original_F.row(i);
//      F.row(2 * i) << face[0], face[1], face[3];
//      F.row(2 * i + 1) << face[1], face[2], face[3];
//    }
//  }
//  else
//  {
//    F = original_F;
//  }
//
//  // Translate all points such that their mean will be at the origin
//  V.col(0).array() -= V.col(0).mean();
//  V.col(1).array() -= V.col(1).mean();
//  V.col(2).array() -= V.col(2).mean();
//
//  // Get the one-dimensional distance between the two most distant points on each axis
//  double xDist = V.col(0).maxCoeff() - V.col(0).minCoeff();
//  double yDist = V.col(1).maxCoeff() - V.col(1).minCoeff();
//  double zDist = V.col(2).maxCoeff() - V.col(2).minCoeff();
//
//  // Get the longest distance between all axes
//  double maxDist = max(max(xDist, yDist), zDist);
//
//  // Scale all points by '1/maxDist' such that their coordinates will be normalized to [-1, 1] domain
//  V.col(0).array() /= maxDist;
//  V.col(1).array() /= maxDist;
//  V.col(2).array() /= maxDist;
//
//  // Initialize parameterization solver
//  solver_wrapper = std::make_shared<SolverWrapper>();
//  solver_wrapper->init(V, F, V, F, Utils::Init::RANDOM);
//  MatX2 Vs = solver_wrapper->solver->Vs;
//  MatX3i Fs = solver_wrapper->solver->Fs;
//
//  // Build mesh faces array
//  model_faces = Nan::New<v8::Array>();
//  for (auto i = 0; i < F.rows(); ++i)
//  {
//    auto face = Nan::New<v8::Array>();
//    face->Set(0, Nan::New<v8::Number>(F(i, 0)));
//    face->Set(1, Nan::New<v8::Number>(F(i, 1)));
//    face->Set(2, Nan::New<v8::Number>(F(i, 2)));
//    model_faces->Set(i, face);
//  }
//
//  // Build mesh vertices array
//  model_vertices = Nan::New<v8::Array>();
//  for (auto i = 0; i < V.rows(); ++i)
//  {
//    auto vertex = Nan::New<v8::Object>();
//    vertex->Set(Nan::New("x").ToLocalChecked(), Nan::New<v8::Number>(V(i, 0)));
//    vertex->Set(Nan::New("y").ToLocalChecked(), Nan::New<v8::Number>(V(i, 1)));
//    vertex->Set(Nan::New("z").ToLocalChecked(), Nan::New<v8::Number>(V(i, 2)));
//    model_vertices->Set(i, vertex);
//  }
//
//  // Build mesh faces array
//  model_faces = Nan::New<v8::Array>();
//  for (auto i = 0; i < F.rows(); ++i)
//  {
//    auto face = Nan::New<v8::Array>();
//    face->Set(0, Nan::New<v8::Number>(F(i, 0)));
//    face->Set(1, Nan::New<v8::Number>(F(i, 1)));
//    face->Set(2, Nan::New<v8::Number>(F(i, 2)));
//    model_faces->Set(i, face);
//  }
//
//  // Build initial solver faces
//  solver_faces = Nan::New<v8::Array>();
//  for (auto i = 0; i < Fs.rows(); ++i)
//  {
//    auto face = Nan::New<v8::Array>();
//    face->Set(0, Nan::New<v8::Number>(Fs(i, 0)));
//    face->Set(1, Nan::New<v8::Number>(Fs(i, 1)));
//    face->Set(2, Nan::New<v8::Number>(Fs(i, 2)));
//    solver_faces->Set(i, face);
//  }
//
//  // Build initial solver vertices array
//  solver_vertices = Nan::New<v8::Array>();
//  for (auto i = 0; i < Vs.rows(); ++i)
//  {
//    auto vertex = Nan::New<v8::Object>();
//    vertex->Set(Nan::New("x").ToLocalChecked(), Nan::New<v8::Number>(Vs(i, 0)));
//    vertex->Set(Nan::New("y").ToLocalChecked(), Nan::New<v8::Number>(Vs(i, 1)));
//    vertex->Set(Nan::New("z").ToLocalChecked(), Nan::New<v8::Number>(0));
//    solver_vertices->Set(i, vertex);
//  }
//}
//
//NAN_METHOD(getSolverVertices)
//{
//  if (solver_wrapper != nullptr && solver_wrapper->progressed())
//  {
//    MatX2 Vs;
//    solver_wrapper->solver->get_mesh(Vs);
//    solver_vertices = Nan::New<v8::Array>();
//
//    for (auto i = 0; i < Vs.rows(); ++i)
//    {
//      auto vertex = Nan::New<v8::Object>();
//      vertex->Set(Nan::New("x").ToLocalChecked(), Nan::New<v8::Number>(Vs(i, 0)));
//      vertex->Set(Nan::New("y").ToLocalChecked(), Nan::New<v8::Number>(Vs(i, 1)));
//      vertex->Set(Nan::New("z").ToLocalChecked(), Nan::New<v8::Number>(0));
//      solver_vertices->Set(i, vertex);
//    }
//  }
//
//  info.GetReturnValue().Set(solver_vertices);
//}
//
//NAN_METHOD(getSolverFaces)
//{
//  model_vertices = Nan::New<v8::Array>();
//  info.GetReturnValue().Set(model_vertices);
//}
//
//NAN_METHOD(getModelVertices)
//{
//  model_vertices = Nan::New<v8::Array>();
//  info.GetReturnValue().Set(model_vertices);
//}
//
//NAN_METHOD(getModelFaces)
//{
//  model_vertices = Nan::New<v8::Array>();
//  info.GetReturnValue().Set(model_vertices);
//}
//
//NAN_METHOD(startSolver)
//{
//  if (solver_wrapper)
//  {
//    solver_thread = std::thread([]() {
//      solver_wrapper->solver->run();
//    });
//    solver_thread.detach();
//  }
//}
//
//NAN_METHOD(stopSolver)
//{
//  if (solver_wrapper)
//  {
//    solver_wrapper->solver->stop();
//  }
//}
//
//NAN_METHOD(solverProgressed)
//{
//  auto has_progressed = solver_wrapper ? solver_wrapper->progressed() : false;
//  info.GetReturnValue().Set(Nan::New<v8::Boolean>(has_progressed));
//}
//
////NAN_METHOD(getUpdatedMesh)
////{
////	auto meshWithSoup = Nan::New<v8::Object>();
////	auto solverData = Nan::New<v8::Object>();
////	meshWithSoup->Set(Nan::New("solverData").ToLocalChecked(), solverData);
////
////	auto solverVertices = Nan::New<v8::Array>();
////	solverData->Set(Nan::New("vertices").ToLocalChecked(), solverVertices);
////
////	if (solver_wrapper)
////	{
////		MatX2 Vs;
////		solver_wrapper->solver->get_mesh(Vs);
////		for (auto i = 0; i < Vs.rows(); ++i)
////		{
////			auto vertex = Nan::New<v8::Object>();
////			vertex->Set(Nan::New("x").ToLocalChecked(), Nan::New<v8::Number>(Vs(i, 0)));
////			vertex->Set(Nan::New("y").ToLocalChecked(), Nan::New<v8::Number>(Vs(i, 1)));
////			vertex->Set(Nan::New("z").ToLocalChecked(), Nan::New<v8::Number>(0));
////			solverVertices->Set(i, vertex);
////		}
////	}
////
////	info.GetReturnValue().Set(meshWithSoup);
////}
//
//NAN_METHOD(setDelta)
//{
//  if (solver_wrapper)
//  {
//    auto delta = info[0]->ToNumber();
//    solver_wrapper->set_delta(delta->Value());
//  }
//}
//
//NAN_METHOD(setLambda)
//{
//  if (solver_wrapper)
//  {
//    auto lambda = info[0]->ToNumber();
//    solver_wrapper->set_lambda(lambda->Value());
//  }
//}
//
//NAN_METHOD(getDelta)
//{
//  info.GetReturnValue().Set(Nan::New<v8::Number>(solver_wrapper->solver->energy->separation->delta));
//}
//
//NAN_METHOD(getLambda)
//{
//  info.GetReturnValue().Set(Nan::New<v8::Number>(solver_wrapper->solver->energy->lambda));
//}
//
////void Init(v8::Local<v8::Object> exports)
////{
////	NAN_EXPORT(exports, loadModel);
////	NAN_EXPORT(exports, getSolverVertices);
////	NAN_EXPORT(exports, getSolverFaces);
////	NAN_EXPORT(exports, getModelVertices);
////	NAN_EXPORT(exports, getModelFaces);
////	NAN_EXPORT(exports, startSolver);
////	NAN_EXPORT(exports, stopSolver);
////	NAN_EXPORT(exports, solverProgressed);
////	//NAN_EXPORT(exports, getUpdatedMesh);
////	NAN_EXPORT(exports, setDelta);
////	NAN_EXPORT(exports, setLambda);
////	NAN_EXPORT(exports, getDelta);
////	NAN_EXPORT(exports, getLambda);
////}
////
////NODE_MODULE(addon, Init)

NAN_MODULE_INIT(InitAll)
{
  AutocutsEngine::Init(target);
}

NODE_MODULE(AutocutsModule, InitAll)