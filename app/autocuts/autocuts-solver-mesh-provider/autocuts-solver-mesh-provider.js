class AutocutsSolverMeshProvider extends MeshProvider {
  constructor(autocutsEngine) {
    super();
    this.autocutsEngine = autocutsEngine;
  }

  getVertices() {
    return this.autocutsEngine.solverVertices;
  }

  getFaces() {
    return this.autocutsEngine.solverFaces;
  }
}