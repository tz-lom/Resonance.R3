convertToMat <- function(file, output = paste0(file, '.mat'), params_output=paste0(file, '.params.mat')){
  data <- extractAllChannels(file)
  
  names(data) <- ave(
    names(data),
    names(data),
    FUN= function(x) if (length(x) == 1) x else sprintf("%s%02d", x, seq_along(x))
    )
  
  params <- lapply(data, function(x) as.array(SI(x)) )
  names(params) <- paste0(names(data), '_params')
  
  extVars <- lapply(data, function(d){
    switch (SI(d)$type,
      event = as.double(do.call(c, lapply(d, attr, 'TS'))),
      channels = as.double(attr(d, 'TS')[[1]])
    )
  })
  names(extVars) <- paste0(names(data), '_time')
  
  data <- lapply(data, function(d){
    switch(SI(d)$type,
      event = {
        if(length(d)>0){
          simplify2array(d)
        } else {
          vector("character")
        }
      },
      channels = {
        if(nrow(d)>0){
          array(d, dim(d))
        } else {
          NULL
        }
      }
    )
  })
  
  data <- Filter(Negate(is.null), data)
  extVars <- Filter(Negate(is.null), extVars)
  
  rmatio::write.mat(
    c(
      extVars,
      data
    ),
    output,compression = FALSE
  )
  do.call(
    R.matlab::writeMat,
    c(
      list(con=params_output),
      params
    )
  )
  
  
}