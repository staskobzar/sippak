{ stdenv, lib, fetchzip, cmake, pjsip, cmocka, dbus, pkg-config, }:

stdenv.mkDerivation {
  pname = "sippak";
  version = "1.1.0";

  src = fetchzip {
    url = "https://github.com/staskobzar/sippak/archive/refs/tags/v1.1.0.zip";
    sha256 = "sha256-B9aANfpdYzUN5hm4NvPNcXi/iSQSTjNfLoKUpyNvTKk=";
  };

  buildInputs = [ pjsip cmocka cmake dbus ];

  nativeBuildInputs = [ pkg-config ];

  buildPhase = ''
    runHook preInstall
      make
      make install
    runHook postInstall
  '';
}
