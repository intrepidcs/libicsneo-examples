%module icsneojava
%include <windows.i>
%include <typemaps.i>
%include <stdint.i>
%include <carrays.i>
%include <arrays_java.i>

#define DLLExport

%typemap(jni) uint8_t const *data "unsigned char *"
%typemap(jtype) uint8_t const *data "int[]"
%typemap(jstype) uint8_t const *data "int[]"

%typemap(in) uint8_t const *data %{
	$1 = $input;
%}

%typemap(jni) char *str "char *"
%typemap(jtype) char *str "String"
%typemap(jstype) char *str "String"

%{
#include "icsneo/icsneoc.h"
%}

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