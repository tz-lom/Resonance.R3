R3ReadChannel <- function(file, stream){
  blocks <- blockLevelRead(file)
  
  streams <- list()
  
  for(h in 1:4){
    if(inherits( blocks[[h]], 'StreamDescription')){
      streams[ blocks[[h]]$name ] <- blocks[[h]]$id
    }
  }
  
  eegL <- Filter(
    function(b){
      if(!inherits(b, "DataBlock")) return(F)
      if(attr(b, 'stream')!=stream) return(F)
      T
    },
    blocks
  )
  cols <- ncol(eegL[[1]])
  do.call(
    rbind,
    lapply(eegL, matrix, ncol=cols)
    )
}