{
  description = "Development environment for the BLIND project";

  inputs = { nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable"; };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs {
        inherit system;
        config.allowUnfree = true;
      };

    in {
      devShells.${system}.default = pkgs.mkShell {
        buildInputs = with pkgs; [
          gcc
          gnumake
          clang-tools
          gdb
          bear
          cmake
          cmake-language-server
          opencv
          arduino
          python310
          boost
        ];
      };
    };
}
