#include "AutocutsEngine.h"

std::string AutocutsEngine::ToLower(std::string data)
{
    transform(data.begin(), data.end(), data.begin(), ::tolower);
    return data;
}

FileType AutocutsEngine::GetFilenameExtension(std::string fileName)
{
    std::string fileExtension = ToLower(fileName.substr(fileName.find_last_of(".") + 1));
    if (fileExtension == "obj")
    {
        return FileType::Obj;
    }
    else if (fileExtension == "off")
    {
        return FileType::Off;
    }

    return FileType::Unknown;
}

AutocutsEngine::AutocutsEngine() :
    solverWrapper(std::make_shared<SolverWrapper>())
{
    modelVerticesArray.Reset(Nan::New<v8::Array>());
    modelFacesArray.Reset(Nan::New<v8::Array>());
    solverVerticesArray.Reset(Nan::New<v8::Array>());
    solverFacesArray.Reset(Nan::New<v8::Array>());
    bufferedSolverVertices.Reset(Nan::New<v8::Array>());
    bufferedModelVertices.Reset(Nan::New<v8::Array>());
}

AutocutsEngine::~AutocutsEngine()
{
    modelVerticesArray.Reset();
    modelFacesArray.Reset();
    solverVerticesArray.Reset();
    solverFacesArray.Reset();
}

void AutocutsEngine::FindEdgeLenghtsForSeparation()
{
    SpMat Esept = solverWrapper->solver->energy->separation->Esept;
    Vec edge_lengths = Vec::Ones(Esept.cols());
    int v1, v2, num_edges, pair1, pair2;
    double edge_len;
    Vec3 v1pos, v2pos;
    double total = 0.;
    int cnt = 0;
    for (int i = 0; i < modelFacesMatrix.rows(); ++i)
    {
        Vec3i face = modelFacesMatrix.row(i);
        for (int j = 0; j < 3; ++j)
        { // loop over all 3 triangle edges
            v1 = face(j);
            v2 = face((j + 1) % 3);
            num_edges = FindCorrespondingUvEdges(v1, v2);
            if (num_edges == 2)
            { // this is an actual separation energy pair
                v1pos = modelVerticesMatrix.row(v1);
                v2pos = modelVerticesMatrix.row(v2);
                edge_len = 0.5 * (v1pos - v2pos).squaredNorm();
                total += (v1pos - v2pos).norm();
                cnt++;
                pair1 = FindCorrespondingPairIndex(uvEdges[0].first, uvEdges[1].first);
                pair2 = FindCorrespondingPairIndex(uvEdges[0].second, uvEdges[1].second);
                if (pair1 == -1 || pair2 == -1)
                {
                    pair1 = FindCorrespondingPairIndex(uvEdges[0].first, uvEdges[1].second);
                    pair2 = FindCorrespondingPairIndex(uvEdges[0].second, uvEdges[1].first);
                }
                edge_lengths(pair1) = edge_len;
                edge_lengths(pair2) = edge_len;
            }
        }
    }

    if (setEdgeLenghtsToAverage)
    {
        edge_lengths = Vec::Constant(Esept.cols(), total / (double)cnt);
    }

    solverWrapper->get_slot();
    solverWrapper->solver->energy->separation->edge_lenghts_per_pair = edge_lengths;
    solverWrapper->release_slot();
}

int AutocutsEngine::FindCorrespondingUvEdges(int v1, int v2)
{
    SpMat V2Vt = solverWrapper->solver->energy->separation->V2Vt;
    vector<int> v1_cand, v2_cand;
    // select candidates
    for (int i = 0; i < V2Vt.outerSize(); ++i)
    {
        SpMat::InnerIterator it(V2Vt, i);
        if (it.col() == v1)
        {
            for (; it; ++it)
                v1_cand.push_back(it.row());
            if (v2_cand.size() > 0)
                break; // we found both
        }
        else if (it.col() == v2)
        {
            for (; it; ++it)
                v2_cand.push_back(it.row());
            if (v1_cand.size() > 0)
                break; // we found both
        }
        continue;
    }
    // find <= 2 existing edges
    uvEdges.clear();
    for (int v1c : v1_cand)
    {
        for (int v2c : v2_cand)
        {
            if (floor(v1c / 3.0) == floor(v2c / 3.0))
                v1c < v2c ? uvEdges.push_back(pair<int, int>(v1c, v2c)) : uvEdges.push_back(pair<int, int>(v2c, v1c));
        }
    }
    return uvEdges.size();
}

