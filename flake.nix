{
  inputs = {
    nixpkgs = {
      url = "github:NixOS/nixpkgs/nixos-unstable";
    };
    flake-utils.url = "github:numtide/flake-utils";
    yotta = {
      url = "github:joxcat/yotta?dir=nix";
    };
    # Targets
    ytTargetMicrobit = {
      url = "git+https://git.mob-dev.fr/Schoumi/yotta-target-bbc-microbit-classic-gcc.git?ref=refs/tags/v0.2.3";
      flake = false;
    };
    ytTargetMbed = {
      url = "git+https://git.mob-dev.fr/Schoumi/target-mbed-gcc.git?ref=refs/tags/v0.1.5";
      flake = false;
    };
    # Yotta dependencies
    ytDepMicrobit = {
      url = "git+https://git.mob-dev.fr/Schoumi/microbit.git?ref=refs/tags/v2.2.0-rc8";
      flake = false;
    };
    ytDepMicrobitDal = {
      url = "git+https://git.mob-dev.fr/Schoumi/microbit-dal.git?ref=refs/tags/v2.2.0-rc8";
      flake = false;
    };
    ytDepMbedClassic = {
      url = "git+https://git.mob-dev.fr/Schoumi/mbed-classic.git?ref=refs/tags/microbit_hfclk+mb6";
      flake = false;
    };
    ytDepBleApi = {
      url = "git+https://git.mob-dev.fr/Schoumi/BLE_API.git?ref=refs/tags/v2.5.0+mb4";
      flake = false;
    };
    ytDepNrf51822 = {
      url = "git+https://git.mob-dev.fr/Schoumi/nrf51822.git?ref=refs/tags/v2.5.0+mb7";
      flake = false;
    };
    ytDepNrf51Sdk = {
      url = "git+https://git.mob-dev.fr/Schoumi/nrf51-sdk.git?ref=refs/tags/v2.2.0+mb4";
      flake = false;
    };
  };

  outputs = { 
    self, 
    nixpkgs, 
    flake-utils, 
    yotta, 
    ytTargetMicrobit, 
    ytTargetMbed,
    ytDepMicrobit,
    ytDepMicrobitDal,
    ytDepMbedClassic,
    ytDepBleApi,
    ytDepNrf51822,
    ytDepNrf51Sdk,
  }: flake-utils.lib.eachSystem (with flake-utils.lib.system; [ x86_64-linux ]) (system:
  let
    pkgs = import nixpkgs {
      system = system;
    };

    yottaPackage = yotta.packages.${system}.default;

    pfpBuildInputs = with pkgs; [
      cmake
      srecord
      ninja
    ];

    pfp = pkgs.stdenvNoCC.mkDerivation {
      pname = "pfp";
      version = "0.1.0";
      src = ./.;

      CMAKE_MAKE_PROGRAM = "ninja";

      # Add extra build inputs here, etc
      # (TLDR: Run time)
      buildInputs = pfpBuildInputs;

      # Add extra native build inputs here, etc.
      # (TLDR: Build time)
      nativeBuildInputs = with pkgs; [
        git
        yottaPackage
        gcc-arm-embedded-8
      ];

      patchPhase = ''
        export HOME="$PWD/.fake-home"
        mkdir .fake-home

        mkdir yotta_targets
        cp -r ${ytTargetMicrobit} yotta_targets/bbc-microbit-classic-gcc
        cp -r ${ytTargetMbed} yotta_targets/mbed-gcc
        mkdir yotta_modules
        cp -r ${ytDepMicrobit} yotta_modules/microbit
        cp -r ${ytDepMicrobitDal} yotta_modules/microbit-dal
        cp -r ${ytDepMbedClassic} yotta_modules/mbed-classic
        cp -r ${ytDepBleApi} yotta_modules/ble
        cp -r ${ytDepNrf51822} yotta_modules/ble-nrf51822
        cp -r ${ytDepNrf51Sdk} yotta_modules/nrf51-sdk
      '';
      configurePhase = ''
        # Do nothing
      '';
      buildPhase = ''
        yotta build
      '';
      installPhase = ''
        mkdir $out
        cp build/bbc-microbit-classic-gcc/src/pfp-combined.hex $out/
      '';
      fixupPhase = ''
        # Do nothing
      '';
    };
  in { 
    packages.default = pfp;
    devShells.${system} = pkgs.mkShell {
      packages = [
        yottaPackage
      ] ++ pfpBuildInputs;
    };

    checks = {};
  });
}

