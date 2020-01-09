.onLoad <- function(libpath, pkgname){
  print('loading pnrf dll...')
  dll_path <- system.file(sprintf('libs%s/pnrfr.dll',
                                  Sys.getenv("R_ARCH")),
                          package = pkgname)
  print(dll_path)
  dyn.load(dll_path, symbol='pnrf.dll')
  options(print.alert = T)
}

.onUnload <- function(libpath){
  print('unloading dll...')
  options(print.alert = F)
  dyn.unload(sprintf('%s/%s/pnrfr.dll', libpath, Sys.getenv("R_ARCH")))
}