int AutocutsEngine::FindCorrespondingPairIndex(int i1, int i2)
{
    SpMat Esept = solverWrapper->solver->energy->separation->Esept;
    for (int i = 0; i < Esept.outerSize(); ++i)
    {
        int idx_xi, idx_xj;
        SpMat::InnerIterator it(Esept, i);
        idx_xi = it.row();
        idx_xj = (++it).row();
        if ((idx_xi == i1 && idx_xj == i2) || (idx_xj == i1 && idx_xi == i2))
            return i;
    }
    return -1;
}

void AutocutsEngine::LoadModel(std::string modelFilePath)
{
    auto modelFileType = GetFilenameExtension(modelFilePath);

    // Read model file
    switch (modelFileType)
    {
    case FileType::Obj:
        igl::readOBJ(modelFilePath, modelVerticesMatrix, modelFacesMatrix);
        break;

    case FileType::Off:
        igl::readOFF(modelFilePath, modelVerticesMatrix, modelFacesMatrix);
        break;

    default:
        Nan::ThrowTypeError("Unknown model type");
    }

    // If faces were received as quads, transform them into triangles
    if (modelFacesMatrix.cols() == 4)
    {
        auto newModelFacesMatrix = MatX3i(modelFacesMatrix.rows() * 2, 3);
        Vec4i face;
        for (int i = 0; i < modelFacesMatrix.rows(); ++i)
        {
            face = modelFacesMatrix.row(i);
            newModelFacesMatrix.row(2 * i) << face[0], face[1], face[3];
            newModelFacesMatrix.row(2 * i + 1) << face[1], face[2], face[3];
        }

        modelFacesMatrix = newModelFacesMatrix;
    }

    // Translate all points such that their mean will be at the origin
    modelVerticesMatrix.col(0).array() -= modelVerticesMatrix.col(0).mean();
    modelVerticesMatrix.col(1).array() -= modelVerticesMatrix.col(1).mean();
    modelVerticesMatrix.col(2).array() -= modelVerticesMatrix.col(2).mean();

    // Get the one-dimensional distance between the two most distant points on each axis
    double xDist = modelVerticesMatrix.col(0).maxCoeff() - modelVerticesMatrix.col(0).minCoeff();
    double yDist = modelVerticesMatrix.col(1).maxCoeff() - modelVerticesMatrix.col(1).minCoeff();
    double zDist = modelVerticesMatrix.col(2).maxCoeff() - modelVerticesMatrix.col(2).minCoeff();

    // Get the longest distance between all axes
    double maxDist = max(max(xDist, yDist), zDist);

    // Scale all points by '1/maxDist' such that their coordinates will be normalized to [-1, 1] domain
    modelVerticesMatrix.col(0).array() /= maxDist;
    modelVerticesMatrix.col(1).array() /= maxDist;
    modelVerticesMatrix.col(2).array() /= maxDist;

    // Initialize parameterization solver
    solverWrapper->init(modelVerticesMatrix, modelFacesMatrix, modelVerticesMatrix, modelFacesMatrix, Utils::Init::RANDOM);

    // Get solver faces and vertices
    solverFacesMatrix = solverWrapper->solver->Fs;
    solverWrapper->solver->get_mesh(solverVerticesMatrix);

    auto localModelVerticesArray = Nan::New<v8::Array>();
    for (auto i = 0; i < modelVerticesMatrix.rows(); ++i)
    {
        auto vertex = Nan::New<v8::Object>();
        vertex->Set(Nan::New("x").ToLocalChecked(), Nan::New<v8::Number>(modelVerticesMatrix(i, 0)));
        vertex->Set(Nan::New("y").ToLocalChecked(), Nan::New<v8::Number>(modelVerticesMatrix(i, 1)));
        vertex->Set(Nan::New("z").ToLocalChecked(), Nan::New<v8::Number>(modelVerticesMatrix(i, 2)));
        localModelVerticesArray->Set(i, vertex);
    }

    auto localModelFacesArray = Nan::New<v8::Array>();
    for (auto i = 0; i < modelFacesMatrix.rows(); ++i)
    {
        auto face = Nan::New<v8::Array>();
        face->Set(0, Nan::New<v8::Number>(modelFacesMatrix(i, 0)));
        face->Set(1, Nan::New<v8::Number>(modelFacesMatrix(i, 1)));
        face->Set(2, Nan::New<v8::Number>(modelFacesMatrix(i, 2)));
        localModelFacesArray->Set(i, face);
    }

    auto localSolverVerticesArray = Nan::New<v8::Array>();
    for (auto i = 0; i < solverVerticesMatrix.rows(); ++i)
    {
        auto vertex = Nan::New<v8::Object>();
        vertex->Set(Nan::New("x").ToLocalChecked(), Nan::New<v8::Number>(solverVerticesMatrix(i, 0)));
        vertex->Set(Nan::New("y").ToLocalChecked(), Nan::New<v8::Number>(solverVerticesMatrix(i, 1)));
        vertex->Set(Nan::New("z").ToLocalChecked(), Nan::New<v8::Number>(0));
        localSolverVerticesArray->Set(i, vertex);
    }

    auto localSolverFacesArray = Nan::New<v8::Array>();
    for (auto i = 0; i < solverFacesMatrix.rows(); ++i)
    {
        auto face = Nan::New<v8::Array>();
        face->Set(0, Nan::New<v8::Number>(solverFacesMatrix(i, 0)));
        face->Set(1, Nan::New<v8::Number>(solverFacesMatrix(i, 1)));
        face->Set(2, Nan::New<v8::Number>(solverFacesMatrix(i, 2)));
        localSolverFacesArray->Set(i, face);
    }

    auto localBufferedModelVertices = Nan::New<v8::Array>();
    for (auto i = 0; i < modelFacesMatrix.rows(); ++i)
    {
        for (auto j = 0; j < 3; ++j) 
        {
            int vertexId = modelFacesMatrix(i, j);
            auto x = modelVerticesMatrix(vertexId, 0);
            auto y = modelVerticesMatrix(vertexId, 1);
            auto z = modelVerticesMatrix(vertexId, 2);

            localBufferedModelVertices->Set(3 * (j + 3 * i), Nan::New<v8::Number>(x));
            localBufferedModelVertices->Set(3 * (j + 3 * i) + 1, Nan::New<v8::Number>(y));
            localBufferedModelVertices->Set(3 * (j + 3 * i) + 2, Nan::New<v8::Number>(z));
        }
    }

    auto localBufferedSolverVertices = Nan::New<v8::Array>();
    for (auto i = 0; i < solverFacesMatrix.rows(); ++i)
    {
        for (auto j = 0; j < 3; ++j)
        {
            int vertexId = solverFacesMatrix(i, j);
            auto x = solverVerticesMatrix(vertexId, 0);
            auto y = solverVerticesMatrix(vertexId, 1);
            auto z = 0;

            localBufferedSolverVertices->Set(3 * (j + 3 * i), Nan::New<v8::Number>(x));
            localBufferedSolverVertices->Set(3 * (j + 3 * i) + 1, Nan::New<v8::Number>(y));
            localBufferedSolverVertices->Set(3 * (j + 3 * i) + 2, Nan::New<v8::Number>(z));
        }
    }

    modelVerticesArray.Reset(localModelVerticesArray);
    modelFacesArray.Reset(localModelFacesArray);
    solverVerticesArray.Reset(localSolverVerticesArray);
    solverFacesArray.Reset(localSolverFacesArray);

    bufferedModelVertices.Reset(localBufferedModelVertices);
    bufferedSolverVertices.Reset(localBufferedSolverVertices);

    FindEdgeLenghtsForSeparation();
}

