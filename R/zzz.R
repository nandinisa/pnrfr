.onAttach <- function(libpath, pkgname){
  packageStartupMessage('loading pnrf dll...')
  dll_path <- system.file(sprintf('libs%s/pnrfr.dll',
                                  Sys.getenv("R_ARCH")),
                          package = pkgname)
  dyn.load(dll_path, symbol='pnrf.dll')
}

.onDetach <- function(libpath){
  packageStartupMessage('unloading dll...')
  dyn.unload(sprintf('%s/libs%s/pnrfr.dll', libpath, Sys.getenv("R_ARCH")))
}
