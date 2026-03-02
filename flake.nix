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
          cmake
          valgrind 
          gdb
        ];

        shellHook = ''
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