void AutocutsEngine::StartSolver()
{
    solverThread = std::thread([this]() {
        solverWrapper->solver->run();
    });
    solverThread.detach();
}

void AutocutsEngine::StopSolver()
{
    solverWrapper->solver->stop();
}

bool AutocutsEngine::SolverProgressed()
{
    return solverWrapper->progressed();
}

void AutocutsEngine::SetDelta(double delta)
{
    solverWrapper->set_delta(delta);
}

double AutocutsEngine::GetDelta()
{
    return solverWrapper->solver->energy->separation->delta;
}

void AutocutsEngine::SetLambda(double lambda)
{
    solverWrapper->set_lambda(lambda);
}

double AutocutsEngine::GetLambda()
{
    return solverWrapper->solver->energy->lambda;
}

const Nan::Persistent<v8::Array>& AutocutsEngine::GetModelVerticesArray()
{
    return modelVerticesArray;
}

const Nan::Persistent<v8::Array>& AutocutsEngine::GetModelFacesArray()
{
    return modelFacesArray;
}

const Nan::Persistent<v8::Array>& AutocutsEngine::GetSolverVerticesArray()
{
    if (solverWrapper->progressed())
    {
        solverWrapper->solver->get_mesh(solverVerticesMatrix);
        auto localSolverVerticesArray = Nan::New<v8::Array>();
        for (auto i = 0; i < solverVerticesMatrix.rows(); ++i)
        {
            auto vertex = Nan::New<v8::Object>();
            vertex->Set(Nan::New("x").ToLocalChecked(), Nan::New<v8::Number>(solverVerticesMatrix(i, 0)));
            vertex->Set(Nan::New("y").ToLocalChecked(), Nan::New<v8::Number>(solverVerticesMatrix(i, 1)));
            vertex->Set(Nan::New("z").ToLocalChecked(), Nan::New<v8::Number>(0));
            localSolverVerticesArray->Set(i, vertex);
        }

        solverVerticesArray.Reset(localSolverVerticesArray);
    }

    return solverVerticesArray;
}

