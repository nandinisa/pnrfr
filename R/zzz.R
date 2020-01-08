.onLoad <- function(libpath, pkgname){
  print('loading pnrf dll...')
  dll_path <- system.file(sprintf('libs%s/pnrfr.dll',
                                  Sys.getenv("R_ARCH")),
                          package = pkgname)
  print(dll_path)
  dyn.load(dll_path)
  options(print.alert = T)
}

.onUnload <- function(libpath){
  print('unloading...')
  # dyn.unload(sprintf('libs%s/pnrfr.dll', Sys.getenv("R_ARCH")))
  dyn.unload('pnrfr.dll')
}
