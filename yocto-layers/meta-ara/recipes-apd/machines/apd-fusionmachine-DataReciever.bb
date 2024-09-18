SECTION = "devel"
LICENSE = "AUTOSAR"
LIC_FILES_CHKSUM = "file://LICENSE;md5=b64e97d3c7b53b1c5789d61baab7ee2e"

inherit apd_application externalsrc

EXTERNALSRC := "${THISDIR}/../../../../sample-applications/applications/DataReciever"

EXTRA_OECMAKE += " \
    -DTARGET_MACHINE=FusionMachine \
    -DAPP_NAME=DataReciever \
    -DPROC_NAME=DataReciever \
"

# Please keep this list alphabetically sorted for convenience.
DEPENDS += " \
    apd-applications-arxmls \
    apd-common-machine-arxmls \
    apd-fusionmachine-arxmls \
    apd-interfaces-arxmls \
    apd-network-arxmls \
    apd-dlt-arxmls \
    ara-arxmls \
    ara-adi-sensoritf-arxmls \
    ara-com-lib \
    ara-core \
    ara-exec \
    ara-log \
"
