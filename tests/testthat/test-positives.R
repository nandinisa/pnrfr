context("test-positives.R")

test_that("Able to file meta", {
  file_path <- 'testdata/VT095.pnrf'

  result <- read_file_meta(file_path)
  expect_equal(length(read_file_meta(file_path)), 4)

  file_path <- 'testdata/VT094.pnrf'
  result <- read_file_meta(file_path)
  expect_equal(length(read_file_meta(file_path)), 4)

})

test_that("Able to read channel and segment data", {
  file_path <- 'testdata/VT095.pnrf'

  result <- read_channel_data_size(file_path, channel_no = 3)
  expect_equal(result, 7422492)

  result <- read_channel_data(file_path, channel_no = 3)
  expect_equal(length(result$channels$signal[[1]]$X), 7422492)
  expect_equal(length(result$channels$signal[[1]]$Y), 7422492)

  result <- read_segment_data(file_path, channel_no = 3, segment_no = 3)
  expect_equal(length(result$channels$signal[[1]]$X), 98000)
  expect_equal(length(result$channels$signal[[1]]$Y), 98000)

  result <- read_segment_data(file_path, channel_no = 3, segment_no = 107)
  expect_equal(length(result$channels$signal[[1]]$X), 125932)
  expect_equal(length(result$channels$signal[[1]]$Y), 125932)
})

test_that("Able to read channel and segment data with sampling", {
  file_path <- 'testdata/VT095.pnrf'

  result <- read_channel_data(file_path, channel_no = 3,
                              sampling = T, sampling_rate = 1000)
  expect_equal(length(result$channels$signal[[1]]$X), 7425)
  expect_equal(length(result$channels$signal[[1]]$Y), 7425)

  result <- read_segment_data(file_path, channel_no = 3, segment_no = 107,
                              sampling = T, sampling_rate = 1000)
  expect_equal(length(result$channels$signal[[1]]$X), 126)
  expect_equal(length(result$channels$signal[[1]]$Y), 126)
})

test_that("Able to save channel data", {
  file_path <- 'testdata/VT095.pnrf'
  save_path <- 'testdata/VT095.csv'

  if(file.exists(save_path)) file.remove(save_path)
  result <- convert_channel_data_csv(file_path, save_path, channel_no = 3)
  expect_equal(file.exists(save_path), TRUE)
  file.remove(save_path)
})

test_that('Able to save sampled channel data', {
  file_path <- 'testdata/VT095.pnrf'
  save_path <- 'testdata/VT095.csv'
  if(file.exists(save_path)) file.remove(save_path)

  result <- convert_channel_data_csv(file_path, save_path,
                                     channel_no = 3, data_type = 1,
                                     sampling = T)
  expect_equal(file.exists(save_path), TRUE)
  file.remove(save_path)
})

test_that("Compute moving average with and without sampling", {
  file_path <- 'testdata/VT095.pnrf'

  result <- compute_moving_average(file_path, channel_no = 3, window_time = 0.01)
  expect_equal(length(result$channels$rms_signal[[1]]$X), 7422492)
  expect_equal(length(result$channels$rms_signal[[1]]$Y), 7422492)

  # with sampling
  result <- compute_moving_average(file_path, channel_no = 3, window_time = 0.01,
                                   sampling = T, sampling_rate = 1000)
  expect_equal(length(result$channels$rms_signal[[1]]$X), 7425)
  expect_equal(length(result$channels$rms_signal[[1]]$Y), 7425)
})


test_that("Read all channel data", {
  file_path <- 'testdata/VT095.pnrf'

  # default sampling rate = 100
  result <- read_all_channel_data(file_path)
  expect_equal(nrow(result$channels), 4)
  expect_equal(result$name, 'VT095.pnrf')
  expect_equal(nrow(result$channels[1,]$signal[[1]]), 74228)
  expect_equal(nrow(result$channels[3,]$signal[[1]]), 74228)

})
