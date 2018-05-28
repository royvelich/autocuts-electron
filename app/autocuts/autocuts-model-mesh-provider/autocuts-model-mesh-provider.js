class AutocutsModelMeshProvider extends MeshProvider {
  constructor(autocutsEngine) {
    super();
    this.autocutsEngine = autocutsEngine;
  }

  get vertices() {
    return this.autocutsEngine.modelVertices;
  }

  get faces() {
    return this.autocutsEngine.modelFaces;
  }

  get bufferedVertices() {
    return this.autocutsEngine.modelBufferedVertices;
  }
}