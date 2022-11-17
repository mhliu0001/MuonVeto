#ifndef MVSpecialModes_h
#define MVSpecialModes_h 1

#include "MVGlobals.hh"

namespace MuonVeto
{

/* Probe mode: use 1keV alpha particle to get LCE of PScint
** The particle location is determined by a probe_config.json file.
*/
void RunProbe(Config config);

/* Random mode: use 1keV alpha particle to get LCE of PScint
** The particle is located randomly inside pscint (outside the grooves).
*/
void RunRandom(Config config);

}

#endif