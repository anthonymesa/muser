{
  description = "Muser";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.05";
  };

  outputs = { nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
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
          PS1="(dev)$PS1"
        '';
      };
    };
}
