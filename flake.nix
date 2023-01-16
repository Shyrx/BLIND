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
      devShells.${system}.default =
        let
          pigpio = pkgs.stdenv.mkDerivation {
            name = "pigpio";
            src = pkgs.fetchFromGitHub {
              owner =  "joan2937";
              repo = "pigpio";
              rev = "c33738a320a3e28824af7807edafda440952c05d";
              sha256 = "Z+SwUlBbtWtnbjTe0IghR3gIKS43ZziN0amYtmXy7HE=";
            };
            buildInputs = [ pkgs.cmake pkgs.gnumake ];
            phases = [ "buildPhase" "installPhase"];
            buildPhase = ''
                cmake -S $src -B .
                make -j
            '';

            installPhase = ''
                mkdir -p $out/include $out/lib $out/bin
                cp $src/pigpio.h $out/include
                cp *.so $out/lib
                cp pigpiod $out/bin
            '';
          };
        in
          pkgs.mkShell {
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
              arduino-cli
              python310
              boost
            ] ++ [ pigpio ];
          };
    };
}
