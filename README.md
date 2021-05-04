
# pnrfr

<!-- badges: start -->

<!-- badges: end -->

The R package pnrfr helps to read channel data and meta information from PNRF files. The package requires the installation of [PNRF Reader Toolkit](https://www.hbm.com/en/2475/support-downloads-perception/
), which is windows dependant. Thus, the package is only supported on windows machine.


## Reference paper
S. Kandanaarachchi, N. Anantharama and M. A. Munoz, "Early detection of vegetation ignition due to powerline faults," in IEEE Transactions on Power Delivery, doi: 10.1109/TPWRD.2020.3006553

## Installation


You can install the pnrfr package using: 


```{r}
devtools::install_github("nandinisa/pnrfr")
```


## Examples

### PNRF read functions

* read_file_meta - Reads the meta data of a PNRF file
``` r
library(pnrfr)
## basic example code
result <- read_file_meta(system.file("extdata", "VT094.pnrf", package="pnrfr"))

```

* read_channel_data - Reads the specified channel data from a PNRF file (at a given samplinng rate)

``` r
library(pnrfr)
## basic example code
result <- read_channel_data(system.file("extdata", "VT095.pnrf", package="pnrfr"), 
          channel_no = 3,
          sampling = TRUE, 
          sampling_rate = 1000)
```

### Other functions include

* read_all_channel_data - Reads all available channel data from a PNRF file (at a given samplinng rate)
* read_segment_data - Reads the segment data corresponding to a channel from a PNRF file (at a given samplinng rate)
* save_channel_data.to_csv - Save channel data to csv

### PNRF RMS function

* compute_moving_average - Computes the moving average for a channel in a PNRF file given a moving window

``` r
library(pnrfr)
## basic example code
result <- compute_moving_average(system.file("extdata", "VT094.pnrf", package="pnrfr"),
        channel_no = 3,
        window_time = 0.01,
        sampling = TRUE, 
        sampling_rate = 1000)
```

