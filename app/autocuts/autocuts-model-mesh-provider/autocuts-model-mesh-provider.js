class AutocutsModelMeshProvider extends MeshProvider {
  constructor(autocutsEngine) {
    super();
    this.autocutsEngine = autocutsEngine;
  }

  getVertices() {
    return this.autocutsEngine.modelVertices;
  }

  getFaces() {
    return this.autocutsEngine.modelFaces;
  }

  getBufferedVertices() {
    return this.autocutsEngine.modelBufferedVertices;
  }
}