message(STATUS "downloading...
     src='http://ftp.research.microsoft.com/downloads/dad6c31e-2c04-471f-b724-ded18bf70fe3/TRW_S-v1.3.zip'
     dst='/home/tbeier/src/inferno/inferno-externals/trws/trws-prefix/src/TRW_S-v1.3.zip'
     timeout='none'")




file(DOWNLOAD
  "http://ftp.research.microsoft.com/downloads/dad6c31e-2c04-471f-b724-ded18bf70fe3/TRW_S-v1.3.zip"
  "/home/tbeier/src/inferno/inferno-externals/trws/trws-prefix/src/TRW_S-v1.3.zip"
  SHOW_PROGRESS
  # no EXPECTED_HASH
  # no TIMEOUT
  STATUS status
  LOG log)

list(GET status 0 status_code)
list(GET status 1 status_string)

if(NOT status_code EQUAL 0)
  message(FATAL_ERROR "error: downloading 'http://ftp.research.microsoft.com/downloads/dad6c31e-2c04-471f-b724-ded18bf70fe3/TRW_S-v1.3.zip' failed
  status_code: ${status_code}
  status_string: ${status_string}
  log: ${log}
")
endif()

message(STATUS "downloading... done")
