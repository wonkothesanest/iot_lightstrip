#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_ADD_INCLUDEDIRS := \
		. \
        driver \
        ../config

COMPONENT_SRCDIRS := \
		. \
        driver \
        ../config
        
        
COMPONENT_EMBED_TXTFILES :=  ${IDF_PATH}/examples/system/ota/server_certs/ca_cert.pem
