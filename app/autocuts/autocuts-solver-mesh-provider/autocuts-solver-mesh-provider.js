class AutocutsSolverMeshProvider extends MeshProvider {
  constructor(autocutsEngine) {
    super();
    this.autocutsEngine = autocutsEngine;
  }

  get vertices() {
    return this.autocutsEngine.solverVertices;
  }

  get faces() {
    return this.autocutsEngine.solverFaces;
  }

  get bufferedVertices() {
    return this.autocutsEngine.solverBufferedVertices;
  }
}