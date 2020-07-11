#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_ADD_INCLUDEDIRS := \
		. \
        driver \
        ../config \
        driver/udp_logging/include

COMPONENT_SRCDIRS := \
		. \
        driver \
        ../config \
        driver/udp_logging
        

# Directive to know where to download the server certificates     
COMPONENT_EMBED_TXTFILES :=  ${IDF_PATH}/examples/system/ota/server_certs/ca_cert.pem
