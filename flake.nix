{
  description = "Muser";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.05";
  };

  outputs = { self, nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };

      projectDir = "$HOME/projects/muser";
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        packages = with pkgs; [
          pkg-config

          xmake

          raylib
          glfw
          xorg.libXrandr
          xorg.libXinerama
          xorg.libXcursor
          xorg.libXi
          libGL

          valgrind 
          gdb
        ];


        shellHook = 
        let 
          libraries = with pkgs; lib.makeLibraryPath [ 
            glfw 
            libGL
            xorg.libXrandr
            xorg.libXinerama
            xorg.libXcursor
            xorg.libXi
          ];
        in ''
          export LD_LIBRARY_PATH="${libraries}:$LD_LIBRARY_PATH"

          function build() {
            currDir="$(pwd)";
            cd "${projectDir}/build"; 
            cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../;
            make .;
            cd $currDir;
          }

          PS1="(dev)$PS1"
        '';
      };
    };
}
