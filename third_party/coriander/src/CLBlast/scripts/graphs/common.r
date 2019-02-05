
# ==================================================================================================
# This file is part of the CLBlast project. The project is licensed under Apache Version 2.0. This
# project uses a tab-size of two spaces and a max-width of 100 characters per line.
#
# Author(s):
#   Cedric Nugteren <www.cedricnugteren.nl>
#
# This file implements the common performance scripts, such as creating a graph.
#
# ==================================================================================================

# Colours
black     = "#000000"
grey      = "#888888"
purplish  = "#550077" # [ 85,  0,119] lumi=26
blueish   = "#4765b1" # [ 71,101,177] lumi=100
redish    = "#d67568" # [214,117,104] lumi=136
greenish  = "#9bd4ca" # [155,212,202] lumi=199
colourset = c(blueish, redish, greenish, purplish)

# Sets the graph markers (circles, triangles, etc.)
pchs = c(15, 18, 17, 12)

# Other constants
kilo = 1024
mega = 1024*1024

# R options
options("width"=170)

# ==================================================================================================

# Constants
num_runs <- 4
devices <- c("-platform","-device")
options_string <- "-q -no_abbrv -cblas 0"
library_names <- c("CLBlast", "clBLAS")

# Command-line arguments
command_line <- commandArgs(trailingOnly=TRUE)
if (length(command_line) != 2) {
  print("Usage for device Z on platform Y: Rscript xxxxx.r Y Z")
  quit()
}
platform_id <- command_line[1]
device_id <- command_line[2]

# Selects the device
devices_values <- c(platform_id, device_id)
devices_string <- paste(devices, devices_values, collapse=" ")

# ==================================================================================================

# The main function
main <- function(routine_name, precision, test_names, test_values,
                test_xlabels, test_xaxis, metric_gflops) {

  # Names
  display_name <- toupper(routine_name)
  if (precision == 16) { display_name <- gsub("^X","H",display_name); }
  if (precision == 32) { display_name <- gsub("^X","S",display_name); }
  if (precision == 64) { display_name <- gsub("^X","D",display_name); }
  if (precision == 3232) { display_name <- gsub("^X","C",display_name); }
  if (precision == 6464) { display_name <- gsub("^X","Z",display_name); }
  executable <- paste("./clblast_client_", routine_name, sep="")

  # Configures the outputfile
  pdf(paste(display_name, ".pdf", sep=""), height=8, width=13)
  par(mfrow=c(2, 3))
  par(oma=c(0, 0, 0, 0))
  par(mar=c(4.6, 4.4, 1.5, 0)) # bottom, left, top, right [c(5.1, 4.1, 4.1, 2.1)]
  par(mgp=c(2.8, 0.6, 0)) # location of xlab/ylab, tick-mark labels, tick marks [c(3, 1, 0)]

  # Loops over the test-cases
  for (test_id in 1:length(test_names)) {
    params_values <- test_values[[test_id]]

    # Loops over the commands within a single list (within a case)
    for (command_id in 1:length(params_values)) {

      # Runs the client and captures the result
      params_string <- paste(parameters, params_values[[command_id]], collapse=" ")
      arguments <- paste(devices_string, params_string, options_string, sep=" ")
      print(paste("Running", executable, arguments, sep=" "))
      raw_result_string <- system2(command=executable, args=arguments, stdout=TRUE)

      # Filter the string: only lines containing a ";" can be valid lines
      result_string <- c()
      for (line in raw_result_string) {
        if (grepl(";",line)) {
          result_string <-
           c(result_string, line)
        }
      }

      # Reads the result into a dataframe
      command_db <- read.csv(text=result_string, sep=";")

      # Append the results to the final dataframe
      if (command_id == 1) {
        db <- command_db
      } else {
        db <- rbind(db, command_db)
      }
    }
    print(db)

    # Sets the values on the x-axis and their labels (test dependent)
    if (is.character(test_xaxis[[test_id]][[1]])) {
      xdata <- db[,test_xaxis[[test_id]][[1]]]
      xtics <- xdata
      log_scale <- test_xaxis[[test_id]][[2]]
    }
    else {
      xdata <- test_xaxis[[test_id]][[1]]
      xtics <- test_xaxis[[test_id]][[2]]
      log_scale <- ""
    }

    # Plots the graph with GFLOPS on the Y-axis
    if (metric_gflops) {
      plot_graph(xdata=xdata, ydata=list(db$GFLOPS_1, db$GFLOPS_2), log_setting=log_scale,
                 xmin=min(xdata), xmax=max(xdata),
                 ymin=0, ymax=max(max(db$GFLOPS_1),max(db$GFLOPS_2)),
                 xtics=xtics,
                 xlabel=test_xlabels[[test_id]], ylabel="GFLOPS (higher is better)",
                 graph_title=paste(display_name, test_names[[test_id]], sep=" "),
                 multiple=50, experiment_names=library_names)
    # Plots the graph with GB/s on the Y-axis
    } else {
      plot_graph(xdata=xdata, ydata=list(db$GBs_1, db$GBs_2), log_setting=log_scale,
                 xmin=min(xdata), xmax=max(xdata),
                 ymin=0, ymax=max(max(db$GBs_1),max(db$GBs_2)),
                 xtics=xtics,
                 xlabel=test_xlabels[[test_id]], ylabel="GB/s (higher is better)",
                 graph_title=paste(display_name, test_names[[test_id]], sep=" "),
                 multiple=10, experiment_names=library_names)
    }
  }
}

# ==================================================================================================

# Plots data
plot_graph <- function(xdata, ydata, log_setting,
                       xmin, xmax, ymin, ymax,
                       xtics, xlabel, ylabel,
                       graph_title,
                       multiple, experiment_names) {

  # Update the ymax to the next multiple of something
  ymax <- multiple*ceiling(ymax/multiple)

  # Add kilo or mega to the x-labels
  for (i in 1:length(xtics)) {
    if (!is.na(as.numeric(xtics[i]))) {
      if (as.numeric(xtics[i])%%mega == 0) {
        xtics[i] <- paste(as.character(as.numeric(xtics[i])/mega), "M", sep="")
      } else if (as.numeric(xtics[i])%%kilo == 0) {
        xtics[i] <- paste(as.character(as.numeric(xtics[i])/kilo), "K", sep="")
      }
    }
  }

  # Creates an initial graph with axis but without data
  par(new=F)
  plot(x=xmin:xmax, y=rep(1, length(xmin:xmax)), log=log_setting,
       main="", xlab="", ylab="",
       ylim=c(ymin, ymax), xlim=c(xmin, xmax), axes=F, "n")
  axis(side=2, las=2)
  axis(side=1, at=xdata, labels=xtics, las=2)
  title(xlab=xlabel, line=-1)
  title(ylab=ylabel, line=2)
  title(graph_title, line=-2)
  par(new=T)

  # Loops over all experiments
  num_experiments <- length(ydata)
  for (id in 1:num_experiments) {

    # Plots the data for this experiment
    plot(x=xdata, y=ydata[[id]], log=log_setting,
         col=colourset[id], pch=pchs[id], lty=1, lwd=1, cex=1,
         xlab="", ylab="", ylim=c(ymin, ymax), xlim=c(xmin, xmax),
         axes=F, "b", xpd=T)
    par(new=T)
  }

  # Add a legend
  legend("bottomright", experiment_names,
         lwd=1, ncol=1, col=colourset, pch=pchs, lty=1, cex=1,
         bty="n", xpd=T)

  # Done
  par(new=F)
}

# ==================================================================================================
