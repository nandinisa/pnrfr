context("test-positives.R")

test_that("Test - Read file meta", {
  file_path <- 'testdata/VT095.pnrf'

  result <- read_file_meta(file_path)
  # check length of result
  expect_equal(length(result), 4)
  # check if main meta information is retrieved
  expect_equal(all(names(result) ==
                      c('title', 'recordCount', 'channelCount', 'records')), T)
  # check if meta information retrieved is correct
  expect_equal(result$title, 'VT095')
  expect_equal(result$recordCount, 2)
  expect_equal(result$channelCount, 7)
  # check for individual record/channel descriptors ( 1st channel)
  expect_equal(result$records$rec1$ch1$channel, 1)
  expect_equal(result$records$rec1$ch1$name, 'Direct voltage')
  expect_equal(result$records$rec1$ch1$type, 'DataChannelType_Analog')
  expect_equal(result$records$rec1$ch1$yunit, 'V')
  expect_equal(round(result$records$rec1$ch1$startTime, 3),
               0)
  expect_equal(round(result$records$rec1$ch1$endTime, 3),
               54.085)
  expect_equal(result$records$rec1$ch1$noofSamples, 80550)
  expect_equal(result$records$rec1$ch1$sampleInterval, 1e-05)
  expect_equal(result$records$rec1$ch1$segCount, 107)

  # File 2
  file_path <- 'testdata/VT094.pnrf'
  result <- read_file_meta(file_path)
  expect_equal(length(result), 4)
  expect_equal(all(names(result) ==
                     c('title', 'recordCount', 'channelCount', 'records')), T)
  expect_equal(result$title, 'VT094')
  expect_equal(result$recordCount, 2)
  expect_equal(result$channelCount, 7)

  expect_equal(result$records$rec2$ch3$channel, 3)
  expect_equal(result$records$rec2$ch3$name, 'Direct current')
  expect_equal(result$records$rec2$ch3$type, 'DataChannelType_Analog')
  expect_equal(result$records$rec2$ch3$yunit, 'A')
  expect_equal(round(result$records$rec1$ch1$startTime, 3),
               0)
  expect_equal(round(result$records$rec1$ch1$endTime, 3),
               31.385)
  expect_equal(result$records$rec2$ch3$noofSamples, 25201)
  expect_equal(result$records$rec2$ch3$sampleInterval, 1e-05)
  expect_equal(result$records$rec2$ch3$segCount, 65)

})


test_that("Test - Read channel and segment data and size", {
  file_path <- 'testdata/VT095.pnrf'

  result <- read_channel_data_obs(file_path, channel_no = 3)
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


test_that("Test - Read channel and segment data with sampling", {
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


test_that("Test - Save channel data as csv", {
  file_path <- 'testdata/VT095.pnrf'
  save_path <- 'testdata/VT095.csv'

  if(file.exists(save_path)) file.remove(save_path)
  save_channel_data.to_csv(file_path, save_path, channel_no = 3)
  expect_equal(file.exists(save_path), TRUE)
  file.remove(save_path)
})

test_that('Test - Save sampled channel data', {
  file_path <- 'testdata/VT095.pnrf'
  save_path <- 'testdata/VT095.csv'
  if(file.exists(save_path)) file.remove(save_path)
  save_channel_data.to_csv(file_path, save_path,
                           channel_no = 3,
                           sampling = T, sampling_rate =1000)
  expect_equal(file.exists(save_path), TRUE)
  result <- read.csv(save_path, stringsAsFactors = F)
  expect_equal(nrow(result), 7425)
  file.remove(save_path)
})

test_that("Test - Compute moving average with and without sampling", {
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


test_that("Test - Read all channel data", {
  file_path <- 'testdata/VT095.pnrf'

  # default sampling rate = 100
  result <- read_all_channel_data(file_path)
  expect_equal(nrow(result$channels), 4)
  expect_equal(result$name, 'VT095.pnrf')
  expect_equal(nrow(result$channels[1,]$signal[[1]]), 74228)
  expect_equal(nrow(result$channels[3,]$signal[[1]]), 74228)

})
