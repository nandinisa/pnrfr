context("test-negatives.R")


test_that("Handles wrong path, segment, channel", {
  # Wrong file path
  expect_error(read_file_meta('dummy/test.pnrf'),
               'Please check if the file exists and you have read access')

  expect_error(read_file_meta(''),
               'Please check if the file exists and you have read access')

  expect_error(read_file_meta(' '),
               'Please check if the file exists and you have read access')

  # Wrong file extension
  #expect_error(read_file_meta('testdata/testdata.txt'),
    #           'Error occurred in pnrfr.dll - Cannot load the file')

  file_path <- 'testdata/VT095.pnrf'

  # Invalid channel/ segment number
  expect_error(read_channel_data(file_path, channel_no = 0),
               'Invalid channel, channel number must be greater or eqaul to 1')
  expect_error(read_channel_data(file_path, channel_no = 10),
               'Error occurred in pnrfr.dll - Index out of range, no channel found')
  expect_error(read_segment_data(file_path, channel_no = 43, segment_no = 13000),
               'Error occurred in pnrfr.dll - Index out of range, no channel found')
  expect_error(read_segment_data(file_path, channel_no = 3, segment_no = 13000),
               'Error occurred in pnrfr.dll - No Segments found')
  expect_error(read_segment_data(file_path, channel_no = 3, segment_no = 0),
               'Invalid segment, segment number must be greater or eqaul to 1')

  save_path <- 'testdata1/test_VT095.csv'
  # Invalid save path
  expect_error(convert_channel_data_csv(file_path, save_path, channel_no = 3),
               'Please check if the directory exists and you have read/write access to dir - E:/bit/pnrfprocessor/pnrfr/tests/testthat/testdata1')
})