const Nan::Persistent<v8::Array>& AutocutsEngine::GetSolverFacesArray()
{
    return solverFacesArray;
}

const Nan::Persistent<v8::Array>& AutocutsEngine::GetBufferedModelVertices()
{
    return bufferedModelVertices;
}

const Nan::Persistent<v8::Array>& AutocutsEngine::GetBufferedSolverVertices()
{
    if (solverWrapper->progressed())
    {
        solverWrapper->solver->get_mesh(solverVerticesMatrix);

        auto localBufferedSolverVertices = Nan::New<v8::Array>();
        for (auto faceIndex = 0; faceIndex < solverFacesMatrix.rows(); ++faceIndex)
        {
            auto faceFirstVertexBufferIndex = 9 * faceIndex;
            for (auto vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
            {
                int vertexId = solverFacesMatrix(faceIndex, vertexIndex);
                auto x = solverVerticesMatrix(vertexId, 0);
                auto y = solverVerticesMatrix(vertexId, 1);
                auto z = 0;

                auto vertexFirstComponentBufferIndex = faceFirstVertexBufferIndex + 3 * vertexIndex;
                localBufferedSolverVertices->Set(vertexFirstComponentBufferIndex, Nan::New<v8::Number>(x));
                localBufferedSolverVertices->Set(vertexFirstComponentBufferIndex + 1, Nan::New<v8::Number>(y));
                localBufferedSolverVertices->Set(vertexFirstComponentBufferIndex + 2, Nan::New<v8::Number>(z));
            }
        }

        bufferedSolverVertices.Reset(localBufferedSolverVertices);
    }

    return bufferedSolverVertices;
}

void AutocutsEngine::AddTriangleToFixedPositionSet(int32_t faceId)
{
    Mat32 pos;
    igl::slice(solverWrapper->solver->Vs, solverWrapper->solver->Fs.row(faceId).eval(), 1, pos);
    solverWrapper->add_or_remove_triangle_from_fixed_position_set(faceId, pos);
}

void AutocutsEngine::SetMovingTriangleFaceId(int32_t faceId)
{
    movingTriangleFaceId = faceId;
    if (!solverWrapper->if_its_a_fixed_triangle_remove_and_store(movingTriangleFaceId, movingTriangleInitialPosition))
        igl::slice(solverWrapper->solver->Vs, solverWrapper->solver->Fs.row(movingTriangleFaceId).eval(), 1, movingTriangleInitialPosition);

    UpdateMovingTrianglePosition(RVec2(0, 0));

    solverWrapper->set_active_triangle(movingTriangleFaceId);
}

void AutocutsEngine::ResetMovingTriangleFaceId()
{
    movingTriangleFaceId = -1;
    solverWrapper->set_active_triangle(movingTriangleFaceId);
}

void AutocutsEngine::UpdateMovingTrianglePosition(const RVec2& offset)
{
    Mat32 movingTrianglePosition = movingTriangleInitialPosition;
    movingTrianglePosition.row(0) += offset;
    movingTrianglePosition.row(1) += offset;
    movingTrianglePosition.row(2) += offset;
    currentTriangleInitialPosition = movingTrianglePosition;
    solverWrapper->set_target_triangle_position(movingTrianglePosition, movingTriangleFaceId);
}