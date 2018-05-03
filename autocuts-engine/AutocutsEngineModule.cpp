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
    v8::Local<v8::Array> modelVerticesArray = Nan::New(autocutsEngine->GetModelVerticesArray());
    info.GetReturnValue().Set(modelVerticesArray);
}

NAN_GETTER(AutocutsEngine::ModelFacesGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());
    v8::Local<v8::Array> modelFacesArray = Nan::New(autocutsEngine->GetModelFacesArray());
    info.GetReturnValue().Set(modelFacesArray);
}

NAN_GETTER(AutocutsEngine::SolverVerticesGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());
    v8::Local<v8::Array> solverVerticesArray = Nan::New(autocutsEngine->GetSolverVerticesArray());
    info.GetReturnValue().Set(solverVerticesArray);
}

NAN_GETTER(AutocutsEngine::SolverFacesGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());
    v8::Local<v8::Array> solverFacesArray = Nan::New(autocutsEngine->GetSolverFacesArray());
    info.GetReturnValue().Set(solverFacesArray);
}





NAN_GETTER(AutocutsEngine::ModelBufferedVerticesGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());
    v8::Local<v8::Array> bufferedModelVertices = Nan::New(autocutsEngine->GetBufferedModelVertices());
    info.GetReturnValue().Set(bufferedModelVertices);
}

NAN_GETTER(AutocutsEngine::SolverBufferedVerticesGet)
{
    // `Unwrap` refer C++ object from JS Object
    auto autocutsEngine = Nan::ObjectWrap::Unwrap<AutocutsEngine>(info.Holder());
    v8::Local<v8::Array> bufferedSolverVertices = Nan::New(autocutsEngine->GetBufferedSolverVertices());
    info.GetReturnValue().Set(bufferedSolverVertices);
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





    auto modelBufferedVerticesProperty = Nan::New("modelBufferedVertices").ToLocalChecked();
    Nan::SetAccessor(ctorInst, modelBufferedVerticesProperty, ModelBufferedVerticesGet);

    auto solverBufferedVerticesProperty = Nan::New("solverBufferedVertices").ToLocalChecked();
    Nan::SetAccessor(ctorInst, solverBufferedVerticesProperty, SolverBufferedVerticesGet);




    Nan::Set(target, cname, Nan::GetFunction(ctor).ToLocalChecked());
}