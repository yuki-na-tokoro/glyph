{
  description = "";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
  };

  outputs =
    inputs@{ ... }:
    inputs.flake-parts.lib.mkFlake { inherit inputs; } (
      { lib, ... }: {
        systems = lib.systems.flakeExposed;

        perSystem = { pkgs, ... }: {

          packages =
            let
              glyph = pkgs.stdenv.mkDerivation {
                pname = "glyph";
                version = "0.1.0";

                src = ./.;

                outputs = [
                  "out"
                  "client"
                  "server"
                ];

                nativeBuildInputs = with pkgs; [
                  autoreconfHook
                  pkg-config
                ];

                buildInputs = with pkgs; [
                  ncurses
                ];

                postInstall = ''
                  mkdir -p $client/bin $server/bin
                  mv $out/bin/glyph-client $client/bin/
                  mv $out/bin/glyph-server $server/bin/
                '';
              };
            in
            {
              inherit glyph;

              glyph-client = glyph.client // {
                meta = glyph.meta // {
                  mainProgram = "glyph-client";
                };
              };

              glyph-server = glyph.server // {
                meta = glyph.meta // {
                  mainProgram = "glyph-server";
                };
              };
            };

          devShells.default = pkgs.mkShell {

            nativeBuildInputs = with pkgs; [
              autoreconfHook
              pkg-config

              clang-tools
              bear
            ];

            buildInputs = with pkgs; [
              ncurses
            ];

          };
        };
      }
    );
}
