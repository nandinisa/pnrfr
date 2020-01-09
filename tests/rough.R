run_all <- function(){
  dyn.load('c:/pnrfr/inst/libs/x64/pnrfr.dll')
  dyn.unload('c:/pnrfr/inst/libs/x64/pnrfr.dll')

  # file_path <- 'tests/testthat/testdata/VT095.pnrf'
  # save_path <- 'tests/testthat/testdata/VT095.csv'
  file_path <- 'C:\\pnrfr\\tests\\testthat\\testdata\\VT095.pnrf'
  save_path <- 'C:\\pnrfr\\tests\\testthat\\testdata\\VT095.csv'
  source('R/utils.R')
  source('R/pnrf_params.R')
  source('R/pnrf_reader.R')
  source('R/pnrf_computations.R')

  result <- read_channel_data(file_path, channel_no = 3, data_type = 3)

  result <- read_file_meta(file_path)
  result <- read_channel_data_obs(file_path, channel_no = 3)
  result <- read_channel_data(file_path, channel_no = 3, data_type = 3)
  result <- read_segment_data(file_path, channel_no = 3, segment_no = 3)
  result <- read_segment_data(file_path, channel_no = 3, segment_no = 107)
  result <- convert_channel_data_csv(file_path, save_path, channel_no = 3)
  result <- compute_moving_average(file_path, channel_no = 3, data_type = 1, window_time = 0.01)

  if(file.exists(save_path)) file.remove(save_path)
  result <- convert_channel_data_csv(file_path, save_path, channel_no = 3)

  result <- compute_moving_average(file_path, channel_no = 3, window_time = 0.01)

  result <- read_all_channel_data(file_path)

  #check with sampling rate
  result <- read_channel_data(file_path, channel_no = 3, sampling = T)
  result <- read_segment_data(file_path, channel_no = 3, segment_no = 107, sampling = T)
  result <- convert_channel_data_csv(file_path, save_path, channel_no = 3, data_type = 1, sampling = T)
  result <- compute_moving_average(file_path, channel_no = 3, window_time = 0.01, sampling = T)


  dyn.unload('bin/pnrfr.dll')


}


run_dir <- function(){
  dyn.load('bin/pnrfr.dll')
  # dyn.unload('bin/pnrfr.dll')

  source('R/utils.R')
  source('R/pnrf_params.R')
  source('R/pnrf_reader.R')
  source('R/pnrf_computations.R')
  import::from(magrittr, '%>%')
  import::from(dplyr, mutate, filter)

  options(print.alert = T)


  # Algorithm functionality testing
  experiments.folder <- 'F:\\experiments\\'
  fault_file <- read.csv(paste0(experiments.folder, 'basic_run_sheet.csv'),
                         check.names = F) %>%
    filter(valid == 1) %>%
    mutate(file_name = sprintf("VT%03d.pnrf", test))
  experiments.folder.processed <- paste0(experiments.folder, 'processed')
  if(dir.exists(experiments.folder.processed)){
    unlink(experiments.folder.processed, recursive = T, force = T)
  }
  dir.create(experiments.folder.processed)

  dirResult <- .C("processDirectory",
                  filePath = as.character(paste0(experiments.folder, 'raw')),
                  fileArray = as.character(fault_file$file_name),
                  fileArraySize = as.integer(nrow(fault_file)),
                  channel = as.integer(3),
                  dataType = as.integer(1),
                  sampling = as.logical(TRUE),
                  samplingRate = as.integer(1000),
                  computeMovAvg = as.logical(TRUE),
                  windowTime = as.double(0.01),
                  savePath = experiments.folder.processed,
                  msg = as.character(''), out = as.integer(1))

  # dirResult <- .C("processDirectory",
  #                 filePath = as.character(filePath),
  #                 fileArray = as.character(c('VT607.pnrf', 'VT608.pnrf', 'VT609.pnrf')),
  #                 fileArraySize = as.integer(3),
  #                 channel = as.integer(3),
  #                 dataType = as.integer(1),
  #                 sampling = T,
  #                 samplingRate = 100,
  #                 computeMovAvg = FALSE,
  #                 windowTime = as.double(0.01),
  #                 savePath = experiments.folder.processed,
  #                 msg = msg, out = as.integer(1))


  result <- read_channel_data_size(pfile, channel_no = 3, data_type = 1)
  result <- compute_moving_average(pfile, channel_no = 3, window_time = 0.01, data_type = 3)
  al.time <- detect_positive_derivative(result, 100)

  plot(result[,2],type="l")


  library(anomalousACM)

  z <- tsmeasures(dat[,c(1,3)])
  dyn.unload('bin/pnrfr.dll')


}
