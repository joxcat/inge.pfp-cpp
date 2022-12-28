{
  inputs = {
    nixpkgs = {
      url = "github:NixOS/nixpkgs/nixos-unstable";
    };
    flake-utils.url = "github:numtide/flake-utils";
    yotta = {
      url = "github:joxcat/yotta?dir=nix";
      # url = "path:/home/kuro/Git/yotta/nix";
    };
    # Targets
    ytTargetMicrobit = {
      url = "git+https://git.mob-dev.fr/Schoumi/yotta-target-bbc-microbit-classic-gcc.git";
      flake = false;
    };
    ytTargetMbed = {
      url = "git+https://git.mob-dev.fr/Schoumi/target-mbed-gcc.git";
      flake = false;
    };
    # Yotta dependencies
    ytDepMicrobit = {
      url = "git+https://git.mob-dev.fr/Schoumi/microbit.git";
      flake = false;
    };
    ytDepMicrobitDal = {
      url = "github:lancaster-university/microbit-dal?ref=v2.2.0-rc6";
      flake = false;
    };
    ytDepMbedClassic = {
      url = "github:lancaster-university/mbed-classic?ref=microbit_hfclk";
      flake = false;
    };
    ytDepBleApi = {
      url = "github:lancaster-university/BLE_API?ref=v2.5.0";
      flake = false;
    };
    ytDepNrf51822 = {
      url = "github:lancaster-university/nrf51822?ref=v2.5.0";
      flake = false;
    };
    ytDepNrf51Sdk = {
      url = "github:lancaster-university/nrf51-sdk?ref=v2.2.0";
      flake = false;
    };
    ytDepMbedDrivers = {
      url = "github:ARMmbed/mbed-drivers";
      flake = false;
    };
    ytDepCompilerPolyfill = {
      url = "github:ARMmbed/compiler-polyfill?ref=v1.2.1";
      flake = false;
    };
    ytDepMbedHal = {
      url = "github:ARMmbed/mbed-hal?ref=v1.2.1";
      flake = false;
    };
    ytDepCmsisCore = {
      url = "github:ARMmbed/cmsis-core";
      flake = false;
    };
    ytDepUalloc = {
      url = "github:ARMmbed/ualloc";
      flake = false;
    };
    ytDepMinar = {
      url = "github:ARMmbed/minar";
      flake = false;
    };
    ytDepCoreUtil = {
      url = "github:ARMmbed/core-util";
      flake = false;
    };
    ytDepGreenteaClient = {
      url = "github:ARMmbed/greentea-client?ref=v1.3.0";
      flake = false;
    };
    ytDepMbedHalNordic = {
      url = "github:ARMmbed/mbed-hal-nordic?ref=v2.1.0";
      flake = false;
    };
    ytDepMbedHalNrf51822Mcu = {
      url = "github:ARMmbed/mbed-hal-nrf51822-mcu?ref=v2.2.0";
      flake = false;
    };
    ytDepCmsisCoreNordic = {
      url = "github:ARMmbed/cmsis-core-nordic";
      flake = false;
    };
    ytDepCmsisCoreNrf51822 = {
      url = "github:ARMmbed/cmsis-core-nrf51822";
      flake = false;
    };
    ytDepDmalloc = {
      url = "github:ARMmbed/dlmalloc";
      flake = false;
    };
    ytDepMinarPlatform = {
      url = "github:ARMmbed/minar-platform";
      flake = false;
    };
    ytDepMinarPlatformMbed = {
      url = "github:ARMmbed/minar-platform-mbed";
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
    ytDepMbedDrivers,
    ytDepCompilerPolyfill,
    ytDepMbedHal,
    ytDepCmsisCore,
    ytDepUalloc,
    ytDepMinar,
    ytDepCoreUtil,
    ytDepGreenteaClient,
    ytDepMbedHalNordic,
    ytDepMbedHalNrf51822Mcu,
    ytDepCmsisCoreNordic,
    ytDepCmsisCoreNrf51822,
    ytDepDmalloc,
    ytDepMinarPlatform,
    ytDepMinarPlatformMbed
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
        cp -r ${ytDepMbedDrivers} yotta_modules/mbed-drivers
        cp -r ${ytDepCompilerPolyfill} yotta_modules/compiler-polyfill
        cp -r ${ytDepMbedHal} yotta_modules/mbed-hal
        cp -r ${ytDepCmsisCore} yotta_modules/cmsis-core
        cp -r ${ytDepUalloc} yotta_modules/ualloc
        cp -r ${ytDepMinar} yotta_modules/minar
        cp -r ${ytDepCoreUtil} yotta_modules/core-util
        cp -r ${ytDepGreenteaClient} yotta_modules/greentea-client
        cp -r ${ytDepMbedHalNordic} yotta_modules/mbed-hal-nordic
        cp -r ${ytDepMbedHalNrf51822Mcu} yotta_modules/mbed-hal-nrf51822-mcu
        cp -r ${ytDepCmsisCoreNordic} yotta_modules/cmsis-core-nordic
        cp -r ${ytDepCmsisCoreNrf51822} yotta_modules/cmsis-core-nrf51822
        cp -r ${ytDepDmalloc} yotta_modules/dlmalloc
        cp -r ${ytDepMinarPlatform} yotta_modules/minar-platform
        cp -r ${ytDepMinarPlatformMbed} yotta_modules/minar-platform-mbed

        chmod 755 -R yotta_modules yotta_targets
      '';
      configurePhase = ''
        # Do nothing
      '';
      buildPhase = ''
        c=yotta_targets/bbc-microbit-classic-gcc/CMake/toolchain.cmake
        echo "set(TARGET_LIKE_NRF51_GENERIC TRUE)" >> $c
        echo "set(NRF51 TRUE)" >> $c
        # echo "set(TARGET_NORDIC TRUE)" >> $c
        # echo "set(TARGET_M0 TRUE)" >> $c
        sed -i 's|--specs=nano\.specs|--specs=nano.specs --specs=nosys.specs|' yotta_targets/mbed-gcc/CMake/Platform/mbedOS-GNU-C.cmake
        sed -i 's|--specs=nano\.specs|--specs=nano.specs --specs=nosys.specs|' yotta_targets/mbed-gcc/CMake/Platform/mbedOS-GNU-CXX.cmake
        yotta build
      '';
      installPhase = ''
        mkdir $out
        cp -r yotta_modules yotta_targets $out/
        # cp build/bbc-microbit-classic-gcc/source/microbit-samples-combined.hex $out/
      '';
      fixupPhase = ''
        # Do nothin
      '';
    };
  in { 
    # TODO: nix build
    packages.default = pfp;
    devShells.${system} = pkgs.mkShell {
      packages = [
        yottaPackage
      ] ++ pfpBuildInputs;
    };

    checks = {};
  });
}

