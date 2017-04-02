readStructurized <- function(file){
  file <- normalizePath(file)
  
  data <- readStructurized_R3(file)
  if(length(data)==0)
  {
    data <- readStructurized_R2(file)
  }
  
  data
}