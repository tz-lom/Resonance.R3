toPlotInit <- function(){
  list(
    minTS = Inf,
    maxTS = -Inf,
    lines = list(),
    marks = c()
  )
}

toPlotChannels <- function(toPlot, channels, times){
  toPlot$minTS <- min(toPlot$minTS, times)
  toPlot$maxTS <- max(toPlot$maxTS, times)
  
  toPlot$lines <- c(toPlot$lines,
                    list(list(time=times, y=channels))
  )
  
  toPlot
}

toPlotMarks <- function(toPlot, times){
  
  toPlot$minTS <- min(toPlot$minTS, times)
  toPlot$maxTS <- max(toPlot$maxTS, times)
  
  toPlot$marks <- c(toPlot$marks, times)
  
  toPlot
}

letsPlot <- function(toPlot){
  par(
    mfrow=c(
      sum(sapply(toPlot$lines, function(X) ncol(X$y))), 
      1),
    mai = c(0,0,0,0)
    )
  
  xlim <- c(toPlot$minTS, toPlot$maxTS)
  
  for(curve in toPlot$lines){
    for(y in 1:ncol(curve$y)){
      plot(curve$time, curve$y[,y], type='l', xlim=xlim, xlab="", ylab="", xaxt="n", yaxt="n", frame.plot=F)
      lapply(toPlot$marks, function(X) abline(v=X))
    }
  }
  
}