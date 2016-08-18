convertToMat <- function(file, output = paste0(file, '.mat')){
  data <- extractAllChannels(file)
  
  params <- lapply(data, function(x) as.array(SI(x)) )
  names(params) <- paste0(names(data), '_params')
  
  extVars <- lapply(data, function(d){
    switch (SI(d)$type,
      event = sapply(d, attr, 'TS'),
      channels = attr(d, 'TS')[[1]]
    )
  })
  names(extVars) <- paste0(names(data), '_time')
  
  do.call(
    R.matlab::writeMat,
    c(
      list(con=output),
      params,
      extVars,
      data
    )
  )
}