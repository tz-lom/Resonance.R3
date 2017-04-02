#include "generate.h"

#include "protocol_definition.h"




namespace Resonance {
namespace R3 {

#include "generate_namespace_include.h"
GENERATE_PREFIX(PROTOCOL_DESCR)
#include "generate_class_prefix.h"
GENERATE_CLASS_H(PROTOCOL_DESCR)
#include "generate_class_postfix.h"
//GENERATE_CLASS_TEMPLATES(PROTOCOL_DESCR)

}
}

#include "generate_undef.h"

