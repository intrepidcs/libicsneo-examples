%module icsneojava
%include <windows.i>
%include <typemaps.i>
%include <stdint.i>
%include <carrays.i>
%include <various.i>
%include <java.i>
%include <arrays_java.i>

#define DLLExport

%typemap(jstype) uint8_t const *data "int[]"
%typemap(jtype) uint8_t const *data "int[]"
%typemap(jni) uint8_t const *data "char *"

%typemap(out) uint8_t const *data %{
// in java_wrap and can use to cast output properly
%}

%typemap(javaout) uint8_t const *data {
    return $jnicall;
}

%typemap(javain) uint8_t const *data %{
    value
%}

//%typemap(jni) char *str "char *"
//%typemap(jtype) char *str "char[]"
//%typemap(jstype) char *str "char[]"

%{
#include "icsneo/icsneoc.h"
%}

%apply int *INOUT {size_t *};
//%apply int *INOUT {size_t *maxLength};

%include "icsneo/icsneoc.h"
%include "icsneo/device/neodevice.h"
%include "icsneo/communication/message/neomessage.h"
%include "icsneo/device/devicetype.h"
%include "icsneo/api/version.h"
%include "icsneo/api/event.h"
%include "icsneo/communication/network.h"

%inline %{
static neomessage_can_t* neomessage_can_t_cast(neomessage_t* msg) {
	return (neomessage_can_t*) msg;
}

static neomessage_eth_t* neomessage_eth_t_cast(neomessage_t* msg) {
	return (neomessage_eth_t*) msg;
}

static neomessage_t* from_can_neomessage_t_cast(neomessage_can_t* msg) {
	return (neomessage_t*) msg;
}

static neomessage_t* from_eth_neomessage_t_cast(neomessage_eth_t* msg) {
	return (neomessage_t*) msg;
}
%}

%array_functions(neodevice_t, neodevice_t_array);
%array_functions(neoevent_t, neoevent_t_array);
%array_functions(neomessage_t, neomessage_t_array);