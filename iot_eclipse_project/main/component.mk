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
COMPONENT_EMBED_TXTFILES :=  /home/dustin/workspace/esp32/iot_lightstrip/iot_eclipse_project/external/certificate_ota/ca_cert.pem
