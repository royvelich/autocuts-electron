#include "AutocutsEngine.h"

NAN_METHOD(AutocutsEngine::New)
{
    if (!info.IsConstructCall())
    {
        return Nan::ThrowError(Nan::New("constructor called without new keyword").ToLocalChecked());
    }

    AutocutsEngine *autocuts_engine = new AutocutsEngine();
    autocuts_engine->Wrap(info.Holder());
}

NAN_METHOD(AutocutsEngine::LoadModel)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Validate input arguments
    if (!info[0]->IsString())
    {
        Nan::ThrowTypeError("Argument should be a string");
        return;
    }

    // Fetch model's path parameter
    v8::String::Utf8Value modelFilePathV8(info[0]->ToString());
    auto modelFilePath = std::string(*modelFilePathV8);

    // Load model file
    autocutsEngine->LoadModel(modelFilePath);

}

NAN_METHOD(AutocutsEngine::StartSolver)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Start solver
    autocutsEngine->StartSolver();
}

NAN_METHOD(AutocutsEngine::StopSolver)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Stop solver
    autocutsEngine->StopSolver();
}

NAN_METHOD(AutocutsEngine::SolverProgressed)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Return solver's progress state
    bool solverProgressed = autocutsEngine->SolverProgressed();
    info.GetReturnValue().Set(Nan::New<v8::Boolean>(solverProgressed));
}

NAN_GETTER(AutocutsEngine::DeltaGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Return delta
    double delta = autocutsEngine->GetDelta();
    info.GetReturnValue().Set(Nan::New<v8::Number>(delta));
}

NAN_SETTER(AutocutsEngine::DeltaSet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Set delta
    auto delta = Nan::To<v8::Number>(value).ToLocalChecked();
    autocutsEngine->SetDelta(delta->Value());
}

NAN_GETTER(AutocutsEngine::LambdaGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Return delta
    double lambda = autocutsEngine->GetLambda();
    info.GetReturnValue().Set(Nan::New<v8::Number>(lambda));
}

NAN_SETTER(AutocutsEngine::LambdaSet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    // Set delta
    auto lambda = Nan::To<v8::Number>(value).ToLocalChecked();
    autocutsEngine->SetLambda(lambda->Value());
}

NAN_GETTER(AutocutsEngine::ModelVerticesGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    auto verticesMatrix = autocutsEngine->GetModelVertices();
    auto verticesArray = Nan::New<v8::Array>();
    for (auto i = 0; i < verticesMatrix.rows(); ++i)
    {
        auto vertex = Nan::New<v8::Object>();
        vertex->Set(Nan::New("x").ToLocalChecked(), Nan::New<v8::Number>(verticesMatrix(i, 0)));
        vertex->Set(Nan::New("y").ToLocalChecked(), Nan::New<v8::Number>(verticesMatrix(i, 1)));
        vertex->Set(Nan::New("z").ToLocalChecked(), Nan::New<v8::Number>(verticesMatrix(i, 2)));
        verticesArray->Set(i, vertex);
    }

    info.GetReturnValue().Set(verticesArray);
}

NAN_GETTER(AutocutsEngine::ModelFacesGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    auto facesMatrix = autocutsEngine->GetModelFaces();
    auto facesArray = Nan::New<v8::Array>();
    for (auto i = 0; i < facesMatrix.rows(); ++i)
    {
        auto face = Nan::New<v8::Array>();
        face->Set(0, Nan::New<v8::Number>(facesMatrix(i, 0)));
        face->Set(1, Nan::New<v8::Number>(facesMatrix(i, 1)));
        face->Set(2, Nan::New<v8::Number>(facesMatrix(i, 2)));
        facesArray->Set(i, face);
    }

    info.GetReturnValue().Set(facesArray);
}

NAN_GETTER(AutocutsEngine::SolverVerticesGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    auto verticesMatrix = autocutsEngine->GetSolverVertices();
    auto verticesArray = Nan::New<v8::Array>();
    for (auto i = 0; i < verticesMatrix.rows(); ++i)
    {
        auto vertex = Nan::New<v8::Object>();
        vertex->Set(Nan::New("x").ToLocalChecked(), Nan::New<v8::Number>(verticesMatrix(i, 0)));
        vertex->Set(Nan::New("y").ToLocalChecked(), Nan::New<v8::Number>(verticesMatrix(i, 1)));
        vertex->Set(Nan::New("z").ToLocalChecked(), Nan::New<v8::Number>(0));
        verticesArray->Set(i, vertex);
    }

    info.GetReturnValue().Set(verticesArray);
}

NAN_GETTER(AutocutsEngine::SolverFacesGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());

    auto facesMatrix = autocutsEngine->GetSolverFaces();
    auto facesArray = Nan::New<v8::Array>();
    for (auto i = 0; i < facesMatrix.rows(); ++i)
    {
        auto face = Nan::New<v8::Array>();
        face->Set(0, Nan::New<v8::Number>(facesMatrix(i, 0)));
        face->Set(1, Nan::New<v8::Number>(facesMatrix(i, 1)));
        face->Set(2, Nan::New<v8::Number>(facesMatrix(i, 2)));
        facesArray->Set(i, face);
    }
    info.GetReturnValue().Set(facesArray);
}

NAN_MODULE_INIT(AutocutsEngine::Init)
{
    auto cname = Nan::New("AutocutsEngine").ToLocalChecked();
    auto ctor = Nan::New<v8::FunctionTemplate>(AutocutsEngine::New);

    // Target for member functions
    auto ctorInst = ctor->InstanceTemplate();

    // As `ctor.name` in JS
    ctor->SetClassName(cname);

    // For ObjectWrap, it should set 1
    ctorInst->SetInternalFieldCount(1);

    // Add member functions
    Nan::SetPrototypeMethod(ctor, "loadModel", LoadModel);
    Nan::SetPrototypeMethod(ctor, "startSolver", StartSolver);
    Nan::SetPrototypeMethod(ctor, "stopSolver", StopSolver);
    Nan::SetPrototypeMethod(ctor, "solverProgressed", SolverProgressed);

    // Add member accessors
    auto deltaProperty = Nan::New("delta").ToLocalChecked();
    Nan::SetAccessor(ctorInst, deltaProperty, DeltaGet, DeltaSet);

    auto lambdaProperty = Nan::New("lambda").ToLocalChecked();
    Nan::SetAccessor(ctorInst, lambdaProperty, LambdaGet, LambdaSet);

    auto modelVerticesProperty = Nan::New("modelVertices").ToLocalChecked();
    Nan::SetAccessor(ctorInst, modelVerticesProperty, ModelVerticesGet);

    auto modelFacesProperty = Nan::New("modelFaces").ToLocalChecked();
    Nan::SetAccessor(ctorInst, modelFacesProperty, ModelFacesGet);

    auto solverVerticesProperty = Nan::New("solverVertices").ToLocalChecked();
    Nan::SetAccessor(ctorInst, solverVerticesProperty, SolverVerticesGet);

    auto solverFacesProperty = Nan::New("solverFaces").ToLocalChecked();
    Nan::SetAccessor(ctorInst, solverFacesProperty, SolverFacesGet);

    Nan::Set(target, cname, Nan::GetFunction(ctor).ToLocalChecked());
}