{
  description = "A build env for the Raquest dsl.";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.11";

  outputs = {
    self,
    nixpkgs,
  }: let
    supportedSystems = ["x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin"];
    forEachSupportedSystem = f:
      nixpkgs.lib.genAttrs supportedSystems (system:
        f {
          pkgs = import nixpkgs {inherit system;};
        });
  in {
    devShells = forEachSupportedSystem ({pkgs}: {
      default =
        pkgs.mkShell.override {}
        {
          packages = with pkgs; [
            llvmPackages_19.libcxxClang
            clang-tools
            cmake
            gtest
            openssl
            valgrind
            ninja
          ];
        };
    });
  };
}
