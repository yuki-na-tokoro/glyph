{
  description = "";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
    neat-flakes.url = "github:wawwior/neat-flakes";
  };

  outputs =
    inputs@{ self, ... }:

    inputs.neat-flakes.lib.eachSystem
      [
        "x86_64-linux"
        "x86_64-darwin"
        "aarch64-linux"
        "aarch64-darwin"
      ]
      (
        system:
        let

          pkgs = import inputs.nixpkgs {
            inherit system;
          };

        in
        {
          devShells.${system}.default = pkgs.mkShell {

            packages = with pkgs; [
              (buildEnv {
                name = "devShell";
                paths = [
                  gcc
                  gnumake
                  clang-tools
                ];
              })
            ];

          };
        }
      );
}
