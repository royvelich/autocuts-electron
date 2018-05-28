{
  "targets": [
    {
      "target_name": "autocuts-engine",
      "sources": [
        "index.cpp",
        "AutocutsEngine.cpp",
        "AutocutsEngine.h",
        "AutocutsEngineModule.cpp",
        "src/BBox.cpp",
        "src/Energy.cpp",
        "src/EnergySymDir.cpp",
        "src/IntegerEnergy.cpp",
        "src/Newton.cpp",
        "src/Position.cpp",
        "src/Separation.cpp",
        "src/Solver.cpp",
        "src/SolverWrapper.cpp",
        "src/Utils.cpp"
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "./libigl/include",
        "./libigl/external/eigen",
        "./include",
        "./"
      ],
      "conditions": [
        [
          "OS=='win'",
          {
            "configurations": {
              "Release": {
                "msvs_settings": {
                  "VCCLCompilerTool": {
                    "OpenMP": "true",
                    "PreprocessorDefinitions": ["NOMINMAX"]
                  }
                },
                "msvs_postbuild": "call $(ProjectDir)..\post-build.bat $(TargetDir)$(TargetFileName) $(ProjectDir)latest-builds\\"
              },
              "Debug": {
                "msvs_settings": {
                  "VCCLCompilerTool": {
                    "OpenMP": "true",
                    "PreprocessorDefinitions": ["NOMINMAX"]
                  },
                },
                "msvs_postbuild": "call $(ProjectDir)..\post-build.bat $(TargetDir)$(TargetFileName) $(ProjectDir)latest-builds\\"
              }
            }
          }
        ]
      ]
    }
  ]
}