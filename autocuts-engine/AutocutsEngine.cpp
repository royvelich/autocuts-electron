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

}

void AutocutsEngine::LoadModel(std::string modelFilePath)
{
    auto modelFileType = GetFilenameExtension(modelFilePath);

    // Read model file
    switch (modelFileType)
    {
    case FileType::Obj:
        igl::readOBJ(modelFilePath, V, F);
        break;

    case FileType::Off:
        igl::readOFF(modelFilePath, V, F);
        break;

    default:
        Nan::ThrowTypeError("Unknown model type");
    }

    // If faces were received as quads, transform them into triangles
    if (F.cols() == 4)
    {
        auto newF = MatX3i(F.rows() * 2, 3);
        Vec4i face;
        for (int i = 0; i < F.rows(); ++i)
        {
            face = F.row(i);
            newF.row(2 * i) << face[0], face[1], face[3];
            newF.row(2 * i + 1) << face[1], face[2], face[3];
        }

        F = newF;
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

    // Initialize parameterization solver
    solverWrapper->init(V, F, V, F, Utils::Init::RANDOM);

    // Get solver faces and vertices
    Fs = solverWrapper->solver->Fs;
    solverWrapper->solver->get_mesh(Vs);
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

const Eigen::MatrixXd& AutocutsEngine::GetModelVertices()
{
    return V;
}

const Eigen::MatrixXi& AutocutsEngine::GetModelFaces()
{
    return F;
}

const Eigen::MatrixX2d& AutocutsEngine::GetSolverVertices()
{
    if (solverWrapper->progressed())
    {
        solverWrapper->solver->get_mesh(Vs);
    }

    return Vs;
}

const Eigen::MatrixXi& AutocutsEngine::GetSolverFaces()
{
    return Fs;
}